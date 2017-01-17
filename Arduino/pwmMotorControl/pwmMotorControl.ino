int motorEnablePin = 9;
int led = 13;

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  pinMode(motorEnablePin, OUTPUT);
  //setPwmFrequency(9, 2); // Sets PWM frequency to 31250 / 2
  //TCCR2A = TCCR2A & 0b111111
  //TCCR2B = (TCCR2B & 0b11111000) | 0b001; // divisor = 1
  pinMode(led, OUTPUT);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  //analogWrite(motorEnablePin, 255);
  //TCCR2B = TCCR2B & 0b11111100;
  digitalWrite(motorEnablePin, LOW);
  digitalWrite(led, LOW);
  delay(1000);
  //analogWrite(motorEnablePin, 200);
  digitalWrite(motorEnablePin, HIGH);
  digitalWrite(led, HIGH);
  delay(1000);
}
