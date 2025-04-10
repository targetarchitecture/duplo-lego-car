/**
 * @file motors.cpp
 * @brief Motor control implementation for the Duplo Lego Car
 * 
 * This file implements the motor control system using two I2C motor drivers.
 * It handles motor initialization, speed control, and directional movement
 * with safety features based on distance sensor input.
 */

#include "motors.h"

/**
 * @brief Constructor for the Motors class
 * 
 * Initializes the motor drivers with their respective I2C addresses:
 * - Left motor driver: 0x09 (custom address)
 * - Right motor driver: 0x30 (default address)
 */
Motors::Motors() : leftMotors(0x09), rightMotors(DEFAULT_I2C_MOTOR_ADDRESS)
{
  Log("Motor Shield load");
}

/**
 * @brief Initialize the motor control system
 * 
 * This function:
 * 1. Waits for both motor shields to be ready
 * 2. Configures PWM frequency to 1000Hz for both motors
 * 3. Sets up motor direction control
 */
void Motors::Begin()
{
  Log("Motor Shield init");

  // Wait for left motor shield to be ready
  while (leftMotors.PRODUCT_ID != PRODUCT_ID_I2C_MOTOR)
  {
    leftMotors.getInfo();
  }

  // Wait for right motor shield to be ready
  while (rightMotors.PRODUCT_ID != PRODUCT_ID_I2C_MOTOR)
  {
    rightMotors.getInfo();
  }

  Log("Change A to CCW, B to CW, Freq: 1000Hz");

  // Configure PWM frequency for both motor shields
  leftMotors.changeFreq(MOTOR_CH_BOTH, 1000);   // Left motor PWM frequency
  rightMotors.changeFreq(MOTOR_CH_BOTH, 1000);  // Right motor PWM frequency
}

/**
 * @brief Set motor speeds based on control inputs and safety checks
 * 
 * @param mapx X-axis control input (-1, 0, 1)
 * @param mapy Y-axis control input (-1, 0, 1)
 * @param laserRangeMilliMeter Current distance reading from laser sensor
 * 
 * This function implements:
 * 1. Speed control based on distance sensor input
 * 2. Directional movement in 8 directions
 * 3. Safety features to prevent collisions
 * 4. Differential drive control for turning
 */
void Motors::setMapped(int mapx, int mapy, int laserRangeMilliMeter)
{
  // Motor control parameters
  int maxDuty = 50;           ///< Maximum duty cycle for forward/backward movement
  int maxRotationDuty = 50;   ///< Maximum duty cycle for rotation
  int Duty = 0;               ///< Current duty cycle
  int SafeDistanceMM = 300;   ///< Safe operating distance in millimeters
  int DeadzoneMM = 60;        ///< Minimum distance before stopping
  int minimumDuty = 16;       ///< Minimum duty cycle for movement
  String Direction = "";      ///< Current movement direction

  // Adjust speed based on distance sensor
  if (laserRangeMilliMeter > SafeDistanceMM)
  {
    Duty = maxDuty;  // Full speed when far from obstacles
  }
  else if (laserRangeMilliMeter <= SafeDistanceMM && laserRangeMilliMeter >= DeadzoneMM)
  {
    // Gradually reduce speed as distance decreases
    Duty = map(laserRangeMilliMeter, DeadzoneMM, SafeDistanceMM, minimumDuty, maxDuty);
  }
  else
  {
    Duty = 0;  // Stop when too close to obstacles
  }

  int maxTurnDuty = maxDuty / 2;  ///< Reduced speed for turning movements

  Log("mapx: " + String(mapx) + " mapy: " + String(mapy) + " Duty: " + String(Duty));

  // Handle different movement directions
  if (mapx == 0 && mapy == 1)
  {
    // North (Forward)
    int DutyLeft = Duty;
    int DutyRight = Duty;

    leftMotors.changeDuty(MOTOR_CH_BOTH, DutyLeft);
    rightMotors.changeDuty(MOTOR_CH_BOTH, DutyRight);
    leftMotors.changeStatus(MOTOR_CH_BOTH, MOTOR_STATUS_CW);
    rightMotors.changeStatus(MOTOR_CH_BOTH, MOTOR_STATUS_CW);
    Direction = "NORTH";
  }
  else if (mapx == 1 and mapy == 1)
  {
    // North East
    leftMotors.changeDuty(MOTOR_CH_BOTH, maxDuty);
    rightMotors.changeDuty(MOTOR_CH_BOTH, maxTurnDuty);
    leftMotors.changeStatus(MOTOR_CH_BOTH, MOTOR_STATUS_CW);
    rightMotors.changeStatus(MOTOR_CH_BOTH, MOTOR_STATUS_CW);
    Direction = "NORTH EAST";
  }
  else if (mapx == 1 and mapy == 0)
  {
    // East (Rotate right)
    leftMotors.changeDuty(MOTOR_CH_BOTH, maxRotationDuty);
    rightMotors.changeDuty(MOTOR_CH_BOTH, maxRotationDuty);
    leftMotors.changeStatus(MOTOR_CH_BOTH, MOTOR_STATUS_CW);
    rightMotors.changeStatus(MOTOR_CH_BOTH, MOTOR_STATUS_CCW);
    Direction = "EAST";
  }
  else if (mapx == 1 and mapy == -1)
  {
    // South East
    leftMotors.changeDuty(MOTOR_CH_BOTH, maxDuty);
    rightMotors.changeDuty(MOTOR_CH_BOTH, maxTurnDuty);
    leftMotors.changeStatus(MOTOR_CH_BOTH, MOTOR_STATUS_CCW);
    rightMotors.changeStatus(MOTOR_CH_BOTH, MOTOR_STATUS_CCW);
    Direction = "SOUTH EAST";
  }
  else if (mapx == 0 and mapy == -1)
  {
    // South (Backward)
    leftMotors.changeDuty(MOTOR_CH_BOTH, maxDuty);
    rightMotors.changeDuty(MOTOR_CH_BOTH, maxDuty);
    leftMotors.changeStatus(MOTOR_CH_BOTH, MOTOR_STATUS_CCW);
    rightMotors.changeStatus(MOTOR_CH_BOTH, MOTOR_STATUS_CCW);
    Direction = "SOUTH";
  }
  else if (mapx == -1 and mapy == -1)
  {
    // South West
    leftMotors.changeDuty(MOTOR_CH_BOTH, maxTurnDuty);
    rightMotors.changeDuty(MOTOR_CH_BOTH, maxDuty);
    leftMotors.changeStatus(MOTOR_CH_BOTH, MOTOR_STATUS_CCW);
    rightMotors.changeStatus(MOTOR_CH_BOTH, MOTOR_STATUS_CCW);
    Direction = "SOUTH WEST";
  }
  else if (mapx == -1 and mapy == 0)
  {
    // West (Rotate left)
    leftMotors.changeDuty(MOTOR_CH_BOTH, maxRotationDuty);
    rightMotors.changeDuty(MOTOR_CH_BOTH, maxRotationDuty);
    leftMotors.changeStatus(MOTOR_CH_BOTH, MOTOR_STATUS_CCW);
    rightMotors.changeStatus(MOTOR_CH_BOTH, MOTOR_STATUS_CW);
    Direction = "WEST";
  }
  else if (mapx == -1 and mapy == 1)
  {
    // North West
    leftMotors.changeDuty(MOTOR_CH_BOTH, maxTurnDuty);
    rightMotors.changeDuty(MOTOR_CH_BOTH, maxDuty);
    leftMotors.changeStatus(MOTOR_CH_BOTH, MOTOR_STATUS_CW);
    rightMotors.changeStatus(MOTOR_CH_BOTH, MOTOR_STATUS_CW);
    Direction = "NORTH WEST";
  }
  else
  {
    // Stop all motors
    leftMotors.changeStatus(MOTOR_CH_BOTH, MOTOR_STATUS_STOP);
    rightMotors.changeStatus(MOTOR_CH_BOTH, MOTOR_STATUS_STOP);
    Direction = "STOP";
  }

  // Publish direction to MQTT topic if moving
  if (Direction != "STOP")
  {
    Log(MQTT_DIRECTION_TOPIC, Direction);
  }
}
