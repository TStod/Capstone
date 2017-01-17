/*
 Interactive mode responds to single characters in the serial input
 0 deflates the pouch
 1-9 inflate the pouch, for 10 * i milliseconds
 */

int pressureReadPin = 14;
const unsigned short maintainPressure = 100;

const int motorControlPin = 38;

char command[20];
long val;

<<<<<<< HEAD
const int inflatePin = 37;
const int deflatePin = 36;
=======
const int inflatePin = 36;
const int deflatePin = 37;
>>>>>>> origin/sean/Arduino

unsigned long previousMillis = 0;
unsigned long previousMotorChange = 0;

void setup() {
  Serial.begin(9600);
  pinMode(inflatePin, OUTPUT);
  pinMode(deflatePin, OUTPUT);
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
      digitalWrite(inflatePin, LOW);
      digitalWrite(deflatePin, HIGH);
    }
    else
    {
      digitalWrite(deflatePin, LOW);
      digitalWrite(inflatePin, HIGH);
      delay(val);
      digitalWrite(inflatePin, LOW);
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
