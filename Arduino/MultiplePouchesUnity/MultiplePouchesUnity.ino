#include <SerialCommand.h>

/*
 Interactive mode responds to single characters in the serial input
 0 deflates the pouch
 1-9 inflate the pouch, for 10 * i milliseconds
 */

int pressureReadPin = 14;
const unsigned short maintainPressure = 250;

const int motorControlPin = 38;

char command[20];
long val;

const int inflatePin1 = 28;
const int deflatePin1 = 29;
const int inflatePin2 = 30;
const int deflatePin2 = 31;
const int inflatePin3 = 32;
const int deflatePin3 = 33;
const int inflatePin4 = 34;
const int deflatePin4 = 35;
const int inflatePin5 = 36;
const int deflatePin5 = 37;


unsigned long previousMillis = 0;
unsigned long previousMotorChange = 0;

void setup() {
  Serial.begin(9600);
  
 while (!Serial);
 
  sCmd.addCommand("17800", pouch1Handler);
  sCmd.addCommand("17800", pouch2Handler);
  sCmd.addCommand("17800", pouch3Handler);
 
  pinMode(inflatePin1, OUTPUT);
  pinMode(deflatePin1, OUTPUT);
  pinMode(inflatePin2, OUTPUT);
  pinMode(deflatePin2, OUTPUT);
  pinMode(inflatePin3, OUTPUT);
  pinMode(deflatePin3, OUTPUT);
  pinMode(inflatePin4, OUTPUT);
  pinMode(deflatePin4, OUTPUT);
  pinMode(inflatePin5, OUTPUT);
  pinMode(deflatePin5, OUTPUT);

  pinMode(motorControlPin, OUTPUT);
  Serial.println("Ready");
}

void loop() {
  unsigned long currentMillis = millis();
  
  unsigned short pressureRead;
  pressureRead = analogRead(pressureReadPin);
  Serial.println(pressureRead);
  
  if (Serial.available())
  {
    int n;
    n = Serial.readBytesUntil('\n', command, 20);
    command[n] = '\0';
    Serial.println(command);
    val = atol(command);
    Serial.println(val);
    if (val == 0)
    {
      digitalWrite(inflatePin5, LOW);
      digitalWrite(deflatePin5, HIGH);
    }
    else
    {
      Serial.println("Write High");
      digitalWrite(deflatePin5, LOW);
      digitalWrite(inflatePin5, HIGH);
      delay(val);
      digitalWrite(inflatePin5, LOW);
      Serial.println("Write Low");
    }
  }
  
  // Control for output pressure
  if ((currentMillis - previousMotorChange) > 1000)
  {
    if (pressureRead < maintainPressure)
    {
      digitalWrite(motorControlPin, HIGH);
      previousMotorChange = currentMillis;
    }
    else
    {
      digitalWrite(motorControlPin, LOW);
      previousMotorChange = currentMillis;
    }
  }
}
