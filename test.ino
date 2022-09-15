// Arduino_JSON - Version: Latest
#include <SoftwareSerial.h>
#include <Arduino_JSON.h>
#include <Servo.h>
#include <WiFi.h>

const char* ssid = "wifiAddress";
const char* password = "password";


#define motorPinNum = 8; // 서보모터 핀 번호 지정
Servo motor;

void setup() {

  Serial.begin(115200);

  motor.attach(motorPinNum); // 서보모터 핀 번호 설정
  delay(100);
  motor.write(0); // 서보모터 0도 위치로 초기화
  delay(100);
  // motor.detach();

  delay(4000);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {

    delay(1000);
    Serial.println("Connecting to WiFi ........");

  }

  Serial.println("Connected to the WiFi ");
}

void loop() {

  if ((WiFi.status() == WL_CONNECTED)) { // 와이파이 연결 확인

    HTTPClient http; // http 통신을 위한 클라이언트

    // QR 코드 스캔




    // QR 코드 스캔 데이터 전송
    http.begin("http://localhost:8080/api/orders/varify"); // 서버 요청 URL
    http.addHeader("Content-Type", "Application/Json"); // RequestBody 데이터 형식 지정
    int httpCode = http.POST(); // http Method 지정

    if (httCode > 0) { // 응답 코드가 왔다면

      String response = http.getString(); // ResponseBody 저장

      StaticJsonBuffer<500> JSonBuffer; // JSON으로 파싱하기 위한 저장 공간

      JsonObject& root = JSONBuffer.parseObject(response); // JSON으로 파싱해서 entity에 저장

      String result = root["data"];

      if (result == "OPEN") {
        // 전기신호로 모터 동작하기
        Serial.println("Success authorization ...... ");

        motor.write(180); // 문 열기
        delay(3000); // 문 여는 동안 시간 지연
        motor.write(0); // 문 닫기

      } else {

        //
        Serial.println("Reject user ........ ");
      }
      /*
        {
         "postId": 1,
          "id": 1,
         "name": "id labore ex et quam laborum",
         "email": "Eliseo@gardner.biz",
          "body": "laudantium enim quasi est quidem magnam voluptate ipsam eos\ntempora quo necessitatibus\ndolor quam autem quasi\nreiciendis et nam sapiente accusantium"
        },

      */



    } else {

      Serial.println("Error on HTTP Request.......");
      Serial.println("httpCode : " + httpCode);

    }

    http.end();

  } else {

    Serial.println("Error in WiFi connection ...... ");

  }

  delay(10000);
}
