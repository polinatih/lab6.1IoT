#include "Control.h"

#include <Arduino.h>
#include <AppConfig.h>
#include <Button.h>
#include <CommandParser.h>
#include <Fsm.h>
#include <Led.h>
#include <LedFsm.h>
#include <SerialStdio.h>

#include "Reporting.h"
#include "Signals.h"

static Button button(PIN_BUTTON, DEBOUNCE_MS, LONG_PRESS_MS);
static Led led(PIN_LED);
static Fsm fsm(LED_FSM_TABLE, LS_COUNT, LE_TIMEOUT);
static LineReader lineReader;

enum CommandId : uint8_t
{
    CMD_HELP,
    CMD_STATUS,
    CMD_PRESS,
    CMD_LONG,
    CMD_RESET,
    CMD_TIMEOUT
};

static const char C_HELP[] PROGMEM = "help";
static const char C_STATUS[] PROGMEM = "status";
static const char C_PRESS[] PROGMEM = "press";
static const char C_LONG[] PROGMEM = "long";
static const char C_RESET[] PROGMEM = "reset";
static const char C_TIMEOUT[] PROGMEM = "timeout";

static const CommandDef COMMANDS[] = {
    {C_HELP, CMD_HELP},   {C_STATUS, CMD_STATUS}, {C_PRESS, CMD_PRESS},
    {C_LONG, CMD_LONG},   {C_RESET, CMD_RESET},   {C_TIMEOUT, CMD_TIMEOUT},
};

static void publish(uint32_t nowMs)
{
    signals.state = fsm.getState();
    signals.previousState = fsm.getPreviousState();
    signals.lastEvent = fsm.getLastEvent();
    signals.transitions = fsm.getTransitionCount();
    signals.remainingMs = fsm.getRemaining(nowMs);
    signals.onTimeoutMs = fsm.getParam(LP_ON_TIMEOUT);
    signals.bounces = button.getBounceCount();
    signals.ledOn = led.isOn();
}

// Выход автомата Мура зависит только от состояния: при каждом переходе
// режим светодиода берётся из таблицы.
static void applyOutput(uint32_t nowMs)
{
    led.setMode((LedMode)fsm.getOutput(), nowMs);
}

static void handleEvent(uint8_t event, uint8_t source, uint32_t nowMs)
{
    signals.lastSource = source;
    if (fsm.dispatch(event, nowMs))
    {
        applyOutput(nowMs);
        publish(nowMs);
        reportingPrintTransition(nowMs);
    }
    else
    {
        publish(nowMs);
        reportingPrintIgnored(event, nowMs);
    }
}

const char *controlStateName(uint8_t state)
{
    return fsm.getStateName(state);
}

void controlInit(uint32_t nowMs)
{
    button.begin();
    led.begin();
    fsm.setParam(LP_ON_TIMEOUT, ON_TIMEOUT_MS);
    fsm.setParam(LP_WARN_TIME, WARN_TIME_MS);
    fsm.begin(LS_OFF, nowMs);
    applyOutput(nowMs);
    signals.lastSource = SRC_SYSTEM;
    publish(nowMs);
}

void controlPollButton(uint32_t nowMs)
{
    const PressEvent press = button.update(nowMs);
    if (press == PRESS_NONE)
    {
        return;
    }
    signals.presses++;
    signals.lastPressMs = button.getLastDurationMs();
    handleEvent(press == PRESS_LONG ? LE_LONG : LE_SHORT, SRC_BUTTON, nowMs);
}

void controlUpdate(uint32_t nowMs)
{
    if (fsm.update(nowMs))
    {
        signals.lastSource = SRC_TIMER;
        applyOutput(nowMs);
        publish(nowMs);
        reportingPrintTransition(nowMs);
    }
    led.update(nowMs);
    signals.remainingMs = fsm.getRemaining(nowMs);
    signals.ledOn = led.isOn();
}

static void executeCommand(const ParsedCommand &cmd, uint32_t nowMs)
{
    switch (cmd.id)
    {
    case CMD_HELP:
        reportingPrintHelp();
        break;
    case CMD_STATUS:
        publish(nowMs);
        reportingPrintStatus(nowMs);
        break;
    case CMD_PRESS:
        handleEvent(LE_SHORT, SRC_CONSOLE, nowMs);
        break;
    case CMD_LONG:
        handleEvent(LE_LONG, SRC_CONSOLE, nowMs);
        break;
    case CMD_RESET:
        handleEvent(LE_RESET, SRC_CONSOLE, nowMs);
        break;
    case CMD_TIMEOUT:
        if (cmd.hasArg && cmd.arg >= ON_TIMEOUT_MIN_S && cmd.arg <= ON_TIMEOUT_MAX_S)
        {
            fsm.setParam(LP_ON_TIMEOUT, (uint32_t)cmd.arg * 1000UL);
            publish(nowMs);
            PRINT_F("OK: ON timeout = %lu s\n", (unsigned long)cmd.arg);
        }
        else
        {
            PRINT_F("ERR: timeout <%d..%d> s\n", ON_TIMEOUT_MIN_S, ON_TIMEOUT_MAX_S);
        }
        break;
    case CMD_NONE:
        break;
    default:
        PRINT_F("ERR: unknown command, type 'help'\n");
        break;
    }
}

void controlPollConsole(uint32_t nowMs)
{
    while (serialStdioAvailable())
    {
        const int c = getchar();
        if (c == EOF)
        {
            break;
        }
        if (lineReader.feed((char)c))
        {
            const ParsedCommand cmd = parseCommand(
                lineReader.getLine(), COMMANDS, sizeof(COMMANDS) / sizeof(COMMANDS[0]));
            executeCommand(cmd, nowMs);
        }
    }
}
