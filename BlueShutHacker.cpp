//
// Created by Toshihiko Arai on 2024/03/14.
//

#include "BlueShutHacker.h"

BlueShutHacker::BlueShutHacker(uint32_t scanDuration) : scanDuration(scanDuration) {}


void BlueShutHacker::begin(void (*clickCallback)(boolean state)) {
    onButtonClick = clickCallback;
    BLEDevice::init("");
    BLEScan *pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks(this), true);
    pBLEScan->setActiveScan(true);
    pBLEScan->start(scanDuration, false);
    connected = false;
}

void BlueShutHacker::handler() {
    if (pServerAddress != nullptr && !connected) {
        pClient = BLEDevice::createClient();
        pClient->setClientCallbacks(new MyClientCallback(this));
        if (pClient->connect(*pServerAddress)) {
            Serial.println("Connected");
            BLERemoteService *pRemoteService = pClient->getService(BLEUUID((uint16_t) 0x1812));
            if (pRemoteService != nullptr) {
                auto mapCharacteristics = pRemoteService->getCharacteristicsByHandle();
                for (auto &characteristic: *mapCharacteristics) {
                    if (characteristic.second->canNotify()) {
                        characteristic.second->registerForNotify(
                                std::bind(&BlueShutHacker::notifyCallback, this, std::placeholders::_1,
                                          std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
                    }
                }
                connected = true;
            }
        }
    }
}

void BlueShutHacker::notifyCallback(BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length,
                                    bool isNotify) {
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

void BlueShutHacker::onDisconnect(BLEClient *pClient) {
    Serial.println("Disconnected");
    // クライアントのクリーンアップや再接続のロジックをここに記述
    delete pClient;
    this->pClient = nullptr;
    connected = false;
}


void BlueShutHacker::MyAdvertisedDeviceCallbacks::onResult(BLEAdvertisedDevice advertisedDevice) {
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.getServiceUUID().equals(BLEUUID((uint16_t) 0x1812))) {
        BLEDevice::getScan()->stop();
        hackerInstance->pServerAddress = new BLEAddress(advertisedDevice.getAddress());
        Serial.print("found device: ");
        Serial.println(hackerInstance->pServerAddress->toString().c_str());
    }
}
