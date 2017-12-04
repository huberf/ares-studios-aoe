#define BLYNK_PRINT Serial
#include <BlynkSimpleCurieBLE.h>
#include <CurieBLE.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

String botName = "Ares9";

// Setup time section for polling connection
#include <Time.h>
#include <TimeLib.h>
int lastPoll = now();

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "your_auth_code";

BLEPeripheral  blePeripheral;

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 

// Select which 'port' M1, M2, M3 or M4.
Adafruit_DCMotor *motor1 = AFMS.getMotor(1);
Adafruit_DCMotor *motor2 = AFMS.getMotor(2);
Adafruit_DCMotor *motor3 = AFMS.getMotor(3);
Adafruit_DCMotor *motor4 = AFMS.getMotor(4);

int timeSinceHit = 1000;
int timeSinceDestroyed = 0;
int hitPin = 9;

int botMotionState = 0;
int sensorCount = 2;
int damage[3][3] = {
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0}
};
// Sensor system, {pinNumber, average, damageCoordinateX, damageCoordinateY, activation, cursor}
// Sensor 0 is the front panel
// Sensor 1 is the top panel
int sensors[][6] = {
  {0, 1000, 1, 1, -30, 0},
  {1, 1000, 1, 0, 40, 0}
};

//######### SETUP ######################################
void setup() {
  Serial.begin(9600);
  delay(1000);

  // The name your bluetooth service will show up as, customize this if you have multiple devices
  //blePeripheral.setLocalName(botName);
  //blePeripheral.setDeviceName(botName);
  //blePeripheral.setAppearance(384);

  Blynk.begin(auth, blePeripheral);

  blePeripheral.begin();
  Serial.println("Waiting for connections...");
    
  Serial.println("Adafruit Motorshield v2 - DC Motor");

  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz
  
  // Set the speed to start, from 0 (off) to 255 (max speed)
  motor1->setSpeed(255);
  motor2->setSpeed(255);
  motor3->setSpeed(255);
  motor4->setSpeed(255);  

  pinMode(hitPin, OUTPUT);
  testAmbient();
}

// Actuation Functions
void applyBrakes() {
  motor1->run(RELEASE);
  motor2->run(RELEASE);
  motor3->run(RELEASE);
  motor4->run(RELEASE);
}

void moveForward() {
  motor1->run(FORWARD);
  motor2->run(BACKWARD);
  motor3->run(FORWARD);
  motor4->run(BACKWARD);
}

void moveBackward() {
  motor1->run(BACKWARD);
  motor2->run(FORWARD);
  motor3->run(BACKWARD);
  motor4->run(FORWARD);
}

void turnRight() {
  motor1->run(FORWARD);
  motor2->run(FORWARD);
  motor3->run(FORWARD);
  motor4->run(FORWARD);
}

void turnLeft() {
  motor1->run(BACKWARD);
  motor2->run(BACKWARD);
  motor3->run(BACKWARD);
  motor4->run(BACKWARD);
}

// Safety Checks
bool isForwardSafe() {
  // TODO: Add support for reading ultrasonic sensor
  return true;
}

int lastNow = millis();
int timer = 0;
//########## LOOP ######################################
void loop() {
  Serial.println(millis() - lastNow);
  lastNow = millis();
  if (timeSinceHit < 10) {
    glowHit();
  } else {
    closeHit();
  }
  if (botMotionState == 0) {
    applyBrakes();
  } else if (botMotionState == 1) {
    moveForward();
  } else if (botMotionState == 2) {
    moveBackward();
  } else if (botMotionState == 3) {
    turnRight();
  } else if (botMotionState == 4) {
    turnLeft();
  } else {
    // Unknown movement operation
    bool doNothing = true;
  }
  if (isDestroyed()) {
    timeSinceDestroyed += 1;
    applyBrakes();
    showDestroyed();
  }
  if (timeSinceDestroyed > 10) {
    repair();
    timeSinceDestroyed = 0;
  }
  // Switch every 1000 millisecods
  if (timer % 500) {
    if (botMotionState == 1) {
      botMotionState = 2;
    } else {
      botMotionState = 1;
    }
  }
  // Read in sensors for damage
  handleSensors();
  timer += 1;
  delay(5);
}


//######### Subroutines ################################

// This function will set the speed
BLYNK_WRITE(V0)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  Serial.print("V0 Slider value is: ");
  Serial.println(pinValue);
  motor1->setSpeed(pinValue);
  motor2->setSpeed(pinValue); 
  motor3->setSpeed(pinValue);
  motor4->setSpeed(pinValue);   
}

bool isDestroyed() {
  int threshold = 5;
  int dimensions = 3;
  for (int a = 0; a < dimensions; a++) {
    for (int b = 0; b < dimensions; b++) {
      if (damage[a][b] > threshold) {
        return true;
      }
    }
  }
  return false;
}

// Read the destroyed status
BLYNK_READ(V8)
{
  bool destroyed = isDestroyed();
  Blynk.virtualWrite(8, destroyed);
}

// Any activation of pin initiates repair
BLYNK_WRITE(V20) {
  repair();
}

BLYNK_WRITE(V21) {
  //abnormalThreshold = param.asInt();
}

/******************** 
 *  Sensor system   *
 *******************/
int ticks = 0;
int hasHit = false;
void handleSensors() {
  Serial.println("Reading sensors");
  for (int a = 0; a < sensorCount; a++) {
    int lightPin = sensors[a][0];
    int reading = analogRead(lightPin);
    Serial.println(reading);
    bool fancyCode = false;
    if (fancyCode) {
      if (sensorLoop(a, reading)) {
        int x = sensors[a][2];
        int y = sensors[a][3];
        damage[x][y] += 1;
      }
    } else {
      int average = sensors[a][1];
      average = average + (reading - average)/30;
      sensors[a][1] = average;
  
      int abnormalThreshold = sensors[a][4];
  
      int activation = average + abnormalThreshold;
      Serial.println(activation);
      //Serial.println(reading);
      
      if ((reading < activation && abnormalThreshold < 0) || (reading > activation && abnormalThreshold > 0) && timeSinceHit > 15) {
        hasHit = true;
        Serial.println("Hit detected");
        timeSinceHit = 0;
        //digitalWrite(ledPin, HIGH);
        //delay(500);
        //digitalWrite(ledPin, LOW);
      } else if ( reading > activation && timeSinceHit > 4) {
        if (hasHit) {
          int x = sensors[a][2];
          int y = sensors[a][3];
          damage[x][y] += 1;
        }
        hasHit = false;
      } else {
        timeSinceHit += 1;
      }
    }
  }
  ticks += 1;
}
//DATA_COLLECTION_SIZE should be 5/3 times the expected duration of the laser
const int DATA_COLLECTION_SIZE = 20;
int THRESHOLD = 5;

int sensorData[2][DATA_COLLECTION_SIZE];
int rewriteCursor[2] = {0, 0};

bool sensorLoop(int a, int reading){
  int THRESHOLD = sensors[a][4];
  if (sensors[a][5] < DATA_COLLECTION_SIZE){
      sensorData[a][sensors[a][5]] = reading;
      sensors[a][5]++;
    } else{
      shift(sensorData[a], DATA_COLLECTION_SIZE, reading);
      int backAvg = averageOnSegment(sensorData[a], 0, DATA_COLLECTION_SIZE/5);
      int midAvg = averageOnSegment(sensorData[a], 2*DATA_COLLECTION_SIZE/5, 3*DATA_COLLECTION_SIZE/5);
      int frontAvg = averageOnSegment(sensorData[a], 4*DATA_COLLECTION_SIZE/5, DATA_COLLECTION_SIZE/5);
      int fluctions = 1;

      int activation = midAvg - (frontAvg + backAvg)/2;
      activation /= fluctions;

      if (activation > THRESHOLD){
        Serial.println("Hit detected");
        sensors[a][5] = 0;
        return true;
      } else {
        return false;
      }
    }
}

void shift (int dataSet[], int listSize, int newData){
  int i = 0;
  while (i < listSize - 1){
    dataSet[i] = dataSet[i+1];
    i = i + 1;
  }
  dataSet[i] = newData;
}

int averageOnSegment(int dataSet[], int init, int fin){
  int average = 0;
  for (int i = init; i < fin; i++){
    average += dataSet[i];
  }
  average /= (fin - init);
  return average;
}
/* End of sensor section */

void glowHit() {
  digitalWrite(hitPin, HIGH);
}

void closeHit() {
  digitalWrite(hitPin, LOW);
}

void showDestroyed() {
  digitalWrite(hitPin, HIGH);
  delay(100);
  digitalWrite(hitPin, LOW);
}

void repair() {
  int dimensions = 3;
  for (int i = 0; i < dimensions; i++) {
    for (int j = 0; j < dimensions; j++) {
      damage[i][j] = 0;
    }
  }
  testAmbient();
}

void testAmbient() {
  for (int a = 0; a < sensorCount; a++) {
    int average = 0;
    for (int i = 0; i < 20; i++) {
      average += analogRead(sensors[a][0]);
    }
    average = average/20;
    sensors[a][1] = average;
  }
}
