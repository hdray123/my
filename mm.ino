#include <ESP8266WiFi.h>
#include <PubSubClient.h>


// Wifi: SSID and password
const char* WIFI_SSID = "CMCC-ggrp";
const char* WIFI_PASSWORD = "kdqfqjrf";

// MQTT: ID, server IP, port, username and password
const PROGMEM char* MQTT_CLIENT_ID = "led_light";
const PROGMEM char* MQTT_SERVER_IP = "192.168.1.6";
const PROGMEM uint16_t MQTT_SERVER_PORT = 1883;
const PROGMEM char* MQTT_USER = "hachina";
const PROGMEM char* MQTT_PASSWORD = "13586600206";

// MQTT: topics 这里的主题要与树莓派中的配置文件中的主题保持一致
const char* MQTT_LASOR_COMMAND_TOPIC = "home-assistant/light_set";

const char* MQTT_LASOR_STATE_TOPIC = "home-assistant/light_state";


// payloads by default (on/off)
const char* DEVICE_ON = "ON";
const char* DEVICE_OFF = "OFF";

const PROGMEM uint8_t lasorPin = 5;


boolean lightSstate = false;


String strRecv = "";
long now = 0;
long lastRecv = 0;
bool newDataComing = false;

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void publishLasorState() {
    if (lightSstate) {
        client.publish(MQTT_LASOR_STATE_TOPIC, DEVICE_ON, true);
    } else {
        client.publish(MQTT_LASOR_STATE_TOPIC, DEVICE_OFF, true);
    }
}

// function called to turn on/off the light
void setLasorState() {
    if (lightSstate) {
       digitalWrite(lasorPin, HIGH);
       //Serial.println("INFO: Turn light on...");
    } else {
       digitalWrite(lasorPin, LOW);
       //Serial.println("INFO: Turn light off...");
    }
  }


// function called when a MQTT message arrived
void callback(char* p_topic, byte* p_payload, unsigned int p_length) {
    // concat the payload into a string
    String payload;
    Serial.println("INFO:callback...");
    for (uint8_t i = 0; i < p_length; i++) {
    payload.concat((char)p_payload[i]);
    }

    // handle message topic
  if (String(MQTT_LASOR_COMMAND_TOPIC).equals(p_topic)) {
    if (payload.equals(String(DEVICE_ON))) {
        lightSstate = true;
    } else if (payload.equals(String(DEVICE_OFF))) {
        lightSstate = false;
    }
    setLasorState();
    publishLasorState();
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("INFO: Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("INFO: connected");
      // Once connected, publish an announcement...
      publishLasorState();
 
      // ... and resubscribe
      client.subscribe(MQTT_LASOR_COMMAND_TOPIC);
    } else {
      Serial.print("ERROR: failed, rc=");
      Serial.print(client.state());
      Serial.println("DEBUG: try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  // init the serial
  Serial.begin(9600);

  // init the led
  pinMode(lasorPin, OUTPUT);

  analogWriteRange(255);
  setLasorState();


  // init the WiFi connection
  Serial.println();
  Serial.println();
  Serial.print("INFO: Connecting to ");
  WiFi.mode(WIFI_STA);
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("INFO: WiFi connected");
  Serial.print("INFO: IP address: ");
  Serial.println(WiFi.localIP());

  // init the MQTT connection
  client.setServer(MQTT_SERVER_IP, MQTT_SERVER_PORT);
  client.setCallback(callback);
}

void loop() {
  //Serial.println("INFO: LOOP: ");
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  if (Serial.available() > 0) {
    char str = char(Serial.read());
    strRecv = strRecv + str;
    lastRecv = millis();
    newDataComing = true;
    delay(2);
  }
  else {
    now = millis();
    if ((now - lastRecv > 100) && (newDataComing == true)) {


      boolean isOK = client.publish(ARDUINO_SENSOR, String(strRecv).c_str(), true);


      strRecv = "";
      newDataComing = false;
    }
  }
}
