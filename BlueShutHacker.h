//
// Created by Toshihiko Arai on 2024/03/14.
//

#ifndef DEV_ESP32_LIBS_BLUESHUTHACKER_H
#define DEV_ESP32_LIBS_BLUESHUTHACKER_H


#include <Arduino.h>
#include "BLEDevice.h"

class BlueShutHacker {
public:
    explicit BlueShutHacker(uint32_t scanDuration = 30);

    void begin(void (*clickCallback)(boolean state));

    void handler();

    void
    notifyCallback(BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify);

private:
    uint32_t scanDuration;
    BLEAddress *pServerAddress = nullptr;
    BLEClient *pClient = nullptr;

    void (*onButtonClick)(boolean state) = nullptr;

    boolean connected = false;

    void onDisconnect(BLEClient *pClient);

    static void onDisconnectCallback(BLEClient *pClient, int reason); // 追加: 切断コールバック

    class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    public:
        MyAdvertisedDeviceCallbacks(BlueShutHacker *hacker) : hackerInstance(hacker) {}

        void onResult(BLEAdvertisedDevice advertisedDevice) override;

    private:
        BlueShutHacker *hackerInstance;
    };

    class MyClientCallback : public BLEClientCallbacks {
    public:
        MyClientCallback(BlueShutHacker *hacker) : hackerInstance(hacker) {}

        void onConnect(BLEClient *pclient) override {}

        void onDisconnect(BLEClient *pclient) override {
            hackerInstance->onDisconnect(pclient);
        }

    private:
        BlueShutHacker *hackerInstance;
    };


    friend class MyAdvertisedDeviceCallbacks;
};

#endif //DEV_ESP32_LIBS_BLUESHUTHACKER_H
