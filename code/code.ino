//# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 
//# remotereset-arduino
//# Version 2.0
//# 03 March 2014
//# Copyright (c) Adrian Jon Kriel : root-at-extremecooling-dot-org
//# eXtremeSHOK :: https://eXtremeSHOK.com
//# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

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
  byte  shortpulse;
  byte  longpulse;
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
    toggle_pin( config[cnum].resetpin, config[cnum].shortpulse );
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
    toggle_pin( config[cnum].powerpin, config[cnum].longpulse );
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
    Serial.print( " For: " );
    Serial.println( config[cnum].longpulse );
    toggle_pin( config[cnum].powerpin, config[cnum].longpulse );
  }
  else
  {
    Serial.print( "Config invalid" );
  }
}

void do_state( byte cnum, int type = 0 )
{
  if( config[cnum].used )
  {
    bool power, state;
    power = digitalRead( config[cnum].powerpin );
    state = digitalRead( config[cnum].resetpin );

    if( type == 1 ){ 
      //csv
      Serial.print( cnum );
      Serial.print( "," );
      Serial.print( config[cnum].name );
      Serial.print( "," );
      if ( power )
      {
        Serial.print( "power_on," );
      }
      else
      {
          Serial.print( "power_off," );  
      }
      if ( state )
      {
        Serial.println( "state_on" );
      }
      else
      {
        Serial.println( "state_off" );
      }
    }else{
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
  }
  else
  {
    Serial.print( "Config invalid" );
  }
}

void do_poweroffon()
{
  bool power, state;
  for( byte i = 0; i < CONFIGS; i++ )
  {
    if((config[i].name != "unused")&&(config[i].used != 0))
    {
     power = digitalRead( config[i].powerpin );
     state = digitalRead( config[i].resetpin );
     if(power) //machine has power
     {
       if(state) //currently on
       {
         do_power( i ); //switch off
       }
     }
   }
  }
}

void do_poweronoff()
{
  bool power, state;
  for( byte i = 0; i < CONFIGS; i++ )
  {
    if((config[i].name != "unused")&&(config[i].used != 0))
    {
     power = digitalRead( config[i].powerpin );
     state = digitalRead( config[i].resetpin );
     if(power) //machine has power
     {
       if(!state) //currently off
       {
         do_power( i ); //switch on
       }
     }
   }
  }
}


void do_powerall()
{
  for( byte i = 0; i < CONFIGS; i++ )
  {
    if((config[i].name != "unused")&&(config[i].used != 0)){
      do_power( i );
    }
  }
}

void do_resetall()
{
  for( byte i = 0; i < CONFIGS; i++ )
  {
    if((config[i].name != "unused")&&(config[i].used != 0)){
      do_reset( i );
    }
  }
}

void do_forceall()
{
  for( byte i = 0; i < CONFIGS; i++ )
  {
    if((config[i].name != "unused")&&(config[i].used != 0)){
      do_force( i );
    }
  }
}

void do_eraseall()
{
  for( byte i = 0; i < CONFIGS; i++ )
  {
    if((config[i].name != "unused")&&(config[i].used != 0)){
      do_erase( i );
    }
  }
}

void do_status( int type = 0 )
{
  if( type == 1 ){
    //Serial.print( "\rCSV\n\r" );
  }else{
    Serial.print( "\rStatus\n\r" );
  }
  for( byte i = 0; i < CONFIGS; i++ )
  {
    if((config[i].name != "unused")&&(config[i].used != 0)){
      do_state( i ,type );
    }
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
  config[confignum].shortpulse = 0;
  config[confignum].longpulse = 0;
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
  ------ eXtremeSHOK ------\n\r\
  reset #     Toggle reset switch on config #\n\r\
  power #     Toggle power switch on config #\n\r\
  force #     Long-Press power switch on pc number #\n\r\
  erase #     Erase config number #\n\r\
  state #     Check power state on pc number #\n\r\
  resetall    Toggle reset switch on All configs\n\r\
  powerall    Toggle power switch on All configs\n\r\
  forceall    Long-Press power switch on All configs\n\r\
  eraseall    Erase All configs\n\r\
  status      Check All Power state ( stateall ) \n\r\
  poweroffon Power OFF machines which are on  \n\r\
  poweronoff Power ON Machines which are off  \n\r\
  ------\n\r\
  config      Edit configuration\n\r\
  config [0-7] resetpin pwrpin name\n\r\
  config [0-7] resetpin pwrpin name shortpulse longpulse\n\r\ 
               shortpulse = reset, default 4500ms\n\r\ 
               longpulse = shutdown, default 250ms\n\r\ 
  ------\n\r\
  dump        Dump configuration to serial\n\r\
  save        Save configuration to EEPROM\n\r\
  load        Load configuration from EEPROM\n\r\
  wipe        Wipe onfiguration and update EEPROM\n\r\
  ------ eXtremeSHOK ------\n\r\
\n\r\n\r" );
}

bool do_config( byte cnum, byte rspin, byte ppin, char * name, byte spulse = 0, byte lpulse = 0)
{
      byte namelen = strnlen( name, NAMELEN +2 );
      if( ( cnum >= 0 ) && ( cnum < CONFIGS ) && ( rspin != ppin ) &&
        ( rspin >= MINPIN ) && ( rspin <= MAXPIN ) &&
        ( ppin >= MINPIN ) && ( ppin <= MAXPIN ) && ( namelen < NAMELEN ) )
      {
        if( spulse <= 0 ){
         spulse = SHORT;
        }
        if( lpulse <= 0 ){
         lpulse = LONG;
        } 
        
        Serial.print( "Do_CONFIG\n\r" );
        config[cnum].used = true;
        config[cnum].resetpin = rspin;
        config[cnum].powerpin = ppin;
        config[cnum].shortpulse = spulse;
        config[cnum].longpulse = lpulse;
        strncpy( config[cnum].name, name, NAMELEN);
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
  //Serial.print("\nCMD:");
  //Serial.println( cmd );
  //Serial.print("\nSAVE: ");
  //Serial.println( save );

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
      do_help();
    }
    else
    {
      if( ( strncmp( cmd, "poweroffon", 10 ) == 0 ) )
      {
          do_poweroffon();
      }
      else if( ( strncmp( cmd, "poweronoff", 10 ) == 0 ) )
      {
          do_poweronoff();
      }
      else if( ( strncmp( cmd, "resetall", 8 ) == 0 ) )
      {
          do_resetall();
      }
      else if( ( strncmp( cmd, "powerall", 8 ) == 0 ) )
      {
          do_powerall();
      }
      else if( ( strncmp( cmd, "forceall", 8 ) == 0 ) )
      {
          do_forceall();
      }
      else if( ( strncmp( cmd, "eraseall", 8 ) == 0 ) )
      {
          do_eraseall();
      }
      else if( ( strncmp( cmd, "status", 6 ) == 0 ) )
      {
          do_status( int(cnum) );
      }
      else if( ( strncmp( cmd, "stateall", 8 ) == 0 ) )
      {
          do_status( int(cnum) );
      }
      else if( ( strncmp( cmd, "reset", 5 ) == 0 ) )
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
        char *resetpin, *pwrpin, *name, *shortpulse, *longpulse;

        resetpin = strtok_r( NULL, " ", &save );
        pwrpin = strtok_r( NULL, " ", &save );
        name = strtok_r( NULL, " ", &save );
        shortpulse = strtok_r( NULL, " ", &save );
        longpulse = strtok_r( NULL, " ", &save );

        if ( resetpin && pwrpin && name )
        {
         byte rspin, ppin;
         rspin = atoi( resetpin );
         ppin = atoi( pwrpin );
         
         if( shortpulse && longpulse ){
            byte spulse, lpulse; 
            spulse = atoi( shortpulse );
            lpulse = atoi( longpulse );
            do_config( cnum, rspin, ppin, name, spulse, lpulse);
         }else{
            do_config( cnum, rspin, ppin, name);
         }
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
