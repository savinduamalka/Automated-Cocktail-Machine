#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Arduino.h>

// Constants
const byte ROWS = 4;
const byte COLS = 4;
char hexaKeys[ROWS][COLS] = {{'1', '2', '3', 'A'},
                             {'4', '5', '6', 'B'},
                             {'7', '8', '9', 'C'},
                             {'*', '0', '#', 'D'}};
byte ROW_PINS[ROWS] = {47, 49, 51, 53};
byte COL_PINS[COLS] = {39, 41, 43, 45};

// Pin definitions
const int DIR_PIN = 5;
const int STEP_PIN = 2;
const int ENABLE_PIN = 29;
const int IN1_PIN = 25;
const int IN2_PIN = 27;
const int DIR_PIN2 = 13;
const int STEP_PIN2 = 12;
const int DIR_PIN3 = 6;
const int STEP_PIN3 = 3;
const int DIR_PIN4 = 7;
const int STEP_PIN4 = 4;
const int OBSTACLE_PIN = 22;
const int OBSTACLE_PIN2 = 24;
const int SENSOR_PIN = 26;
const int LIMIT_SWITCH_PIN = 37;

// Motor settings
const int STEPS_TO_POINTS[] = {0, 4900, 7600, 10100, 12600, 17800};
const int MOTOR_SPEED = 510;
const int STEPS_TO_MOVE = 1800;
const int ADDITIONAL_MOTOR_SPEED = 800;
const int STEPS2 = 3200;
const int MOTOR_SPEED2 = 600;

// Options
const char *OPTIONS[] = {
        "1. Martini",
        "2. Negroni",
        "3. St.Clements frizz",
        "4. Gin and tonic",
};
const int NUM_OPTIONS = sizeof(OPTIONS) / sizeof(OPTIONS[0]);

// Global variables
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), ROW_PINS, COL_PINS, ROWS, COLS);
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Enums
enum MotorDirection {
    FORWARD,
    BACKWARD
};

// Function prototypes
void rotateMotor(int steps, MotorDirection direction);

void rotateMotor2(int steps2, MotorDirection direction);

void displayMessage(const String &message, int line);

void displayOption(int option);

void clearScreen();

void clearMessage(int line);

void scrollOptions();

bool moveToPoint(int point);

void waitForObstacleRemovalAndReturn();

void runStepper2AndDCMotor();

void runAdditionalMotors();

void moveMotorsSimultaneously(int steps, bool dir);

char waitForInput();

bool isReadyToStart();

void displayOptionsAndAsk();

void executeOption(int selectedOption);

void customizeDrink();

bool rotateMotorWithCancellation(int steps, MotorDirection direction);

void waitForGlass();

void setup() {
    Serial.begin(9600);
    lcd.init();
    lcd.backlight();

    pinMode(DIR_PIN, OUTPUT);
    pinMode(STEP_PIN, OUTPUT);
    pinMode(DIR_PIN2, OUTPUT);
    pinMode(STEP_PIN2, OUTPUT);
    pinMode(DIR_PIN3, OUTPUT);
    pinMode(STEP_PIN3, OUTPUT);
    pinMode(DIR_PIN4, OUTPUT);
    pinMode(STEP_PIN4, OUTPUT);
    pinMode(ENABLE_PIN, OUTPUT);
    pinMode(IN1_PIN, OUTPUT);
    pinMode(IN2_PIN, OUTPUT);
    pinMode(OBSTACLE_PIN, INPUT);
    pinMode(OBSTACLE_PIN2, INPUT);
    pinMode(LIMIT_SWITCH_PIN, INPUT_PULLUP);
    pinMode(SENSOR_PIN, OUTPUT);
}

void loop() {
    if (isReadyToStart()) {
        clearScreen();
        displayMessage("Press 'A' to see", 0);
        displayMessage("the menu", 2);
        char key = waitForInput();
        if (key == 'A') {
            scrollOptions();
            displayOptionsAndAsk();
        }
    } else {
        if (digitalRead(OBSTACLE_PIN) == HIGH && digitalRead(OBSTACLE_PIN2) == HIGH && digitalRead(LIMIT_SWITCH_PIN) == HIGH) {
            clearScreen();
            moveToPoint(0);
            displayMessage("Please put the glass", 1);
            waitForGlass();
        } else {
            if (digitalRead(OBSTACLE_PIN) == HIGH && digitalRead(OBSTACLE_PIN2) == HIGH) {
                clearScreen();
                displayMessage("Please put the glass", 1);
                waitForGlass();
            } else {
                clearScreen();
                displayMessage("Can not find tray", 1);
                moveToPoint(0);
            }
        }
    }
}

void rotateMotor(int steps, MotorDirection direction) {
    digitalWrite(DIR_PIN, direction == FORWARD ? LOW : HIGH);
    for (int i = 0; i < steps; i++) {
        digitalWrite(STEP_PIN, HIGH);
        delayMicroseconds(MOTOR_SPEED);
        digitalWrite(STEP_PIN, LOW);
        delayMicroseconds(MOTOR_SPEED);
    }
}

void rotateMotor2(int steps, MotorDirection direction) {
    digitalWrite(DIR_PIN2, direction == FORWARD ? LOW : HIGH);
    for (int i = 0; i < steps; i++) {
        digitalWrite(STEP_PIN2, HIGH);
        delayMicroseconds(MOTOR_SPEED2);
        digitalWrite(STEP_PIN2, LOW);
        delayMicroseconds(MOTOR_SPEED2);
    }
}

void displayMessage(const String &message, int line) {
    lcd.setCursor(0, line);
    lcd.print(message);
}

void displayOption(int option) {
    clearScreen();
    lcd.setCursor(0, 0);
    lcd.print("Selected option:");
    lcd.setCursor(0, 1);
    lcd.print(OPTIONS[option]);
}

void clearScreen() {
    lcd.clear();
}

void clearMessage(int line) {
    lcd.setCursor(0, line);
    lcd.print("                    ");
}

void scrollOptions() {
    int currentOption = 0;
    while (true) {
        for (int i = 0; i < 3 && currentOption + i < NUM_OPTIONS; i++) {
            lcd.setCursor(0, i);
            lcd.print(OPTIONS[currentOption + i]);
        }
        lcd.setCursor(0, 3);
        lcd.print(currentOption + 2 < NUM_OPTIONS ? "Press '*' next" : "Press 'A' to select");

        char key = customKeypad.getKey();
        if (key == '*') {
            currentOption = (currentOption + 3) % NUM_OPTIONS;
            clearScreen();
        } else if (key == 'A') {
            return;
        }
    }
}

bool moveToPoint(int point) {
    static int currentPointIndex = 0;
    clearScreen();
    displayMessage("Moving to point " + String(point), 0);
    displayMessage("Press 0 to Cancel", 1);

    if (point == 0) {
        digitalWrite(DIR_PIN, HIGH);
        while (digitalRead(LIMIT_SWITCH_PIN) == HIGH) {
            digitalWrite(STEP_PIN, HIGH);
            delayMicroseconds(MOTOR_SPEED);
            digitalWrite(STEP_PIN, LOW);
            delayMicroseconds(MOTOR_SPEED);
        }
        currentPointIndex = 0;
        return true;
    }

    int steps = STEPS_TO_POINTS[point] - STEPS_TO_POINTS[currentPointIndex];
    if (steps != 0) {
        if (!rotateMotorWithCancellation(abs(steps), steps > 0 ? FORWARD : BACKWARD)) {
            return false;
        }
    }
    currentPointIndex = point;

    // Run additional motors for all points except 5
    if (point != 5) {
        runAdditionalMotors();
    }

    // Special handling for point 5
    if (point == 5) {
        runStepper2AndDCMotor();
    }

    return true;
}

void waitForObstacleRemovalAndReturn() {
    clearScreen();
    while (digitalRead(OBSTACLE_PIN) == LOW || digitalRead(OBSTACLE_PIN2) == LOW) {
        delay(100);
    }
    moveToPoint(0);
}

void runStepper2AndDCMotor() {
    rotateMotor2(STEPS2, BACKWARD);
    delay(1000);
    digitalWrite(ENABLE_PIN, LOW);
    digitalWrite(IN1_PIN, HIGH);
    delay(100);
    digitalWrite(IN2_PIN, LOW);
    delay(7000);
    digitalWrite(ENABLE_PIN, HIGH);
    rotateMotor2(STEPS2, FORWARD);
    delay(1000);
}

void runAdditionalMotors() {
    moveMotorsSimultaneously(STEPS_TO_MOVE, HIGH);
    delay(3000);
    moveMotorsSimultaneously(STEPS_TO_MOVE, LOW);
    delay(1000);
}

void moveMotorsSimultaneously(int steps, bool dir) {
    digitalWrite(DIR_PIN3, dir);
    digitalWrite(DIR_PIN4, dir);
    for (int i = 0; i < steps; i++) {
        digitalWrite(STEP_PIN3, HIGH);
        digitalWrite(STEP_PIN4, HIGH);
        delayMicroseconds(ADDITIONAL_MOTOR_SPEED);
        digitalWrite(STEP_PIN3, LOW);
        digitalWrite(STEP_PIN4, LOW);
        delayMicroseconds(ADDITIONAL_MOTOR_SPEED);
    }
}

char waitForInput() {
    char key;
    while (!(key = customKeypad.getKey())) {
        delay(10);
    }
    return key;
}

void waitForGlass() {
    while (digitalRead(OBSTACLE_PIN) == HIGH && digitalRead(OBSTACLE_PIN2) == HIGH) {
        delay(100);
    }
}

bool isReadyToStart() {
    return ((digitalRead(OBSTACLE_PIN) == LOW || digitalRead(OBSTACLE_PIN2) == LOW) && digitalRead(LIMIT_SWITCH_PIN) == LOW);
}

void displayOptionsAndAsk() {
    clearScreen();
    displayMessage("1: Menu", 0);
    displayMessage("2: Customize", 1);
    displayMessage("0: Back", 2);
    while (true) {
        char key = waitForInput();
        if (key == '1') {
            clearScreen();
            displayMessage("Select drink 1-5", 0);
            displayMessage("0: Back", 1);
            while (true) {
                key = waitForInput();
                if (key == '0') return;
                int selectedOption = key - '0';
                if (selectedOption >= 1 && selectedOption <= 5) {
                    executeOption(selectedOption);
                    return;
                } else {
                    displayMessage("Invalid drink!", 2);
                    delay(2000);
                    clearMessage(2);
                }
            }
        } else if (key == '2') {
            customizeDrink();
            return;
        } else if (key == '0') {
            return;
        }
    }
}

void executeOption(int selectedOption) {
    clearScreen();
    displayMessage("Drink selected:", 0);
    displayOption(selectedOption - 1);
    delay(2000);
    clearScreen();
    displayMessage("Making drink...", 0);
    bool success = true;
    switch (selectedOption) {
        case 1:
            success = moveToPoint(1) && moveToPoint(2) && moveToPoint(5);
            break;
        case 2:
            success = moveToPoint(1) && moveToPoint(2) && moveToPoint(3) && moveToPoint(5);
            break;
        case 3:
            success = moveToPoint(1) && moveToPoint(2) && moveToPoint(3) && moveToPoint(4) && moveToPoint(5);
            break;
        case 4:
            success = moveToPoint(1) && moveToPoint(4) && moveToPoint(5);
            break;
        case 5:
            success = moveToPoint(2) && moveToPoint(3) && moveToPoint(5);
            break;
        default:
            clearScreen();
            displayMessage("Invalid drink", 0);
            delay(2000);
            return;
    }
    if (success) {
        clearScreen();
        displayMessage("Drink ready!", 1);
        delay(3000);
        waitForObstacleRemovalAndReturn();
    } else {
        clearScreen();
        displayMessage("Drink making", 0);
        displayMessage("cancelled", 1);
        delay(3000);
    }
}

void customizeDrink() {
    clearScreen();
    displayMessage("Select Drinks (1-4):", 0);
    displayMessage("D: Done, #: Remove", 1);
    displayMessage("0: Back", 2);

    int selectedPoints[5];  // Max 4 ingredients + 1 for mixing
    int selectedCount = 0;

    while (true) {
        char key = waitForInput();

        if (key == '0') {
            return;  // Return to previous menu
        }

        if (key == 'D') {
            if (selectedCount == 0) {
                displayMessage("No drinks selected", 3);
                delay(2000);
                clearMessage(3);
            } else {
                break;  // Exit selection loop
            }
        }

        if (key == '#' && selectedCount > 0) {
            selectedCount--;
            clearMessage(3);
        } else if (key >= '1' && key <= '4' && selectedCount < 4) {
            selectedPoints[selectedCount++] = key - '0';
        }

        // Display current selection
        clearMessage(3);
        for (int i = 0; i < selectedCount; i++) {
            lcd.setCursor(i * 2, 3);
            lcd.print(selectedPoints[i]);
        }
    }

    if (selectedCount > 0) {
        // Check if mixing is needed
        bool needMixing = false;
        for (int i = 1; i < selectedCount; i++) {
            if (selectedPoints[i] != selectedPoints[0]) {
                needMixing = true;
                break;
            }
        }

        // Ask about mixing if needed
        if (needMixing) {
            clearScreen();
            displayMessage("Mix the drink?", 0);
            displayMessage("1: Yes, 0: No", 1);
            char key = waitForInput();
            if (key == '1') {
                selectedPoints[selectedCount++] = 5;  // Add mixing point
            }
        }

        // Make the drink
        clearScreen();
        displayMessage("Making drink...", 0);
        bool success = true;

        for (int i = 0; i < selectedCount; i++) {
            displayMessage("Adding ingredient " + String(i + 1), 1);
            if (!moveToPoint(selectedPoints[i])) {
                success = false;
                break;
            }
            delay(1000);
        }

        if (success) {
            clearScreen();
            displayMessage("Drink ready!", 1);
            delay(3000);
            displayMessage("Returning to start", 2);
            waitForObstacleRemovalAndReturn();
        } else {
            clearScreen();
            displayMessage("Drink making", 0);
            displayMessage("cancelled", 1);
            delay(3000);
            // moveToPoint(0);  // Return to starting position. Uncomment if needed
        }
    } else {
        clearScreen();
        displayMessage("No drinks selected", 0);
        delay(2000);
    }
}

bool rotateMotorWithCancellation(int steps, MotorDirection direction) {
    digitalWrite(DIR_PIN, direction == FORWARD ? LOW : HIGH);
    for (int i = 0; i < steps; i++) {
        digitalWrite(STEP_PIN, HIGH);
        delayMicroseconds(MOTOR_SPEED);
        digitalWrite(STEP_PIN, LOW);
        delayMicroseconds(MOTOR_SPEED);
        if (customKeypad.getKey() == '0') {
            clearScreen();
            displayMessage("Cancelling...", 0);
            moveToPoint(0);
            return false;
        }
    }
    return true;
}