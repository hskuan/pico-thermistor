#include <Arduino.h>
#include <math.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "config.h"

const int   ADC_PIN_1 = 26;
const int   ADC_PIN_2 = 27;
const float VCC       = 3.3f;
const float R_FIXED   = 100000.0f;
const float R0        = 100000.0f;
const float T0        = 298.15f;
const float BETA      = 3950.0f;

WiFiClient   wifiClient;
PubSubClient mqtt(wifiClient);

float readTempC(int pin) {
    int raw       = analogRead(pin);
    float voltage = raw * (VCC / 4095.0f);
    float r_ntc   = R_FIXED * voltage / (VCC - voltage);
    float tempK   = 1.0f / (1.0f / T0 + logf(r_ntc / R0) / BETA);
    return tempK - 273.15f;
}

void publishTemp(int sensor, float tempC) {
    char topic[48];
    char payload[16];
    snprintf(topic,   sizeof(topic),   "%s/%d/temperature", MQTT_TOPIC_BASE, sensor);
    snprintf(payload, sizeof(payload), "%.2f", tempC);
    mqtt.publish(topic, payload);
}

void connectWiFi() {
    Serial.print("Connecting to WiFi");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println(" connected");
}

void connectMQTT() {
    Serial.print("Connecting to MQTT");
    while (!mqtt.connected()) {
        if (mqtt.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD)) {
            Serial.println(" connected");
        } else {
            Serial.print(".");
            delay(500);
        }
    }
}

void setup() {
    Serial.begin(115200);
    analogReadResolution(12);
    pinMode(LED_BUILTIN, OUTPUT);
    connectWiFi();
    mqtt.setServer(MQTT_BROKER, MQTT_PORT);
}

void loop() {
    if (WiFi.status() != WL_CONNECTED) connectWiFi();
    if (!mqtt.connected()) connectMQTT();
    mqtt.loop();

    float t1 = readTempC(ADC_PIN_1);
    float t2 = readTempC(ADC_PIN_2);

    Serial.print("T1: "); Serial.print(t1, 2); Serial.print(" C  ");
    Serial.print("T2: "); Serial.print(t2, 2); Serial.println(" C");

    publishTemp(1, t1);
    publishTemp(2, t2);

    digitalWrite(LED_BUILTIN, HIGH);
    delay(50);
    digitalWrite(LED_BUILTIN, LOW);
    delay(4950);
}
