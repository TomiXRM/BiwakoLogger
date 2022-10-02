#include <WiFi.h>
// #include <ESPmDNS.h>
// #include <WiFiUdp.h>
#include <BluetoothSerial.h>
// #include <ArduinoOTA.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"

const char *ssid = "GL-SFT1200-3a3"; //自分のSSIDを設定する
const char *password = "goodlife";   // SSIDのパスワードを設定する
const char *hostname = "BEP_ESP1";   //ホスト名を設定する
const int ledpin = 2;

File myfile; // SDカードの状態を格納

BluetoothSerial SerialBT;
uint16_t logcnt = 0;

void setup() {
    Serial.begin(2000000);
    SerialBT.begin(hostname);
    Serial.println("Booting");
    SerialBT.println("Booting");
    // WiFi.mode(WIFI_STA);
    // WiFi.begin(ssid, password);
    // uint8_t wifiTryCount = 0;
    int8_t c = 0;
    while (!SD.begin(4) && c < 10) {
        c++;
        delay(1000);
        Serial.println("SD Card Mount Failed");
        //初期化失敗の処理
    }

    // while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    //     Serial.println(wifiTryCount);
    //     SerialBT.println(wifiTryCount);
    //     Serial.println("Connection Failed! Rebooting...");
    //     SerialBT.println("Connection Failed! Rebooting...");
    //     delay(5000);
    //     wifiTryCount++;
    //     if (wifiTryCount > 2) break;
    //     // ESP.restart();
    // }
    // MDNS.begin(hostname);

    // Port defaults to 3232
    // ArduinoOTA.setPort(3232);

    // Hostname defaults to esp3232-[MAC]
    // ArduinoOTA.setHostname(hostname);
    /*use mdns for host name resolution*/
    // if (!MDNS.begin(hostname)) { // http://esp32.local
    //     Serial.println("Error setting up MDNS responder!");
    //     delay(1000);
    // } else {
    //     Serial.println("MDNS responder started");
    //     MDNS.addService("esp", "tcp", 8080);
    // }

    // No authentication by default
    // ArduinoOTA.setPassword("admin");

    // Password can be set with it's md5 value as well
    // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
    // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

    // ArduinoOTA
    //     .onStart([]() {
    //         String type;
    //         if (ArduinoOTA.getCommand() == U_FLASH)
    //             type = "sketch";
    //         else // U_SPIFFS
    //             type = "filesystem";

    //         // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    //         Serial.println("Start updating " + type);
    //     })
    //     .onEnd([]() {
    //         Serial.println("\nEnd");
    //         SerialBT.println("\nEnd");
    //     })
    //     .onProgress([](unsigned int progress, unsigned int total) {
    //         Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    //     })
    //     .onError([](ota_error_t error) {
    //         Serial.printf("Error[%u]: ", error);
    //         if (error == OTA_AUTH_ERROR) {
    //             Serial.println("Auth Failed");
    //             SerialBT.println("Auth Failed");
    //         } else if (error == OTA_BEGIN_ERROR) {
    //             Serial.println("Begin Failed");
    //             SerialBT.println("Begin Failed");
    //         } else if (error == OTA_CONNECT_ERROR) {
    //             Serial.println("Connect Failed");
    //             SerialBT.println("Connect Failed");
    //         } else if (error == OTA_RECEIVE_ERROR) {
    //             Serial.println("Receive Failed");
    //             SerialBT.println("Receive Failed");
    //         } else if (error == OTA_END_ERROR) {
    //             Serial.println("End Failed");
    //             SerialBT.println("End Failed");
    //         }
    //     });

    // ArduinoOTA.begin();

    Serial.println("Ready");
    SerialBT.println("Ready");
    Serial.print("IP address: ");
    SerialBT.print("IP address:");
    Serial.println(WiFi.localIP());
    SerialBT.println(WiFi.localIP());

    /* LED 点灯用*/
    pinMode(ledpin, OUTPUT);
    Serial.println("Ready");
    SerialBT.println("Ready");
}

void writeSD(String fileName, String data) {
    String filePath = "/" + fileName + ".csv";

    myfile = SD.open(filePath, FILE_APPEND);
    if (myfile) {
        myfile.println(data);
        myfile.close(); //ファイルを閉じる
    } else {
        Serial.println("SD-ERR");
    }
}

int value[2];
void loop() {
    /* OTA対応 */
    // ArduinoOTA.handle();
    value[0] = value[1];
    value[1] = analogRead(33);
    value[1] = value[1] * 0.5 + value[0] * 0.5;
    Serial.printf("pressure:%d\r\n", value[1]);
    SerialBT.printf("pressure:%d\r\n", value[1]);

    char text[64];
    sprintf(text, "%d", analogRead(33));
    writeSD("test", text);
    delay(100);
}