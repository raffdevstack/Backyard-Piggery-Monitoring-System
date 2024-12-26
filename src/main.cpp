
#define BLYNK_TEMPLATE_ID "TMPL6oTAALs3i"
#define BLYNK_TEMPLATE_NAME "Backyard Piggery"
#define BLYNK_AUTH_TOKEN "9m5XLGdZ6dQTm8fXzHY88WwK9jQ1NfwC"

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <LiquidCrystal_I2C.h>

const char* ssid = "Hotspot_ko";
const char* pass = "abcdefghij";
// const char* ssid = "LUNA BOARDING HOUSE PISO WIFI";
// const char* pass = "";

bool wifi_connected = false;
unsigned long previousMillis = 0; // Stores the last time an action occurred

BlynkTimer timer;
LiquidCrystal_I2C lcd(0x27, 16, 2);

BLYNK_CONNECTED()
{
    Serial.println("CONNECTED TO BLYNK");
}

void connectToWifi() {

    if (WiFi.status() != WL_CONNECTED) {
        Serial.print("connecting ");
        lcd.setCursor(5,1);
        lcd.print("0");
        wifi_connected = false;
        WiFi.begin(ssid, pass);
    } else {
        Serial.println();
        Serial.println("Connected to WiFi!");
        lcd.setCursor(5,1);
        lcd.print("1");
        wifi_connected = true;
    }
}

void setup()
{
    Serial.begin(115200);
    delay(10);

    lcd.init();
    lcd.clear();
    lcd.backlight();

// first row
    lcd.setCursor(0,0);
    lcd.print("T:");
    lcd.setCursor(4,0);
    lcd.print("C");
    lcd.setCursor(6,0);
    lcd.print("H:");
    lcd.setCursor(10,0);
    lcd.print("%");
    lcd.setCursor(12,0);
    lcd.print("L:");

//  second row
    lcd.setCursor(0,1);
    lcd.print("WiFi:");
    lcd.setCursor(8,1);
    lcd.print("BLYNK:");

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
