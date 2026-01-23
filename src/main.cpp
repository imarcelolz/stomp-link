#include <Arduino.h>

#define INPUT_1 0
#define OUTPUT_1 5

#define INPUT_2 1
#define OUTPUT_2 6

#define INPUT_3 21
#define OUTPUT_3 7

#define INPUT_4 20
#define OUTPUT_4 3

void setup() {
  Serial.begin(115200);


  pinMode(INPUT_1, INPUT);
  pinMode(OUTPUT_1, OUTPUT);

  pinMode(INPUT_2, INPUT);
  pinMode(OUTPUT_2, OUTPUT);

  pinMode(INPUT_3, INPUT);
  pinMode(OUTPUT_3, OUTPUT);

  pinMode(INPUT_4, INPUT);
  pinMode(OUTPUT_4, OUTPUT);

  Serial.println("ping setup complete");
}

void blinkPin(int pin) {
  digitalWrite(pin, HIGH);
  delay(100);
  digitalWrite(pin, LOW);
  delay(100);
  digitalWrite(pin, HIGH);
  delay(100);
  digitalWrite(pin, LOW);
}

void loop() {
  Serial.println("ping");

  if(digitalRead(INPUT_1) == HIGH) {
    blinkPin(OUTPUT_1);
  }
  if(digitalRead(INPUT_2) == HIGH) {
    blinkPin(OUTPUT_2);
  }
  if(digitalRead(INPUT_3) == HIGH) {
    blinkPin(OUTPUT_3);
  }
  if(digitalRead(INPUT_4) == HIGH) {
    blinkPin(OUTPUT_4);
  }

  delay(300);
}
