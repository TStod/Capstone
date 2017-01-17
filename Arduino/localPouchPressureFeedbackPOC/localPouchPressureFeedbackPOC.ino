#define CONTROL_SCALAR 1
#define POUCH_CONTROL_DELAY 50
#define MOTOR_CONTROL_DELAY 1000

const int pressureReadPin = 15;
const unsigned short maintainPressure = 150;

const int pouchPressurePin = 14;

const int motorControlPin = 9;

enum ControlStates_t
{
  StateInflate = 0,
  StateInflateWait,
  StateDeflate,
  StateDeflateWait,
};

ControlStates_t controlState = StateInflate;

int inflateState = LOW;
int deflateState = LOW;

char command;
unsigned short val;

const int inflatePin = 13;
const int deflatePin = 12;

unsigned long previousMillis = 0;
unsigned long previousMotorChange = 0;
unsigned long previousPouchChange = 0;

void setup() {
  Serial.begin(9600);
  pinMode(inflatePin, OUTPUT);
  pinMode(deflatePin, OUTPUT);
  pinMode(motorControlPin, OUTPUT);
  controlState = StateDeflateWait;
}

void loop() {
  unsigned long currentMillis = millis();
  
  unsigned short pressureRead;
  unsigned short currentPouchPressure;
  
  pressureRead = analogRead(pressureReadPin);
  //Serial.println(pressureRead);
  
  static unsigned int targetPouchPressure;
  
  // Check if a new target pressure is available
  if (Serial.available())
  {
    command = Serial.read();
    val = command - '0';
    if (val < 10)
    {
      targetPouchPressure = val;
    }
  }
  
  // Control for pouch pressure
  currentPouchPressure = analogRead(pouchPressurePin);
  
  if ((currentMillis - previousPouchChange) > POUCH_CONTROL_DELAY)
  {
    int pressureDelta = targetPouchPressure - currentPouchPressure;
    
    if (pressureDelta > 0)
    {
      digitalWrite(inflatePin, HIGH);
      delay(abs(pressureDelta) / CONTROL_SCALAR);
      digitalWrite(inflatePin, LOW);
    }
    else if (pressureDelta < 0)
    {
      digitalWrite(deflatePin, HIGH);
      delay(abs(pressureDelta) / CONTROL_SCALAR);
      digitalWrite(deflatePin, LOW);
    }
  }
     
  // Control for tank pressure
  if ((currentMillis - previousMotorChange) > MOTOR_CONTROL_DELAY)
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
