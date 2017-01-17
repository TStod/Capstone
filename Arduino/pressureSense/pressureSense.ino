int pressureReadPin = 15;
const unsigned short maintainPressure = 130;

const int motorControlPin = 9;

const long inflateInterval = 200;
const long inflateWaitInterval     = 2000;
const long deflateInterval = 2000;
const long deflateWaitInterval     = 2000;

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

const int inflatePin = 13;
const int deflatePin = 12;

unsigned long previousMillis = 0;
unsigned long previousMotorChange = 0;

void setup() {
  Serial.begin(9600);
  pinMode(inflatePin, OUTPUT);
  pinMode(deflatePin, OUTPUT);
  pinMode(motorControlPin, OUTPUT);
  controlState = StateDeflateWait;
}

void loop() {
  unsigned long currentMillis = millis();
  unsigned long delta = currentMillis - previousMillis;
  
  unsigned short pressureRead;
  pressureRead = analogRead(pressureReadPin);
  //Serial.println(pressureRead);
  
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

  // Control for valve state
  if (controlState == StateInflate)
  {
    if (delta >= inflateInterval)
    {
      digitalWrite(inflatePin, LOW);
      previousMillis = currentMillis;
      controlState = StateInflateWait;
      Serial.println("State Inflate Wait");
    }   
  }
  else if (controlState == StateInflateWait)
  {
    if (delta >= inflateWaitInterval)
    {
      digitalWrite(deflatePin, HIGH);
      previousMillis = currentMillis;
      controlState = StateDeflate;
      Serial.println("State Deflate");
    }
  }

  else if (controlState == StateDeflate)
  {
    if (delta >= deflateInterval)
    {
      digitalWrite(deflatePin, LOW);
      previousMillis = currentMillis;
      controlState = StateDeflateWait;
      Serial.println("State Deflate Wait");
    }
  }

  else if (controlState == StateDeflateWait)
  {
    if (delta >= deflateWaitInterval)
    {
      digitalWrite(inflatePin, HIGH);
      previousMillis = currentMillis;
      controlState = StateInflate;
      Serial.println("State Inflate");
    }
  }
}
