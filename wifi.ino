#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Servo.h>

const char* ssid = "wifi";
const char* password = "password";

const char* serverName = "http://localhost:8080/api/orders/verify-serial";

unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

const int motorPinNum = 7;
Servo motor;



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("Connecting");

  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Timer set to 5 sec ");

  motor.attach(motorPinNum);
  delay(100);
  motor.write(0);
  delay(100);

}

void loop() {
  // put your main code here, to run repeatedly:

  if((millis() - lastTime) > timerDelay) {
    if(WiFi.status() == WL_CONNECTED) {
      WiFiClient client;
      HTTPClient http;

      http.begin(client, serverName);

      // QR 스캔

      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.POST("{\"serialNum\":\"asapD2022090421\",\"orderId\":\"2\"}");
      
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);

      // 응답 후처리
      if(httpResponseCode > 0) {
        if(httpResponseCode == 200) {
          motor.write(180);
          delay(3000);
          motor.write(0);
        } else {
          Serial.println("Verify serialNum Fail !!!!!!!");
        }
      }

      http.end();
    } else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}
