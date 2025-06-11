#include <Wire.h>
#include <MPU6050_light.h>

MPU6050 mpu(Wire);

// 핀 설정
const int SPEAKER_PIN = 6;
const int LASER1_PIN = 9;
const int LASER2_PIN = 10;
const int CRASH_SENSOR_FRONT_1 = 2;
const int CRASH_SENSOR_FRONT_2 = 3;
const int CRASH_SENSOR_REAR = 8;

// 후방 수신 데이터
int dangerLevel = 0;

// 레이저2 깜빡임 제어
unsigned long previousMillis = 0;
const long blinkInterval = 500;
bool laser2State = false;

// 충격 및 기울기 감지 임계값 설정
const unsigned long DEBOUNCE_TIME = 50;  // 디바운스 시간 (ms)
const float TILT_THRESHOLD = 1.5;  // 기울기 감지 임계값 (g)

// 충격 기록용
unsigned long crash1LastTime = 0;
unsigned long crash2LastTime = 0;
unsigned long crashRearLastTime = 0;
const unsigned long crashDuration = 300; // ms
bool crash1Alarmed = false;
bool crash2Alarmed = false;
bool crashRearAlarmed = false;

// 기울기 알람 관련
bool prevTiltDetected = false;
unsigned long lastTiltAlarmTime = 0;
const unsigned long TILT_ALARM_COOLDOWN = 1000; // 기울기 알람 재발생 대기 시간 (ms)

// 센서 상태 저장
int prevSensor1State = HIGH;
int prevSensor2State = HIGH;
int prevRearSensorState = HIGH;
int currentRearSensorState = HIGH;

// 디바운스를 위한 마지막 상태 변경 시간
unsigned long lastDebounceTime1 = 0;
unsigned long lastDebounceTime2 = 0;
unsigned long lastDebounceTimeRear = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  byte status = mpu.begin();
  Serial.println(F("MPU6050 상태: "));
  Serial.println(status);
  
  while(status != 0) { } // MPU6050이 준비될 때까지 대기
  
  Serial.println(F("MPU6050 보정 중..."));
  mpu.calcOffsets(); // 자이로/가속도 오프셋 보정
  Serial.println(F("MPU6050 준비 완료!"));

  pinMode(SPEAKER_PIN, OUTPUT);
  pinMode(LASER1_PIN, OUTPUT);
  pinMode(LASER2_PIN, OUTPUT);

  // 내부 풀업 저항 활성화
  pinMode(CRASH_SENSOR_FRONT_1, INPUT_PULLUP);
  pinMode(CRASH_SENSOR_FRONT_2, INPUT_PULLUP);
  pinMode(CRASH_SENSOR_REAR, INPUT_PULLUP);

  // 초기 상태 읽기
  currentRearSensorState = digitalRead(CRASH_SENSOR_REAR);

  // 초기 테스트용: 스피커 소리 확인
  tone(SPEAKER_PIN, 1000, 300);
  delay(500);
}

void playAlarm() {
  tone(SPEAKER_PIN, 523, 200);  // 도
  delay(250);
  tone(SPEAKER_PIN, 587, 200);  // 레
  delay(250);
  tone(SPEAKER_PIN, 659, 200);  // 미
  delay(250);
  noTone(SPEAKER_PIN);
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

// 전방 충격 감지 함수
bool checkFrontImpact(int sensorPin, int &prevState, unsigned long &lastDebounceTime) {
  int currentReading = digitalRead(sensorPin);
  bool impactDetected = false;

  if (currentReading != prevState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > DEBOUNCE_TIME) {
    if (currentReading == LOW && prevState == HIGH) {
      impactDetected = true;
    }
    prevState = currentReading;
  }

  return impactDetected;
}

// 후방 충격 감지 함수 (수정됨)
bool checkRearImpact() {
  int reading = digitalRead(CRASH_SENSOR_REAR);
  bool impactDetected = false;
  
  // 상태가 변경되었을 때만 체크
  if (reading != currentRearSensorState) {
    if (reading == LOW) { // 충격이 감지됨
      impactDetected = true;
    }
    currentRearSensorState = reading;
  }
  
  return impactDetected;
}

// 기울기 감지 함수
bool checkTilt() {
  float ax = mpu.getAccX();
  float ay = mpu.getAccY();
  return (abs(ax) > TILT_THRESHOLD || abs(ay) > TILT_THRESHOLD);
}

void loop() {
  mpu.update();
  unsigned long now = millis();

  // 전방부 기울기 감지 (MPU6050)
  bool currentTiltDetected = checkTilt();
  if (currentTiltDetected && !prevTiltDetected && 
      (now - lastTiltAlarmTime > TILT_ALARM_COOLDOWN)) {
    Serial.println("전방부 기울기 감지됨");
    playAlarm();
    lastTiltAlarmTime = now;
  }
  prevTiltDetected = currentTiltDetected;

  // 전방 볼내장 센서1 충격 감지
  if (checkFrontImpact(CRASH_SENSOR_FRONT_1, prevSensor1State, lastDebounceTime1)) {
    Serial.println("전방 볼내장 센서 1 충격 감지됨");
    crash1LastTime = now;
    if (!crash1Alarmed) {
      playAlarm();
      crash1Alarmed = true;
    }
  }

  // 전방 볼내장 센서2 충격 감지
  if (checkFrontImpact(CRASH_SENSOR_FRONT_2, prevSensor2State, lastDebounceTime2)) {
    Serial.println("전방 볼내장 센서 2 충격 감지됨");
    crash2LastTime = now;
    if (!crash2Alarmed) {
      playAlarm();
      crash2Alarmed = true;
    }
  }

  // 후방 볼내장 센서 충격 감지 (수정된 부분)
  if (checkRearImpact()) {
    Serial.println("후방 볼내장 센서 충격 감지됨");
    crashRearLastTime = now;
    if (!crashRearAlarmed) {
      playAlarm();
      crashRearAlarmed = true;
    }
  }

  // 알람 상태 리셋
  if (now - crash1LastTime >= crashDuration) {
    crash1Alarmed = false;
  }
  if (now - crash2LastTime >= crashDuration) {
    crash2Alarmed = false;
  }
  if (now - crashRearLastTime >= crashDuration) {
    crashRearAlarmed = false;
  }

  // 후방 데이터 수신 (위험 레벨)
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    Serial.println("수신된 데이터: " + input);
