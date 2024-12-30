
#define BLYNK_TEMPLATE_ID "TMPL6oTAALs3i"
#define BLYNK_TEMPLATE_NAME "Backyard Piggery"
#define BLYNK_AUTH_TOKEN "9m5XLGdZ6dQTm8fXzHY88WwK9jQ1NfwC"

#define BLYNK_PRINT Serial
#define RELAY_LIGHT 14  // D5 (GPIO14) for Light
#define RELAY_FAN 13    // D7 (GPIO13) for Fan

// for time
#define NTP_SERVER     "pool.ntp.org"
#define UTC_OFFSET     8 * 3600
#define UTC_OFFSET_DST 0 0

#include <Arduino.h>
#include <DHT11.h>
#include <MQ135.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h> // better lcd communication
#include <NTPClient.h>
#include <WiFiUdp.h>

const char* ssid = "Hotspot_ko";
const char* pass = "abcdefghij";
// const char* ssid = "LUNA BOARDING HOUSE PISO WIFI";
// const char* pass = "";

bool wifi_connected = false;
bool blynk_connected = false;
int temperature = 0;
int humidity = 0;
double heat_index_celsius = 0;
int currentHour = 0;

BlynkTimer timer;
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT11 dht(D6);
MQ135 mq135_sensor(A0);
WiFiUDP udp;
NTPClient timeClient(udp, NTP_SERVER, UTC_OFFSET, 60000);  // Update every 60 seconds

// for later use
void spinner() {
    static int8_t counter = 0;
    const char* glyphs = "\xa1\xa5\xdb";
    lcd.setCursor(15, 1);
    lcd.print(glyphs[counter++]);
    if (counter == strlen(glyphs)) {
        counter = 0;
    }
}

String getLocalTime() {
    timeClient.update();
    unsigned long epochTime = timeClient.getEpochTime();
    time_t rawTime = (time_t)epochTime;
    
    struct tm timeinfo;
    if (localtime_r(&rawTime, &timeinfo) == NULL) {
        return "Error";
    }

    int localHour = timeinfo.tm_hour % 24;  // Ensures 0-23 range
    
    char timeStr[9];
    sprintf(timeStr, "%02d:%02d:%02d", localHour, timeinfo.tm_min, timeinfo.tm_sec);
    return String(timeStr);
    // This will give you time in 24-hour format (00:00:00 to 23:59:59).
}

void lcdPrinter(int cursor, int row, String text) {
    lcd.setCursor(cursor, row); // Assuming you want to set the cursor to a specific column on row 0
    lcd.print(text);
}

void automateLightAndFan() {
    // automate fan and light based on heatIndex
    if (heat_index_celsius > 45) {
        digitalWrite(RELAY_FAN, LOW); // turn on
    } else {
        digitalWrite(RELAY_FAN, HIGH); // turn off
    }

    if (heat_index_celsius < 25) {
        digitalWrite(RELAY_LIGHT, LOW); // turn on
    } else {
        digitalWrite(RELAY_LIGHT, HIGH); // turn off
    }

    // automate light based on time
    if (wifi_connected){
        
        currentHour = getLocalTime().substring(0,2).toInt();

        if (currentHour >= 18)  // Check if hour is 6pm or later
        {
            digitalWrite(RELAY_LIGHT, LOW); // turn on
        } else {
             digitalWrite(RELAY_LIGHT, HIGH); // turn oFF
        }
    } else {
        digitalWrite(RELAY_LIGHT, HIGH); // turn oFF
    }
    
    
}

double calculateHeatIndexCelsius(double temperatureC, double humidity) {
    // Ensure inputs are valid
    if (temperatureC < 0 || humidity < 0 || humidity > 100) {
        Serial.println("Temperature must be non-negative and humidity must be between 0 and 100.");
    }

    // No heat index calculation for temperatures below 27°C
    if (temperatureC < 27) {
        return temperatureC;
    }

    // Convert Celsius to Fahrenheit
    double temperatureF = (temperatureC * 9.0) / 5.0 + 32.0;

    // Heat index formula in Fahrenheit
    double T = temperatureF;
    double RH = humidity;

    double heatIndexF = 
        -42.379 +
        2.04901523 * T +
        10.14333127 * RH -
        0.22475541 * T * RH -
        6.83783e-3 * T * T -
        5.481717e-2 * RH * RH +
        1.22874e-3 * T * T * RH +
        8.5282e-4 * T * RH * RH -
        1.99e-6 * T * T * RH * RH;

    // Convert the result back to Celsius
    double heatIndexC = ((heatIndexF - 32.0) * 5.0) / 9.0;

    // Round to 2 decimal places
    return round(heatIndexC * 100.0) / 100.0;
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

        heat_index_celsius =  calculateHeatIndexCelsius(temperature, humidity);
        Blynk.virtualWrite(V2, heat_index_celsius);
        
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
    
    int odor_level = map( round(correctedPPM), 5, 200, 0, 10);

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

        timeClient.begin();
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

    pinMode(RELAY_LIGHT, OUTPUT);  // Set GPIO14 as output
    pinMode(RELAY_FAN, OUTPUT);    // Set GPIO13 as output
    // Start with relays off
    digitalWrite(RELAY_LIGHT, HIGH);  // Start with the relay OFF for my ACTIVE LOW RELAY
    digitalWrite(RELAY_FAN, HIGH); // Start with the relay OFF for my ACTIVE LOW RELAY

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
    timer.setInterval(5000L, automateLightAndFan);
}

void loop()
{
    if (Blynk.connected()) {
        Blynk.run();
    }
    timer.run();
}
