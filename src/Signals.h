// Signals.h — общее состояние приложения (модель provider/consumer):
// Control записывает, Reporting читает.
#ifndef SIGNALS_H
#define SIGNALS_H

#include <stdint.h>

enum EventSource : uint8_t
{
    SRC_BUTTON = 0,
    SRC_CONSOLE,
    SRC_TIMER,
    SRC_SYSTEM
};

struct AppSignals
{
    uint8_t state;            // текущее состояние автомата
    uint8_t previousState;
    uint8_t lastEvent;
    uint8_t lastSource;
    uint16_t transitions;     // число переходов с момента запуска
    uint32_t remainingMs;     // до тайм-аута текущего состояния (0 — нет)
    uint32_t onTimeoutMs;     // настройка тайм-аута ON
    uint32_t lastPressMs;     // длительность последнего нажатия
    uint16_t bounces;         // отфильтровано дребезга
    uint16_t presses;         // принято нажатий
    bool ledOn;               // фактический уровень вывода LED
};

extern AppSignals signals;

#endif
