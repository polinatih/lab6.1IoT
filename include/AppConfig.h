// AppConfig.h — все константы ЛР 6.1 (выводы, времена, интерфейсы).
#ifndef APP_CONFIG_H
#define APP_CONFIG_H

// ---- Выводы Arduino Uno ----
#define PIN_BUTTON          2     // кнопка S1 на GND, INPUT_PULLUP
#define PIN_LED             7     // LED1 через резистор 220 Ом

// ---- Последовательный порт (STDIO) ----
#define SERIAL_BAUD         9600

// ---- LCD 16x2 через PCF8574 (A4 = SDA, A5 = SCL) ----
#define LCD_I2C_ADDRESS     0x27

// ---- Кнопка ----
#define BUTTON_POLL_MS      5      // период опроса кнопки
#define DEBOUNCE_MS         40     // время стабильности уровня
#define LONG_PRESS_MS       1000   // порог длинного нажатия

// ---- Конечный автомат ----
#define ON_TIMEOUT_MS       30000UL   // ON -> WARN без нажатий (авто-таймаут)
#define WARN_TIME_MS        5000UL    // длительность предупреждения WARN
#define ON_TIMEOUT_MIN_S    5
#define ON_TIMEOUT_MAX_S    600

// ---- Отображение ----
#define DISPLAY_PERIOD_MS   250

#endif
