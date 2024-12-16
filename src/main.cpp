
#include <ESP8266WiFi.h>

const char* ssid = "Hotspot_ko";
const char* pass = "abcdefghij";

void setup()
{
    Serial.begin(115200);
    delay(10);

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

void loop()
{
    
}
