#define adcPrescaler 16
#define baudRate 115200
#define inflate true
#define deflate false

const unsigned char PS_16 = (1 << ADPS2);
const unsigned char PS_32 = (1 << ADPS2) | (1 << ADPS0);
const unsigned char PS_64 = (1 << ADPS2) | (1 << ADPS1);
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

const unsigned long halfUnsignedLong = 2000000000;

const int pumpPin = 53;
const int maxNumPouches = 15;
const int numPouches = 9;
//const int pouchPinOffset = 22;
const long minActuation = 6000; // TODO: Verify this is smallest imperceptible delay 
//const long inflateScalar = 8;
//const long deflateScalar = -30000;
const int settleTime = 7000; // 8000
unsigned long currentTime;
unsigned long times[7]; // TODO: Delete after testing

const int maxOffset = 12;
const int minOffset = -12;
long offset;

int sensorOffset[numPouches];
const int sensorScalar = 15;
bool isOpen[numPouches] = {false};
bool isBusy[numPouches] = {false};
unsigned long closeTime[numPouches];
unsigned long doneTime[numPouches];

long current[numPouches];
long target[numPouches];
bool valve[numPouches];

long calculatedDeflate;
long calculatedInflate;

// Serial Variables
const int minMessageLength = (numPouches / 2) + 2; // 1 for version and type, 1 for newline
const int maxMessageLength = minMessageLength;
char* message;
const int serialBufferSize = 21;
char serialBuffer[serialBufferSize];
int filled = 0;
  
char protocolVersion;
char messageType;
char messageValue;

bool deflating;
bool inflating;
bool inflatingAndDeflating;
bool pumping;
bool didPump;
const unsigned long tankValveDelay = 8000;
unsigned long tankDelayTime;

const int highTankPin = 47;
const int lowTankPin = 48;
const int highAmbientPin = 50;
const int lowAmbientPin = 51;

const int highSensorPin = 8;
const int lowSensorPin = 7;
int lowSensorOffset;
int highSensorOffset;

int currentHighPressure;
int currentLowPressure;

const int maxHighPressure = 200;
const int minHighPressure = 150;
const int maxLowPressure = 150;
const int minLowPressure = 100;

const int inflatePins[] = {
  22, // Pouch #0
  23, // Pouch #1
  26, // Pouch #2
  27, // Pouch #3
  28, // Pouch #4
  32, // Pouch #5
  33, // Pouch #6
  21, // Pouch #7
  37, // Pouch #8
  46, // Pouch #9
  38, // Pouch #10
  43, // Pouch #11
  20 // Pouch #12
};

const int deflatePins[] = {
  24, // Pouch #0
  25, // Pouch #1
  29, // Pouch #2
  30, // Pouch #3
  31, // Pouch #4
  34, // Pouch #5
  35, // Pouch #6
  39, // Pouch #7
  36, // Pouch #8
  49, // Pouch #9
  44, // Pouch #10
  45, // Pouch #11
  41 // Pouch #12
};

const int pressureSensorPins[] = {
  1, // Pouch #0
  4, // Pouch #1
  9, // Pouch #2
  12, // Pouch #3
  14, // Pouch #4
  2, // Pouch #5
  5, // Pouch #6
  10, // Pouch #7
  13, // Pouch #8
  11, // Pouch #9
  3, // Pouch #10
  6, // Pouch #11
  15 // Pouch #12
};

long minInflateActuation[] = {
  4000,
  4000,
  4000,
  4000,
  4000,
  4000,
  4000,
  4000,
  4000,
  4000,
  4000,
  4000,
  4000
};

long minDeflateActuation[] = {
  4000,
  4000,  
  4000,  
  4000,  
  4000,  
  4000,  
  4000,  
  4000,  
  4000,  
  4000,  
  4000,  
  4000,  
  4000    
};

long inflateScalarArray[] = {
    300,
    300,
    300,
    300,
    300,
    300,
    300,
    300,
    300,
    300,
    300,
    300,
    300

};

long deflateScalarArray[] = {
    -60000,
    -60000,
    -60000,
    -60000,
    -60000,
    -60000,
    -60000,
    -60000,
    -60000,
    -60000,
    -60000,
    -60000,
    -60000
};
bool lessThan(unsigned long a, unsigned long b) {
  return (a < b) ? (b - a < halfUnsignedLong) : (a - b > halfUnsignedLong);
}

bool greaterThan(unsigned long a, unsigned long b) {
  return lessThan(b, a);
}

void calibrate() {
  digitalWrite(pumpPin, LOW);
  digitalWrite(highTankPin, HIGH);
  digitalWrite(lowTankPin, HIGH);
  digitalWrite(highAmbientPin, HIGH);
  digitalWrite(lowAmbientPin, HIGH);
  for (int pouchCounter = 0; pouchCounter < numPouches; pouchCounter++) {
    digitalWrite(deflatePins[pouchCounter], HIGH);
  }
  delay(5000);
  for (int pouchCounter = 0; pouchCounter < numPouches; pouchCounter++) {
    sensorOffset[pouchCounter] = analogRead(pressureSensorPins[pouchCounter]);
  }
  lowSensorOffset = analogRead(lowSensorPin);
  highSensorOffset = analogRead(highSensorPin);
  for (int pouchCounter = 0; pouchCounter < numPouches; pouchCounter++) {
    digitalWrite(deflatePins[pouchCounter], LOW);
  }
  digitalWrite(highTankPin, LOW);
  digitalWrite(lowTankPin, LOW);
  digitalWrite(highAmbientPin, LOW);
  digitalWrite(lowAmbientPin, LOW);
  delay(100);
}
// calibrate
// drain tanks
// open all low valves
// read from each pressure sensor
// close everything
// neutral tank state

void setup() {
  ADCSRA &= ~PS_128;  // Remove bits set by Arduino library

  if (adcPrescaler == 64) {
    ADCSRA |= PS_64; // 64 prescaler
  }
  else if (adcPrescaler == 32) {
    ADCSRA |= PS_32; // 32 prescaler  
  }
  else if (adcPrescaler == 16) {
    ADCSRA |= PS_16; // 16 prescaler
  }
  else {
    ADCSRA |= PS_128; // 128 prescaler
  }
  
  for (int pouchCounter = 0; pouchCounter < numPouches; pouchCounter++) {
    pinMode(inflatePins[pouchCounter], OUTPUT);
    pinMode(deflatePins[pouchCounter], OUTPUT);
  }
  
  pinMode(pumpPin, OUTPUT);
  pinMode(highTankPin, OUTPUT);
  pinMode(lowTankPin, OUTPUT);
  pinMode(highAmbientPin, OUTPUT);
  pinMode(lowAmbientPin, OUTPUT);
  
  deflating = false;
  inflating = false;

  calibrate();
  
  for (int pouchCounter = 0; pouchCounter < numPouches; pouchCounter++) {
    target[pouchCounter] = sensorOffset[pouchCounter];
  }

  Serial.begin(baudRate);
  //Serial.println("Ready");
  
  for (int pouchCounter = 0; pouchCounter < numPouches; pouchCounter++)
  {
    isBusy[pouchCounter] = true;
    doneTime[pouchCounter] = micros() + 10000000;
  }
}

bool safeToContinue;
unsigned long delayTime;
unsigned long nextCloseTime;
void closeValves(int nextBlockingTime) {
  do {
    safeToContinue = true;
    int pouchCounter;
    for (pouchCounter = 0; pouchCounter < numPouches; pouchCounter++) { // 100us
      if (isOpen[pouchCounter]) {
        nextCloseTime = closeTime[pouchCounter];
        safeToContinue = false;
        break;
      }
    }
    if (!safeToContinue) {
      for ( ; pouchCounter < numPouches; pouchCounter++) {
        if ((isOpen[pouchCounter]) && (lessThan(closeTime[pouchCounter], nextCloseTime))) {
          nextCloseTime = closeTime[pouchCounter];
        }
      }
      // Serial.println(micros() - currentTime);
      currentTime = micros();
      delayTime = nextCloseTime - currentTime;
      if (lessThan(delayTime, nextBlockingTime)) {
        if (delayTime < halfUnsignedLong) {
          delayMicroseconds(delayTime); // TODO: add additional time here?
        }
        for (pouchCounter = 0; pouchCounter < numPouches; pouchCounter++) {
          if ((isOpen[pouchCounter]) && (lessThan(closeTime[pouchCounter], currentTime))) {
            isOpen[pouchCounter] = false;
            if (valve[pouchCounter] == inflate) {
              digitalWrite(inflatePins[pouchCounter], LOW);
            }
            else {
              digitalWrite(deflatePins[pouchCounter], LOW);
            }
          }
        }
      }
      else {
        safeToContinue = true;
      }
    }
  } while(!safeToContinue);
  currentTime = micros();
  for (int pouchCounter = 0; pouchCounter < numPouches; pouchCounter++) {
    if ((isBusy[pouchCounter]) && (!isOpen[pouchCounter]) && (lessThan(doneTime[pouchCounter], currentTime))) {
      isBusy[pouchCounter] = false;
    }
  }
}

void loop() {
  times[0] = micros();
  
  closeValves(480);
  
  times[1] = micros();
  
  // Open Valves
  for (int pouchCounter = 0; pouchCounter < numPouches; pouchCounter++) {
    if (!isBusy[pouchCounter]) {
      current[pouchCounter] = analogRead(pressureSensorPins[pouchCounter]);
      offset = target[pouchCounter] - current[pouchCounter];
      if (offset < 0) {
        //calculatedDeflate = offset * deflateScalar;
        calculatedDeflate = offset * deflateScalarArray[pouchCounter];
        calculatedDeflate /= target[pouchCounter] + current[pouchCounter];
        //calculatedDeflate += minActuation;
        calculatedDeflate += minDeflateActuation[pouchCounter];
        isOpen[pouchCounter] = true;
        isBusy[pouchCounter] = true;
        valve[pouchCounter] = deflate;
//        Serial.println(calculatedDeflate);
        closeTime[pouchCounter] = micros() + calculatedDeflate;
        doneTime[pouchCounter] = closeTime[pouchCounter] + settleTime;
        digitalWrite(deflatePins[pouchCounter], HIGH);
        //Serial.println("Deflate");
        //Serial.println(calculatedDeflate);
      }
      else if (offset > maxOffset) {
        //calculatedInflate = (offset - maxOffset) * (current[pouchCounter] + target[pouchCounter]) * inflateScalar;
        calculatedInflate = (offset - maxOffset) * (current[pouchCounter] + target[pouchCounter]) * inflateScalarArray[pouchCounter];
        calculatedInflate /= currentHighPressure;
//        Serial.println(calculatedInflate);
        //calculatedInflate += minActuation;
        calculatedInflate += minInflateActuation[pouchCounter];
        isOpen[pouchCounter] = true;
        isBusy[pouchCounter] = true;
        valve[pouchCounter] = inflate;
        closeTime[pouchCounter] = micros() + calculatedInflate;
        doneTime[pouchCounter] = closeTime[pouchCounter] + settleTime;
        digitalWrite(inflatePins[pouchCounter], HIGH);
        //Serial.println("Inflate");
        //Serial.println(calculatedInflate);
      }
    }
  }
  
  times[2] = micros();
  
  currentHighPressure = analogRead(highSensorPin) - highSensorOffset;
  currentLowPressure = analogRead(lowSensorPin) - lowSensorOffset;
  if ((currentHighPressure < maxHighPressure) && (currentLowPressure < maxLowPressure)) {
    digitalWrite(highTankPin, HIGH);
    digitalWrite(lowTankPin, HIGH);
    digitalWrite(highAmbientPin, LOW);
    digitalWrite(lowAmbientPin, LOW);
    digitalWrite(pumpPin, HIGH);
  }
  else if (currentHighPressure < maxHighPressure) {
    digitalWrite(highTankPin, HIGH);
    digitalWrite(lowTankPin, LOW);
    digitalWrite(highAmbientPin, LOW);
    digitalWrite(lowAmbientPin, HIGH);
    digitalWrite(pumpPin, HIGH);
  }
  else if (currentLowPressure < maxLowPressure) {
    digitalWrite(highTankPin, LOW);
    digitalWrite(lowTankPin, HIGH);
    digitalWrite(highAmbientPin, HIGH);
    digitalWrite(lowAmbientPin, LOW);
    digitalWrite(pumpPin, HIGH);
  }
  else {
    digitalWrite(highTankPin, LOW);
    digitalWrite(lowTankPin, LOW);
    digitalWrite(highAmbientPin, LOW);
    digitalWrite(lowAmbientPin, LOW);
    digitalWrite(pumpPin, LOW);
  }

  times[3] = micros();
  
  closeValves(100);
  
  times[4] = micros();
  
  // Read Serial
  int serialAvailable = Serial.available();
  if (serialAvailable > 0) {
    if (serialAvailable > serialBufferSize) { // doesnt fit
      while (serialAvailable > serialBufferSize) {
        Serial.readBytes(serialBuffer, min(serialAvailable - serialBufferSize, serialBufferSize));
      }
      Serial.readBytes(serialBuffer, serialBufferSize);
      filled = serialBufferSize;
    }
    else if (serialAvailable <= serialBufferSize - filled) { // fits without shifting
      Serial.readBytes(&serialBuffer[filled], serialAvailable);
      filled += serialAvailable;
    }
    else { // fits after shifting
      memcpy(serialBuffer, &serialBuffer[serialAvailable - (serialBufferSize - filled)], serialBufferSize - serialAvailable);
      Serial.readBytes(&serialBuffer[serialBufferSize - serialAvailable], serialAvailable);
      filled = serialBufferSize;
    }
  }
  int pos;
  int endPos = -1;
  int startPos;
  for (pos = filled - 1; pos >= maxMessageLength - 1; pos--) { // find a message
    if ((0b11111111 & serialBuffer[pos]) == 0b11111111) {
      endPos = pos;
      startPos = pos - (maxMessageLength - 1);
      pos = 0;
    }
  }
  if (endPos != -1) { // found a message
    message = &serialBuffer[startPos];
    protocolVersion = (message[0] & 0b11000000) >> 6;
    if (protocolVersion == 1) {
      messageType = (message[0] & 0b00110000) >> 4;
      if (messageType == 1) {
        for (int pouchCounter = 0; pouchCounter < numPouches; pouchCounter++) {
          if (0b1 & pouchCounter) {
            messageValue = (message[(pouchCounter + 1) / 2] & 0b01110000) >> 4;
          }
          else {
            messageValue = (message[(pouchCounter + 1) / 2] & 0b00000111);
          }
          target[pouchCounter] = messageValue * sensorScalar + sensorOffset[pouchCounter];
        }
      }
    }
    filled = filled - endPos - 1;
    memcpy(serialBuffer, &serialBuffer[endPos + 1], filled);
  }
  
  times[5] = micros();
  
//  Serial.print("Close: ");
//  Serial.println(times[1] - times[0]);
//  Serial.print("Open: ");
//  Serial.println(times[2] - times[1]);
//  Serial.print("Tank: ");
//  Serial.println(times[3] - times[2]);
//  Serial.print("Close: ");
//  Serial.println(times[4] - times[3]);
////  if (iFound) {
//    Serial.print("Serial: ");
//    Serial.println(times[5] - times[4]);
////  }
//  Serial.print("Total: ");
//  Serial.println(times[5] - times[0]);
//  Serial.println("");
}
