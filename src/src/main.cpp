/**
 * @file main.cpp
 * @brief Main control program for Duplo Lego Car with ESP8266
 * 
 * This program controls a Duplo Lego Car using an ESP8266 microcontroller.
 * It integrates various sensors (laser) and control mechanisms (nunchuck, MQTT)
 * to provide remote and local control of the vehicle.
 * 
 * Features:
 * - WiFi connectivity with OTA updates
 * - MQTT communication for remote control
 * - Nunchuck controller for local control
 * - Laser distance sensor
 * - Battery monitoring
 * - I2C device management
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>
#include <Wire.h>
#include <PubSubClient.h>
#include "credentials.h"
#include "common.h"
#include "mqttClient.h"
#include "laser.h"
#include "motors.h"
#include "batteries.h"
#include "nunchuck.h"

// Global object declarations
PubSubClient MQTTClient;    ///< MQTT client for network communication
MQTT mqtt;                  ///< Custom MQTT handler
Battery battery;            ///< Battery monitoring system
Motors motors;              ///< Motor control system
Nunchuck nunchuck;          ///< Nunchuck controller interface
Laser laser;                ///< Laser distance sensor

/**
 * @brief Initializes all system components
 * 
 * This function sets up:
 * - Serial communication for debugging
 * - I2C bus for sensor communication
 * - WiFi and OTA update capabilities
 * - MQTT client for network communication
 * - All sensors and control systems
 * 
 * @note The system will restart if no I2C devices are found
 */
void setup()
{
  // Initialize serial communication for debugging
  Serial.begin(115200);
  Serial.println("Starting Duplo Lego Car System");

  // Initialize I2C bus for sensor communication
  Wire.begin();

  // Setup network connectivity
  setupWifi();
  setupOTA();

  // Initialize MQTT client for remote control
  mqtt.Begin();

  // Scan for I2C devices and verify connectivity
  i2c_scanner();

  // Initialize all sensors and control systems
  laser.Begin();            // Laser distance sensor
  battery.Begin();          // Battery monitoring
  nunchuck.nunchuck_init(); // Nunchuck controller
  motors.Begin();           // Motor control system
}

/**
 * @brief Main control loop
 * 
 * This function:
 * - Maintains network connectivity
 * - Processes remote (MQTT) and local (nunchuck) control inputs
 * - Reads sensor values
 * - Controls motor movement based on inputs and sensor data
 * 
 * @note The loop runs with a 50ms delay to prevent overwhelming the system
 */
void loop()
{
  // Maintain network connectivity if connected
  if (WiFi.isConnected() == true)
  {
    MDNS.update();
    ArduinoOTA.handle();
  }

  // Get motor control values from MQTT
  MotorXY motorXY;
  motorXY = mqtt.Loop();

  // Fallback to nunchuck control if no MQTT input
  if (motorXY.fromMQTT == false)
  {
    motorXY = nunchuck.Loop();
  }

  // Read sensor values
  int laserRangeMilliMeter = laser.Loop();      // Get distance measurement
  int motor_x = motorXY.motor_x;                // X-axis motor control
  int motor_y = motorXY.motor_y;                // Y-axis motor control

  // Apply motor control with sensor feedback
  motors.setMapped(motor_x, motor_y, laserRangeMilliMeter);

  // Prevent system overload
  delay(50);
}

/**
 * @brief Scans the I2C bus for connected devices
 * 
 * This function:
 * - Scans all possible I2C addresses (1-127)
 * - Reports found devices to serial output
 * - Restarts the system if no devices are found
 * 
 * @note Uses yield() to prevent ESP8266 watchdog timer issues
 * @warning System will restart if no I2C devices are detected
 */
void i2c_scanner()
{
  yield();

  byte error, address;
  int nDevices;

  Log("Scanning I2C bus for connected devices...");

  nDevices = 0;
  for (address = 1; address < 127; address++)
  {
    yield();

    delay(5);

    // Check if device responds at current address
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Log("I2C device found at address 0x");
      if (address < 16)
      {
        Log("0");
      }
      Log(String(address));
      Log(" !");

      nDevices++;
    }
    else if (error == 4)
    {
      Log("Unknown error at address 0x");
      if (address < 16)
      {
        Log("0");
      }
      Log(String(address));
    }
  }

  // Handle scan results
  if (nDevices == 0)
  {
    Log("No I2C devices found\n");
    delay(500);
    ESP.restart();
  }
  else
  {
    Log("I2C scan complete. Found ");
    Log(String(nDevices));
    Log(" devices.\n");
  }
}
