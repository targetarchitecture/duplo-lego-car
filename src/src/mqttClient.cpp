/**
 * @file mqttClient.cpp
 * @brief MQTT client implementation for the Duplo Lego Car
 * 
 * This file implements the MQTT client functionality for remote control
 * and telemetry. It handles connection to the MQTT broker, message
 * publishing, and processing of motor control commands.
 */

#include "mqttClient.h"

// MQTT client settings
const int MQTT_BUFFER_SIZE = 1024;  // Increased buffer size for larger messages
const int MQTT_TIMEOUT = 5;         // Connection timeout in seconds

/**
 * @brief Constructor for the MQTT class
 * 
 * Initializes the MQTT client with default settings.
 */
MQTT::MQTT()
{
}

/**
 * @brief Destructor for the MQTT class
 * 
 * Ensures proper cleanup of MQTT resources.
 */
MQTT::~MQTT()
{
  if (MQTTClient.connected())
  {
    MQTTClient.disconnect();
  }
}

/**
 * @brief Initialize the MQTT client
 * 
 * This function:
 * 1. Resets motor control values
 * 2. Sets up JSON document capacity
 * 3. Configures MQTT client with server and credentials
 * 4. Sets up callback for incoming messages
 * 5. Connects to MQTT broker and subscribes to control topic
 */
void MQTT::Begin()
{
  // Reset motor control values
  motorXY.motor_x = 0;
  motorXY.motor_y = 0;
  motorXY.fromMQTT = false;

  // Set JSON document capacity for message parsing
  capacity = JSON_OBJECT_SIZE(29) + 370;

  if (WiFi.isConnected() == true)
  {
    Serial.println("Connecting to MQTT server");

    // Configure MQTT client with modern settings
    MQTTClient.setClient(espClient);
    MQTTClient.setServer(MQTT_SERVER, 1883);
    MQTTClient.setBufferSize(MQTT_BUFFER_SIZE);
    MQTTClient.setSocketTimeout(MQTT_TIMEOUT);

    // Set up callback for incoming messages
    MQTTClient.setCallback([this](char *topic, byte *payload, unsigned int length) {
      this->callback(topic, payload, length);
    });

    Serial.println("connect mqtt...");

    // Connect to MQTT broker with proper error handling
    String clientId = MQTT_CLIENTID;
    if (MQTTClient.connect(clientId.c_str(), MQTT_USERNAME, MQTT_KEY))
    {
      Serial.println("Connected");
      if (!MQTTClient.publish(MQTT_LOG_TOPIC, "Connected to MQTT server"))
      {
        Serial.println("Failed to publish connection message");
      }

      // Subscribe to control topic with error handling
      Serial.println("subscribe");
      if (!MQTTClient.subscribe(MQTT_TOPIC_SUBSCRIBE))
      {
        Serial.println("Failed to subscribe to control topic");
      }
      else
      {
        Serial.println("subscribed");
      }
    }
    else
    {
      Serial.print("Failed to connect to MQTT server, rc=");
      Serial.println(MQTTClient.state());
    }
  }
  else
  {
    Serial.println("Wifi Not Connected");
  }
}

/**
 * @brief Reconnect to the MQTT broker
 * 
 * This function attempts to reconnect to the MQTT broker if the connection
 * is lost. It includes retry logic with a 5-second delay between attempts.
 */
void MQTT::reconnect()
{
  // Loop until reconnected
  while (!MQTTClient.connected())
  {
    yield();

    Serial.print("Attempting MQTT connection...");
    String clientId = MQTT_CLIENTID;

    // Attempt to connect with proper error handling
    if (MQTTClient.connect(clientId.c_str(), MQTT_USERNAME, MQTT_KEY))
    {
      Serial.println("connected");
      if (!MQTTClient.publish(MQTT_LOG_TOPIC, "Reconnected"))
      {
        Serial.println("Failed to publish reconnection message");
      }
      
      if (!MQTTClient.subscribe(MQTT_TOPIC_SUBSCRIBE))
      {
        Serial.println("Failed to resubscribe to control topic");
      }
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(MQTTClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

/**
 * @brief Process incoming MQTT messages
 * 
 * @param topic The topic the message was received on
 * @param payload The message payload
 * @param length The length of the payload
 * 
 * This function:
 * 1. Parses incoming JSON messages
 * 2. Extracts joystick control values
 * 3. Updates motor control values based on joystick input
 */
void MQTT::callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  // Convert payload to string
  String message = "";
  for (unsigned int i = 0; i < length; i++)
  {
    message += (char)payload[i];
  }
  Serial.println(message);

  // Process control messages
  if (String(topic) == String(MQTT_TOPIC_SUBSCRIBE))
  {
    DynamicJsonDocument json(capacity);
    deserializeJson(json, message);

    if (json.containsKey("left_x_mapped") == true)
    {
      // Extract joystick values
      int left_x_mapped = json["left_x_mapped"];
      int left_y_mapped = json["left_y_mapped"];

      // Reset motor control values
      motorXY.motor_x = 0;
      motorXY.motor_y = 0;
      motorXY.fromMQTT = true;

      // Map joystick values to motor control
      if (left_x_mapped < -10)
      {
        motorXY.motor_x = -1;  // Left
      }
      if (left_x_mapped > 10)
      {
        motorXY.motor_x = 1;   // Right
      }
      if (left_y_mapped < -10)
      {
        motorXY.motor_y = 1;   // Forward
      }
      if (left_y_mapped > 10)
      {
        motorXY.motor_y = -1;  // Backward
      }

      Log("MQTT joyx: " + left_x_mapped);
      Log("MQTT joyy: " + left_y_mapped);
    }
  }
}

/**
 * @brief Publish a message to the default topic
 * @param msg The message to publish
 */
void MQTT::publishMQTTmessage(String msg)
{
  MQTTClient.publish(MQTT_LOG_TOPIC, msg.c_str());
}

/**
 * @brief Publish a message to a specific topic
 * @param topic The MQTT topic to publish to
 * @param msg The message to publish
 */
void MQTT::publishMQTTmessage(String topic, String msg)
{
  MQTTClient.publish(topic.c_str(), msg.c_str());
}

/**
 * @brief Process MQTT messages and return motor control values
 * @return MotorXY structure containing the latest motor control values
 * 
 * This function:
 * 1. Returns the current motor control values
 * 2. Resets the control values for the next cycle
 * 3. Maintains the connection to the MQTT broker
 */
MotorXY MQTT::Loop()
{
  // Take a copy of the current motor control values
  MotorXY returnValue;
  returnValue.fromMQTT = motorXY.fromMQTT;
  returnValue.motor_x = motorXY.motor_x;
  returnValue.motor_y = motorXY.motor_y;

  // Reset motor control values for next cycle
  motorXY.motor_x = 0;
  motorXY.motor_y = 0;
  motorXY.fromMQTT = false;

  return returnValue;
}
