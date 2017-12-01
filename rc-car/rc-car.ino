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
// Sensor system, {pinNumber, average, damageCoordinateX, damageCoordinateY}
int sensors[][4] = {
  {0, 1000, 1, 1},
  {1, 1000, 1, 0}
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

//########## LOOP ######################################
void loop() {
  if (timeSinceHit < 10) {
    glowHit();
  } else {
    closeHit();
  }
  Blynk.run();
  blePeripheral.poll();
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
  if (now() - lastPoll > 2) {
    //Serial.println("Killing motors. Connection dropped.");
    applyBrakes();
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
  // Read in sensors for damage
  handleSensors();
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

// Move forward
BLYNK_WRITE(V1)
{
  int pinValue = param.asInt();
  if (pinValue == 1) {
    botMotionState = 1;
  } else {
    botMotionState = 0;
  }
}

// Move backward
BLYNK_WRITE(V2)
{
  int pinValue = param.asInt();
  if (pinValue == 1) {
    botMotionState = 2;
  } else {
    botMotionState = 0;
  }
}

// Turn left
BLYNK_WRITE(V3)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  if (pinValue == 1) {
    botMotionState = 4;
  } else {
    botMotionState = 0;
  }
}  

// Turn right
BLYNK_WRITE(V4)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  if (pinValue == 1) {
    botMotionState = 3;
  } else {
    botMotionState = 0;
  }
}  

// Emergency stop
BLYNK_WRITE(V10)
{
  Serial.println("Stopping all motors...");
  applyBrakes();
  motor1->run(RELEASE);
  motor2->run(RELEASE);
  motor3->run(RELEASE);
  motor4->run(RELEASE);
}

BLYNK_READ(V5)
{
  Blynk.virtualWrite(5, 1);
  lastPoll = now();
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

/******************** 
 *  Sensor system   *
 *******************/
int ticks = 0;
void handleSensors() {
  Serial.println("Reading sensors");
  for (int a = 0; a < sensorCount; a++) {
    int lightPin = sensors[a][0];
    int reading = analogRead(lightPin);
    Serial.println(reading);
    int average = sensors[a][1];
    average = average + (reading - average)/30;
    sensors[a][1] = average;
    
    int activation = average + 15;
    //Serial.println(reading);
    
    if (reading > activation && timeSinceHit > 10) {
      Serial.println("Hit detected");
      timeSinceHit = 0;
      //digitalWrite(ledPin, HIGH);
      //delay(500);
      //digitalWrite(ledPin, LOW);
      int x = sensors[a][2];
      int y = sensors[a][3];
      damage[x][y] += 1;
    } else {
      timeSinceHit += 1;
    }
  }
  ticks += 1;
}

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

