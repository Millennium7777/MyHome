#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "RedmiK70Pro";
const char* password = "12345678";
const int TOUCH_PIN = 12;

WebServer server(80);

String makePage() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>实时传感器仪表盘</title>
  <style>
    body { font-family: Arial; text-align: center; margin-top: 100px; }
    .value-box { font-size: 100px; font-weight: bold; color: #2c3e50; margin: 30px 0; }
    .label { font-size: 24px; color: #7f8c8d; }
  </style>
</head>
<body>
  <h1>触摸传感器实时监控面板</h1>
  <p class="label">当前触摸传感器数值</p>
  <div id="sensorValue" class="value-box">--</div>
  <p class="label">手指靠近引脚时数值减小</p>

  <script>
    const valueElement = document.getElementById('sensorValue');
    
    function updateValue() {
      fetch('/sensor')
        .then(res => res.text())
        .then(val => {
          valueElement.textContent = val;
        })
        .catch(err => console.log('读取失败:', err));
    }
    
    updateValue();
    setInterval(updateValue, 200);
  </script>
</body>
</html>
)rawliteral";
  return html;
}

void handleRoot() {
  server.send(200, "text/html; charset=UTF-8", makePage());
}

void handleSensor() {
  int value = touchRead(TOUCH_PIN);
  server.send(200, "text/plain", String(value));
}

void setup() {
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.print("访问地址: http://");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/sensor", handleSensor);
  server.begin();
}

void loop() {
  server.handleClient();
}