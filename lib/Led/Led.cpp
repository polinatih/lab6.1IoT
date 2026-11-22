#include "Led.h"

Led::Led(uint8_t pin, bool activeHigh)
    : pin(pin), activeHigh(activeHigh), mode(LED_MODE_OFF), phaseStartMs(0),
      level(false)
{
}

void Led::begin()
{
    pinMode(pin, OUTPUT);
    write(false);
}

void Led::write(bool on)
{
    level = on;
    digitalWrite(pin, (on == activeHigh) ? HIGH : LOW);
}

void Led::set(bool on)
{
    mode = on ? LED_MODE_ON : LED_MODE_OFF;
    write(on);
}

void Led::setMode(LedMode newMode, uint32_t nowMs)
{
    mode = newMode;
    phaseStartMs = nowMs;
    write(ledPatternLevel(mode, nowMs, phaseStartMs));
}

void Led::update(uint32_t nowMs)
{
    const bool wanted = ledPatternLevel(mode, nowMs, phaseStartMs);
    if (wanted != level)
    {
        write(wanted);
    }
}
