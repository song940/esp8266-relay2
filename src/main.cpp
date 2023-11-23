#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiManager.h>

const char* apName = "ESP8266 Relay";
const char* clientID = "esp8266-relay";
const char* topic = "relay"; // Topic to control the relay

const int LED = 16;
const int RY1 = 5;
const int RY2 = 4;

WiFiClient wlan;
PubSubClient mqtt(wlan);
WiFiManager wifiManager;

// Add custom parameters for configuring MQTT server information
WiFiManagerParameter custom_mqtt_server("mqtt_server", "MQTT Server", "192.168.8.160", 40);
WiFiManagerParameter custom_mqtt_port("mqtt_port", "MQTT Port", "1883", 6);
WiFiManagerParameter custom_mqtt_user("mqtt_user", "MQTT User", "mqtt", 40);
WiFiManagerParameter custom_mqtt_password("mqtt_password", "MQTT Password", "mqtt123", 40);


// WiFiManager callback function
void saveConfigCallback() {
  Serial.println("Configuration saved");
}

void reconnect() {
  while (!mqtt.connected()) {
    Serial.println("Attempting to connect to the MQTT server...");
    if (mqtt.connect(clientID)) {
      Serial.println("MQTT connected");
      mqtt.subscribe(topic); // Subscribe to the control topic
    } else {
      Serial.println("MQTT connect failed, retrying...");
      delay(2000);
    }
  }
}

void onMessage(char* topic, byte* payload, unsigned int length) {
  // Handle received messages
  String payloadStr = "";
  for (unsigned int i = 0; i < length; i++) {
    payloadStr += (char)payload[i];
  }

  Serial.print("Received message: [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println(payloadStr);

  if (strcmp(topic, topic) == 0) { // Compare with the control topic
    if (payloadStr == "relay1on") {
      digitalWrite(RY1, HIGH); // Open the first relay
    } else if (payloadStr == "relay1off") {
      digitalWrite(RY1, LOW); // Close the first relay
    } else if (payloadStr == "relay2on") {
      digitalWrite(RY2, HIGH); // Open the second relay
    } else if (payloadStr == "relay2off") {
      digitalWrite(RY2, LOW); // Close the second relay
    } else if (payloadStr == "led1on") {
      digitalWrite(LED, LOW); // Turn on the LED
    } else if (payloadStr == "led1off") {
      digitalWrite(LED, HIGH); // Turn off the LED
    } else if (payloadStr == "led2on") {
      digitalWrite(LED_BUILTIN, LOW);
    } else if (payloadStr == "led2off") {
      digitalWrite(LED_BUILTIN, HIGH);
    }
  }
}

void setup() {

  Serial.begin(115200);

  pinMode(RY1, OUTPUT);
  pinMode(RY2, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_port);
  wifiManager.addParameter(&custom_mqtt_user);
  wifiManager.addParameter(&custom_mqtt_password);

  // Initialize WiFi connection and connect to a saved WiFi network or start AP mode for configuration
  wifiManager.autoConnect(apName);

  // Set the MQTT server and port
  const char * mqtt_server = custom_mqtt_server.getValue();
  int mqtt_port = atoi(custom_mqtt_port.getValue());
  Serial.println(mqtt_server);
  mqtt.setServer(mqtt_server, mqtt_port);
  mqtt.setCallback(onMessage);
  mqtt.setClient(wlan);
}

void loop() {
  if (!mqtt.connected()) {
    reconnect();
  }
  mqtt.loop();
}
