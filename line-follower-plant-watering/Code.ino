/*
==========================================================
 Smart Automatic Plant Watering Robot
 ESP32 Arduino Code
==========================================================
*/

#include <ESP32Servo.h>

//--------------------------------------------------
// Pin Configuration
//--------------------------------------------------

// IR Sensors
#define IR_LEFT        34
#define IR_CENTER      32
#define IR_RIGHT       18

// Motor Driver
#define IN1            25
#define IN2            27
#define IN3            14
#define IN4            19

#define ENA            23
#define ENB            22

// Servo
#define SERVO_PIN      13

// Soil Moisture
#define SOIL_PIN       36

// Relay
#define RELAY_PIN      33

//--------------------------------------------------
// Threshold Values
//--------------------------------------------------

#define DRY_THRESHOLD      3000
#define WET_THRESHOLD      2600

//--------------------------------------------------
// Pump Timing
//--------------------------------------------------

#define PUMP_DURATION      5000
#define PUMP_DELAY         2000

//--------------------------------------------------
// Servo Positions
//--------------------------------------------------

#define SERVO_UP           90
#define SERVO_DOWN         0

//--------------------------------------------------

Servo soilServo;

//--------------------------------------------------
// Function Prototypes
//--------------------------------------------------

void moveForward();
void stopRobot();
void lowerServo();
void raiseServo();
void waterPlant();

//--------------------------------------------------

void setup()
{
    Serial.begin(115200);

    // IR Sensors
    pinMode(IR_LEFT, INPUT);
    pinMode(IR_CENTER, INPUT);
    pinMode(IR_RIGHT, INPUT);

    // Motor Driver
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);

    pinMode(ENA, OUTPUT);
    pinMode(ENB, OUTPUT);

    // Relay
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, LOW);

    // Servo
    soilServo.attach(SERVO_PIN);
    soilServo.write(SERVO_UP);

    stopRobot();

    Serial.println("System Initialized");
}

//--------------------------------------------------

void loop()
{
    int left   = digitalRead(IR_LEFT);
    int center = digitalRead(IR_CENTER);
    int right  = digitalRead(IR_RIGHT);

    //--------------------------------------------------
    // Line Following
    //--------------------------------------------------

    if (center != left && center != right)
    {
        moveForward();

        if (soilServo.read() != SERVO_DOWN)
        {
            lowerServo();
        }
    }

    //--------------------------------------------------
    // Plant Station Detected
    //--------------------------------------------------

    else if ((left == center) && (center == right))
    {
        stopRobot();

        delay(3000);

        lowerServo();

        delay(1000);

        int moisture = analogRead(SOIL_PIN);

        Serial.print("Soil Moisture: ");
        Serial.println(moisture);

        //--------------------------------------------------
        // Dry Soil
        //--------------------------------------------------

        if (moisture >= DRY_THRESHOLD)
        {
            delay(PUMP_DELAY);

            waterPlant();
        }

        //--------------------------------------------------
        // Wet Soil
        //--------------------------------------------------

        else
        {
            Serial.println("Soil is Wet. Watering Skipped.");
        }

        raiseServo();
    }

    //--------------------------------------------------
    // Safety Condition
    //--------------------------------------------------

    else
    {
        stopRobot();
    }

    delay(50);
}

//--------------------------------------------------
// Robot Movement
//--------------------------------------------------

void moveForward()
{
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);

    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);

    digitalWrite(ENA, HIGH);
    digitalWrite(ENB, HIGH);
}

void stopRobot()
{
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);

    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);

    digitalWrite(ENA, LOW);
    digitalWrite(ENB, LOW);
}

//--------------------------------------------------
// Servo Control
//--------------------------------------------------

void lowerServo()
{
    soilServo.write(SERVO_DOWN);
    delay(500);
}

void raiseServo()
{
    soilServo.write(SERVO_UP);
    delay(500);
}

//--------------------------------------------------
// Pump Control
//--------------------------------------------------

void waterPlant()
{
    Serial.println("Water Pump ON");

    digitalWrite(RELAY_PIN, HIGH);

    delay(PUMP_DURATION);

    digitalWrite(RELAY_PIN, LOW);

    Serial.println("Water Pump OFF");
}