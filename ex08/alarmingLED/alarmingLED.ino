#include <WiFi.h>
#include <WebServer.h>

// WiFi配置
const char* ssid = "RedmiK70Pro";
const char* password = "12345678";

// 硬件引脚配置
const int LED_PIN = 2;       // 板载LED引脚
const int TOUCH_PIN = 32;     // 触摸引脚 T0 对应 GPIO32
const int TOUCH_THRESHOLD = 500; // 触摸触发阈值，可根据实际环境微调

WebServer server(80);

// 系统状态变量（中断修改的变量加volatile防止编译器优化）
volatile bool systemArmed = false;   // 系统布防状态
volatile bool alarmTriggered = false; // 报警触发状态

// LED非阻塞闪烁相关变量
unsigned long lastBlinkTime = 0;
const long blinkInterval = 100;      // 报警闪烁间隔 100ms（高频狂闪）
bool ledCurrentState = false;

// 触摸中断服务函数（触发报警）
void touchISR() {
  // 仅在布防且未报警时，触发报警
  if (systemArmed && !alarmTriggered) {
    alarmTriggered = true;
  }
}

// 生成网页
String makePage() {
  String statusText;
  if (alarmTriggered) {
    statusText = "报警中";
  } else if (systemArmed) {
    statusText = "已布防";
  } else {
    statusText = "未布防";
  }
  
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>安防报警器模拟</title>
</head>
<body style="font-family:Arial; text-align:center; margin-top:50px;">
  <h1>物联网安防报警器实验</h1>
  <p>系统当前状态：<b style="font-size:22px;">)rawliteral" + statusText + R"rawliteral(</b></p>
  
  <div style="margin-top:30px;">
    <a href="/arm">
      <button style="padding:15px 30px; font-size:18px; margin:10px; 
                     background:#d9534f; color:white; border:none; border-radius:6px;">
        布防 Arm
      </button>
    </a>
    <a href="/disarm">
      <button style="padding:15px 30px; font-size:18px; margin:10px; 
                     background:#5cb85c; color:white; border:none; border-radius:6px;">
        撤防 Disarm
      </button>
    </a>
  </div>
  
  <p style="margin-top:40px; color:#666; font-size:14px;">
    布防后触碰触摸引脚，立即触发LED高频闪烁报警<br>
    报警状态不会自动解除，仅可通过网页撤防重置
  </p>
</body>
</html>
)rawliteral";
  return html;
}

// 处理主页请求
void handleRoot() {
  server.send(200, "text/html; charset=UTF-8", makePage());
}

// 处理布防请求
void handleArm() {
  systemArmed = true;
  alarmTriggered = false;
  ledCurrentState = false;
  digitalWrite(LED_PIN, LOW); // 布防时重置LED为熄灭
  server.sendHeader("Location", "/");
  server.send(303);
}

// 处理撤防请求
void handleDisarm() {
  systemArmed = false;
  alarmTriggered = false;
  ledCurrentState = false;
  digitalWrite(LED_PIN, LOW); // 撤防熄灭LED，解除报警
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);
  
  // 初始化LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  // 绑定触摸中断
  touchAttachInterrupt(TOUCH_PIN, touchISR, TOUCH_THRESHOLD);
  
  // 连接WiFi
  WiFi.begin(ssid, password);
  Serial.print("连接WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n连接成功");
  Serial.print("网页访问地址: http://");
  Serial.println(WiFi.localIP());
  
  // 注册Web路由
  server.on("/", handleRoot);
  server.on("/arm", handleArm);
  server.on("/disarm", handleDisarm);
  server.begin();
}

void loop() {
  server.handleClient(); // 处理Web客户端请求
  
  // 报警状态下，LED高频闪烁（非阻塞实现，不影响Web响应）
  if (alarmTriggered) {
    unsigned long currentTime = millis();
    if (currentTime - lastBlinkTime >= blinkInterval) {
      lastBlinkTime = currentTime;
      ledCurrentState = !ledCurrentState;
      digitalWrite(LED_PIN, ledCurrentState);
    }
  }
}