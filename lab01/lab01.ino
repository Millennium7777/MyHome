#define LED_PIN 2

void setup() {
  // put your setup code here, to run once:
    Serial.begin(115200);
    pinMode(LED_PIN,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
    Serial.println("Hello ESP32!");
    digitalWrite(LED_PIN, HIGH);
    delay(1000);
    digitalWrite(LED_PIN,LOW);
    delay(1000);
}
