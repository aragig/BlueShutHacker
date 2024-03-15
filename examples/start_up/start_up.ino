#include <Arduino.h>
#include "BLEDevice.h"

#define LED_PIN 13

static uint16_t GATT_HID = 0x1812;
//static BLEUUID GATT_HID_REPORT((uint16_t) 0x2a4d);

static BLEAddress *pServerAddress = NULL;

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        if (advertisedDevice.haveServiceUUID() && advertisedDevice.getServiceUUID().equals(GATT_HID)) {
            advertisedDevice.getScan()->stop();

            pServerAddress = new BLEAddress(advertisedDevice.getAddress());
            Serial.print("found device:");
            Serial.println(pServerAddress->toString().c_str()); // 2a:07:98:10:33:fa
        }
    }
};


void updateLedState() {
    static boolean ledState = true;
    if (ledState == 0) {
        digitalWrite(LED_PIN, LOW);
    } else {
        digitalWrite(LED_PIN, HIGH);
    }
    ledState = !ledState;
}

static void
notifyCallback(BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify) {
    Serial.println("notifyCallback");
    switch (pData[0]) {
        case 0x01:  // Volume Up
            Serial.println("Volume Up");
            updateLedState();
            break;
        case 0x02:  // Volume Down
            Serial.println("Volume Down");
            updateLedState();
            break;
    }
}

void setup() {
    Serial.begin(115200);

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    BLEDevice::init("");
    BLEScan *pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
    pBLEScan->start(30); // 30秒間スキャンする
}

void loop() {
    static boolean connected = false;

    if (pServerAddress != NULL && !connected) {
        BLEClient *pClient = BLEDevice::createClient();
        pClient->connect(*pServerAddress);

        if(pClient->isConnected()) {
            Serial.println("connected");
        } else {
            return;
        }

        //! arduino-esp32のバージョンによってはクラッシュするので注意
        //! ver 2.0.2 だとここで落ちる → ver 2.0.14 または ver 3.20014.0 で動作確認済み
        BLERemoteService *pRemoteService = pClient->getService(GATT_HID);
        if (pRemoteService) {
//            BLERemoteCharacteristic *pRemoteCharacteristic = pRemoteService->getCharacteristic(GATT_HID_REPORT);
//            pRemoteCharacteristic->registerForNotify(notifyCallback);

            std::map<uint16_t, BLERemoteCharacteristic*>* mapCharacteristics = pRemoteService->getCharacteristicsByHandle();
            for (std::map<uint16_t, BLERemoteCharacteristic*>::iterator i = mapCharacteristics->begin(); i != mapCharacteristics->end(); ++i) {
                Serial.print("connected to:");
                Serial.println(i->second->getUUID().toString().c_str());
                if (i->second->canNotify()) {
                    Serial.println(" - Add Notify");
                    i->second->registerForNotify(notifyCallback);
                }
            }

            connected = true;
        }
    }
    delay(10); // 高負荷防止
}
