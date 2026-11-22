#include "Fsm.h"

Fsm::Fsm(const FsmState *table, uint8_t stateCount, uint8_t timeoutEvent)
    : table(table), stateCount(stateCount), timeoutEvent(timeoutEvent),
      state(0), previousState(0), lastEvent(FSM_STAY), transitionCount(0),
      enteredMs(0)
{
    for (uint8_t i = 0; i < FSM_MAX_PARAMS; i++)
    {
        params[i] = 0;
    }
    load(0, current);
}

void Fsm::load(uint8_t s, FsmState &out) const
{
    memcpy_P(&out, &table[s], sizeof(FsmState));
}

void Fsm::begin(uint8_t initialState, uint32_t nowMs)
{
    state = initialState < stateCount ? initialState : 0;
    previousState = state;
    lastEvent = FSM_STAY;
    transitionCount = 0;
    enteredMs = nowMs;
    load(state, current);
}

void Fsm::enter(uint8_t newState, uint8_t event, uint32_t nowMs)
{
    previousState = state;
    state = newState;
    lastEvent = event;
    enteredMs = nowMs;
    transitionCount++;
    load(state, current);
}

bool Fsm::dispatch(uint8_t event, uint32_t nowMs)
{
    if (event >= FSM_MAX_EVENTS)
    {
        return false;
    }
    const uint8_t next = current.next[event];
    if (next == FSM_STAY || next >= stateCount)
    {
        return false;   // событие не определено в этом состоянии
    }
    enter(next, event, nowMs);
    return true;
}

bool Fsm::update(uint32_t nowMs)
{
    const uint32_t timeout = getTimeout();
    if (timeout == FSM_NO_TIMEOUT)
    {
        return false;
    }
    if (nowMs - enteredMs >= timeout)
    {
        return dispatch(timeoutEvent, nowMs);
    }
    return false;
}

void Fsm::forceState(uint8_t newState, uint32_t nowMs)
{
    if (newState < stateCount)
    {
        enter(newState, FSM_STAY, nowMs);
    }
}

uint32_t Fsm::getTimeout() const
{
    const uint32_t t = current.timeoutMs;
    if ((t & 0xFFFFFF00UL) == 0xFFFFFF00UL)
    {
        const uint8_t index = (uint8_t)(t & 0xFF);
        return index < FSM_MAX_PARAMS ? params[index] : FSM_NO_TIMEOUT;
    }
    return t;
}

uint32_t Fsm::getRemaining(uint32_t nowMs) const
{
    const uint32_t timeout = getTimeout();
    if (timeout == FSM_NO_TIMEOUT)
    {
        return 0;
    }
    const uint32_t elapsed = nowMs - enteredMs;
    return elapsed >= timeout ? 0 : timeout - elapsed;
}

const char *Fsm::getStateName(uint8_t s) const
{
    if (s >= stateCount)
    {
        return 0;
    }
    FsmState tmp;
    load(s, tmp);
    return tmp.name;
}

void Fsm::setParam(uint8_t index, uint32_t value)
{
    if (index < FSM_MAX_PARAMS)
    {
        params[index] = value;
    }
}

uint32_t Fsm::getParam(uint8_t index) const
{
    return index < FSM_MAX_PARAMS ? params[index] : 0;
}
