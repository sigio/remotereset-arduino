#include <string.h>
#include <EEPROM.h>
#include "EEPROMAnything.h"

#define MINPIN 2                // Pin 0 and 1 are RX/TX
#define MAXPIN 12               // Stick to digital for now
#define SHORT 250               // Short keypress, in 'ms'
#define LONG 4500               // Long keypress, in 'ms'
#define INPUTLEN 50             // Length of imputstring we parse
#define NAMELEN 10              // Length of a host/config name
#define CONFIGS 8               // Amount of configs to store
#define PROMPT "\n> "


// Function declarations
void processString();


char * inputstring = (char *) malloc(INPUTLEN+1);
char * isbegin = inputstring;
byte islen = 0;
bool inputdone = false;

struct configdata
{
  bool  used;
  byte  resetpin;
  byte  powerpin;
  char  name[NAMELEN];
} config[CONFIGS];

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
  Serial.print( "RemoteReset-Arduino started\n\r" );

  /*
  int i = sizeof(config);
  Serial.print( "Configuration is " );
  Serial.print( i, DEC );
  Serial.print( " bytes\n\r\n\r" );
  */

  inputstring[0] = '\0';          // Initialize to empty
  inputstring[INPUTLEN+1] = '\0'; // Should never be overwritten
  Serial.print( PROMPT );

  EEPROM_readAnything(0, config);
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
    Serial.print( PROMPT );
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

void toggle_pin( byte pin, int ms )
{
  pinMode( pin, OUTPUT );
  digitalWrite(pin, LOW);
  delay( ms );
  pinMode(pin, INPUT);
  digitalWrite(pin, LOW);
}

void do_reset( byte cnum )
{
  if( config[cnum].used )
  {
    Serial.print( "Sending reset signal to pin: " );
    Serial.println( config[cnum].resetpin );
    toggle_pin( config[cnum].resetpin, SHORT );
  }
  else
  {
    Serial.print( "Config invalid" );
  }
}

void do_power( byte cnum )
{
  if( config[cnum].used )
  {
    Serial.print( "Sending power signal to pin: " );
    Serial.println( config[cnum].powerpin );
    toggle_pin( config[cnum].powerpin, SHORT );
  }
  else
  {
    Serial.print( "Config invalid" );
  }
}

void do_force( byte cnum )
{
  if( config[cnum].used )
  {
    Serial.print( "Sending long power signal to pin: " );
    Serial.println( config[cnum].powerpin );
    toggle_pin( config[cnum].powerpin, LONG );
  }
  else
  {
    Serial.print( "Config invalid" );
  }
}

void do_state( byte cnum )
{
  if( config[cnum].used )
  {
    bool power, state;
    power = digitalRead( config[cnum].powerpin );
    state = digitalRead( config[cnum].resetpin );

    Serial.print( "System " );
    Serial.print( config[cnum].name );
    Serial.print( " has " );
    if ( ! power )
    {
      Serial.print( "NO " );
    }
    Serial.print( "ATX power." );

    if ( state )
    {
      Serial.println( " It seems to be powered on." );
    }
    else
    {
      Serial.println( " It seems to be off." );
    }
  }
  else
  {
    Serial.print( "Config invalid" );
  }
}

void do_load()
{
  EEPROM_readAnything(0, config);
}

void do_save()
{
  EEPROM_writeAnything(0, config);
}

void do_erase( byte confignum )
{
  config[confignum].used = false;
  config[confignum].resetpin = 255;
  config[confignum].powerpin = 255;
  strncpy( config[confignum].name, "unused", NAMELEN );
}


void do_dump()
{
  Serial.print( "\rConfig\n\r" );
  for( byte i = 0; i < CONFIGS; i++ )
  {
    Serial.print( "Config: " );
    Serial.print( i );
    Serial.print( "    Data: " );
    Serial.print( config[i].used );
    Serial.print( " / " );
    Serial.print( config[i].resetpin );
    Serial.print( " / " );
    Serial.print( config[i].powerpin );
    Serial.print( " / " );
    Serial.println( config[i].name );
  }
}

void do_wipe()
{
  for( byte i = 0; i < CONFIGS; i++ )
  {
    do_erase( i );
  }
  do_save();
}

void do_help()
{
  Serial.print( "Help: \n\r\
  config      Edit configuration\n\r\
    config [0-7] resetpin pwrpin name\n\r\
  reset #     Toggle reset switch on config #\n\r\
  power #     Toggle power switch on config #\n\r\
  force #     Long-Press power switch on pc number #\n\r\
  erase #     Erase config number #\n\r\
  state       Check power state\n\r\
  dump        Dump configuration to serial\n\r\
  save        Save configuration to EEPROM\n\r\
  load        Load configuration from EEPROM\n\r\
  wipe        Wipe onfiguration and update EEPROM\n\r\
\n\r\n\r" );
}

bool do_config( byte cnum, byte rspin, byte ppin, char * name)
{
      byte namelen = strnlen( name, NAMELEN +2 );
      if( ( cnum >= 0 ) && ( cnum < CONFIGS ) && ( rspin != ppin ) &&
        ( rspin >= MINPIN ) && ( rspin <= MAXPIN ) &&
        ( ppin >= MINPIN ) && ( ppin <= MAXPIN ) && ( namelen < NAMELEN ) )
      {
        Serial.print( "Do_CONFIG\n\r" );
        config[cnum].used = true;
        config[cnum].resetpin = rspin;
        config[cnum].powerpin = ppin;
        strncpy( config[cnum].name, name, NAMELEN );
        return true;
      }
      else
      {
        Serial.print( "Illegal config\n\r" );
        return false;
      }
};

void processString()
{
  char *cmd, *save;

  cmd = strtok_r(inputstring," ",&save);
  //Serial.print(cmd);

  // Simple commands without arguments
  if( ( strncmp( cmd, "help", 4 ) == 0 ) )
  {
    do_help();
  }
  else if( ( strncmp( cmd, "save", 4 ) == 0 ) )
  {
    do_save();
  }
  else if( ( strncmp( cmd, "load", 4 ) == 0 ) )
  {
    do_load();
  }
  else if( ( strncmp( cmd, "dump", 4 ) == 0 ) )
  {
    do_dump();
  }
  else if( ( strncmp( cmd, "wipe", 4 ) == 0 ) )
  {
    do_wipe();
  }
  else
  { 
    // Commands with at least 1 argument
    char * confignum;
    byte cnum;
    confignum = strtok_r( NULL, " ", &save );
    cnum = (byte) atoi(confignum);

    if( ! ( cnum >= 0 ) && ( cnum < CONFIGS ) )
    {
      Serial.println( "Invalid config number\n\r" );
    }
    else
    {
      if( ( strncmp( cmd, "reset", 5 ) == 0 ) )
      {
          do_reset( cnum );
      }
      else if( ( strncmp( cmd, "power", 5 ) == 0 ) )
      {
          do_power( cnum );
      }
      else if( ( strncmp( cmd, "force", 5 ) == 0 ) )
      {
          do_force( cnum );
      }
      else if( ( strncmp( cmd, "erase", 5 ) == 0 ) )
      {
          do_erase( cnum );
      }
      else if( ( strncmp( cmd, "state", 5 ) == 0 ) )
      {
          do_state( cnum );
      }
      else if( ( strncmp( cmd, "config", 6 ) == 0 ) )
      {
        char *resetpin, *pwrpin, *name;

        resetpin = strtok_r( NULL, " ", &save );
        pwrpin = strtok_r( NULL, " ", &save );
        name = strtok_r( NULL, " ", &save );

        if ( resetpin && pwrpin && name )
        {
         byte rspin, ppin;
         rspin = atoi( resetpin );
         ppin = atoi( pwrpin );

         do_config( cnum, rspin, ppin, name);
        }
        else
        {
          Serial.print( "ERROR: Please specify configuration number, resetpin, pwrpin and a name\n\r\n\r" );
        }
      }
      else
      {
      Serial.print( "Unknown command: " );
      Serial.print( cmd );
      Serial.print( "\n\r\n\r" );
      }
    }
  }
}

