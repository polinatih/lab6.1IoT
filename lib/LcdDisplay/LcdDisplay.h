// LcdDisplay.h — обёртка LCD 16x2 с I2C-преобразователем PCF8574.
// Строки дополняются пробелами до ширины экрана, а запись выполняется
// только при изменении текста, что убирает мерцание и экономит время шины.
#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#define LCD_COLS 16
#define LCD_ROWS 2

class LcdDisplay
{
public:
    LcdDisplay(uint8_t address);
    void begin();
    void printLine(uint8_t row, const char *text);

private:
    LiquidCrystal_I2C lcd;
    char shown[LCD_ROWS][LCD_COLS + 1];
};

#endif
