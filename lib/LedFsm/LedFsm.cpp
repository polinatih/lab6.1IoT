#include "LedFsm.h"

static const char NAME_OFF[] PROGMEM = "OFF";
static const char NAME_ON[] PROGMEM = "ON";
static const char NAME_BLINK[] PROGMEM = "BLINK";
static const char NAME_WARN[] PROGMEM = "WARN";

#define S FSM_STAY

// Таблица переходов автомата Мура.
// Столбцы next[]:            SHORT     LONG      TIMEOUT   RESET
const FsmState LED_FSM_TABLE[LS_COUNT] PROGMEM = {
    {NAME_OFF, LED_MODE_OFF, FSM_NO_TIMEOUT,
                              {LS_ON,    LS_BLINK, S,        LS_OFF, S, S}},
    {NAME_ON, LED_MODE_ON, FSM_PARAM(LP_ON_TIMEOUT),
                              {LS_OFF,   LS_BLINK, LS_WARN,  LS_OFF, S, S}},
    {NAME_BLINK, LED_MODE_BLINK_SLOW, FSM_NO_TIMEOUT,
                              {LS_OFF,   LS_ON,    S,        LS_OFF, S, S}},
    {NAME_WARN, LED_MODE_BLINK_FAST, FSM_PARAM(LP_WARN_TIME),
                              {LS_ON,    LS_BLINK, LS_OFF,   LS_OFF, S, S}},
};

#undef S

static const char EV_SHORT[] PROGMEM = "SHORT";
static const char EV_LONG[] PROGMEM = "LONG";
static const char EV_TIMEOUT[] PROGMEM = "TIMEOUT";
static const char EV_RESET[] PROGMEM = "RESET";
static const char EV_INIT[] PROGMEM = "INIT";

const char *ledFsmEventName(uint8_t event)
{
    switch (event)
    {
    case LE_SHORT:
        return EV_SHORT;
    case LE_LONG:
        return EV_LONG;
    case LE_TIMEOUT:
        return EV_TIMEOUT;
    case LE_RESET:
        return EV_RESET;
    default:
        return EV_INIT;
    }
}
