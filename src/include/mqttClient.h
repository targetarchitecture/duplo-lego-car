/**
 * @file mqttClient.h
 * @brief MQTT client interface for the Duplo Lego Car
 * 
 * This header defines the MQTT client system for remote control and telemetry.
 * It provides an interface for publishing messages and receiving motor control
 * commands over MQTT protocol.
 */

#ifndef mqttClient_h
#define mqttClient_h

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJSON.h>
#include "credentials.h"
#include "motors.h"
#include "common.h"

// External MQTT client instance
extern PubSubClient MQTTClient;

// External logging functions for debugging
extern void Log(const String &payload);
extern void Log(const char *payload);
extern void Log(const char *topic, const char *payload);
extern void Log(String topic, String payload);

/**
 * @brief MQTT client class for remote control and telemetry
 * 
 * This class manages MQTT communication for the Duplo Lego Car.
 * It handles connection to the MQTT broker, message publishing,
 * and receiving motor control commands.
 */
class MQTT
{
public:
    MQTT();
    
    /**
     * @brief Initialize the MQTT client
     * 
     * Sets up the MQTT client with the configured broker and topics.
     */
    void Begin();
    
    /**
     * @brief Publish a message to the default topic
     * @param msg The message to publish
     */
    void publishMQTTmessage(String msg);
    
    /**
     * @brief Publish a message to a specific topic
     * @param topic The MQTT topic to publish to
     * @param msg The message to publish
     */
    void publishMQTTmessage(String topic, String msg);
    
    /**
     * @brief Reconnect to the MQTT broker if connection is lost
     * 
     * Attempts to reconnect to the MQTT broker and resubscribe to topics.
     */
    void reconnect();
    
    /**
     * @brief Process MQTT messages and return motor control values
     * @return MotorXY structure containing the latest motor control values
     * 
     * This function should be called regularly in the main loop to
     * process incoming MQTT messages and maintain the connection.
     */
    MotorXY Loop();

private:
    /**
     * @brief MQTT message callback function
     * @param topic The topic the message was received on
     * @param payload The message payload
     * @param length The length of the payload
     * 
     * Processes incoming MQTT messages and updates motor control values.
     */
    void callback(char *topic, byte *payload, unsigned int length);
    
    size_t capacity;                ///< JSON document capacity for message parsing
    volatile MotorXY motorXY;       ///< Current motor control values from MQTT
    WiFiClient espClient;          ///< WiFi client for MQTT communication
};

#endif