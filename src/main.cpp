// ЛР 6.1 — Поведенческое управление с конечными автоматами: Button-LED.
// Arduino Uno, кооперативное планирование по millis() без delay().
#include <Arduino.h>
#include <AppConfig.h>
#include <SerialStdio.h>

#include "Control.h"
#include "Reporting.h"

static uint32_t lastButtonMs = 0;
static uint32_t lastDisplayMs = 0;

void setup()
{
    serialStdioInit(SERIAL_BAUD);
    reportingInit();
    const uint32_t now = millis();
    controlInit(now);
    reportingPrintBanner();
    reportingPrintStatus(now);
    reportingUpdateDisplay(now);
}

void loop()
{
    const uint32_t now = millis();

    if (now - lastButtonMs >= BUTTON_POLL_MS)
    {
        lastButtonMs = now;
        controlPollButton(now);
    }

    controlPollConsole(now);
    controlUpdate(now);

    if (now - lastDisplayMs >= DISPLAY_PERIOD_MS)
    {
        lastDisplayMs = now;
        reportingUpdateDisplay(now);
    }
}
