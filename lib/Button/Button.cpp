#include "Button.h"

Button::Button(uint8_t pin, uint16_t debounceMs, uint16_t longPressMs)
    : pin(pin), debouncer(debounceMs, false), detector(longPressMs),
      pressedEdge(false)
{
}

void Button::begin()
{
    pinMode(pin, INPUT_PULLUP);
}

PressEvent Button::update(uint32_t nowMs)
{
    const bool raw = digitalRead(pin) == LOW;   // активный низкий уровень
    const bool changed = debouncer.update(raw, nowMs);
    pressedEdge = changed && debouncer.isPressed();
    return detector.update(debouncer.isPressed(), nowMs);
}
