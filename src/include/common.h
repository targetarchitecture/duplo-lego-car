/**
 * @file common.h
 * @brief Common utilities and setup functions for the Duplo Lego Car
 * 
 * This header defines common utilities and setup functions used across
 * the project. It includes WiFi and OTA setup functions, as well as
 * logging utilities for debugging.
 */

#ifndef Common_h
#define Common_h

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>
#include <Wire.h>
#include <PubSubClient.h>
#include "credentials.h"

// External MQTT client instance
extern PubSubClient MQTTClient;

/**
 * @brief Set up WiFi connection
 * 
 * Configures and establishes WiFi connection using credentials from
 * credentials.h. Includes connection retry logic.
 */
void setupWifi();

/**
 * @brief Set up Over-The-Air (OTA) updates
 * 
 * Configures the ESP8266 for OTA firmware updates. This allows updating
 * the firmware without physical access to the device.
 */
void setupOTA();

/**
 * @brief Log a message to serial output
 * @param payload The message to log
 */
void Log(const String &payload);

/**
 * @brief Log a message to serial output
 * @param payload The message to log as a C string
 */
void Log(const char *payload);

/**
 * @brief Log a message with topic to serial output
 * @param topic The topic/category of the message
 * @param payload The message to log
 */
void Log(const char *topic, const char *payload);

/**
 * @brief Log a message with topic to serial output
 * @param topic The topic/category of the message
 * @param payload The message to log
 */
void Log(String topic, String payload);

#endif