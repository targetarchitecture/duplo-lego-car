/**
 * @file batteries.h
 * @brief Battery monitoring interface for the Duplo Lego Car
 * 
 * This header defines the interface for monitoring the vehicle's
 * battery status. It provides methods to read and report battery
 * voltage levels for power management.
 */

#ifndef Batteries_h
#define Batteries_h

#include <Arduino.h>
#include "credentials.h"

// External logging functions for debugging
extern void Log(const String &payload);
extern void Log(const char *payload);
extern void Log(const char *topic, const char *payload);
extern void Log(String topic, String payload);

/**
 * @brief Battery monitoring class
 * 
 * This class manages battery voltage monitoring for the vehicle.
 * It provides methods to read and report battery status, which
 * can be used for power management and low battery warnings.
 */
class Battery
{
public:
  Battery();
  
  /**
   * @brief Initialize battery monitoring
   * 
   * Sets up the analog input for battery voltage measurement
   * and configures the voltage divider if used.
   */
  void Begin();

private:
  // Note: Implementation details for battery monitoring
  // would typically include voltage divider configuration
  // and calibration values
};

#endif