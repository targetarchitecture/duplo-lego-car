/**
 * @file motors.h
 * @brief Motor control interface for the Duplo Lego Car
 * 
 * This header defines the motor control system for the Duplo Lego Car.
 * It provides an interface to control two I2C motor drivers (left and right)
 * with support for differential drive control.
 */

#ifndef motors_h
#define motors_h

#include <Arduino.h>
#include "credentials.h"
#include "LOLIN_I2C_MOTOR.h"

// External logging functions for debugging
extern void Log(const String &payload);
extern void Log(const char *payload);
extern void Log(const char *topic, const char *payload);
extern void Log(String topic, String payload);

/**
 * @brief Structure representing motor control values
 * 
 * This structure holds the control values for both X and Y axes of movement,
 * along with a flag indicating the source of the control input.
 */
struct MotorXY
{
  int motor_x;     ///< X-axis motor control value (-255 to 255)
  int motor_y;     ///< Y-axis motor control value (-255 to 255)
  bool fromMQTT;   ///< Flag indicating if control values came from MQTT
};

/**
 * @brief Motor control class for the Duplo Lego Car
 * 
 * This class manages the motor control system using two I2C motor drivers.
 * It provides methods for initialization and setting motor speeds with
 * consideration for sensor inputs like laser range.
 */
class Motors
{
public:
  Motors();
  
  /**
   * @brief Initialize the motor control system
   * 
   * Sets up the I2C motor drivers and configures them for operation.
   */
  void Begin();
  
  /**
   * @brief Set motor speeds with mapping and safety checks
   * 
   * @param mapx X-axis motor control value (-255 to 255)
   * @param mapy Y-axis motor control value (-255 to 255)
   * @param laserRangeMilliMeter Current distance reading from laser sensor
   * 
   * This function maps the input values to appropriate motor speeds,
   * implements differential drive control, and considers safety
   * based on the laser range sensor input.
   */
  void setMapped(int mapx, int mapy, int laserRangeMilliMeter);

private:
  LOLIN_I2C_MOTOR leftMotors;   ///< Left motor driver (custom I2C address)
  LOLIN_I2C_MOTOR rightMotors;  ///< Right motor driver (I2C address 0x30)
};

#endif