// Button.h — драйвер кнопки: вывод в режиме INPUT_PULLUP (нажатие = LOW),
// антидребезг и распознавание короткого/длинного нажатия (ButtonLogic).
#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>
#include <ButtonLogic.h>

class Button
{
public:
    Button(uint8_t pin, uint16_t debounceMs, uint16_t longPressMs);

    void begin();
    // Вызывать периодически (каждые несколько мс). Возвращает событие.
    PressEvent update(uint32_t nowMs);
    // true однократно после подтверждённого нажатия (фронт)
    bool wasPressedEdge() const { return pressedEdge; }

    bool isPressed() const { return debouncer.isPressed(); }
    uint16_t getBounceCount() const { return debouncer.getBounceCount(); }
    uint32_t getLastDurationMs() const { return detector.getLastDurationMs(); }

private:
    uint8_t pin;
    Debouncer debouncer;
    PressDetector detector;
    bool pressedEdge;
};

#endif
