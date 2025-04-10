/**
 * @file laser.h
 * @brief Laser distance sensor interface for the Duplo Lego Car
 * 
 * This header defines the interface for the VL53L0X laser distance sensor.
 * It provides methods to read distance measurements for obstacle detection
 * and safety features.
 */

#ifndef Laser_h
#define Laser_h

#include <Arduino.h>
#include <limits.h>
#include "Adafruit_VL53L0X.h"
#include "credentials.h"

// External logging functions for debugging
extern void Log(const String &payload);
extern void Log(const char *payload);
extern void Log(const char *topic, const char *payload);
extern void Log(String topic, String payload);

/**
 * @brief Laser distance sensor class
 * 
 * This class manages the VL53L0X laser distance sensor and provides
 * distance measurements. The sensor uses time-of-flight technology
 * to measure distances with high accuracy.
 */
class Laser
{
public:
  Laser();
  
  /**
   * @brief Initialize the laser distance sensor
   * 
   * Sets up the VL53L0X sensor and configures it for operation.
   * Includes error handling for sensor initialization.
   */
  void Begin();
  
  /**
   * @brief Read distance measurement
   * @return Distance in millimeters, or INT_MAX if measurement failed
   * 
   * This function should be called regularly in the main loop to
   * update the distance measurement. It includes error handling
   * for failed measurements.
   */
  int Loop();

private:
  Adafruit_VL53L0X lox;  ///< VL53L0X laser distance sensor instance
};

#endif
