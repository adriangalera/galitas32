#include <Arduino.h>

int buzzerPin = 21;

void setup() {
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  analogWrite(buzzerPin, 200); // louder
  delay(1000);

  analogWrite(buzzerPin, 0);  // quieter
  delay(1000);
}