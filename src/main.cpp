
#define BLYNK_TEMPLATE_ID "TMPL6lDFeOAuC"
#define BLYNK_TEMPLATE_NAME "Backyard Piggery"
#define BLYNK_AUTH_TOKEN "a4MdgoRqs5ljj7yhqHRd9aOUdIyKAk6j"

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

const char* ssid = "Hotspot_ko";
const char* pass = "abcdefghij";

BlynkTimer timer;

BLYNK_CONNECTED()
{
    Serial.println("CONNECTED TO BLYNK");
}

void myTimerEvent()
{
    Serial.println("blynk timer works");
}

void connectToWifi() {
    while (WiFi.status() != WL_CONNECTED) {
        WiFi.begin(ssid, pass);
        Serial.print("connecting ");
        delay(5000);
    }
    Serial.println();
    Serial.println("Connected to WiFi!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}

void setup()
{
    Serial.begin(115200);
    delay(10);

    

    timer.setInterval(1000L, myTimerEvent);
}

void loop()
{
    Blynk.run();
    timer.run();
}
