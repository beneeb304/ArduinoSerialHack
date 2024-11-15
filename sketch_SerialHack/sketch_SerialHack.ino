// *********************************************************************************************************************
// File:          sketch_SerialHack.ino
// Project:       Hackathons
// Created:       11/15/2024
// Author:        Ben Neeb
// Purpose:       Arduino and sensors designed to teach how to use serial to interface with microcontrollers
// Input:         USB or Bluetooth serial, 115200 baud
// Output:        Various physical outputs
// Requirements:  ESP32-WROOM-DA board, an ultrasonic module HC-SR04, SG90 servo, LED light, 220 resistor, active buzzer
// Revised:       
// Change Notes:  Initial commit
// *********************************************************************************************************************

// Future Ideas:
// Change baud rate on each restart
// Seperate flags/points for USB vs BT serial
// 

#include <BluetoothSerial.h>
#include <ESP32Servo.h>
#include <cctype>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;
Servo sg90;

char chrSerialInput;
String strSerialInput = "";


// Digital PINs
const int pinBuzzer = 32;
const int pinTrig = 25;
const int pinEcho = 33;
const int pinServo = 26;
const int pinLED = 27;

// Define the minimum and maximum pulse widths for the servo
const int minPulseWidth = 500;  // 0.5 ms
const int maxPulseWidth = 2500; // 2.5 ms

void setup() {
  // Initialize USB serial
  Serial.begin(115200);

  // Initialize Bluetooth serial
  SerialBT.begin("Sensor Box 2C132c"); // Bluetooth device name
  Serial.println("The device started, now you can connect it with bluetooth!"); // FIND ME

  // Attach the servo to the specified pin and set its pulse width range
  sg90.attach(pinServo, minPulseWidth, maxPulseWidth);

  // Set the PWM frequency for the servo (standard 50Hz servo)
  sg90.setPeriodHertz(50);

  // Set pin modes
  pinMode(pinBuzzer, OUTPUT);
  pinMode(pinLED, OUTPUT);
  pinMode(pinTrig, OUTPUT);
  pinMode(pinEcho, INPUT);
}

void loop() {
  // Loop while either bluetooth or USB serial is available
  while (SerialBT.available() || Serial.available())
  {
    // Read bt serial, if not available read usb serial
    if (SerialBT.available()) {
      chrSerialInput = SerialBT.read(); 
    }
    else if (Serial.available()) {
      chrSerialInput = Serial.read();
    }

    // Check for serial triggers
    switch (chrSerialInput) {
      case 'A': //FIND ME
        PlayBuzzer();
        Serial.println("Played the buzzer! Flag is 'BuzzerBeater'");
        SerialBT.println("Played the buzzer! Flag is 'BuzzerBeater'");
        break;
       
       case 'B': //FIND ME

        Serial.println("Distance: " + String(GetUltrasonicReading()));

        if (GetUltrasonicReading() == 5) {
          Serial.println("Correct distance measured! Flag is 'ProximityWarning'");
          SerialBT.println("Correct distance measured! Flag is 'ProximityWarning'");
        }
        break;

        case 'C': //FIND ME
          MoveServo();

          Serial.println("Servo operated! Flag is 'ThingsAreMoving'");
          SerialBT.println("Servo operated! Flag is 'ThingsAreMoving'");
          break;

        case 'D': //FIND ME
          LEDMorseCode();

          Serial.println("You have discovered light! We used to use that for sending codes...");
          SerialBT.println("You have discovered light! We used to use that for sending codes...");
          
       default:        
        // Convert input to lowercase
        chrSerialInput = tolower(chrSerialInput);


        if (chrSerialInput == 'h') {
          strSerialInput += String(chrSerialInput);
        } else if (chrSerialInput == 'e' && strSerialInput == "h") {
          strSerialInput += String(chrSerialInput);
        } else if (chrSerialInput == 'l' && strSerialInput == "he") {
          strSerialInput += String(chrSerialInput);
        } else if (chrSerialInput == 'p' && strSerialInput == "hel") {
          Serial.println(PrintHelp());
          SerialBT.println(PrintHelp());
        } else {
          strSerialInput = "";
        }
    }
  }

  // Quick delay to help loop
  delay(20);
}

String PrintHelp() {
  /*
    Desc:     Method to return help instructions
    Params:   None
    Returns:  String
  */

  String strHelp = "HELPING STUFF";
  return strHelp;
}

void PlayBuzzer() {
  /*
    Desc:     Method to play the buzzer
    Params:   None
    Returns:  None
  */

  for(int i = 0; i < 3; i++) {
    digitalWrite(pinBuzzer, HIGH);
    delay(1000);
    digitalWrite(pinBuzzer, LOW);
    delay(1000);
  }
}

int GetUltrasonicReading() {
  /*
    Desc:     This method uses the ultrasonic sensor to get the current distance
    Params:   None
    Returns:  Current distance as integer (in cm)
  */

  float fltDuration, fltDistance;

  // Get readings from HC-SR04 ultrasonic sensor
  digitalWrite(pinTrig, LOW);
  delayMicroseconds(2);
  digitalWrite(pinTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinTrig, LOW);

  // Translate readings to cm
  fltDuration = pulseIn(pinEcho, HIGH);
  fltDistance = (fltDuration*.0343)/2;
  return (int) fltDistance;
}

void MoveServo() {
  /*
    Desc:     This method moves the servo motor
    Params:   None
    Returns:  None
  */
  
  // Rotate the servo from 0 to 180 degrees
  for (int angle = 0; angle <= 180; angle++) {
    int pulseWidth = map(angle, 0, 180, minPulseWidth, maxPulseWidth);
    sg90.writeMicroseconds(pulseWidth);
    delay(15);
  }

  // Rotate the servo from 180 to 0 degrees
  for (int angle = 180; angle >= 0; angle--) {
    int pulseWidth = map(angle, 0, 180, minPulseWidth, maxPulseWidth);
    sg90.writeMicroseconds(pulseWidth);
    delay(15);
  }
}

void LEDMorseCode() {
  /*
    Desc:     This method uses the LED to transmit Morse Code to spell out "LED"
    Params:   None
    Returns:  None
  */

  // L
  MorseDot();
  MorseDash();
  MorseDot();
  MorseDot();
  delay(3000);

  // E
  MorseDot();
  delay(3000);

  // D
  MorseDash();
  MorseDot();
  MorseDot();
  delay(3000);
}

void MorseDot() {
  /*
    Desc:     This method uses the LED to send a "dot" in Morse Code
    Params:   None
    Returns:  None
  */
  
  digitalWrite(pinLED, HIGH);
  delay(1000);
  digitalWrite(pinLED, LOW);
  delay(1000);
}

void MorseDash() {
  /*
    Desc:     This method uses the LED to send a "dash" in Morse Code
    Params:   None
    Returns:  None
  */
  
  digitalWrite(pinLED, HIGH);
  delay(3000);
  digitalWrite(pinLED, LOW);
  delay(1000);
}