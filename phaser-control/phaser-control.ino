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
    gunSound();
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

void gunSound() {
  for (int i = 0; i < 100; i++) {
    buzzPiezzo((100 - i)*50 + 10, 2);
    delay(2);
  }
  buzzPiezzo(500, 20);
  delay(20);
}

void reload() {
  shotsLeft = SHOT_CAPACITY;
  reloadSound(0);
  //reloadSound(1);
  //reloadSound(2);
  //reloadSound(3);
  //reloadSound(4);
}

void reloadSound(int type) {
  if (type == 0) {
    for (int i = 0; i < 100; i++) {
      buzzPiezzo(i*10*(sqrt(i)) + 100, 10);
      delay(10);
    }
    for (int i = 0; i < 10; i++) {
      buzzPiezzo(1100*sqrt(100) - i*10, 10);
      delay(10);
    }
  } else if (type == 1) {
    for (int i = 0; i < 100; i++) {
      buzzPiezzo(i*10*(log(i)) + 100, 10);
      delay(10);
    }
    for (int i = 0; i < 10; i++) {
      buzzPiezzo(1100*log(100) - i*10, 10);
      delay(10);
    }
  } else if (type == 2) {
    for (int i = 0; i < 100; i++) {
      buzzPiezzo(i*10*i + 100, 10);
      delay(10);
    }
    for (int i = 0; i < 10; i++) {
      buzzPiezzo(1100*i- i*10, 10);
      delay(10);
    }
  } else if (type == 3) {
    for (int i = 0; i < 100; i++) {
      buzzPiezzo(i*10*sin(i) + 100, 10);
      delay(10);
    }
    for (int i = 0; i < 10; i++) {
      buzzPiezzo(1100 - i*10, 10);
      delay(10);
    }
  } else if (type == 4) {
    for (int i = 0; i < 100; i++) {
      buzzPiezzo(500*(1 - 1/(pow(1.1, i))) + 100, 10);
      delay(10);
    }
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
