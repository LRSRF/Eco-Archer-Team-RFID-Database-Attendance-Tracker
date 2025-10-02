#include <Arduino.h>

void scan_buzz(int buzzerPin) {
  tone(buzzerPin, 2200, 300);
  delay(300);
}

void success_buzz(int buzzerPin) {
  tone(buzzerPin, 2000, 100);
  delay(150);
  tone(buzzerPin, 2000, 100);
  delay(150);
}

void error_buzz(int buzzerPin) {
  int count = 5;
  for (int i = 0; i < count; i++) {
    tone(buzzerPin, 1800, 250);
    delay(500);
  }
}