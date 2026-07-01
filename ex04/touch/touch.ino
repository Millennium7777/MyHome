#define TOUCH_PIN 4
#define LED_PIN 2
#define EXT_ISR_MODE 0

int threshold = 60;
int touchValue;

// 题目要求布尔状态变量：保存LED当前亮灭
bool ledState = false;
// 记录上一轮触摸状态，用于边缘检测
bool lastTouchFlag = false;
// 防抖计时变量
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}

void loop() {
  touchValue = touchRead(TOUCH_PIN);
  Serial.print("Touch Value: ");
  Serial.println(touchValue);

  // 当前是否触摸
  bool currentTouchFlag = (touchValue < threshold);
  unsigned long currentTime = millis();

  // 软件防抖：间隔50ms才判定一次触摸状态
  if (currentTime - lastDebounceTime > debounceDelay)
  {
    // 边缘检测：上次没触摸，本次刚触摸（上升沿瞬间）
    if (currentTouchFlag == true && lastTouchFlag == false)
    {
      // 翻转LED自锁状态
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState);
    }
    lastDebounceTime = currentTime;
  }

  // 更新上一次触摸标记
  lastTouchFlag = currentTouchFlag;
  delay(100);
}