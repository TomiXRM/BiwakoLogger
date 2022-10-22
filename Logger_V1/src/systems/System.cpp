#include "system.hpp"

System::System(const char *name, int id, BluetoothSerial *_SerialBT) {
    SerialBT = _SerialBT;
    this->name = name;
    this->id = id;
}

void System::begin() {
    pinMode(LED_PIN, OUTPUT);
    tick.attach_ms(650, []() {
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    });

    modeQty = 0;
    runningModeIndex = 0;
    modePrev = 'M';
    mode = 'M';
}

void System::setMode(char mode) {
    this->mode = mode;
}

void System::addMode(Mode_t mode) {
    this->modes[modeQty] = mode;
    modeQty++;
}

void System::checkSerial() {
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

uint8_t System::checkModeMatch(char &m) {
    if (modeQty == 0) {
        return 0;
    }
    for (size_t i = 0; i < modeQty; i++) {
        if (modes[i].modeLetter == m) {
            return i;
        }
    }
    return MODE_UNMATCH;
}

void System::run() {
    checkSerial();
    runningModeIndex = checkModeMatch(mode);
    if (modeQty == 0) {
        Serial.println("No mode added");
        return;
    }
    // Serial.printf("ModeQty:%d mode:%c, runningModeIndex:%d, modePrev:%c, mode:%c", modeQty, mode, runningModeIndex, modePrev, mode);
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
