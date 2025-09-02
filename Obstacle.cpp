#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_VL53L0X.h>


#define XSHUT_1 2
#define XSHUT_2 3
#define XSHUT_3 4
#define XSHUT_4 5

Adafruit_VL53L0X sensor1 = Adafruit_VL53L0X();
Adafruit_VL53L0X sensor2 = Adafruit_VL53L0X();
Adafruit_VL53L0X sensor3 = Adafruit_VL53L0X();
Adafruit_VL53L0X sensor4 = Adafruit_VL53L0X();

void initTOF(){
  Wire.begin();

  // On désactive tous les capteurs
  pinMode(XSHUT_1, OUTPUT); digitalWrite(XSHUT_1, LOW);
  pinMode(XSHUT_2, OUTPUT); digitalWrite(XSHUT_2, LOW);
  pinMode(XSHUT_3, OUTPUT); digitalWrite(XSHUT_3, LOW);
  pinMode(XSHUT_4, OUTPUT); digitalWrite(XSHUT_4, LOW);
  delay(10);

  // Initialisation 1 par 1
  pinMode(XSHUT_1, INPUT);
  delay(10);
  sensor1.begin(0x30); // Nouvelle adresse pour capteur 1

  pinMode(XSHUT_2, INPUT);
  delay(10);
  sensor2.begin(0x31); // Capteur 2

  pinMode(XSHUT_3, INPUT);
  delay(10);
  sensor3.begin(0x32); // Capteur 3

  pinMode(XSHUT_4, INPUT);
  delay(10);
  sensor4.begin(0x33); // Capteur 4

  Serial.println("Tous les capteurs VL53L0X sont initialisés");
}

void loop() {
  VL53L0X_RangingMeasurementData_t measure;

  sensor1.rangingTest(&measure, false);
  Serial.print("Capteur 1 : ");
  Serial.println(measure.RangeMilliMeter);

  sensor2.rangingTest(&measure, false);
  Serial.print("Capteur 2 : ");
  Serial.println(measure.RangeMilliMeter);

  sensor3.rangingTest(&measure, false);
  Serial.print("Capteur 3 : ");
  Serial.println(measure.RangeMilliMeter);

  sensor4.rangingTest(&measure, false);
  Serial.print("Capteur 4 : ");
  Serial.println(measure.RangeMilliMeter);

  delay(500);
}
