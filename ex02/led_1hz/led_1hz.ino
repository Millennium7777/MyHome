const int ledPin = 13;
unsigned long prevTime = 0;
const unsigned long interval = 500;

void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop() {
  unsigned long curTime = millis();
  if(curTime - prevTime >= interval){
    digitalWrite(ledPin, !digitalRead(ledPin));
    prevTime = curTime;
  }
}