#include "systemManager.hpp"

system::system(char *name, int id, BluetoothSerial *_SerialBT) {
    SerialBT = _SerialBT;
    this->name = name;
    this->id = id;

    SerialBT->begin(name);
    tick.attach_ms(10, []() {
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    });

    modeQty = 0;
    runningModeIndex = 0;
    modePrev = 'M';
    mode = 'M';
}

void system::setMode(char mode) {
    this->mode = mode;
}

void system::addMode(Mode_t mode) {
    this->modes[modeQty] = mode;
    modeQty++;
}

void system::checkMode() {
    uint16_t serialAvailable = Serial.available();
    uint16_t serialBTAvailable = SerialBT->available();
    modePrev = mode;
    if (serialAvailable > 0 || serialBTAvailable > 0) {
        char buf[256];
        if (serialAvailable > 0) {
            size_t bufSize = serialAvailable;
            Serial.readBytesUntil('\n', buf, bufSize);
            Serial.printf(" --read:%s,", buf);
            mode = buf[0];
        } else if (serialBTAvailable > 0) {
            size_t bufSize = serialBTAvailable;
            SerialBT->readBytesUntil('\n', buf, bufSize);
            SerialBT->printf(" --read:%s,", buf);
            mode = buf[0];
        }
        delay(500);
    }
}

uint8_t system::checkModeMatch(char &m) {
    for (size_t i = 0; i < modeQty; i++) {
        if (modes[i].modeLetter == m) {
            return i;
        }
    }
    return MODE_UNMATCH;
}

void system::run() {
    runningModeIndex = checkModeMatch(mode);
    if (runningModeIndex != MODE_UNMATCH) {
        modeRunning = modes[runningModeIndex];
        modePrevRunning = modes[runningModeIndexPrev];
        if (runningModeIndexPrev == runningModeIndex) {
            modeRunning.body();
        } else {
            modePrevRunning.after();
            modeRunning.before();
        }
    } else {
        Serial.printf("Couldn't find a mode on letter:%c\r\n", mode);
        mode = modes[runningModeIndexPrev].modeLetter;
        runningModeIndex = runningModeIndexPrev;
        Serial.printf("-----------------------------------------------\r\n");
        Serial.printf("This Robots has %d modes!!\r\n", modeQty);
        for (size_t i = 0; i < modeQty; i++) {
            Serial.printf(" -- %c : %s\r\n", modes[i].modeLetter, modes[i].modeName);
        }
        Serial.printf("-----------------------------------------------\r\n");
        delay(500);
    }
    runningModeIndexPrev = runningModeIndex;
}
