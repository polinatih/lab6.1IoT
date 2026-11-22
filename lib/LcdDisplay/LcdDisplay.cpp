#include "LcdDisplay.h"

#include <string.h>

LcdDisplay::LcdDisplay(uint8_t address) : lcd(address, LCD_COLS, LCD_ROWS)
{
    for (uint8_t r = 0; r < LCD_ROWS; r++)
    {
        shown[r][0] = '\0';
    }
}

void LcdDisplay::begin()
{
    lcd.init();
    lcd.backlight();
    lcd.clear();
}

void LcdDisplay::printLine(uint8_t row, const char *text)
{
    if (row >= LCD_ROWS)
    {
        return;
    }
    char padded[LCD_COLS + 1];
    uint8_t i = 0;
    for (; i < LCD_COLS && text[i] != '\0'; i++)
    {
        padded[i] = text[i];
    }
    for (; i < LCD_COLS; i++)
    {
        padded[i] = ' ';
    }
    padded[LCD_COLS] = '\0';

    if (strcmp(padded, shown[row]) == 0)
    {
        return;
    }
    strcpy(shown[row], padded);
    lcd.setCursor(0, row);
    lcd.print(padded);
}
