#include <Arduino.h>

#define SIGNAL_A_PIN 23
#define SIGNAL_B_PIN 15
#define BUTTON_1_PIN 34
#define BUTTON_2_PIN 12

uint16_t a = 8 * 100; //S = 8
uint16_t b = 9 * 100; //I = 9
uint16_t c = 11 + 4; //K = 11
uint16_t d = 12 * 500; //O = 12
uint16_t mode = (9 % 4) + 1; //R = 9
bool printedOutput = false;

void setup() {
  pinMode(SIGNAL_A_PIN, OUTPUT);
  pinMode(SIGNAL_B_PIN, OUTPUT);
  pinMode(BUTTON_1_PIN, INPUT);
  pinMode(BUTTON_2_PIN, INPUT);

  Serial.begin(9600);
  while (!Serial); //initialise serial output
  Serial.println("\n\nBeginning...");
}

void loop() {
  if (!printedOutput) {
    Serial.printf("A = %d\nB = %d\nC = %d\nD = %d\nMode = %d\n", a, b, c, d, mode);
      printedOutput = true;
  }
}
