
#define BLYNK_TEMPLATE_ID "TMPL6lDFeOAuC"
#define BLYNK_TEMPLATE_NAME "Backyard Piggery"
#define BLYNK_AUTH_TOKEN "a4MdgoRqs5ljj7yhqHRd9aOUdIyKAk6j"

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

const char* ssid = "Hotspot_ko";
const char* pass = "abcdefghij";
bool wifi_connected = false;
unsigned long previousMillis = 0; // Stores the last time an action occurred
const long wifi_con_interval = 5000;

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
    if (WiFi.status() != WL_CONNECTED) {
        WiFi.begin(ssid, pass);
        Serial.print("connecting ");
        wifi_connected = false;
    }
    Serial.println();
    Serial.println("Connected to WiFi!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    wifi_connected = true;
}

void setup()
{
    Serial.begin(115200);
    delay(10);

    connectToWifi();

    if (wifi_connected)
    {
        timer.setInterval(1000L, myTimerEvent);
    }
    
}

void loop()
{
    Blynk.run();
    timer.run();

    unsigned long currentMillis = millis();
  // Check if the interval has passed
    if (currentMillis - previousMillis >= wifi_con_interval) {
        previousMillis = currentMillis;

        connectToWifi();
    }

    if (wifi_connected)
    {
        Serial.println("I  am connected, program me!");
    } else {
        Serial.println("I  am NOT connected, program me!");
    }
    
}
