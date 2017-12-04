/**
 * Ares Studios: Gun Control
 */
// Constants
const int SHOT_CAPACITY = 12;
const int RECHARGE_DURATION = 50;
const int TRIGGER_BUTTON = 5;
const int LASER_PIN = 6;
const int BUZZER_PIN = 9;

const int DATA_PIN = 2;
const int CLOCK_PIN = 4;
const int LATCH_PIN = 3;

byte data = 0;

// States
boolean isFiring = false;
boolean buttonPressedLastLoop = false;
int cyclesWithButtonPressed = 0;

int timeFired = 0;
int shotsLeft = SHOT_CAPACITY;
byte leds = 0;

// To be used to record round durations
int timer = 0;
int beforeTime = millis();

void setup() {
  Serial.begin(9600);
  Serial.println("Booting phaser...");
  pinMode(TRIGGER_BUTTON, INPUT);
  pinMode(LASER_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  // Setup shift register
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);  
  pinMode(LATCH_PIN, OUTPUT);
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
  updateDisplay();
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
    digitalWrite(LASER_PIN, HIGH);
    int fireDuration = 100;
    //delay(fireDuration);
    gunSound();
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

void gunSoundIndexed(int i) {
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
  int maxLeds = 8;
  int litLeds = (shotsLeft+1)/2;
  //litLeds = 7;
  writeLeds(litLeds);
  /*
  Serial.println(litLeds);
  for (int i = 0; i < litLeds; i++) {
    shiftWrite(i, true);
  }
  for (int i = (litLeds); i < maxLeds; i++) {
    shiftWrite(i, false);
  }
  */
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

void writeLeds(int totalLit) {
  leds = 0;
  int ledMap[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
  for (int i = 0; i< totalLit; i++) {
    int index = ledMap[i];
    bitWrite(leds, index, HIGH);
  }
  //bitSet(leds, desiredPin);
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, leds);

  // Once the data is in the shift register, we still need to
  // make it appear at the outputs. We'll toggle the state of
  // the latchPin, which will signal the shift register to "latch"
  // the data to the outputs. (Latch activates on the low-to
  // -high transition).

  digitalWrite(LATCH_PIN, HIGH);
}

/**
 * Shift Register Lighting
 * From Adafruit shift register tutorial
 **/
void shiftWrite(int desiredPin, boolean desiredState)

// This function lets you make the shift register outputs
// HIGH or LOW in exactly the same way that you use digitalWrite().

// Like digitalWrite(), this function takes two parameters:

//    "desiredPin" is the shift register output pin
//    you want to affect (0-7)

//    "desiredState" is whether you want that output
//    to be HIGH or LOW

// Inside the Arduino, numbers are stored as arrays of "bits",
// each of which is a single 1 or 0 value. Because a "byte" type
// is also eight bits, we'll use a byte (which we named "data"
// at the top of this sketch) to send data to the shift register.
// If a bit in the byte is "1", the output will be HIGH. If the bit
// is "0", the output will be LOW.

// To turn the individual bits in "data" on and off, we'll use
// a new Arduino commands called bitWrite(), which can make
// individual bits in a number 1 or 0.
{
  // First we'll alter the global variable "data", changing the
  // desired bit to 1 or 0:

  bitWrite(data,desiredPin,desiredState);

  // Now we'll actually send that data to the shift register.
  // The shiftOut() function does all the hard work of
  // manipulating the data and clock pins to move the data
  // into the shift register:

  byte leds = 0;
  for (int i = 0; i< desiredPin; i++) {
    bitSet(leds, i);
  }
  //bitSet(leds, desiredPin);
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, data);

  // Once the data is in the shift register, we still need to
  // make it appear at the outputs. We'll toggle the state of
  // the latchPin, which will signal the shift register to "latch"
  // the data to the outputs. (Latch activates on the low-to
  // -high transition).

  digitalWrite(LATCH_PIN, HIGH);
  digitalWrite(LATCH_PIN, LOW);
}
