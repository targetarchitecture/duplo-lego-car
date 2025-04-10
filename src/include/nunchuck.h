/**
 * @file nunchuck.h
 * @brief Nintendo Nunchuck controller interface for the Duplo Lego Car
 * 
 * This header defines the interface for the Nintendo Nunchuck controller.
 * It provides methods to read and decode the controller's inputs including
 * joystick position, accelerometer data, and button states.
 */

#ifndef Nunchuck_h
#define Nunchuck_h

#include <Arduino.h>
#include "credentials.h"
#include "motors.h"

// External logging functions for debugging
extern void Log(const String &payload);
extern void Log(const char *payload);
extern void Log(const char *topic, const char *payload);
extern void Log(String topic, String payload);

/**
 * @brief Nunchuck controller class
 * 
 * This class manages communication with a Nintendo Nunchuck controller
 * over I2C. It provides methods to read and decode all controller inputs
 * and convert them to motor control values.
 */
class Nunchuck
{
public:
  Nunchuck();
  
  /**
   * @brief Initialize the Nunchuck controller
   * 
   * Sets up I2C communication and configures the controller for operation.
   */
  void nunchuck_init();
  
  /**
   * @brief Process controller inputs and return motor control values
   * @return MotorXY structure containing motor control values based on controller input
   * 
   * This function should be called regularly in the main loop to
   * process controller inputs and update motor control values.
   */
  MotorXY Loop();

private:
  // Raw controller data
  byte accx, accy;     ///< Accelerometer X and Y values
  byte zbut, cbut;     ///< Z and C button states
  byte joyx, joyy;     ///< Joystick X and Y positions
  
  /**
   * @brief Request new data from the controller
   */
  void nunchuck_send_request();
  
  /**
   * @brief Decode a byte from the controller
   * @param x The byte to decode
   * @return The decoded byte value
   */
  char nunchuk_decode_byte(char x);
  
  /**
   * @brief Read and decode all controller data
   * @return Status of the data read operation
   */
  int nunchuck_get_data();
  
  /**
   * @brief Print controller data for debugging
   */
  void nunchuck_print_data();
  
  // Controller input access methods
  int nunchuck_zbutton();    ///< Get Z button state
  int nunchuck_cbutton();    ///< Get C button state
  int nunchuck_joyx();       ///< Get joystick X position
  int nunchuck_joyy();       ///< Get joystick Y position
  int nunchuck_accelx();     ///< Get accelerometer X value
  int nunchuck_accely();     ///< Get accelerometer Y value
  int nunchuck_accelz();     ///< Get accelerometer Z value
  
  uint8_t nunchuck_buf[6];   ///< Buffer for storing raw controller data
};

#endif