// LedPattern.h — режимы свечения светодиода и вычисление уровня по времени.
// Чистая логика без обращения к выводам (тестируется на ПК).
#ifndef LED_PATTERN_H
#define LED_PATTERN_H

#include <stdint.h>

enum LedMode : uint8_t
{
    LED_MODE_OFF = 0,
    LED_MODE_ON,
    LED_MODE_BLINK_SLOW,   // 1 Гц  (500 мс вкл / 500 мс выкл)
    LED_MODE_BLINK_FAST    // 4 Гц  (125 мс вкл / 125 мс выкл)
};

static const uint16_t LED_SLOW_HALF_PERIOD_MS = 500;
static const uint16_t LED_FAST_HALF_PERIOD_MS = 125;

// Уровень светодиода в момент nowMs; фаза мигания отсчитывается от
// phaseStartMs, чтобы мигание всегда начиналось с «включено».
inline bool ledPatternLevel(LedMode mode, uint32_t nowMs, uint32_t phaseStartMs)
{
    const uint32_t t = nowMs - phaseStartMs;
    switch (mode)
    {
    case LED_MODE_ON:
        return true;
    case LED_MODE_BLINK_SLOW:
        return ((t / LED_SLOW_HALF_PERIOD_MS) & 1UL) == 0;
    case LED_MODE_BLINK_FAST:
        return ((t / LED_FAST_HALF_PERIOD_MS) & 1UL) == 0;
    case LED_MODE_OFF:
    default:
        return false;
    }
}

#endif
