
#define BLYNK_TEMPLATE_ID "TMPL6oTAALs3i"
#define BLYNK_TEMPLATE_NAME "Backyard Piggery"
#define BLYNK_AUTH_TOKEN "9m5XLGdZ6dQTm8fXzHY88WwK9jQ1NfwC"

#define BLYNK_PRINT Serial

#include <Arduino.h>
#include <DHT11.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <LiquidCrystal_I2C.h>

const char* ssid = "Hotspot_ko";
const char* pass = "abcdefghij";
// const char* ssid = "LUNA BOARDING HOUSE PISO WIFI";
// const char* pass = "";

bool wifi_connected = false;
bool blynk_connected = false;
unsigned long previousMillis = 0; // Stores the last time an action occurred
int temperature = 0;
int humidity = 0;

BlynkTimer timer;
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT11 dht(D6); // d4

BLYNK_CONNECTED()
{
    //
}

void runDhtSensor() {

    int result = dht.readTemperatureHumidity(temperature, humidity);

    lcd.clear();

    lcd.setCursor(0,1);
    lcd.print("T: __");
    lcd.setCursor(5,1);
    lcd.print("C");

    lcd.setCursor(9,1);
    lcd.print("H: __");
    lcd.setCursor(14,1);
    lcd.print("%");

    if (result == 0) {

        lcd.setCursor(3,1);
        lcd.print(temperature);

        lcd.setCursor(12,1);
        lcd.print(humidity);

    } else {
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.print("sensor error");
    }
}

void connectToWifiBlynk() {

    if (WiFi.status() != WL_CONNECTED && wifi_connected) { // if not connected to wifi

        lcd.home();
        lcd.print("WiFi DISCONNECTED!");
        wifi_connected = false;
        blynk_connected = false;
        
        WiFi.begin(ssid, pass);

        Serial.print("connecting ");

    } else if (WiFi.status() == WL_CONNECTED && !wifi_connected) {

        lcd.home();
        lcd.print("WiFi CONNECTED");
        wifi_connected = true;

        Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass); 
        
        // blynk connection
        if (!Blynk.connected() && blynk_connected) {

            lcd.home();
            lcd.print("Blynk DISCONNECTED");
            blynk_connected = false;
            
            // try to connect again
            Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass); 
        
        } else if (Blynk.connected() && !blynk_connected) {
            
            lcd.home();
            lcd.print("Blynk CONNECTED");
            blynk_connected = true;
    
        }
    }
}
    

void setup() {
    Serial.begin(115200);
    delay(10);

    lcd.init();
    lcd.clear();
    lcd.backlight();

    lcd.setCursor(1,0);
    lcd.print("Welcome!");

    lcd.setCursor(0,1);
    lcd.print("PIGGERY MONITOR");

     // Set the ESP8266 to STA mode
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);

    timer.setInterval(5000L, connectToWifiBlynk);
    timer.setInterval(2000L, runDhtSensor);
}

void loop()
{
    Blynk.run();
    timer.run();
}
