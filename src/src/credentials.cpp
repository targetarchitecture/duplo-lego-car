#include "../include/credentials.h"

const char* WIFI_SSID = "";
const char* WIFI_PASSWORD = "";

const char* MQTT_SERVER = "";

const char* MQTT_CLIENTID = "";
const char* MQTT_USERNAME = "";
const char* MQTT_KEY = "";

const char* MQTT_LOG_TOPIC = "";

const char* MQTT_TOPIC_SUBSCRIBE = "";

const char* MQTT_LASER_TOPIC = "";
const char* MQTT_DIRECTION_TOPIC = "";
const char* MQTT_BATTERY_TOPIC = "duplolegocar/battery";  // Topic for battery status updates

// mDNS hostname for local network discovery
const char* MDNS_HOSTNAME = "duplolegocar";

// OTA update password
const char* OTA_PASSWORD = "duplo123"; 