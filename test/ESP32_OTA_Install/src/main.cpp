#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <BluetoothSerial.h>
#include <ArduinoOTA.h>

const char *ssid = "GL-SFT1200-3a3"; //自分のSSIDを設定する
const char *password = "goodlife";   // SSIDのパスワードを設定する
const char *hostname = "BEP_ESP1";   //ホスト名を設定する
const int ledpin = 2;

BluetoothSerial SerialBT;

void setup() {
    Serial.begin(2000000);
    SerialBT.begin(hostname);
    Serial.println("Booting");
    SerialBT.println("Booting");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    uint8_t wifiTryCount = 0;
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println(wifiTryCount);
        SerialBT.println(wifiTryCount);
        Serial.println("Connection Failed! Rebooting...");
        SerialBT.println("Connection Failed! Rebooting...");
        delay(5000);
        wifiTryCount++;
        if (wifiTryCount > 3) break;
        // ESP.restart();
    }
    MDNS.begin(hostname);

    // Port defaults to 3232
    // ArduinoOTA.setPort(3232);

    // Hostname defaults to esp3232-[MAC]
    ArduinoOTA.setHostname(hostname);
    /*use mdns for host name resolution*/
    if (!MDNS.begin(hostname)) { // http://esp32.local
        Serial.println("Error setting up MDNS responder!");
        delay(1000);
    } else {
        Serial.println("MDNS responder started");
        MDNS.addService("esp", "tcp", 8080);
    }

    // No authentication by default
    // ArduinoOTA.setPassword("admin");

    // Password can be set with it's md5 value as well
    // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
    // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

    ArduinoOTA
        .onStart([]() {
            String type;
            if (ArduinoOTA.getCommand() == U_FLASH)
                type = "sketch";
            else // U_SPIFFS
                type = "filesystem";

            // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
            Serial.println("Start updating " + type);
        })
        .onEnd([]() {
            Serial.println("\nEnd");
            SerialBT.println("\nEnd");
        })
        .onProgress([](unsigned int progress, unsigned int total) {
            Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
        })
        .onError([](ota_error_t error) {
            Serial.printf("Error[%u]: ", error);
            if (error == OTA_AUTH_ERROR) {
                Serial.println("Auth Failed");
                SerialBT.println("Auth Failed");
            } else if (error == OTA_BEGIN_ERROR) {
                Serial.println("Begin Failed");
                SerialBT.println("Begin Failed");
            } else if (error == OTA_CONNECT_ERROR) {
                Serial.println("Connect Failed");
                SerialBT.println("Connect Failed");
            } else if (error == OTA_RECEIVE_ERROR) {
                Serial.println("Receive Failed");
                SerialBT.println("Receive Failed");
            } else if (error == OTA_END_ERROR) {
                Serial.println("End Failed");
                SerialBT.println("End Failed");
            }
        });

    ArduinoOTA.begin();

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

void loop() {
    /* OTA対応 */
    ArduinoOTA.handle();
    /* OTA後にLEDが点灯する */
    digitalWrite(ledpin, HIGH);
    Serial.println("LED ON");
    SerialBT.println("LED ON");
    delay(200);
    digitalWrite(ledpin, LOW);
    Serial.println("LED OFF");
    SerialBT.println("LED OFF");
    delay(200);
}