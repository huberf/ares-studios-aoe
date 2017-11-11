int lightPin = 0;  //define a pin for Photo resistor
int ledPin=11;     //define a pin for LED
int activation = 600;

int damage[3][3] = {
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0}
};

// Sensor system, {pinNumber, average, damageCoordinateX, damageCoordinateY}
int sensors[][4] = {
  {0, 1000, 1, 1}
  };

int sensorCount = 1;

void setup()
{
    Serial.begin(9600);  //Begin serial communcation
    pinMode( ledPin, OUTPUT );
    for (int a = 0; a < sensorCount; a++) {
      int average = 0;
      for (int i = 0; i < 20; i++) {
        average += analogRead(sensors[a][0]);
      }
      average = average/20;
      sensors[a][1] = average;
    }
}

int ticks = 0;

void loop()
{
  handleSensors();
}

void handleSensors() {
  for (int a = 0; a < sensorCount; a++) {
    int reading = analogRead(lightPin);
    int average = sensors[a][1];
    average = average + (reading - average)/20;
    sensors[a][1] = average;
    
    activation = average - 30;
    Serial.println(reading);
    
    if (reading < activation) {
      Serial.println("Hit detected");
      digitalWrite(ledPin, HIGH);
      delay(500);
      digitalWrite(ledPin, LOW);
      int x = sensors[a][2];
      int y = sensors[a][3];
      damage[x][y] += 1;
    }
  }
  delay(10); //short delay for faster response to light.
  ticks += 1;
}

