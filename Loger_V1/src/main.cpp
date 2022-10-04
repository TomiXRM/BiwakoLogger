// read waterTemperature with multitasking and display it on the screen
#include "./setup/setup.hpp"

sensor1_t temp(100, "Temp", "°C");

void setup() {
    Serial.begin(2000000);

    CAN.setPins(25, 26);
    uint8_t c = 0;
    while (!CAN.begin(1000E3)) {
        c++;
        Serial.println("Starting CAN failed!");
        // SerialBT.println("Starting CAN failed!");
        delay(1000);
        if (c > 20) {
            Serial.println("Reboot reason : CAN failed");
            // SerialBT.println("Reboot reason : CAN failed");
            ESP.restart();
        }
    }
    c = 0;
    

    

    

    // alive LED initialization

    // create tasks
}

// void sendFloat(int id, float data) {
//     uBytes data_;
//     data_.f = data;
//     if (id == MY_ID) {
//         CAN.beginPacket(MY_ID);
//     } else {
//         CAN.beginExtendedPacket(id);
//     }
//     CAN.write(data_.b, 4);
//     CAN.endPacket();
// }

// void sendUint32(int id, uint32_t data) {
//     uBytes data_;
//     data_.i = data;
//     if (id == MY_ID) {
//         CAN.beginPacket(MY_ID);
//     } else {
//         CAN.beginExtendedPacket(id);
//     }
//     CAN.write(data_.b, 4);
//     CAN.endPacket();
// }

void sendData() {
    // sendFloat(MY_ID,(float)data.temp);
    // sendUint32(MY_ID + 1,(float)data.pressプレス);
    // sendFloat(MY_ID + 2,(float)data.grav.x);
    // sendFloat(MY_ID + 3,(float)data.grav.y);
    // sendFloat(MY_ID + 4,(float)data.grav.z);
    // sendFloat(MY_ID + 5,(float)data.accel.x);
    // sendFloat(MY_ID + 6,(float)data.accel.y);
    // sendFloat(MY_ID + 7,(float)data.accel.z);
    // sendFloat(MY_ID + 8,(float)data.mag.x);
    // sendFloat(MY_ID + 9,(float)data.mag.y);
    // sendFloat(MY_ID + 10,(float)data.mag.z);
    // sendFloat(MY_ID + 11,(float)data.euler.x);
    // sendFloat(MY_ID + 12,(float)data.euler.y);
    // sendFloat(MY_ID + 13,(float)data.euler.z);
    // sendFloat(MY_ID + 14,(float)data.quat.w);
    // sendFloat(MY_ID + 15,(float)data.quat.x);
    // sendFloat(MY_ID + 16,(float)data.quat.y);
    // sendFloat(MY_ID + 17,(float)data.quat.z);
    // sendFloat(MY_ID + 18,(float)data.gyro.x);
    // sendFloat(MY_ID + 19,(float)data.gyro.y);
    // sendFloat(MY_ID + 20,(float)data.gyro.z);
}

void loop() {
}
