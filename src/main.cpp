
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
DHT11 dht(2); // d4

BLYNK_CONNECTED()
{
    Serial.println("CONNECTED TO BLYNK");
}

void dhtSensor() {

    Serial.println("hello from sensor");

    int result = dht.readTemperatureHumidity(temperature, humidity);

    if (result == 0) {
        Blynk.virtualWrite(V0, temperature);
        Blynk.virtualWrite(V1, humidity);
        Serial.println(temperature);
        Serial.println(humidity);

        lcd.setCursor(6,0);
        lcd.print(temperature);

        lcd.setCursor(14,0);
        lcd.print(humidity);
    } 
}

void connectToWifiBlynk() {

    if (WiFi.status() != WL_CONNECTED) { // if not connected to wifi

        Serial.print("connecting ");

        if (wifi_connected){
            lcd.clear();
            lcd.home();
            lcd.print("WiFi DISCONNECTED!");
        }

        wifi_connected = false;

        WiFi.begin(ssid, pass);

    } else {

        Serial.println();
        Serial.println("Connected to WiFi!");

        if (!wifi_connected){
            lcd.clear();
            lcd.home();
            lcd.print("WiFi CONNECTED");
        }
        
        wifi_connected = true;

// blynk connection
        if (!Blynk.connected()) {

            Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass); 

            Serial.print("connecting to blynk");

            if (blynk_connected){
            lcd.clear();
            lcd.home();
            lcd.print("Blynk DISCONNECTED");
        
        } else {
            Serial.println("Connected to Blynk!");
            
            if (!blynk_connected){
            lcd.clear();
            lcd.home();
            lcd.print("Blynk CONNECTED");
        }
        

    }
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

    connectToWifiBlynk();

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

        connectToWifiBlynk();
    }

}
