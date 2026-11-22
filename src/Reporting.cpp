#include "Reporting.h"

#include <Arduino.h>
#include <AppConfig.h>
#include <LcdDisplay.h>
#include <LedFsm.h>
#include <SerialStdio.h>
#include <stdio.h>

#include "Control.h"
#include "Signals.h"

static LcdDisplay display(LCD_I2C_ADDRESS);

static const char SRC_BTN[] PROGMEM = "BTN";
static const char SRC_CMD[] PROGMEM = "CMD";
static const char SRC_TMR[] PROGMEM = "TMR";
static const char SRC_SYS[] PROGMEM = "SYS";

static const char *sourceName(uint8_t source)
{
    switch (source)
    {
    case SRC_BUTTON:
        return SRC_BTN;
    case SRC_CONSOLE:
        return SRC_CMD;
    case SRC_TIMER:
        return SRC_TMR;
    default:
        return SRC_SYS;
    }
}

// Метка времени [секунды.миллисекунды] без printf для float
static void printTime(uint32_t nowMs)
{
    PRINT_F("[%5lu.%03u] ", (unsigned long)(nowMs / 1000UL), (unsigned)(nowMs % 1000UL));
}

void reportingInit()
{
    display.begin();
}

void reportingPrintBanner()
{
    PRINT_F("\nLab 6.1 FSM Button-LED (states: OFF, ON, BLINK, WARN)\n");
    PRINT_F("short press: OFF<->ON, long press (>=%u ms): BLINK\n", LONG_PRESS_MS);
    PRINT_F("Type 'help' for commands\n");
}

void reportingPrintHelp()
{
    PRINT_F("Commands:\n");
    PRINT_F("  status       - current state and counters\n");
    PRINT_F("  press        - emulate short press\n");
    PRINT_F("  long         - emulate long press\n");
    PRINT_F("  reset        - go to OFF\n");
    PRINT_F("  timeout <s>  - ON auto-off timeout (%d..%d s)\n", ON_TIMEOUT_MIN_S, ON_TIMEOUT_MAX_S);
}

void reportingPrintTransition(uint32_t nowMs)
{
    printTime(nowMs);
    PRINT_F("%-5S -> %-5S ev=%-7S src=%S", controlStateName(signals.previousState),
            controlStateName(signals.state), ledFsmEventName(signals.lastEvent),
            sourceName(signals.lastSource));
    if (signals.lastSource == SRC_BUTTON)
    {
        PRINT_F(" press=%lums", (unsigned long)signals.lastPressMs);
    }
    PRINT_F(" LED=%S n=%u\n", signals.ledOn ? PSTR("1") : PSTR("0"), signals.transitions);
}

void reportingPrintIgnored(uint8_t event, uint32_t nowMs)
{
    printTime(nowMs);
    PRINT_F("%-5S : ev=%S ignored\n", controlStateName(signals.state), ledFsmEventName(event));
}

void reportingPrintStatus(uint32_t nowMs)
{
    printTime(nowMs);
    PRINT_F("state=%S LED=%u transitions=%u presses=%u bounces=%u\n",
            controlStateName(signals.state), signals.ledOn ? 1 : 0, signals.transitions,
            signals.presses, signals.bounces);
    PRINT_F("           ON timeout=%lu s, remaining=%lu ms\n",
            (unsigned long)(signals.onTimeoutMs / 1000UL), (unsigned long)signals.remainingMs);
}

void reportingUpdateDisplay(uint32_t nowMs)
{
    (void)nowMs;
    char line[LCD_COLS + 1];
    snprintf_P(line, sizeof(line), PSTR("State:%-5S #%u"), controlStateName(signals.state),
               signals.transitions);
    display.printLine(0, line);

    const unsigned long secondsLeft = (signals.remainingMs + 999UL) / 1000UL;
    switch (signals.state)
    {
    case LS_ON:
        snprintf_P(line, sizeof(line), PSTR("LED on  off:%lus"), secondsLeft);
        break;
    case LS_WARN:
        snprintf_P(line, sizeof(line), PSTR("WARN! off in %lus"), secondsLeft);
        break;
    case LS_BLINK:
        snprintf_P(line, sizeof(line), PSTR("Blink 1Hz"));
        break;
    default:
        snprintf_P(line, sizeof(line), PSTR("Short:ON Long:BL"));
        break;
    }
    display.printLine(1, line);
}
