/*
  Flower Class Corvette FS-I6X Code
  via IBus

  Channel functions by Ricardo Paiva - https://gist.github.com/werneckpaiva/

  Thomas Sperling 2024
*/

// Bibliotheken
#include <IBusBM.h> // iBusBM Library
#include <Stepper.h> //Stepper Library
#include <AccelStepper.h> // AccelStepper Library

// Erstelle IBus-Objekt
IBusBM ibus;

// Funkkanal auslesen und ggf. mappen
int readChannel(byte channelInput, int minLimit, int maxLimit, int defaultValue) {
  uint16_t ch = ibus.readChannel(channelInput);
  if (ch < 100) return defaultValue; // Standardwert setzen, wenn Kanal "aus"
  return map(ch, 1000, 2000, minLimit, maxLimit);
}

// Funkkanal auslesen und ggf. boolean setzen
bool readSwitch(byte channelInput, bool defaultValue) {
  int intDefaultValue = (defaultValue) ? 100 : 0;
  int ch = readChannel(channelInput, 0, 100, intDefaultValue);
  return (ch > 50);
}


// Definition der Pins
int mosfetPin1 = 2; // Schaltfunktion Kanal 7
int mosfetPin2 = 3; // Schaltfunktion Kanal 8
int mosfetPin3 = 4; // Schaltfunktion Kanal 9
int mosfetPin4 = 5; // Schaltfunktion Kanal 10

// Intervall für die MOSFET-Steuerung (in Millisekunden) als
// Testplatzhalter bis zum FS-Code
int intervalmosfetPin1 = 1000;
int intervalmosfetPin2 = 1000;
int intervalmosfetPin3 = 1000;
int intervalmosfetPin4 = 1000;

// Variablen für den letzten Zeitpunkt der Steuerung
unsigned long previousmosfetPin1 = 0;
unsigned long previousmosfetPin2 = 0;
unsigned long previousmosfetPin3 = 0;
unsigned long previousmosfetPin4 = 0;

// alle MOSFETs auf (LOW)
int mosfetPin1State = LOW;
int mosfetPin2State = LOW;
int mosfetPin3State = LOW;
int mosfetPin4State = LOW;

// Define Constants

const float STEPS_PER_REV = 32; // Number of steps per internal motor revolution 
const float GEAR_RED = 64; //  Amount of Gear Reduction
const float STEPS_PER_OUT_REV = STEPS_PER_REV * GEAR_RED; // Number of steps per geared output rotation

// Define step constants

#define FULLSTEP 4
#define HALFSTEP 8

// Define Variables

int StepsRequired; // Number of Steps Required

// Define Motor Pins
 
#define motorPin1  8     // 28BYJ48 pin 1
#define motorPin2  9     // 28BYJ48 pin 2
#define motorPin3  10    // 28BYJ48 pin 3
#define motorPin4  11    // 28BYJ48 pin 4

// Create Instance of Stepper Class
// Specify Pins used for motor coils
// The pins used are 8,9,10,11 
// Connected to ULN2003 Motor Driver In1, In2, In3, In4 
// Pins entered in sequence 1-3-2-4 for proper step sequencing

Stepper steppermotor(STEPS_PER_REV, 8, 10, 9, 11);

AccelStepper stepper1(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);


void setup() {
  Serial.begin(57600);

  ibus.begin(Serial1); // Attach iBus object to serial port

  // Pins als Output setzen
  pinMode(mosfetPin1, OUTPUT);
  pinMode(mosfetPin2, OUTPUT);
  pinMode(mosfetPin3, OUTPUT);
  pinMode(mosfetPin4, OUTPUT);

  // Alle MOSFETs nochmal auf (LOW)
  digitalWrite(mosfetPin1, LOW);
  digitalWrite(mosfetPin2, LOW);
  digitalWrite(mosfetPin3, LOW);
  digitalWrite(mosfetPin4, LOW);

  // 1 RPM Stepper 1 CW
  stepper1.setMaxSpeed(1000.0);
  stepper1.setAcceleration(120.0);
  stepper1.setSpeed(1000);
  stepper1.moveTo(400);
}

void checkmosfetPin1() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousmosfetPin1 >= intervalmosfetPin1) {
     previousmosfetPin1 = currentMillis; // Speichern des aktuellen Zeitpunkts
   
    if (mosfetPin1State == LOW) // Zustand des MOSFETs umschalten 
    {
      mosfetPin1State = HIGH;
    } else {
      mosfetPin1State = LOW;
    }
    digitalWrite(mosfetPin1, mosfetPin1State);
  }
}

void checkmosfetPin2() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousmosfetPin2 >= intervalmosfetPin2) {
        previousmosfetPin2 = currentMillis;

    if (mosfetPin2State == LOW) {
      mosfetPin2State = HIGH;
    } else {
      mosfetPin2State = LOW;
    }
    digitalWrite(mosfetPin2, mosfetPin2State);
  }
}

void checkmosfetPin3() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousmosfetPin3 >= intervalmosfetPin3) {
    previousmosfetPin3 = currentMillis;

    if (mosfetPin3State == LOW) {
      mosfetPin3State = HIGH;
    } else {
      mosfetPin3State = LOW;
    }
    digitalWrite(mosfetPin3, mosfetPin3State);
  }
}

void checkmosfetPin4() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousmosfetPin4 >= intervalmosfetPin4) {
    previousmosfetPin4 = currentMillis;
    if (mosfetPin4State == LOW) {
      mosfetPin4State = HIGH;
    } else {
      mosfetPin4State = LOW;
    }
    digitalWrite(mosfetPin4, mosfetPin4State);
  }
}

void loop() {
  checkmosfetPin1();
  checkmosfetPin2(); 
  checkmosfetPin3();
  checkmosfetPin4();

  // alle zehn Kanäle auslesen und die Werte auf ser. Monitor anzeigen
  for (byte i = 0; i < 10; i++) {
    int value = readChannel(i, -100, 100, 0);
    Serial.print("Ch");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(value);
    Serial.print(" | ");
  }

  // exempl. Kanal 6 (switch) boolean value
  Serial.print("Ch6: ");
  Serial.print(readSwitch(5, false));
  Serial.println();

  //Richtungsumkehr an Limits
  if (stepper1.distanceToGo() == 0) 
    stepper1.moveTo(-stepper1.currentPosition());

   
  
  stepper1.run();
}
