// read waterTemperature with multitasking and display it on the screen
#include <setup.hpp>

void setup() {
    Serial.begin(2000000);
    Serial2.begin(9600, SERIAL_8N1, 16, 17);
    Serial2.setRxBufferSize(2048);
    SerialBT.begin(hostname);
    pinMode(LED_PIN, OUTPUT);
    // alive LED initialization
    tick.attach_ms(650, []() {
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    });

    // create tasks
    xQueue_1 = xQueueCreate(10, 16);
    xTaskCreatePinnedToCore(Core0a, "Core0a", 4096, NULL, 2, &thp[0], 0);
    xTaskCreatePinnedToCore(Core1a, "Core1a", 4096, NULL, 1, &thp[1], 1);
    uint8_t v = 0;
    while (!SD.begin(4) && v < 10) {
        v++;
        delay(1000);
        Serial.println("SD Card Mount Failed");
        //初期化失敗の処理
    }

    Serial.println("Ready");
    SerialBT.println("Ready");
}

void loop() {
    // delay(100);
    while (Serial2.available()) {
        char buf[256]{NULL};
        Serial.printf("\n\n -Avairable:%d\n\n", Serial2.available());
        Serial2.readBytes(buf, Serial2.available());
        for (size_t i = 0; i < 256; i++) {
            if (buf[i] == NULL) break;
            tinyGPS.encode(buf[i]);
            Serial.print(buf[i]);
        }
    }
    if (tinyGPS.location.isUpdated()) {
        data.gps.latitude = tinyGPS.location.lat();
        data.gps.longitude = tinyGPS.location.lng();
        Serial.printf("%f,%f\r\n", data.gps.latitude, data.gps.longitude);
    }

    // char text[128];

    read_pressure();

    // sprintf(text, "wp,%d,wt,%.2f", data.pressure, data.temp);
    // Serial.printf("%s\r\n", text);
    // SerialBT.printf("%s\r\n", text);
    // writeSD("test", text);
    // delay(100);
}
