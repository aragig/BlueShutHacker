/**
 * @file SharedBlueShutHacker.cpp
 * @brief  This is a library for connecting ESP32 and Daiso remote control via BLE.
 * @author Toshihiko Arai
 *
 * @version 0.1.0
 * @date 2023-03-14
 * @license MIT
 *
 * https://101010.fun/
 */
#include "SharedBlueShutHacker.h"

#include "SharedBlueShutHacker.h"

// Initialize static members of SharedBlueShutHacker class
SharedBlueShutHacker* SharedBlueShutHacker::instance = nullptr;
BLEAddress* SharedBlueShutHacker::pServerAddress = nullptr;
void (*SharedBlueShutHacker::onButtonClick)(boolean state) = nullptr;
void (*SharedBlueShutHacker::onError)() = nullptr;

SharedBlueShutHacker::SharedBlueShutHacker() {}

// Singleton instance getter
SharedBlueShutHacker* SharedBlueShutHacker::shared() {
    if (!instance) {
        instance = new SharedBlueShutHacker();
    }
    return instance;
}

void SharedBlueShutHacker::begin(void (*clickCallback)(boolean state), void (*errorCallback)()) {
    onButtonClick = clickCallback;
    onError = errorCallback;
    BLEDevice::init("");
    BLEScan* pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks(), true);
    pBLEScan->setActiveScan(true);
    pBLEScan->start(30, false);
}

void SharedBlueShutHacker::handler() {
    static boolean connected = false;

    if (pServerAddress != nullptr && !connected) {
        BLEClient* pClient = BLEDevice::createClient();
        if (pClient->connect(*pServerAddress)) {
            Serial.println("Connected");
            BLERemoteService* pRemoteService = pClient->getService(BLEUUID((uint16_t)0x1812));
            if (pRemoteService != nullptr) {
                auto mapCharacteristics = pRemoteService->getCharacteristicsByHandle();
                for (auto& characteristic : *mapCharacteristics) {
                    if (characteristic.second->canNotify()) {
                        characteristic.second->registerForNotify(notifyCallback);
                    }
                }
                connected = true;
            } else if (onError != nullptr) {
                onError();
            }
        }
    }
}

void SharedBlueShutHacker::notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
    Serial.println("notifyCallback");
    switch (pData[0]) {
        case 0x01:  // Volume Up
            Serial.println("Volume Up");
            if (onButtonClick != nullptr) {
                onButtonClick(true);
            }
            break;
        case 0x02:  // Volume Down
            Serial.println("Volume Down");
            if (onButtonClick != nullptr) {
                onButtonClick(true);
            }
            break;
        default:
            Serial.println("Button Off");
            if (onButtonClick != nullptr) {
                onButtonClick(false);
            }
    }
}

void SharedBlueShutHacker::MyAdvertisedDeviceCallbacks::onResult(BLEAdvertisedDevice advertisedDevice) {
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.getServiceUUID().equals(BLEUUID((uint16_t)0x1812))) {
        BLEDevice::getScan()->stop();
        pServerAddress = new BLEAddress(advertisedDevice.getAddress());
        Serial.print("found device: ");
        Serial.println(pServerAddress->toString().c_str());
    }
}
