// ButtonLogic.h — платформенно-независимая обработка кнопки:
//   Debouncer     — фильтр дребезга контактов по времени стабильности;
//   PressDetector — классификация нажатий: короткое (по отпусканию)
//                   и длинное (по удержанию дольше порога).
// Модуль не обращается к выводам, поэтому проверяется тестами на ПК.
#ifndef BUTTON_LOGIC_H
#define BUTTON_LOGIC_H

#include <stdint.h>

class Debouncer
{
public:
    // stableMs — сколько уровень должен держаться без изменений,
    // чтобы считаться достоверным.
    explicit Debouncer(uint16_t stableMs, bool initialPressed = false);

    // rawPressed — «сырое» состояние контакта. Возвращает true, если
    // отфильтрованное состояние изменилось в этом вызове.
    bool update(bool rawPressed, uint32_t nowMs);

    bool isPressed() const { return stable; }
    // Число отброшенных «дребезговых» переключений сырого сигнала
    uint16_t getBounceCount() const { return (uint16_t)(rawChanges - edges); }
    uint16_t getEdgeCount() const { return edges; }
    void setStableTime(uint16_t ms) { stableMs = ms; }

private:
    uint16_t stableMs;
    bool stable;
    bool lastRaw;
    uint32_t lastChangeMs;
    uint16_t rawChanges;
    uint16_t edges;
    bool pendingChange;
};

enum PressEvent : uint8_t
{
    PRESS_NONE = 0,
    PRESS_SHORT,    // нажали и отпустили быстрее порога
    PRESS_LONG      // удерживают дольше порога (выдаётся один раз)
};

class PressDetector
{
public:
    explicit PressDetector(uint16_t longPressMs);

    // pressed — уже отфильтрованное состояние (выход Debouncer)
    PressEvent update(bool pressed, uint32_t nowMs);

    uint32_t getLastDurationMs() const { return lastDurationMs; }
    void setLongPressTime(uint16_t ms) { longPressMs = ms; }

private:
    uint16_t longPressMs;
    bool wasPressed;
    bool longFired;
    uint32_t pressedAtMs;
    uint32_t lastDurationMs;
};

#endif
