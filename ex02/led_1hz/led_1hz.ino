const int ledPin = 2;
unsigned long prevTime = 0;
const unsigned long interval = 500;

void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop() {
  unsigned long curTime = millis();
  // 计时到达间隔，翻转LED状态
  if (curTime - prevTime >= interval) {
    digitalWrite(ledPin, !digitalRead(ledPin));
    prevTime = curTime;
  }
}