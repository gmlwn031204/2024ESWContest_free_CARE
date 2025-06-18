#include <Wire.h>
#include <MPU6050_light.h>
#include <SoftwareSerial.h>

MPU6050 mpu(Wire);

// 블루투스 모듈 핀 설정
const int BT_RX = 11;  // HC-06의 TX와 연결
const int BT_TX = 12;  // HC-06의 RX와 연결
SoftwareSerial bluetooth(BT_RX, BT_TX);

// 핀 설정
const int SPEAKER_PIN = 6;
const int LASER1_PIN = 9;
const int LASER2_PIN = 10;
const int CRASH_SENSOR_FRONT_1 = 2;
const int CRASH_SENSOR_FRONT_2 = 3;

// 후방 수신 데이터
bool rearCrash = false;
int dangerLevel = 0;

// 레이저2 깜빡임 제어
unsigned long previousMillis = 0;
const long blinkInterval = 500;
bool laser2State = false;

// 충격 기록용
unsigned long crash1LastTime = 0;
unsigned long crash2LastTime = 0;
const unsigned long crashDuration = 300; // ms
bool crash1Alarmed = false;
bool crash2Alarmed = false;

// 기울기 알람 중복 방지
bool prevTiltDetected = false;

void setup() {
  Serial.begin(9600);      // 디버깅용 시리얼
  bluetooth.begin(9600);   // 블루투스 통신 초기화
  Wire.begin();

  byte status = mpu.begin();
  mpu.calcOffsets();

  pinMode(SPEAKER_PIN, OUTPUT);
  pinMode(LASER1_PIN, OUTPUT);
  pinMode(LASER2_PIN, OUTPUT);

  // 내부 풀업 저항 활성화
  pinMode(CRASH_SENSOR_FRONT_1, INPUT_PULLUP);
  pinMode(CRASH_SENSOR_FRONT_2, INPUT_PULLUP);

  // 초기 테스트용: 스피커 소리 확인
  tone(SPEAKER_PIN, 1000, 300);
  delay(500);
}

void playAlarm() {
  for (int i = 0; i < 4; i++) {
    tone(SPEAKER_PIN, 1200);  // 짧고 날카로운 삐
    delay(150);
    noTone(SPEAKER_PIN);
    delay(100);
  }
}

void sendCrashData(String sensorId) {
  // JSON 형식으로 데이터 전송
  String jsonData = "{\"crash\":\"" + sensorId + "\"}";
  bluetooth.println(jsonData);
  Serial.println("블루투스 전송: " + jsonData);  // 디버깅용
}

void updateLasers() {
  unsigned long currentMillis = millis();

  switch (dangerLevel) {
    case 0:  // 안전
      digitalWrite(LASER1_PIN, HIGH);
      digitalWrite(LASER2_PIN, LOW);
      break;

    case 1:  // 경고
      digitalWrite(LASER1_PIN, LOW);
      digitalWrite(LASER2_PIN, HIGH);
      break;

    case 2:  // 위험
      digitalWrite(LASER1_PIN, LOW);

      if (currentMillis - previousMillis >= blinkInterval) {
        previousMillis = currentMillis;
        laser2State = !laser2State;
      }

      digitalWrite(LASER2_PIN, laser2State);
      break;
  }
}

void loop() {
  mpu.update();

  float ax = mpu.getAccX();
  float ay = mpu.getAccY();
  bool tiltDetected = abs(ax) > 1.5 || abs(ay) > 1.5;

  unsigned long now = millis();

  // 전방 충돌 감지 (풀업 사용: 충격 시 LOW)
  bool crash1 = digitalRead(CRASH_SENSOR_FRONT_1) == LOW;
  bool crash2 = digitalRead(CRASH_SENSOR_FRONT_2) == LOW;

  if (crash1) {
    Serial.println("센서 1 충격 감지됨");
    sendCrashData("FRONT1");  // 블루투스로 전송
    crash1LastTime = now;
  }
  if (crash2) {
    Serial.println("센서 2 충격 감지됨");
    sendCrashData("FRONT2");  // 블루투스로 전송
    crash2LastTime = now;
  }

  // 센서1 충격 발생 후 crashDuration 이내면 알람
  if (now - crash1LastTime < crashDuration && !crash1Alarmed) {
    playAlarm();
    crash1Alarmed = true;
  }
  if (now - crash1LastTime >= crashDuration) {
    crash1Alarmed = false;
  }

  // 센서2 충격 발생 후 crashDuration 이내면 알람
  if (now - crash2LastTime < crashDuration && !crash2Alarmed) {
    playAlarm();
    crash2Alarmed = true;
  }
  if (now - crash2LastTime >= crashDuration) {
    crash2Alarmed = false;
  }

  // 기울기 감지 알람
  if (tiltDetected && !prevTiltDetected) {
    Serial.println("기울기 감지됨");
    sendCrashData("TILT");  // 블루투스로 전송
    playAlarm();
  }
  prevTiltDetected = tiltDetected;

  // 후방 데이터 수신 (블루투스)
  if (bluetooth.available()) {
    String input = bluetooth.readStringUntil('\n');
    Serial.println("블루투스 수신: " + input);

    int sIndex = input.indexOf("S:");
    int cIndex = input.indexOf(",C:");

    if (sIndex >= 0 && cIndex > sIndex) {
      dangerLevel = input.substring(sIndex + 2, cIndex).toInt();
      rearCrash = input.substring(cIndex + 3).toInt() == 1;
    }
  }

  // 후방 충격 알람
  if (rearCrash) {
    Serial.println("후방 충격 감지됨");
    sendCrashData("REAR");  // 블루투스로 전송
    playAlarm();
    rearCrash = false;
  }

  // 레이저 상태 갱신
  updateLasers();
}
