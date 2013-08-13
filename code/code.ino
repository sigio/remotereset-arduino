#include <string.h>

#define MINPIN 2                // Pin 0 and 1 are RX/TX
#define MAXPIN 12               // Stick to digital for now
#define SHORT 50                // Short keypress, in 'ms'
#define LONG 4500               // Long keypress, in 'ms'
#define INPUTLEN 50             // Length of imputstring we parse

char * inputstring = (char *) malloc(INPUTLEN+1);
char * isbegin = inputstring;
byte islen = 0;
bool inputdone = false;

// Arduino PINS default to 'input' and 'LOW', but set them anyway
// so we can call reset later to revert to defaults if needed.
void setup()
{
  for( int i = MINPIN; i <= MAXPIN; i++)
  {
    pinMode(i, INPUT);
    digitalWrite(i, LOW);
  }
  
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
  Serial.print( "RemoteReset-Arduino started\n\r\n\r" );
  inputstring[0] = '\0';          // Initialize to empty
  inputstring[INPUTLEN+1] = '\0'; // Should never be overwritten
  Serial.print( "\n> " );
}

// the loop routine runs over and over again forever:
void loop()
{
  // If we have a input to process, do so
  if (inputdone) {
    processString();
    // clear the string:
    inputstring[0] = '\0';
    islen=0;
    inputdone = false;
    Serial.print( "\n> " );
  }

  if( Serial.available() > 0)
  {
    char input = Serial.read();
    Serial.print(input);

    // Newline / Carriage return close the input
    if( ( input == '\n' ) || ( input == '\r' ) )
    {
      // Close input with \0
      inputstring[islen++] = '\0';
      inputdone=true;

      Serial.print( "\n\r\n\r" );
    }
    else if( input == 127 )
    {
      // Backspace
      if ( islen >= 1 )
      {
        Serial.print( "\b \b" );
        inputstring[islen--] = '\0';
      }
    }
    else if( ( input >= ' ' ) && ( input <= 'z' ) )
    {
      // Add valid character, increment islen
      inputstring[islen++] = input;
    }
    else
    {
      Serial.print( "Illegal character: '" );
      Serial.print( input, HEX );
      Serial.print( "'\n" );
    }

    // Have we reached max length, also close input and flush remainder
    if( islen >= INPUTLEN )
    {
      inputstring[islen] = '\0';
      inputdone=true;

      // Flush remaining characters in input to /dev/zero
      while( Serial.available() > 0 )
      {
        // This doesn't seem to flush all input
        Serial.read();
      }
    }
  }
}

/*  Serial.println( "Restart loop" );
  for( int i = 3; i < 14; i++ )
  {
     val = digitalRead(i);
     Serial.print( "Pin " );
     Serial.print( i, DEC );
     Serial.print( " is currently " );
     Serial.println( val, DEC );
  }
  if ( Serial.available() > 0 )
  {
     int inbyte = Serial.read();
     if (( inbyte >= '0' ) && ( inbyte <= '9' ))
     {
       int port = inbyte - '0';
       Serial.print( "Toggling pin " );
       Serial.print( port );
       Serial.println( " to output for 500ms" );
       pinMode(port, OUTPUT);
       digitalWrite(port, LOW);
       delay(500);
       pinMode(port, INPUT);
       digitalWrite(port, LOW);
     }
     else
     {
       Serial.println( "Incorrect input" );
     }
  }
  delay(1000);
  
  */

void processString()
{
  char *p, *i;

  p = strtok_r(inputstring," ",&i);
  Serial.print(p);

  if( ( strncmp( p, "help", 4 ) == 0 ) )
  {
    Serial.print( "Help: \n\r\
  setup       Enter setup mode\n\r\
  reset #     Toggle reset switch on pc number #\n\r\
  power #     Toggle power switch on pc number #\n\r\
  force #     Long-Press power switch on pc number #\n\r\
  check       Check power status\n\r\
\n\r\n\r" );
  }
  else if( ( strncmp( p, "setup", 5 ) == 0 ) )
  {
    Serial.print( "Setup: Used to configure the controller\n\r\n" );
  }
  else
  {
    Serial.print( "Unknown command: " );
    Serial.print( p );
    Serial.print( "\n\r\n\r" );
  }

  //  Second strtok iteration
  /*
  p = strtok_r(NULL," ",&i);
  Serial.print(p);
  Serial.println("");

  Serial.println( "\n" );
  */
}
