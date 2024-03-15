#include <Arduino.h>
#include "BLEDevice.h"
#include "SharedBlueShutHacker.h"

#define LED_PIN 13


void updateLedState(boolean state) {
    Serial.println("updateLedState");
    digitalWrite(LED_PIN, state ? HIGH : LOW);
}

void onButtonClick(boolean state) {
    updateLedState(state);
}

void onError() {
    Serial.println("Error");
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    SharedBlueShutHacker::shared()->begin(onButtonClick, onError);
}

void loop() {
    SharedBlueShutHacker::shared()->handler();
    delay(10); // 高負荷防止
}
