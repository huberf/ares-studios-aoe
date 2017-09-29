/**
 * Ares Studios: Gun Control
 */
// Constants
const int SHOT_CAPACITY = 6;
const int TRIGGER_BUTTON = 5;

int shotsLeft = SHOT_CAPACITY;

void setup() {
  pinMode(TRIGGER_BUTTON, INPUT);
}

void loop() {
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
  // TODO: Built firing logic
}

