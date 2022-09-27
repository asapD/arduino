/*
  블루투스 통신을 위한 라이브러리 및 서보 모터 제어 라이브러리
*/
#include <SoftwareSerial.h>
#include <Servo.h>

/*
  QR 코드 리더기 라이브러리
*/
#include <usbhid.h>
#include <usbhub.h>
#include <hiduniversal.h>
#include <hidboot.h>
#include <SPI.h>

 
 /*
  블루투스 모듈 전용 핀 정의 및 서보 모터 변수 정의
 */
#define rxPin 8
#define txPin 7
SoftwareSerial bluetooth(rxPin, txPin);
Servo servo;

/*
  서보 모터 제어용 변수 및 블루투스 응답 데이터용 변수 정의
*/
int angle = 0;
String val = "";
 
 /*
  QR 코드 리더기 예제 코드
 */
class MyParser : public HIDReportParser {
  public:
    MyParser();
    void Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf);
  protected:
    uint8_t KeyToAscii(bool upper, uint8_t mod, uint8_t key);
    virtual void OnKeyScanned(bool upper, uint8_t mod, uint8_t key);
    virtual void OnScanFinished();
};

MyParser::MyParser() {}

void MyParser::Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf) {
  // If error or empty, return
  if (buf[2] == 1 || buf[2] == 0) return;

  for (uint8_t i = 7; i >= 2; i--) {
    // If empty, skip
    if (buf[i] == 0) continue;

    // If enter signal emitted, scan finished
    if (buf[i] == UHS_HID_BOOT_KEY_ENTER) {
      OnScanFinished();
    }

    // If not, continue normally
    else {
      // If bit position not in 2, it's uppercase words
      OnKeyScanned(i > 2, buf, buf[i]);
    }

    return;
  }
}

uint8_t MyParser::KeyToAscii(bool upper, uint8_t mod, uint8_t key) {
  // Letters
  if (VALUE_WITHIN(key, 0x04, 0x1d)) {
    if (upper) return (key - 4 + 'A');
    else return (key - 4 + 'a');
  }

  // Numbers
  else if (VALUE_WITHIN(key, 0x1e, 0x27)) {
    return ((key == UHS_HID_BOOT_KEY_ZERO) ? '0' : key - 0x1e + '1');
  }

  return 0;
}

void MyParser::OnKeyScanned(bool upper, uint8_t mod, uint8_t key) {
  uint8_t ascii = KeyToAscii(upper, mod, key);
  Serial.print((char)ascii);
}

void MyParser::OnScanFinished() {
  Serial.println(" - Finished");
}

USB          Usb;
USBHub       Hub(&Usb);
HIDUniversal Hid(&Usb);
MyParser     Parser;


 /*
  아두이노 초기 세팅
 */
void setup(){
  Serial.begin(9600);
  bluetooth.begin(9600); 

  servo.attach(9); // 서보 모터 9번 핀
  servo.write(10);  // 초기값 10도

  /* QR 코드 */
  if (Usb.Init() == -1) {
    Serial.println("OSC did not start.");
  }

  delay( 200 );

  Hid.SetReportParser(0, &Parser);
}
 
 /*
  핵심 로직
 */
void loop(){

  if(bluetooth.available()) {
    /*
      QR 코드 모듈로부터 데이터 입력 받기
    */
     Usb.Task();

    /*
      블루투스 통신으로 안드로이드에게 데이터 전송
    */
    bluetooth.println(); // 전송 함수 인자로 데이터 넘기면 됨.
  }

  while(bluetooth.available()) { // 불루투스 연결 성공 시,
    char c = (char)bluetooth.read(); // 안드로이드로부터 데이터 수신
    val += c; 
    delay(5);
  }

  if(val.equals("open")) { // 인증 성공 시
    
    /*
      문 열기
    */
    for(angle = 10; angle < 150; angle++) { // 서보모터로 150도까지 헤드 움직이기 
      servo.write(angle); 
      delay(20); 
    }

    delay(1000);
    
    Serial.println("response value : " + val);
    val = "";
    delay(5000);

    /*
      문 닫기
    */
    for(angle = 150; angle > 10; angle--) {  // 서보모터로 10도까지 헤드 움직이기
      servo.write(angle); 
      delay(20); 
    }
  } else if(val.equals("close")) { // 인증 실패 시

    for(angle = 150; angle > 10; angle--) { 
      servo.write(angle); 
      delay(20); 
    }

    delay(1000);
    
    Serial.println("response value : " + val);
    val = "";
    delay(100);
  } else {
    val = "";
  }

  delay(100);
}
