#define LED_YELLOW  5
#define LED_RED     2

void setup() {
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);
}

void loop() {
  // 黄灯渐亮，红灯同步渐暗
  for(int brightness = 0; brightness <= 255; brightness++){
    analogWrite(LED_YELLOW, brightness);
    analogWrite(LED_RED, 255 - brightness);
    delay(20);
  }
  // 黄灯渐暗，红灯同步渐亮
  for(int brightness = 255; brightness >= 0; brightness--){
    analogWrite(LED_YELLOW, brightness);
    analogWrite(LED_RED, 255 - brightness);
    delay(20);
  }
}
