#include <Arduino.h>
#include "BLEDevice.h"
#include "BlueShutHacker.h"

#define LED_PIN 13

BlueShutHacker blueShutHacker;

void updateLedState(boolean state) {
    Serial.println("updateLedState");
    digitalWrite(LED_PIN, state ? HIGH : LOW);
}

void onButtonClick(boolean state) {
    updateLedState(state);
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    blueShutHacker.begin(onButtonClick);
}

void loop() {
    blueShutHacker.handler();
    delay(10); // 高負荷防止
}
