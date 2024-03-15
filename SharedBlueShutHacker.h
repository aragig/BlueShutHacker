/**
 * @file SharedBlueShutHacker.h
 * @brief  This is a library for connecting ESP32 and Daiso remote control via BLE.
 * @author Toshihiko Arai
 *
 * @version 0.1.0
 * @date 2023-03-14
 * @license MIT
 *
 * https://101010.fun/
 */
#ifndef DEV_ESP32_LIBS_SHAREDBLUESHUTHACKER_H
#define DEV_ESP32_LIBS_SHAREDBLUESHUTHACKER_H

#include <Arduino.h>
#include "BLEDevice.h"


class SharedBlueShutHacker {
public:
    static SharedBlueShutHacker *shared();

    void begin(void (*clickCallback)(boolean state), void (*errorCallback)());

    void handler();

private:
    SharedBlueShutHacker(); // Private so that it can not be called
    SharedBlueShutHacker(SharedBlueShutHacker const &); // Copy constructor is private
    static SharedBlueShutHacker *instance;
    static BLEAddress *pServerAddress;

    static void (*onButtonClick)(boolean state);

    static void (*onError)();

    static void
    notifyCallback(BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify);

    class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
        void onResult(BLEAdvertisedDevice advertisedDevice) override;
    };
};


#endif //DEV_ESP32_LIBS_SHAREDBLUESHUTHACKER_H
