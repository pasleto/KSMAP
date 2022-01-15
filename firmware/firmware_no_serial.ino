#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "config.h"

#define D0 16
#define D1 5
#define D2 4
#define D3 0
#define D4 2
#define D5 14
#define D6 12 
#define D7 13
#define D8 15
#define TX 1
#define RX 3

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;
const char* hostname = WIFI_HOSTNAME;

const char* mqtt_broker = MQTT_BROKER;
const char* mqtt_clientId = MQTT_CLIENTID;
const char* mqtt_username = MQTT_USERNAME;
const char* mqtt_password = MQTT_PASSWORD;

const char* door_alias = DOOR_ALIAS;
const char* mqtt_door_action_topic = MQTT_DOOR_ACTION_TOPIC;
const char* mqtt_door_status_topic = MQTT_DOOR_STATUS_TOPIC;
const char* mqtt_door_action_state = MQTT_DOOR_ACTION_STATE;
const char* mqtt_door_action_open = MQTT_DOOR_ACTION_OPEN;
const char* mqtt_door_action_close = MQTT_DOOR_ACTION_CLOSE;
const char* mqtt_door_action_trigger = MQTT_DOOR_ACTION_TRIGGER;
const char* mqtt_door_status_on = MQTT_DOOR_STATUS_ON;
const char* mqtt_door_status_off = MQTT_DOOR_STATUS_OFF;
const int door_triggerPin = DOOR_TRIGGER_PIN;
const int door_statusPin = DOOR_STATUS_PIN;
const char* door_statusSwitchLogic = DOOR_STATUS_SWITCH_LOGIC;

const char* car_alias = CAR_ALIAS;
const char* mqtt_car_action_topic = MQTT_CAR_ACTION_TOPIC;
const char* mqtt_car_status_topic = MQTT_CAR_STATUS_TOPIC;
const char* mqtt_car_action_state = MQTT_CAR_ACTION_STATE;
const char* mqtt_car_status_on = MQTT_CAR_STATUS_ON;
const char* mqtt_car_status_off = MQTT_CAR_STATUS_OFF;
const int car_triggerPin = CAR_SENSOR_TRIGGER_PIN;
const int car_echoPin = CAR_SENSOR_ECHO_PIN;
const int car_sensorDistance = CAR_SENSOR_DISTANCE;

const char* person_alias = PERSON_ALIAS;
const char* mqtt_person_action_topic = MQTT_PERSON_ACTION_TOPIC;
const char* mqtt_person_status_topic = MQTT_PERSON_STATUS_TOPIC;
const char* mqtt_person_action_state = MQTT_PERSON_ACTION_STATE;
const char* mqtt_person_status_on = MQTT_PERSON_STATUS_ON;
const char* mqtt_person_status_off = MQTT_PERSON_STATUS_OFF;
const int person_triggerPin = PERSON_SENSOR_TRIGGER_PIN;

const char* co_alias = CO_ALIAS;
const char* mqtt_co_action_topic = MQTT_CO_ACTION_TOPIC;
const char* mqtt_co_status_topic = MQTT_CO_STATUS_TOPIC;
const char* mqtt_co_action_state = MQTT_CO_ACTION_STATE;
const char* mqtt_co_status_on = MQTT_CO_STATUS_ON;
const char* mqtt_co_status_off = MQTT_CO_STATUS_OFF;
const float co_sensorThreshold = CO_SENSOR_THRESHOLD;

const int relayActiveTime = 500;
int debounceTime = 2000;

int door_lastStatusValue = 2;
unsigned long door_lastSwitchTime = 0;

int person_lastStatusValue = 2;
unsigned long person_lastSwitchTime = 0;

long car_sensor_duration;
int car_sensor_distance;
boolean car_lastStatusValue = 2;
unsigned long car_lastSwitchTime = 0;

float co_RS_gas = 0;
float co_ratio = 0;
float co_sensorValue = 0;
float co_sensor_volt = 0;
float co_R0 = 20755.55;
float co_sensor_value;
float co_lastStatusValue = 2;
unsigned long co_lastSwitchTime = 0;

const char* mqtt_availabilityTopic = MQTT_AVAILABILITY_TOPIC;
const char* birthMessage = MQTT_AVAILABILITY_BIRTH;
const char* lwtMessage = MQTT_AVAILABILITY_LWT;

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void setupWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.hostname(hostname);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500);
  }
}

void publish_birth_message() {
  client.publish(mqtt_availabilityTopic, birthMessage, true);
}

void publish_door_status() {
  if (digitalRead(door_statusPin) == LOW) {
    if (door_statusSwitchLogic == "NO") {                         
      client.publish(mqtt_door_status_topic, mqtt_door_status_off, true);
    }
    else if (door_statusSwitchLogic == "NC") {
      client.publish(mqtt_door_status_topic, mqtt_door_status_on, true);
    }
  }
  else {
    if (door_statusSwitchLogic == "NO") {
      client.publish(mqtt_door_status_topic, mqtt_door_status_on, true);
    }
    else if (door_statusSwitchLogic == "NC") {
      client.publish(mqtt_door_status_topic, mqtt_door_status_off, true);  
    }
  }
}

void publish_car_status() {
  if (car_lastStatusValue) { 
    client.publish(mqtt_car_status_topic, mqtt_car_status_on, true);
  } else {
    client.publish(mqtt_car_status_topic, mqtt_car_status_off, true);
  }
}

void publish_person_status() {
  if (digitalRead(person_triggerPin) == HIGH) {
    client.publish(mqtt_person_status_topic, mqtt_person_status_on, true);
  }
  else {
    client.publish(mqtt_person_status_topic, mqtt_person_status_off, true);
  }
}

void publish_co_status() {
  if (co_lastStatusValue) {
    client.publish(mqtt_co_status_topic, mqtt_co_status_on, true);
  } else {
    client.publish(mqtt_co_status_topic, mqtt_co_status_off, true);
  }
}

void check_door_status() {
  int currentStatusValue = digitalRead(door_statusPin);
  if (currentStatusValue != door_lastStatusValue) {
    unsigned int currentTime = millis();
    if (currentTime - door_lastSwitchTime >= debounceTime) {
      publish_door_status();
      door_lastStatusValue = currentStatusValue;
      door_lastSwitchTime = currentTime;
    }
  }
}

void check_car_status() {
  digitalWrite(car_triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(car_triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(car_triggerPin, LOW);
  
  car_sensor_duration = pulseIn(car_echoPin, HIGH);
  car_sensor_distance = car_sensor_duration * 0.034 / 2;

  boolean currentStatusValue = (car_sensor_distance <= car_sensorDistance);
  if (currentStatusValue != car_lastStatusValue) {
    unsigned int currentTime = millis();
    if (currentTime - car_lastSwitchTime >= debounceTime) {
      car_lastStatusValue = currentStatusValue;
      car_lastSwitchTime = currentTime;
      publish_car_status();
    }
  }
}

void check_person_status() {
  int currentStatusValue = digitalRead(person_triggerPin);
  if (currentStatusValue != person_lastStatusValue) {
    unsigned int currentTime = millis();
    if (currentTime - person_lastSwitchTime >= debounceTime) {
      publish_person_status();
      person_lastStatusValue = currentStatusValue;
      person_lastSwitchTime = currentTime;
    }
  }
}

void check_co_status() { // ppm calculation for MQ7 on 5 volt
  co_sensorValue = analogRead(A0);
  co_sensor_volt = co_sensorValue/1024*5.0;
  co_RS_gas = (5.0-co_sensor_volt)/co_sensor_volt;
  co_ratio = co_RS_gas/co_R0;
  float x = 1538.46 * co_ratio;
  co_sensor_value = pow(x,-1.709);
  delay(1000);
  
  boolean currentStatusValue = (co_sensor_value >= co_sensorThreshold);
  if (currentStatusValue != co_lastStatusValue) {
    unsigned int currentTime = millis();
    if (currentTime - co_lastSwitchTime >= debounceTime) {
      co_lastStatusValue = currentStatusValue;
      co_lastSwitchTime = currentTime;
      publish_co_status();
    }
  }
}

void toggleRelay(int pin) {
  digitalWrite(pin, HIGH);
  delay(relayActiveTime);
  digitalWrite(pin, LOW);
}

void triggerAction(String requestedTopic, String requestedAction) {
  if (requestedTopic == mqtt_door_action_topic && requestedAction == mqtt_door_action_trigger) {
    toggleRelay(door_triggerPin);
  }
  if (requestedTopic == mqtt_door_action_topic && requestedAction == mqtt_door_action_open) {
    toggleRelay(door_triggerPin);
  }
  else if (requestedTopic == mqtt_door_action_topic && requestedAction == mqtt_door_action_close) {
    toggleRelay(door_triggerPin);
  }
  else if (requestedTopic == mqtt_door_action_topic && requestedAction == mqtt_door_action_state) {
    publish_birth_message();
    publish_door_status();
  }
  else if (requestedTopic == mqtt_car_action_topic && requestedAction == mqtt_car_action_state) {
    publish_birth_message();
    publish_car_status();
  }
  else if (requestedTopic == mqtt_person_action_topic && requestedAction == mqtt_person_action_state) {
    publish_birth_message();
    publish_person_status();
  }
   else if (requestedTopic == mqtt_co_action_topic && requestedAction == mqtt_co_action_state) {
    publish_birth_message();
    publish_co_status();
  }
}

void callback(char* topic, byte* payload, unsigned int length) {                      
  String topicToProcess = topic;
  payload[length] = '\0';
  String payloadToProcess = (char*)payload;
  triggerAction(topicToProcess, payloadToProcess);
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect(mqtt_clientId, mqtt_username, mqtt_password, mqtt_availabilityTopic, 0, true, lwtMessage)) {
      publish_birth_message();
      client.subscribe(mqtt_door_action_topic);
      client.subscribe(mqtt_car_action_topic);
      client.subscribe(mqtt_person_action_topic);
      client.subscribe(mqtt_co_action_topic);
      
      publish_door_status();
      check_car_status();
      check_person_status();
    } 
    else {
      delay(5000);
    }
  }
}

void setup() {
  pinMode(door_triggerPin, OUTPUT);
  digitalWrite(door_triggerPin, LOW);

  pinMode(door_statusPin, INPUT_PULLUP);
  door_lastStatusValue = digitalRead(door_statusPin);

  pinMode(car_triggerPin, OUTPUT);
  pinMode(car_echoPin, INPUT);

  pinMode(person_triggerPin, INPUT);

  setupWifi();
  client.setServer(mqtt_broker, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) { 
    reconnect(); 
  }
  client.loop();

  check_door_status();
  check_person_status();
  check_car_status();
  check_co_status();
}
