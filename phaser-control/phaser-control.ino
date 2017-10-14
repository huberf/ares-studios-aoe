/**
 * Ares Studios: Gun Control
 */
// Constants
const int SHOT_CAPACITY = 6;
const int TRIGGER_BUTTON = 5;
const int LASER_PIN = 6
const int BUZZER_PIN = 9;

int timeFired = 0;
int shotsLeft = SHOT_CAPACITY;

// To be used to record round durations
int timer = 0;
int beforeTime = millis();

void setup() {
  pinMode(TRIGGER_BUTTON, INPUT);
  pinMode(LASER_PIN, OUTPUT)
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
  timer += millis() - beforeTime;
  beforeTime = millis();
  // put your main code here, to run repeatedly:
  if (isTriggerPulled()) {
    fireGun();
  }
}

boolean isTriggerPulled() {
  boolean buttonPressed = digitalRead(TRIGGER_BUTTON);
  return buttonPressed;
}

void fireGun() {
  if (shotsLeft > 0) {
    digitalWrite(LASER_PIN, HIGH);
    int fireDuration = 100;
    delay(fireDuration)
    timeFired += fireDuration;
    shotsLeft -= 1;
  } else {
    alertLowAmmo()
  }
}

void alertLowAmmo() {
  // TODO: Write full logic to inform user of low ammo
  buzzPiezzo(1000, 500)
}

void updateDisplay() {
  // TODO: Build logic to update LCD with shots left
}

void buzzPiezzo(int frequency, int duration) {
  tone(BUZZER_PIN, frequency, duration);
}
