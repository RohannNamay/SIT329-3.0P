extern "C" {
  #include "GPIO.h"
}

const char port = 'A';
const char ledPins[6] = {18, 20, 21, 16, 19, 17}; // PA16 to PA21 (adjust per your wiring)

int mode = 0;  // 0 = no mode, 1 = Knight Rider, 2 = Catch Me If You Can
int previousMode = 0; // Track previous mode for reset

// Software timers (milliseconds)
const unsigned long knightDelay = 1000;
const unsigned long catchRightDelay = 500;

// Knight Rider variables
int krIndex = 0;
bool krForward = true;
unsigned long krLastTime = 0;

// Catch Me If You Can variables
int cmLeftIndex = 0;
int cmRightIndex = 5;
int cmLeftDir = 1;
int cmRightDir = -1;
unsigned long cmLastRightMove = 0;
unsigned long cmLastLeftMove = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial); // Wait for serial

  Serial.println("Select mode:");
  Serial.println("1: Knight Rider");
  Serial.println("2: Catch Me If You Can");
  Serial.println("Enter mode number via serial:");

  // Initialize GPIOs
  for (int i = 0; i < 6; i++) {
    Config_GPIO(port, ledPins[i], OUTPUT);
    Write_GPIO(port, ledPins[i], LOW);
  }
}

void loop() {
  // Poll for mode selection
  if (Serial.available()) {
    char ch = Serial.read();
    if (ch == '1') {
      mode = 1;
      Serial.println("Mode set to Knight Rider");
    } else if (ch == '2') {
      mode = 2;
      Serial.println("Mode set to Catch Me If You Can");
    }
  }

  // Detect mode change for Mode 2 and reset 
  if (mode == 2 && previousMode != 2) {
    resetCatchMeIfYouCan();
  }
  previousMode = mode;

  // Run active mode
  if (mode == 1) {
    knightRider();
  } else if (mode == 2) {
    catchMeIfYouCan();
  }
}


void knightRider() {
  if (millis() - krLastTime >= knightDelay) {
    // Turn off all LEDs
    for (int i = 0; i < 6; i++) 
      Write_GPIO(port, ledPins[i], LOW);

    // Turn on current LED
    Write_GPIO(port, ledPins[krIndex], HIGH);

    // Move index forward or backward
    if (krForward) {
      krIndex++;
      if (krIndex >= 5) krForward = false;
    } else {
      krIndex--;
      if (krIndex <= 0) krForward = true;
    }

    krLastTime = millis();
  }
}


void resetCatchMeIfYouCan() {
  // Turn off all LEDs
  for (int i = 0; i < 6; i++) {
    Write_GPIO(port, ledPins[i], LOW);
  }

  // Turn on starting LEDs
  Write_GPIO(port, ledPins[cmLeftIndex], HIGH);
  Write_GPIO(port, ledPins[cmRightIndex], HIGH);

  // Reset timers
  cmLastLeftMove = millis();
  cmLastRightMove = millis();
}

void catchMeIfYouCan() {
  unsigned long currentTime = millis();

  bool leftMoved = false, rightMoved = false;

  if (currentTime - cmLastRightMove >= catchRightDelay) {
    int nextRight = cmRightIndex + cmRightDir;
    if (nextRight < 0 || nextRight > 5) {
      cmRightDir *= -1;
      cmRightIndex += cmRightDir;
    } else {
      cmRightIndex = nextRight;
    }
    cmLastRightMove = currentTime;
    rightMoved = true;
  }

  if (currentTime - cmLastLeftMove >= 2 * catchRightDelay) {
    int nextLeft = cmLeftIndex + cmLeftDir;
    if (nextLeft < 0 || nextLeft > 5) {
      cmLeftDir *= -1;
      cmLeftIndex += cmLeftDir;
    } else {
      cmLeftIndex = nextLeft;
    }
    cmLastLeftMove = currentTime;
    leftMoved = true;
  }

  if (leftMoved || rightMoved) {
    // Clear all LEDs before lighting the new ones
    for (int i = 0; i < 6; i++) Write_GPIO(port, ledPins[i], LOW);

    // Make sure we don't double-light the same pin
    if (cmLeftIndex == cmRightIndex) {
      Write_GPIO(port, ledPins[cmLeftIndex], HIGH);  // only light once
    } else {
      Write_GPIO(port, ledPins[cmLeftIndex], HIGH);
      Write_GPIO(port, ledPins[cmRightIndex], HIGH);
    }
  }
}

