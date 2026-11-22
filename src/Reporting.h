// Reporting.h — отчёт о состоянии автомата: LCD 16x2 и консоль (printf).
#ifndef REPORTING_H
#define REPORTING_H

#include <stdint.h>

void reportingInit();
void reportingPrintBanner();
void reportingPrintHelp();
void reportingPrintTransition(uint32_t nowMs);
void reportingPrintIgnored(uint8_t event, uint32_t nowMs);
void reportingPrintStatus(uint32_t nowMs);
void reportingUpdateDisplay(uint32_t nowMs);

#endif
