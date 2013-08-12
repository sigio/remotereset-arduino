
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

// the setup routine runs once when you press reset:
void setup() {
  int i;
  
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
  Serial.write( "Initializing" );
  inputString.reserve(200);

  for( int i = 0; i < 14; i++)
  {
    pinMode(i, INPUT);
    digitalWrite(i, LOW);
  }
}

// the loop routine runs over and over again forever:
void loop() {
  int val;
  
  if (stringComplete) {
    processString();
    Serial.println(inputString); 
    // clear the string:
    inputString = "";
    stringComplete = false;
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
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read(); 
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    } 
  }
}

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
