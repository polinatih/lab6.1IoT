#include "ButtonLogic.h"

Debouncer::Debouncer(uint16_t stableMs, bool initialPressed)
    : stableMs(stableMs), stable(initialPressed), lastRaw(initialPressed),
      lastChangeMs(0), rawChanges(0), edges(0), pendingChange(false)
{
}

bool Debouncer::update(bool rawPressed, uint32_t nowMs)
{
    if (rawPressed != lastRaw)
    {
        // Сырой сигнал изменился: перезапускаем отсчёт стабильности.
        // Все изменения, не ставшие подтверждённым фронтом, — дребезг.
        rawChanges++;
        lastRaw = rawPressed;
        lastChangeMs = nowMs;
        pendingChange = (rawPressed != stable);
        return false;
    }
    if (pendingChange && (uint32_t)(nowMs - lastChangeMs) >= stableMs)
    {
        stable = lastRaw;
        pendingChange = false;
        edges++;
        return true;
    }
    return false;
}

PressDetector::PressDetector(uint16_t longPressMs)
    : longPressMs(longPressMs), wasPressed(false), longFired(false),
      pressedAtMs(0), lastDurationMs(0)
{
}

PressEvent PressDetector::update(bool pressed, uint32_t nowMs)
{
    PressEvent event = PRESS_NONE;
    if (pressed && !wasPressed)
    {
        pressedAtMs = nowMs;
        longFired = false;
    }
    else if (pressed && wasPressed)
    {
        if (!longFired && (uint32_t)(nowMs - pressedAtMs) >= longPressMs)
        {
            longFired = true;
            lastDurationMs = nowMs - pressedAtMs;
            event = PRESS_LONG;
        }
    }
    else if (!pressed && wasPressed)
    {
        lastDurationMs = nowMs - pressedAtMs;
        if (!longFired)
        {
            event = PRESS_SHORT;
        }
    }
    wasPressed = pressed;
    return event;
}
