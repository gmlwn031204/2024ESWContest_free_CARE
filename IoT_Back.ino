// 후방 레이저 항상 ON
const int LASER_PIN = 9;  

// 초음파 센서 핀
const int TRIG1 = 3;
const int ECHO1 = 6;
const int TRIG2 = 4;
const int ECHO2 = 7;
const int TRIG3 = 2;
const int ECHO3 = 5;

// 핀 배열 정의
const int TRIG_PINS[] = {TRIG1, TRIG2, TRIG3};
const int ECHO_PINS[] = {ECHO1, ECHO2, ECHO3};

// 볼내장 충격센서 핀
const int CRASH_SENSOR_PIN = 8;

// 거리 임계값
const int WARNING_DISTANCE = 150;  // 150cm (1.5m)
const int DANGER_DISTANCE = 50;   // 50cm (0.5m)
const unsigned long MIN_DETECT_TIME = 500;  // 0.5초

unsigned long lastSendTime = 0;
const unsigned long SEND_INTERVAL = 1000;

bool crashDetected = false;
unsigned long objectDetectedTime[3] = {0, 0, 0};

// 이전 측정값 저장
int prevDistances[3] = {0, 0, 0};

void setup() {
  Serial.begin(9600);
  
  // 레이저 핀 설정
  pinMode(LASER_PIN, OUTPUT);
  digitalWrite(LASER_PIN, HIGH);

  // 초음파 센서 핀 설정
  for (int i = 0; i < 3; i++) {
    pinMode(TRIG_PINS[i], OUTPUT);
    pinMode(ECHO_PINS[i], INPUT);
  }

  // 충돌 센서 핀 설정 - 풀업 저항 사용
  pinMode(CRASH_SENSOR_PIN, INPUT_PULLUP);
  
  Serial.println("=== 센서 초기화 완료 ===");
  Serial.println("왼쪽거리, 중앙거리, 오른쪽거리, 충돌감지, 위험레벨");
}

int measureDistance(int trigPin, int echoPin, int sensorIndex) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // 타임아웃 40ms로 연장 (약 6.8m 거리까지 측정 가능)
  long duration = pulseIn(echoPin, HIGH, 40000);
  int distance = duration * 0.034 / 2;
  
  // 유효한 거리값 확인 (0cm 초과 400cm 미만)
  if(distance > 0 && distance < 400) {
    // 필터링 제거 (급격한 변화 허용)
    prevDistances[sensorIndex] = distance;
    return distance;
  }
  
  // 측정 실패 시 이전 값 반환
  return prevDistances[sensorIndex];
}

void loop() {
  unsigned long now = millis();
  
  // 충돌 감지 상태 확인 (풀업 저항 사용으로 LOW가 충돌 상태)
  crashDetected = digitalRead(CRASH_SENSOR_PIN) == LOW;

  int dangerLevel = 0; // 0: 안전, 1: 경고, 2: 위험
  int distances[3];  // 각 센서의 거리값 저장

  // 각 초음파 센서 확인
  for (int i = 0; i < 3; i++) {
    distances[i] = measureDistance(TRIG_PINS[i], ECHO_PINS[i], i);

    if (distances[i] <= WARNING_DISTANCE) {
      if (objectDetectedTime[i] == 0) {
        objectDetectedTime[i] = now;
      }

      if ((now - objectDetectedTime[i]) >= MIN_DETECT_TIME) {
        if (distances[i] <= DANGER_DISTANCE) {
          dangerLevel = max(dangerLevel, 2);
        } else {
          dangerLevel = max(dangerLevel, 1);
        }
      }
    } else {
      objectDetectedTime[i] = 0;
    }
  }

  // 일정 주기로 데이터 전송
  if (now - lastSendTime >= SEND_INTERVAL) {
    Serial.print("거리(cm) - L: ");
    Serial.print(distances[0]); 
    Serial.print(", C: ");
    Serial.print(distances[1]); 
    Serial.print(", R: ");
    Serial.print(distances[2]);
    Serial.print(", Crash: ");
    Serial.print(crashDetected ? "X" : "O");
    Serial.print(", dangerLevel: ");
    switch(dangerLevel) {
      case 0:
        Serial.println("0");
        break;
      case 1:
        Serial.println("1");
        break;
      case 2:
        Serial.println("2");
        break;
    }
    
    Serial.print("S:"); Serial.print(dangerLevel);
    Serial.print(",C:"); Serial.println(crashDetected ? 1 : 0);
    
    lastSendTime = now;
  }
}
