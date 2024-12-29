
#define BLYNK_TEMPLATE_ID "TMPL6oTAALs3i"
#define BLYNK_TEMPLATE_NAME "Backyard Piggery"
#define BLYNK_AUTH_TOKEN "9m5XLGdZ6dQTm8fXzHY88WwK9jQ1NfwC"

#define BLYNK_PRINT Serial

#include <Arduino.h>
#include <DHT11.h>
#include <MQ135.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <LiquidCrystal_I2C.h>

const char* ssid = "Hotspot_ko";
const char* pass = "abcdefghij";
// const char* ssid = "LUNA BOARDING HOUSE PISO WIFI";
// const char* pass = "";

bool wifi_connected = false;
bool blynk_connected = false;
int temperature = 0;
int humidity = 0;

BlynkTimer timer;
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT11 dht(D6);
MQ135 mq135_sensor(A0);

void lcdPrinter(int cursor, int row, String text) {
    lcd.setCursor(cursor, row); // Assuming you want to set the cursor to a specific column on row 0
    lcd.print(text);
}

void readDisplaySensorData() {

    int result = dht.readTemperatureHumidity(temperature, humidity);

    lcdPrinter(0,1,"                ");

    lcdPrinter(0,1,"T:");
    lcdPrinter(4,1,"C");

    lcdPrinter(6,1,"H:");
    lcdPrinter(10,1,"%");

    if (result == 0) {

        lcdPrinter(2,1,String(temperature));
        Blynk.virtualWrite(V0, temperature);

        lcdPrinter(8,1,String(humidity));
        Blynk.virtualWrite(V1, humidity);

    } else {
        lcd.clear();
        lcdPrinter(0,1,"dht11 sensor error");
    }
    
    // mq135 sensor

    // float rzero = mq135_sensor.getRZero();
    // float correctedRZero = mq135_sensor.getCorrectedRZero(temperature, humidity);
    // float resistance = mq135_sensor.getResistance();
    // float ppm = mq135_sensor.getPPM();
    float correctedPPM = mq135_sensor.getCorrectedPPM(temperature, humidity);
    
    int odor_level = map( round(correctedPPM), 5, 500, 0, 10);

    lcdPrinter(12,1,"O:");

    if (correctedPPM){
        lcdPrinter(14,1,String(odor_level));
        Blynk.sendInternal("A0", correctedPPM);
    }
    
}

void connectToWifiBlynk() {

    if (WiFi.status() != WL_CONNECTED) { // if not connected to wifi

        if (wifi_connected){
            // nofication
            lcd.clear();
            lcdPrinter(0,0,"WiFi DISCONNECTED!");
            wifi_connected = false;
            blynk_connected = false;
        }

        // try to reconnect
        WiFi.reconnect();
        Serial.print("reconnecting ");

        return; // wait for the next iteration
    } 

    if (!wifi_connected) {
        lcd.clear();
        lcdPrinter(0,0,"WiFi CONNECTED!");
        wifi_connected = true;
    }

    // code for connected wifi

    if (!Blynk.connected())
    {
        if (blynk_connected) {
            Serial.println("Blynk connection Lost, reconnecting.");
            blynk_connected = false;
        }
        
        if (Blynk.connect()) {
            Serial.println("Connected to Blynk!");
            blynk_connected = true;
        } else {
            Serial.println("Failed to connect to Blynk");
            blynk_connected = false;
        }
        
        return;

    }

    if (!blynk_connected){
        Serial.println("Blynk is online again!");
        blynk_connected = true;
        return;
    }
        
}

void topBar() {

    lcdPrinter(0,0,"                ");

    lcd.setCursor(9,0);
    if(wifi_connected == false) {
        lcd.print("W:0");
    } else {
        lcd.print("W:1");
    }

    lcd.setCursor(13,0);
    if(blynk_connected == false) {
        lcd.print("B:0");
    } else {
        lcd.print("B:1");
    }

}
    

void setup() {
    
    Serial.begin(115200);
    delay(10);

    lcd.init();
    lcd.clear();
    lcd.backlight();

    lcdPrinter(1,0,"Welcome");

    lcdPrinter(0,1,"PIGGERY MONITOR");

     // Set the ESP8266 to STA mode
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);

    // blynk init
    Blynk.config(BLYNK_AUTH_TOKEN);

    timer.setInterval(5000L, connectToWifiBlynk);
    timer.setInterval(5000L, readDisplaySensorData);
    timer.setInterval(5000L, topBar);
}

void loop()
{
    if (Blynk.connected()) {
        Blynk.run();
    }
    timer.run();
}
