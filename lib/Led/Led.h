// Led.h — драйвер светодиода с режимами ON/OFF/мигание (LedPattern).
#ifndef LED_H
#define LED_H

#include <Arduino.h>
#include <LedPattern.h>

class Led
{
public:
    explicit Led(uint8_t pin, bool activeHigh = true);

    void begin();
    void set(bool on);                         // прямое управление
    void setMode(LedMode mode, uint32_t nowMs); // режим с миганием
    void update(uint32_t nowMs);               // вызывать периодически

    LedMode getMode() const { return mode; }
    bool isOn() const { return level; }

private:
    void write(bool on);

    uint8_t pin;
    bool activeHigh;
    LedMode mode;
    uint32_t phaseStartMs;
    bool level;
};

#endif
