#include <Arduino.h>

#define SIGNAL_A_PIN 23
#define SIGNAL_B_PIN 16
#define BUTTON_1_PIN 14
#define BUTTON_2_PIN 15

// Debug mode select
//#define DEBUG_MODE

#ifdef DEBUG_MODE
#define TIME_SLOW_FACTOR 1000
#else
#define TIME_SLOW_FACTOR 1
#endif

#define PULSE_DURATION ((8 * 100) * TIME_SLOW_FACTOR)          // microseconds
#define BETWEEN_PULSE_OFF_TIME ((9 * 100) * TIME_SLOW_FACTOR)  // microseconds
#define PULSE_COUNT (11 + 4)
#define IDLE_TIME ((12 * 500) * TIME_SLOW_FACTOR)  // microseconds
#define SYNC_ON_TIME (50 * TIME_SLOW_FACTOR)       // microseconds
#define MODE 2

// Signal variables
bool outputEnabled = false;
bool alternateMode = false;

// Button state tracking
bool lastEnableState = false;
bool lastSelectState = false;

void setup() {
  pinMode(SIGNAL_A_PIN, OUTPUT);
  pinMode(SIGNAL_B_PIN, OUTPUT);
  pinMode(BUTTON_1_PIN, INPUT_PULLDOWN);
  pinMode(BUTTON_2_PIN, INPUT_PULLDOWN);
  Serial.begin(9600);
}

void loop() {
  buttonDebounce();
  if (outputEnabled) {
    outputEnable();
  }
}

void buttonDebounce() {
  bool currentEnableState = digitalRead(BUTTON_1_PIN);
  bool currentSelectState = digitalRead(BUTTON_2_PIN);

  // Check for state change with simple debounce logic
  if (currentEnableState != lastEnableState && currentEnableState == HIGH) {
    outputEnabled = !outputEnabled;  // Toggle output enable state
    Serial.println(outputEnabled ? "Output Enabled\n" : "Output Disabled\n");
  }

  if (currentSelectState != lastSelectState && currentSelectState == HIGH) {
    alternateMode = !alternateMode;  // Toggle output mode/state
    Serial.println(alternateMode ? "Mode 2 Selected\n" : "Default Mode Selected\n");
  }

  // Update last states
  lastEnableState = currentEnableState;
  lastSelectState = currentSelectState;
}

void outputEnable() {
  // Start SYNC signal
  digitalWrite(SIGNAL_B_PIN, HIGH);
  delayMicroseconds(SYNC_ON_TIME);
  digitalWrite(SIGNAL_B_PIN, LOW);

  if (!alternateMode) { //Default signal generation
    for (int i = 0; i < PULSE_COUNT; i++) {
      // Generate DATA pulse
      digitalWrite(SIGNAL_A_PIN, HIGH);
      delayMicroseconds(PULSE_DURATION + (i * (50 * TIME_SLOW_FACTOR)));  //increase pulse duration for each pulse after 1st
      digitalWrite(SIGNAL_A_PIN, LOW);
      delayMicroseconds(BETWEEN_PULSE_OFF_TIME);
    }
  } else if (alternateMode) { //Alternate signal generation
    //changed loop to decrement i to have durations swap for alternate mode
    for (int i = PULSE_COUNT; i > 0; i--) {
      digitalWrite(SIGNAL_A_PIN, HIGH);
      delayMicroseconds(PULSE_DURATION + (i * (50 * TIME_SLOW_FACTOR)));  //decrease pulse duration for each pulse after 1st
      digitalWrite(SIGNAL_A_PIN, LOW);
      delayMicroseconds(BETWEEN_PULSE_OFF_TIME);
    }
  }

  // Idle time before next SYNC pulse
  delayMicroseconds(IDLE_TIME);
}