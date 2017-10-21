/**
 * Ares Studios: Gun Control
 */
// Constants
const int SHOT_CAPACITY = 12;
const int RECHARGE_DURATION = 200;
const int TRIGGER_BUTTON = 5;
const int LASER_PIN = 6;
const int BUZZER_PIN = 9;

// States
boolean isFiring = false;
boolean buttonPressedLastLoop = false;
int cyclesWithButtonPressed = 0;

int timeFired = 0;
int shotsLeft = SHOT_CAPACITY;

// To be used to record round durations
int timer = 0;
int beforeTime = millis();

void setup() {
  Serial.begin(9600);
  Serial.println("Booting phaser...");
  pinMode(TRIGGER_BUTTON, INPUT);
  pinMode(LASER_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
  timer += millis() - beforeTime;
  beforeTime = millis();
  // put your main code here, to run repeatedly:
  if (isTriggerPulled()) {
    if (isFiring == false && buttonPressedLastLoop == false) {
      Serial.println("Firing shot");
      fireGun();
    }
    buttonPressedLastLoop = true;
    cyclesWithButtonPressed += 1;
    if (cyclesWithButtonPressed > RECHARGE_DURATION) {
      reload();
      cyclesWithButtonPressed = 0;
    }
  } else {
    cyclesWithButtonPressed = 0;
    buttonPressedLastLoop = false;
  }
  delay(10);
}

boolean isTriggerPulled() {
  boolean buttonPressed = digitalRead(TRIGGER_BUTTON);
  return buttonPressed == HIGH;
}

void fireGun() {
  if (shotsLeft > 0) {
    isFiring = true;
    //soundFX(3.0,100.0);
    buzzPiezzo(2000, 100);
    digitalWrite(LASER_PIN, HIGH);
    int fireDuration = 100;
    delay(fireDuration);
    digitalWrite(LASER_PIN, LOW);
    isFiring = false;
    timeFired += fireDuration;
    shotsLeft -= 1;
  } else {
    alertLowAmmo();
  }
}

void reload() {
  shotsLeft = SHOT_CAPACITY;
  for (int i = 0; i < 10; i++) {
    buzzPiezzo(1000, 50);
    delay(50);
    buzzPiezzo(500, 50);
    delay(50);
  }
}

void alertLowAmmo() {
  buzzPiezzo(100, 500);
}

void updateDisplay() {
  // TODO: Build logic to update LCD with shots left
}

void buzzPiezzo(int frequency, int duration) {
  tone(BUZZER_PIN, frequency, duration);
}

// Taken from: https://forum.arduino.cc/index.php?topic=118757.0
void soundFX(float amplitude,float period){ 
 int uDelay=2+amplitude+amplitude*sin(millis()/period);
 for(int i=0;i<5;i++){
   digitalWrite(BUZZER_PIN,HIGH);
   delayMicroseconds(uDelay);
   digitalWrite(BUZZER_PIN,LOW);
   delayMicroseconds(uDelay);
 }
}
