
#define BLYNK_TEMPLATE_ID "TMPL6oTAALs3i"
#define BLYNK_TEMPLATE_NAME "Backyard Piggery"
#define BLYNK_AUTH_TOKEN "9m5XLGdZ6dQTm8fXzHY88WwK9jQ1NfwC"

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
        Serial.print("connecting ");
        wifi_connected = false;
        WiFi.begin(ssid, pass);
    } else {
        Serial.println();
        Serial.println("Connected to WiFi!");
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
