/**
 * @file nunchuck.cpp
 * @brief Nintendo Nunchuck controller implementation for the Duplo Lego Car
 * 
 * This file implements the interface for the Nintendo Nunchuck controller.
 * It handles I2C communication with the controller and processes its inputs
 * including joystick position, accelerometer data, and button states.
 * 
 * Based on Tod E. Kurt's implementation: http://thingm.com/
 */

#include <Arduino.h>
#include "nunchuck.h"

/*
   Duplo Lego Car
   Based on Tod E. Kurt, http://thingm.com/
*/

// Global buffer for storing raw controller data
uint8_t nunchuck_buf[6];

/**
 * @brief Constructor for the Nunchuck class
 * 
 * Initializes the Nunchuck controller interface.
 * Note: Initialization is handled separately by nunchuck_init()
 */
Nunchuck::Nunchuck() 
{
  //_MQTTClient = MQTTClient;

  //nunchuck_init(); // send the initilization handshake

  // Log("Wii Nunchuck ready\n");
}

/**
 * @brief Process controller inputs and return motor control values
 * @return MotorXY structure containing motor control values
 * 
 * This function:
 * 1. Reads all controller inputs
 * 2. Maps joystick values to motor control ranges
 * 3. Returns control values for the motor system
 */
MotorXY Nunchuck::Loop()
{
  // Read raw controller data
  nunchuck_get_data();

  // Process all controller inputs
  accx = nunchuck_accelx();  // X-axis accelerometer (70-182)
  accy = nunchuck_accely();  // Y-axis accelerometer (65-173)
  zbut = nunchuck_zbutton(); // Z button state
  cbut = nunchuck_cbutton(); // C button state
  joyx = nunchuck_joyx();    // X-axis joystick
  joyy = nunchuck_joyy();    // Y-axis joystick

  // Map joystick values to motor control range (-1 to 1)
  int motor_x = map(joyx, 0, 255, -1, 1);
  int motor_y = map(joyy, 0, 255, -1, 1);

  // Create and return motor control structure
  MotorXY motorXY;
  motorXY.motor_x = motor_x;
  motorXY.motor_y = motor_y;
  motorXY.fromMQTT = false;

  return motorXY;
}

/**
 * @brief Initialize the Nunchuck controller
 * 
 * This function:
 * 1. Sets up I2C communication with the controller
 * 2. Sends initialization sequence
 * 3. Configures the controller for operation
 */
void Nunchuck::nunchuck_init()
{
  Log("Nunchuck initialise");

  // Send initialization sequence to controller
  Wire.beginTransmission(0x52);  // Nunchuck I2C address
#if (ARDUINO >= 100)
  Wire.write((uint8_t)0x40);     // Memory address
  Wire.write((uint8_t)0x00);     // Initialization value
#else
  Wire.send((uint8_t)0x40);      // Memory address
  Wire.send((uint8_t)0x00);      // Initialization value
#endif
  Wire.endTransmission();
}

/**
 * @brief Request new data from the controller
 * 
 * Sends a request to the Nunchuck controller to prepare
 * the next set of data for reading.
 */
void Nunchuck::nunchuck_send_request()
{
  Wire.beginTransmission(0x52);  // Nunchuck I2C address
#if (ARDUINO >= 100)
  Wire.write((uint8_t)0x00);     // Request data
#else
  Wire.send((uint8_t)0x00);      // Request data
#endif
  Wire.endTransmission();
}

/**
 * @brief Decode a byte from the controller
 * @param x The byte to decode
 * @return The decoded byte value
 * 
 * The Nunchuck uses a simple encoding scheme that needs to be
 * decoded for proper interpretation of the data.
 */
char Nunchuck::nunchuk_decode_byte(char x)
{
  x = (x ^ 0x17) + 0x17;
  return x;
}

/**
 * @brief Read and decode controller data
 * @return 1 if successful, 0 if failed
 * 
 * This function:
 * 1. Requests 6 bytes of data from the controller
 * 2. Decodes each byte
 * 3. Stores the data in the global buffer
 * 4. Requests the next data set
 */
int Nunchuck::nunchuck_get_data()
{
  int cnt = 0;
  Wire.requestFrom(0x52, 6);  // Request 6 bytes from controller
  
  while (Wire.available())
  {
    // Receive and decode each byte
#if (ARDUINO >= 100)
    nunchuck_buf[cnt] = nunchuk_decode_byte(Wire.read());
#else
    nunchuck_buf[cnt] = nunchuk_decode_byte(Wire.receive());
#endif
    cnt++;
  }
  
  nunchuck_send_request();  // Request next data set
  
  return (cnt >= 5) ? 1 : 0;  // Return success if we got enough bytes
}

/**
 * @brief Print controller data for debugging
 * 
 * This function processes and prints all controller inputs:
 * - Joystick X and Y positions
 * - Accelerometer X, Y, and Z values
 * - Z and C button states
 */
void Nunchuck::nunchuck_print_data()
{
  int i = 0;
  int joy_x_axis = nunchuck_buf[0];
  int joy_y_axis = nunchuck_buf[1];
  int accel_x_axis = nunchuck_buf[2];
  int accel_y_axis = nunchuck_buf[3];
  int accel_z_axis = nunchuck_buf[4];

  int z_button = 0;
  int c_button = 0;

  // Process button states and accelerometer LSBs from byte 5
  if ((nunchuck_buf[5] >> 0) & 1) z_button = 1;
  if ((nunchuck_buf[5] >> 1) & 1) c_button = 1;

  // Add LSBs to accelerometer values
  if ((nunchuck_buf[5] >> 2) & 1) accel_x_axis += 1;
  if ((nunchuck_buf[5] >> 3) & 1) accel_x_axis += 2;
  if ((nunchuck_buf[5] >> 4) & 1) accel_y_axis += 1;
  if ((nunchuck_buf[5] >> 5) & 1) accel_y_axis += 2;
  if ((nunchuck_buf[5] >> 6) & 1) accel_z_axis += 1;
  if ((nunchuck_buf[5] >> 7) & 1) accel_z_axis += 2;

  i++;
}

/**
 * @brief Get Z button state
 * @return 1 if pressed, 0 if not pressed
 */
int Nunchuck::nunchuck_zbutton()
{
  return ((nunchuck_buf[5] >> 0) & 1) ? 0 : 1;
}

/**
 * @brief Get C button state
 * @return 1 if pressed, 0 if not pressed
 */
int Nunchuck::nunchuck_cbutton()
{
  return ((nunchuck_buf[5] >> 1) & 1) ? 0 : 1;
}

/**
 * @brief Get joystick X position
 * @return Raw X-axis value (0-255)
 */
int Nunchuck::nunchuck_joyx()
{
  return nunchuck_buf[0];
}

/**
 * @brief Get joystick Y position
 * @return Raw Y-axis value (0-255)
 */
int Nunchuck::nunchuck_joyy()
{
  return nunchuck_buf[1];
}

/**
 * @brief Get X-axis accelerometer value
 * @return Raw X-axis accelerometer value
 * @note This implementation leaves out 2 LSBs of the data
 */
int Nunchuck::nunchuck_accelx()
{
  return nunchuck_buf[2];
}

/**
 * @brief Get Y-axis accelerometer value
 * @return Raw Y-axis accelerometer value
 * @note This implementation leaves out 2 LSBs of the data
 */
int Nunchuck::nunchuck_accely()
{
  return nunchuck_buf[3];
}

/**
 * @brief Get Z-axis accelerometer value
 * @return Raw Z-axis accelerometer value
 * @note This implementation leaves out 2 LSBs of the data
 */
int Nunchuck::nunchuck_accelz()
{
  return nunchuck_buf[4];
}
