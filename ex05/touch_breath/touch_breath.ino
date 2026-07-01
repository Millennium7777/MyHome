/*
 * 多档位触摸调速呼吸灯
 * 基础：实验3 (PWM呼吸灯) + 实验4 (触摸引脚)
 * 功能：LED持续呼吸，每次触摸引脚循环切换速度档位 (1慢->2中->3快->1慢...)
 * 串口输出：实时显示 Touch Value 和当前档位
 */

// ========== 引脚定义 ==========
#define TOUCH_PIN 4      // 触摸引脚 T0 对应 GPIO4
#define LED_PIN 2        // LED引脚 (ESP32 DevKit 板载LED)

// ========== PWM设置 ==========
const int freq = 5000;       // PWM频率 5000Hz
const int resolution = 8;    // 分辨率 8位 (0-255)

// ========== 档位配置 ==========
// 3个档位的延时时间(ms)，值越小呼吸越快
// 第1档：慢速呼吸   第2档：中速呼吸   第3档：急促呼吸
const int delayLevel[3] = {30, 10, 3};  
int currentLevel = 0;        // 当前档位索引 0,1,2

// ========== 触摸检测变量 ==========
int threshold = 60;              // 触摸阈值，需根据实际环境调整
bool lastTouchState = false;     // 上次触摸状态（用于上升沿检测）
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 300; // 消抖时间 300ms

// ========== 串口输出控制 ==========
unsigned long lastPrintTime = 0;
const unsigned long printInterval = 200; // 每200ms输出一次，避免刷屏

void setup() {
  Serial.begin(115200);
  delay(1000); // 等待串口稳定

  // 【新版API】绑定PWM到引脚
  ledcAttach(LED_PIN, freq, resolution);
  
  Serial.println("=== 多档位触摸调速呼吸灯 启动 ===");
  Serial.println("触摸引脚切换档位: 1(慢)->2(中)->3(快)->1...");
  Serial.println("串口输出格式: [Touch Value: xx] [档位: x] [延时: xxms]");
  Serial.println("----------------------------------------");
}

void loop() {
  // ---- 1. 读取触摸值 ----
  int touchValue = touchRead(TOUCH_PIN);
  
  // ---- 2. 检测触摸并切换档位 ----
  bool triggered = handleTouch(touchValue);

  // ---- 3. 串口输出触摸值和档位 ----
  if (millis() - lastPrintTime > printInterval) {
    Serial.print("Touch Value: ");
    Serial.print(touchValue);
    Serial.print("  |  档位: ");
    Serial.print(currentLevel + 1);
    Serial.print("  |  延时: ");
    Serial.print(delayLevel[currentLevel]);
    Serial.println("ms");
    lastPrintTime = millis();
  }

  // ---- 4. 获取当前档位的延时时间 ----
  int currentDelay = delayLevel[currentLevel];

  // ---- 5. 呼吸灯：逐渐变亮 ----
  for (int dutyCycle = 0; dutyCycle <= 255; dutyCycle++) {
    ledcWrite(LED_PIN, dutyCycle);
    delay(currentDelay);
    
    // 呼吸过程中也检测触摸，实现即时响应
    if (handleTouch(touchRead(TOUCH_PIN))) break;
  }

  // ---- 6. 呼吸灯：逐渐变暗 ----
  for (int dutyCycle = 255; dutyCycle >= 0; dutyCycle--) {
    ledcWrite(LED_PIN, dutyCycle);
    delay(currentDelay);
    
    // 呼吸过程中也检测触摸，实现即时响应
    if (handleTouch(touchRead(TOUCH_PIN))) break;
  }
}

// 触摸处理函数：检测上升沿、消抖、切换档位
// 返回 true 表示本次调用触发了档位切换
bool handleTouch(int touchValue) {
  bool currentTouchState = (touchValue < threshold);
  bool triggered = false;
  
  // 【自锁开关逻辑】检测上升沿：只在"按下瞬间"触发一次
  if (currentTouchState && !lastTouchState) {
    // 消抖：距离上次触发超过300ms才视为有效
    if (millis() - lastDebounceTime > debounceDelay) {
      currentLevel = (currentLevel + 1) % 3;  // 循环切换 0->1->2->0
      lastDebounceTime = millis();
      triggered = true;
      
      // 档位切换时额外输出提示
      Serial.println("");
      Serial.print(">>> 【触摸触发】档位切换为: ");
      Serial.print(currentLevel + 1);
      Serial.print("  呼吸延时: ");
      Serial.print(delayLevel[currentLevel]);
      Serial.println("ms");
      Serial.println("----------------------------------------");
    }
  }
  
  lastTouchState = currentTouchState;
  return triggered;
}