#include <Stepper.h>
#include <ESP32Servo.h> // Library remains but logic is disabled below

// --- Stepper Config ---
const int stepsPerRevolution = 2048; 
const int maxSteps = 512; // 90 degrees
#define IN1 19
#define IN2 18
#define IN3 5
#define IN4 17
Stepper myStepper(stepsPerRevolution, IN1, IN3, IN2, IN4);
int currentPosition = 0; 

/* // --- Servo Config (COMMENTED OUT) ---
Servo myServo;
const int servoPin = 14;
const int yAxisPin = 35; 
float currentServoAngle = 180.0;
unsigned long lastMoveTime = 0;
bool isAttached = false;
*/

// --- Joystick Config ---
const int xAxisPin = 34;
const int yAxisPin = 35; // Kept pin definition for calibration
const int buttonPin = 13;
const int deadzone = 400; 
int centerValueX = 2048;
int centerValueY = 2048;

void setup() {
  Serial.begin(115200);
  
  // Initialize Stepper
  myStepper.setSpeed(15);
  
  /*
  // --- Servo Setup (COMMENTED OUT) ---
  ESP32PWM::allocateTimer(0);
  myServo.setPeriodHertz(50);
  */
  
  pinMode(buttonPin, INPUT_PULLUP);

  Serial.println("Calibrating... DO NOT TOUCH JOYSTICK.");
  delay(1500);
  centerValueX = analogRead(xAxisPin);
  centerValueY = analogRead(yAxisPin);
  Serial.println("Calibration complete.");
}

void loop() {
  // --- X-AXIS (STEPPER) PROPORTIONAL CONTROL ---
  int xValue = analogRead(xAxisPin);
  int xDiff = xValue - centerValueX;

  if (abs(xDiff) > deadzone) {
    // Speed increases (1 to 20 steps) based on stick tilt
    int stepSpeed = map(abs(xDiff), deadzone, 2048, 1, 20); 
    
    if (xDiff > 0 && currentPosition < maxSteps) {
      myStepper.step(stepSpeed);
      currentPosition += stepSpeed;
      Serial.print("Stepper Pos: "); Serial.println(currentPosition);
    } 
    else if (xDiff < 0 && currentPosition > -maxSteps) {
      myStepper.step(-stepSpeed);
      currentPosition -= stepSpeed;
      Serial.print("Stepper Pos: "); Serial.println(currentPosition);
    }
  }

  /*
  // --- Y-AXIS (SERVO) PROPORTIONAL CONTROL (COMMENTED OUT) ---
  int yValue = analogRead(yAxisPin);
  int yDiff = yValue - centerValueY;

  if (abs(yDiff) > deadzone) {
    if (!isAttached) {
      myServo.attach(servoPin, 500, 2400);
      isAttached = true;
    }

    float dynamicSpeed = map(abs(yDiff), deadzone, 2048, 1, 25) / 10.0;

    if (yDiff < 0 && currentServoAngle > 90.0) {
      currentServoAngle -= dynamicSpeed;
    } else if (yDiff > 0 && currentServoAngle < 180.0) {
      currentServoAngle += dynamicSpeed;
    }

    myServo.write((int)currentServoAngle);
    lastMoveTime = millis();
  } else {
    if (isAttached && millis() - lastMoveTime > 1000) {
      myServo.detach();
      isAttached = false;
    }
  }
  */

  // --- RESET BUTTON (SW) ---
  if (digitalRead(buttonPin) == LOW) {
    currentPosition = 0;
    // currentServoAngle = 180.0; // Commented out
    Serial.println(">> Stepper Position Reset <<");
    delay(500);
  }

  delay(10);
}
