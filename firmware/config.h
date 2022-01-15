// Wifi Parameters

#define WIFI_SSID "<wifi-name>"
#define WIFI_PASSWORD "<wifi-password>"
#define WIFI_HOSTNAME "<wifi-device-hostname>"

// MQTT Parameters

#define MQTT_BROKER "<mqtt-broker-ip-address>"
#define MQTT_USERNAME "<mqtt-username>"
#define MQTT_PASSWORD "<mqtt-password>"
#define MQTT_CLIENTID "GarageController"
#define MQTT_AVAILABILITY_TOPIC "garage/controller/availability"
#define MQTT_AVAILABILITY_BIRTH "ONLINE"
#define MQTT_AVAILABILITY_LWT "OFFLINE"

// Door Parameters

#define DOOR_ALIAS "Door"
#define MQTT_DOOR_ACTION_TOPIC "garage/controller/door/action"
#define MQTT_DOOR_STATUS_TOPIC "garage/controller/door/status"
#define MQTT_DOOR_ACTION_OPEN "OPEN"
#define MQTT_DOOR_ACTION_CLOSE "CLOSE"
#define MQTT_DOOR_ACTION_TRIGGER "TRIGGER"
#define MQTT_DOOR_ACTION_STATE "STATE"
#define MQTT_DOOR_STATUS_ON "OPEN"
#define MQTT_DOOR_STATUS_OFF "CLOSED"
#define DOOR_TRIGGER_PIN D6
#define DOOR_STATUS_PIN D1
#define DOOR_STATUS_SWITCH_LOGIC "NO" // NO or NC

// Car Parameters

#define CAR_ALIAS "Car"
#define MQTT_CAR_ACTION_TOPIC "garage/controller/car/action"
#define MQTT_CAR_STATUS_TOPIC "garage/controller/car/status"
#define MQTT_CAR_ACTION_STATE "STATE"
#define MQTT_CAR_STATUS_ON "HOME"
#define MQTT_CAR_STATUS_OFF "AWAY"
#define CAR_SENSOR_TRIGGER_PIN D7
#define CAR_SENSOR_ECHO_PIN D8
#define CAR_SENSOR_DISTANCE 75 // ultrasound threshold

// PIR Parameters

#define PERSON_ALIAS "Person"
#define MQTT_PERSON_ACTION_TOPIC "garage/controller/person/action"
#define MQTT_PERSON_STATUS_TOPIC "garage/controller/person/status"
#define MQTT_PERSON_ACTION_STATE "STATE"
#define MQTT_PERSON_STATUS_ON "ON"
#define MQTT_PERSON_STATUS_OFF "OFF"
#define PERSON_SENSOR_TRIGGER_PIN D5

// MQ7 Parameters

#define CO_ALIAS "CO"
#define MQTT_CO_ACTION_TOPIC "garage/controller/co/action"
#define MQTT_CO_STATUS_TOPIC "garage/controller/co/status"
#define MQTT_CO_ACTION_STATE "STATE"
#define MQTT_CO_STATUS_ON "ON"
#define MQTT_CO_STATUS_OFF "OFF"
#define CO_SENSOR_THRESHOLD 400.0 // threshold of ppm
