#include <Arduino.h>

int buzzerPin = 4;

void setup() {
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  analogWrite(buzzerPin, 200); // turn on
  delay(1000);

  analogWrite(buzzerPin, 0);  // turn off
  delay(1000);
}