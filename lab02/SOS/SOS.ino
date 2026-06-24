const int ledPin = 2;
unsigned long currentTime;
unsigned long previousTime = 0;
unsigned long delayTime = 0;

// 状态编号
int stage = 0;

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}

void loop() {
  currentTime = millis();
  // 未到计时时间，直接退出
  if (currentTime - previousTime < delayTime) {
    return;
  }

  switch (stage) {
    // 第一组：3短闪
    case 0: digitalWrite(ledPin, HIGH); delayTime = 150; stage = 1; break;
    case 1: digitalWrite(ledPin, LOW);  delayTime = 100; stage = 2; break;
    case 2: digitalWrite(ledPin, HIGH); delayTime = 150; stage = 3; break;
    case 3: digitalWrite(ledPin, LOW);  delayTime = 100; stage = 4; break;
    case 4: digitalWrite(ledPin, HIGH); delayTime = 150; stage = 5; break;
    case 5: digitalWrite(ledPin, LOW);  delayTime = 100; stage = 6; break;

    // 第二组：3长闪
    case 6: digitalWrite(ledPin, HIGH); delayTime = 400; stage = 7; break;
    case 7: digitalWrite(ledPin, LOW);  delayTime = 100; stage = 8; break;
    case 8: digitalWrite(ledPin, HIGH); delayTime = 400; stage = 9; break;
    case 9: digitalWrite(ledPin, LOW);  delayTime = 100; stage = 10; break;
    case 10:digitalWrite(ledPin, HIGH); delayTime = 400; stage = 11; break;
    case 11:digitalWrite(ledPin, LOW);  delayTime = 100; stage = 12; break;

    // 第三组：3短闪
    case 12:digitalWrite(ledPin, HIGH); delayTime = 150; stage = 13; break;
    case 13:digitalWrite(ledPin, LOW);  delayTime = 100; stage = 14; break;
    case 14:digitalWrite(ledPin, HIGH); delayTime = 150; stage = 15; break;
    case 15:digitalWrite(ledPin, LOW);  delayTime = 100; stage = 16; break;
    case 16:digitalWrite(ledPin, HIGH); delayTime = 150; stage = 17; break;
    case 17:digitalWrite(ledPin, LOW);  delayTime = 100; stage = 18; break;

    // 整套SOS完成，长时间熄灭停顿，回到开头循环
    case 18:digitalWrite(ledPin, LOW);  delayTime = 1200; stage = 0; break;
  }

  previousTime = currentTime;
}