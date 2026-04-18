#include <Arduino.h>
#include <math.h>

const int   ADC_PIN  = 26;
const float VCC      = 3.3f;
const float R_FIXED  = 100000.0f;
const float R0       = 100000.0f;
const float T0       = 298.15f;
const float BETA     = 3950.0f;

void setup() {
    Serial.begin(115200);
    analogReadResolution(12);
}

void loop() {
    int raw     = analogRead(ADC_PIN);
    float voltage = raw * (VCC / 4095.0f);
    float r_ntc   = R_FIXED * voltage / (VCC - voltage);
    float tempK   = 1.0f / (1.0f / T0 + logf(r_ntc / R0) / BETA);
    float tempC   = tempK - 273.15f;

    Serial.print("raw="); Serial.print(raw);
    Serial.print("  V="); Serial.print(voltage, 3);
    Serial.print("  R_ntc="); Serial.print(r_ntc, 0);
    Serial.print("  Temp="); Serial.print(tempC, 2);
    Serial.println(" C");
    delay(1000);
}
