#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "RedmiK70Pro";
const char* password = "12345678";
const int LED_PIN = 2;

// PWM 配置（与参考代码一致）
const int freq = 5000;          // PWM频率 5000Hz
const int resolution = 8;       // 8位分辨率，亮度范围 0-255

WebServer server(80);

String makePage() {
  // 读取当前PWM占空比，同步到网页滑动条
  int currentBrightness = ledcRead(LED_PIN);
  String state = currentBrightness > 0 ? "ON" : "OFF";
  
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>实验2</title>
</head>
<body style="font-family:Arial; text-align:center; margin-top:50px;">
  <h1>第二部分：按钮+滑动条控制 LED</h1>
  <p>当前状态：<b>)rawliteral" + state + R"rawliteral(</b></p>
  
  <a href="/on"><button style="padding:10px 20px; margin:5px;">点亮 LED</button></a>
  <a href="/off"><button style="padding:10px 20px; margin:5px;">熄灭 LED</button></a>

  <!-- 亮度滑动条 -->
  <div style="margin-top:30px;">
    <label for="brightnessRange">LED亮度调节：</label>
    <br><br>
    <input type="range" id="brightnessRange" min="0" max="255" 
           value=")rawliteral" + String(currentBrightness) + R"rawliteral(" 
           style="width: 300px;">
    <span id="brightnessText">)rawliteral" + String(currentBrightness) + R"rawliteral(</span>
  </div>

  <script>
    const range = document.getElementById('brightnessRange');
    const text = document.getElementById('brightnessText');

    // 监听滑动条拖动，实时发送亮度值
    range.addEventListener('input', function() {
      const val = this.value;
      text.textContent = val;
      // 发送GET请求到ESP32
      fetch(`/brightness?value=${val}`)
        .catch(err => console.log('请求失败:', err));
    });
  </script>
</body>
</html>
)rawliteral";
  return html;
}

void handleRoot() {
  server.send(200, "text/html; charset=UTF-8", makePage());
}

void handleOn() {
  ledcWrite(LED_PIN, 255);  // 最高亮度
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleOff() {
  ledcWrite(LED_PIN, 0);    // 熄灭
  server.sendHeader("Location", "/");
  server.send(303);
}

// 处理滑动条亮度调节请求
void handleBrightness() {
  if (server.hasArg("value")) {
    int brightness = server.arg("value").toInt();
    brightness = constrain(brightness, 0, 255); // 限制数值范围
    ledcWrite(LED_PIN, brightness);             // 设置PWM亮度
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "参数错误");
  }
}

void setup() {
  Serial.begin(115200);
  
  // 新版PWM初始化：直接绑定引脚、频率、分辨率，自动分配通道
  ledcAttach(LED_PIN, freq, resolution);
  ledcWrite(LED_PIN, 0); // 初始状态熄灭
  
  WiFi.begin(ssid, password);
  Serial.print("连接WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n连接成功");
  Serial.print("访问地址: http://");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff);
  server.on("/brightness", handleBrightness);
  server.begin();
}

void loop() {
  server.handleClient();
}