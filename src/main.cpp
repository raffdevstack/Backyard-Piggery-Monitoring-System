
#define BLYNK_TEMPLATE_ID "TMPL6lDFeOAuC"
#define BLYNK_TEMPLATE_NAME "Backyard Piggery"
#define BLYNK_AUTH_TOKEN "a4MdgoRqs5ljj7yhqHRd9aOUdIyKAk6j"

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

const char* ssid = "Hotspot_ko";
const char* pass = "abcdefghij";
// const char* ssid = "LUNA BOARDING HOUSE PISO WIFI";
// const char* pass = "";

bool wifi_connected = false;
unsigned long previousMillis = 0; // Stores the last time an action occurred

BlynkTimer timer;

BLYNK_CONNECTED()
{
    Serial.println("CONNECTED TO BLYNK");
}

void connectToWifi() {

    if (WiFi.status() != WL_CONNECTED) {
        WiFi.begin(ssid, pass);
        Serial.println("connecting ");
        wifi_connected = false;
    } else {
        Serial.println();
        Serial.println("Connected to WiFi!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
        wifi_connected = true;
    }
}

void setup()
{
    Serial.begin(115200);
    delay(10);

    connectToWifi();

    //  timer.setInterval(1000L, myTimerEvent);
}

void loop()
{
    Blynk.run();
    timer.run();

    unsigned long currentMillis = millis();

    // for wifi connection only, every 5sec
    if (currentMillis - previousMillis >= 5000) {
        previousMillis = currentMillis;

        connectToWifi();
    }

}
