#define MINPIN 2                // Pin 0 and 1 are RX/TX
#define MAXPIN 12               // Stick to digital for now
#define SHORT 50                // Short keypress, in 'ms'
#define LONG 4500               // Long keypress, in 'ms'
#define INPUTLEN 20             // Length of imputstring we parse

char inputstring[INPUTLEN+1] = "";// a string to hold incoming data
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
  Serial.write( "RemoteReset-Arduino started" );
  inputstring[0] = '\0';          // Initialize to empty
  inputstring[INPUTLEN+1] = '\0'; // Should never be overwritten
}

// the loop routine runs over and over again forever:
void loop()
{
  // If we have a input to process, do so
  if (inputdone) {
    processString();
    Serial.println(inputstring); 
    // clear the string:
    inputstring[0] = '\0';
    islen=0;
    inputdone = false;
  }

  if ( Serial.available() > 0)
  {
    char input = Serial.read();

    // Newline / Carriage return close the input
    if( ( input == '\n' ) || ( input == '\r' ) )
    {
      // Close input with \0
      inputstring[islen++] = '\0';
      inputdone=true;
    }
    else if( ( input >= ' ' ) && ( input <= 'z' ) )
    {
      // Add valid character, increment islen
      inputstring[islen++] = input
    }
    else
    {
      Serial.print( "Ignoring character: " );
      Serial.print( input, HEX );
    }

    // Have we reached max length, also close input and flush remainder
    if( islen >= INPUTLEN )
    {
      inputstring[islen] = '\0';
      inputdone=true;

      // Flush remaining characters in input to /dev/zero
      while( Serial.available() > 0 )
      {
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
  Serial.print( "Got a string: '" );
  Serial.print( inputString );
  Serial.print( "'" );
  
  if( inputString.startsWith("help") )
  {
    Serial.println( "Ik snap de volgende commando's: help reset power status." );
  }
  else if( inputString.startsWith("reset")  )
  {
     Serial.println( "Dat was een reset" );
  }
  else
  {
    Serial.println( "Onbekend commando" );
  }
}
