// Control.h — управление: кнопка -> события автомата -> светодиод,
// команды консоли.
#ifndef CONTROL_H
#define CONTROL_H

#include <stdint.h>

void controlInit(uint32_t nowMs);
void controlPollButton(uint32_t nowMs);   // каждые BUTTON_POLL_MS
void controlPollConsole(uint32_t nowMs);  // на каждом проходе loop()
void controlUpdate(uint32_t nowMs);       // тайм-ауты автомата и мигание LED

const char *controlStateName(uint8_t state);

#endif
