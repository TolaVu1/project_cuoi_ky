#include <Arduino.h>
#include "secrets/wifi.h"
#include "wifi_connect.h"
#include <WiFiClientSecure.h>
#include "secrets/mqtt.h"
#include "ca_cert_hivemq.h"
#include "MQTT.h"
#include <PubSubClient.h>
#include <ESP32Servo.h>

// Servo setup
Servo horizontal;
Servo vertical;
int servoh = 90, servov = 90;
const int servohLimitHigh = 180, servohLimitLow = 0, servovLimitHigh = 150, servovLimitLow = 30;

// WiFi and MQTT setup
namespace {
    const char *ssid = WiFiSecrets::ssid;
    const char *password = WiFiSecrets::pass;
    const char *client_id = (String("esp32-client") + WiFi.macAddress()).c_str();

    WiFiClientSecure tlsClient;
    PubSubClient mqttClient(tlsClient);

    const char *control_topic = "solar/control";
    const char *light_topic = "solar/light";
    const char *status_topic = "solar/status";
    const char *servo_angle_topic = "solar/servo/angles";

}

// LDR pins
const int ldrPins[4] = {36, 39, 34, 35};

bool auto_mode = true;

void mqttCallback(char *topic, uint8_t *payload, unsigned int length) {
    char message[length + 1];
    for (unsigned int i = 0; i < length; i++) {
        message[i] = (char)payload[i];
    }
    message[length] = '\0';

    if (String(topic) == control_topic) {
        if (String(message) == "AUTO_ON") auto_mode = true;
        else if (String(message) == "AUTO_OFF") auto_mode = false;
        else if (String(message) == "MANUAL_HORIZONTAL:UP") {
            auto_mode = false;
            servoh = constrain(servoh + 5, servohLimitLow, servohLimitHigh);
            horizontal.write(servoh);
        }
        else if (String(message) == "MANUAL_HORIZONTAL:DOWN") {
            auto_mode = false;
            servoh = constrain(servoh - 5, servohLimitLow, servohLimitHigh);
            horizontal.write(servoh);
        }
        else if (String(message) == "MANUAL_VERTICAL:UP") {
            auto_mode = false;
            servov = constrain(servov + 5, servovLimitLow, servovLimitHigh);
            vertical.write(servov);
        }
        else if (String(message) == "MANUAL_VERTICAL:DOWN") {
            auto_mode = false;
            servov = constrain(servov - 5, servovLimitLow, servovLimitHigh);
            vertical.write(servov);
        }
    }
}

void setup() {
    Serial.begin(115200);
    delay(10);

    // Setup WiFi
    setup_wifi(ssid, password);

    // Setup TLS
    tlsClient.setCACert(ca_cert);

    // Setup MQTT
    mqttClient.setServer(HiveMQ::broker, HiveMQ::port);
    mqttClient.setCallback(mqttCallback);

    // Servo setup
    horizontal.attach(25, 500, 2400); // Horizontal servo pin
    vertical.attach(26, 500, 2400);   // Vertical servo pin
    horizontal.write(servoh);
    vertical.write(servov);
}

void loop() {
    MQTT::reconnect(mqttClient, client_id, HiveMQ::username, HiveMQ::password, &control_topic, 1);
    mqttClient.loop();

    if (auto_mode) {
        int ldrValues[4];
        for (int i = 0; i < 4; i++) ldrValues[i] = analogRead(ldrPins[i]);
        int avt = (ldrValues[0] + ldrValues[1]) / 2, avd = (ldrValues[2] + ldrValues[3]) / 2;
        int avl = (ldrValues[0] + ldrValues[2]) / 2, avr = (ldrValues[1] + ldrValues[3]) / 2;
        int dvert = avt - avd, dhoriz = avl - avr;
        if (servov <= 90) {
            if (abs(dvert) > 50) {
                servov += (dvert > 0 ? 1 : -1);
                servov = constrain(servov, servovLimitLow, servovLimitHigh);
                vertical.write(servov);
            }
            if (abs(dhoriz) > 50) {
                servoh += (dhoriz > 0 ? -1 : 1);
                servoh = constrain(servoh, servohLimitLow, servohLimitHigh);
                horizontal.write(servoh);
            }
        }
        else
        {
            if (abs(dvert) > 80) {
                servov += (dvert > 0 ? 1 : -1);
                servov = constrain(servov, servovLimitLow, servovLimitHigh);
                vertical.write(servov);
            }
            if (abs(dhoriz) > 80) {
                servoh += (dhoriz > 0 ? 1 : -1);
                servoh = constrain(servoh, servohLimitLow, servohLimitHigh);
                horizontal.write(servoh);
            }            
        }   
    }

    // Publish servo angles to MQTT
    String servoData = String("Horizontal Angle: ") + servoh + ", Vertical Angle: " + servov;
    mqttClient.publish(servo_angle_topic, servoData.c_str());

    // Send servo angles to Serial Monitor
    Serial.print("Horizontal Angle: ");
    Serial.print(servoh);
    Serial.print(", Vertical Angle: ");
    Serial.println(servov);

    // Publish light intensity to MQTT
    String lightData = String("Light Intensity: ") + analogRead(ldrPins[0]);
    mqttClient.publish(light_topic, lightData.c_str());

    delay(30);
}
