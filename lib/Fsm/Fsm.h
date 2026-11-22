// Fsm.h — универсальный табличный конечный автомат Мура.
//
// Автомат описывается массивом FsmState, который хранится во флеш-памяти
// (PROGMEM на AVR): у каждого состояния есть имя, выход (код, который
// приложение переводит в уровни выводов), тайм-аут и таблица переходов
// next[событие]. Движок не знает ничего об оборудовании, поэтому один и тот
// же модуль используется в ЛР 6.1 (кнопка-светодиод) и в ЛР 6.2 (светофор),
// а также проверяется модульными тестами на ПК.
#ifndef FSM_H
#define FSM_H

#include <stdint.h>
#include <string.h>

#ifdef __AVR__
#include <avr/pgmspace.h>
#else
#ifndef PROGMEM
#define PROGMEM
#endif
#ifndef memcpy_P
#define memcpy_P memcpy
#endif
#endif

// Максимальное число событий (столбцов таблицы переходов)
#ifndef FSM_MAX_EVENTS
#define FSM_MAX_EVENTS 6
#endif

#define FSM_MAX_PARAMS 3

// Значение next[] «остаться в состоянии, событие игнорируется»
static const uint8_t FSM_STAY = 0xFF;
// Тайм-аут отсутствует
static const uint32_t FSM_NO_TIMEOUT = 0UL;
// Тайм-аут берётся из настраиваемого параметра экземпляра автомата:
// одна таблица может обслуживать несколько автоматов с разными временами.
#define FSM_PARAM(i) (0xFFFFFF00UL | (uint32_t)(i))

struct FsmState
{
    const char *name;               // имя для отчёта (строка PROGMEM на AVR)
    uint8_t output;                 // выход автомата Мура
    uint32_t timeoutMs;             // мс, FSM_NO_TIMEOUT или FSM_PARAM(i)
    uint8_t next[FSM_MAX_EVENTS];   // следующее состояние для каждого события
};

class Fsm
{
public:
    // timeoutEvent — номер события, которое движок генерирует сам по
    // истечении времени пребывания в состоянии.
    Fsm(const FsmState *table, uint8_t stateCount, uint8_t timeoutEvent);

    void begin(uint8_t initialState, uint32_t nowMs);

    // Подать событие. Возвращает true, если произошёл переход
    // (в том числе переход в то же состояние — он перезапускает таймер).
    bool dispatch(uint8_t event, uint32_t nowMs);

    // Проверить тайм-аут текущего состояния; true — был переход.
    bool update(uint32_t nowMs);

    // Принудительная установка состояния (сброс, аварийный режим).
    void forceState(uint8_t state, uint32_t nowMs);

    uint8_t getState() const { return state; }
    uint8_t getPreviousState() const { return previousState; }
    uint8_t getLastEvent() const { return lastEvent; }
    uint8_t getOutput() const { return current.output; }
    const char *getStateName() const { return current.name; }
    const char *getStateName(uint8_t s) const;
    uint16_t getTransitionCount() const { return transitionCount; }

    // Фактический тайм-аут текущего состояния (с учётом параметров), мс
    uint32_t getTimeout() const;
    uint32_t getTimeInState(uint32_t nowMs) const { return nowMs - enteredMs; }
    // Сколько осталось до тайм-аута (0 — истёк или тайм-аута нет)
    uint32_t getRemaining(uint32_t nowMs) const;
    bool hasTimeout() const { return getTimeout() != FSM_NO_TIMEOUT; }

    void setParam(uint8_t index, uint32_t value);
    uint32_t getParam(uint8_t index) const;

private:
    void enter(uint8_t newState, uint8_t event, uint32_t nowMs);
    void load(uint8_t s, FsmState &out) const;

    const FsmState *table;
    uint8_t stateCount;
    uint8_t timeoutEvent;
    uint8_t state;
    uint8_t previousState;
    uint8_t lastEvent;
    uint16_t transitionCount;
    uint32_t enteredMs;
    uint32_t params[FSM_MAX_PARAMS];
    FsmState current;   // копия описания текущего состояния в RAM
};

#endif
