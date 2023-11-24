#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <PubSubClient.h>

const char *apName = "ESP8266 Relay";
const char *clientID = "esp8266-relay2";
const char *relayTopic = "esp8266-relay2";

const int RY1 = 5;
const int RY2 = 4;
const int LED2 = 16;
const int LED1 = LED_BUILTIN;

WiFiClient wlan;
PubSubClient mqtt(wlan);
WiFiManager wifiManager(apName);

// Add custom parameters for configuring MQTT server information
WiFiManagerParameter custom_mqtt_server("mqtt_server", "MQTT Server", "192.168.8.160", 40);
WiFiManagerParameter custom_mqtt_port("mqtt_port", "MQTT Port", "1883", 6);
WiFiManagerParameter custom_mqtt_user("mqtt_user", "MQTT User", "mqtt", 40);
WiFiManagerParameter custom_mqtt_password("mqtt_password", "MQTT Password", "mqtt123", 40);

void reconnect()
{
  String user = custom_mqtt_user.getValue();
  String pass = custom_mqtt_password.getValue();
  while (!mqtt.connected())
  {
    Serial.println("Attempting to connect to the MQTT server...");
    if (mqtt.connect(clientID, user.c_str(), pass.c_str()))
    {
      Serial.println("MQTT connected");
      mqtt.subscribe(relayTopic);
    }
    else
    {
      Serial.println("MQTT connect failed, retrying...");
      delay(2000);
    }
  }
}

void onMessage(char *topic, byte *payload, unsigned int length)
{
  // Handle received messages
  String payloadStr = "";
  for (unsigned int i = 0; i < length; i++)
  {
    payloadStr += (char)payload[i];
  }
  Serial.println("Received message: [" + String(topic) + "] " + payloadStr);

  if (strcmp(topic, topic) == 0)
  { // Compare with the control topic
    if (payloadStr == "relay1on")
    {
      digitalWrite(RY1, HIGH); // Open the first relay
    }
    else if (payloadStr == "relay1off")
    {
      digitalWrite(RY1, LOW); // Close the first relay
    }
    else if (payloadStr == "relay2on")
    {
      digitalWrite(RY2, HIGH); // Open the second relay
    }
    else if (payloadStr == "relay2off")
    {
      digitalWrite(RY2, LOW); // Close the second relay
    }
    else if (payloadStr == "led1on")
    {
      digitalWrite(LED1, LOW); // Turn on the LED
    }
    else if (payloadStr == "led1off")
    {
      digitalWrite(LED1, HIGH); // Turn off the LED
    }
    else if (payloadStr == "led2on")
    {
      digitalWrite(LED2, LOW);
    }
    else if (payloadStr == "led2off")
    {
      digitalWrite(LED2, HIGH);
    }
  }
}

void setup()
{

  Serial.begin(115200);
  pinMode(RY1, OUTPUT);
  pinMode(RY2, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_port);
  wifiManager.addParameter(&custom_mqtt_user);
  wifiManager.addParameter(&custom_mqtt_password);
  wifiManager.autoConnect();

  // Set the MQTT server and port
  mqtt.setServer(custom_mqtt_server.getValue(), atoi(custom_mqtt_port.getValue()));
  mqtt.setCallback(onMessage);
  mqtt.setClient(wlan);
}

void loop()
{
  if (!mqtt.connected())
  {
    reconnect();
  }
  mqtt.loop();
}