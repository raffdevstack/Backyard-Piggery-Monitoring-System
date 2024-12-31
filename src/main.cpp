
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h> // better lcd communication


LiquidCrystal_I2C lcd(0x27, 16, 2);

void lcdPrinter(int cursor, int row, String text) {
    lcd.setCursor(cursor, row); // Assuming you want to set the cursor to a specific column on row 0
    lcd.print(text);
}

void setup() {
    

    lcd.init();
    lcd.clear();
    lcd.backlight();

    lcdPrinter(1,0,"Welcome");

    lcdPrinter(0,1,"PIGGERY MONITOR");

}

void loop() {
    
}