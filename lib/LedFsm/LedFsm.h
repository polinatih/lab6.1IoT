// LedFsm.h — описание конечного автомата ЛР 6.1 «кнопка — светодиод».
//
// Базовый автомат (2 состояния): OFF <-> ON по короткому нажатию.
// Расширение (дополнительное поведение):
//   BLINK — мигание 1 Гц, вход по длинному нажатию (>= 1 с);
//   WARN  — предупреждение перед автоматическим выключением: если в ON
//           нет нажатий в течение ON_TIMEOUT, LED мигает 4 Гц 5 с и гаснет;
//           короткое нажатие в WARN продлевает работу (возврат в ON);
//   RESET — команда консоли, из любого состояния переводит в OFF.
#ifndef LED_FSM_H
#define LED_FSM_H

#include <Fsm.h>
#include <LedPattern.h>

enum LedFsmState : uint8_t
{
    LS_OFF = 0,
    LS_ON,
    LS_BLINK,
    LS_WARN,
    LS_COUNT
};

enum LedFsmEvent : uint8_t
{
    LE_SHORT = 0,   // короткое нажатие
    LE_LONG,        // длинное нажатие
    LE_TIMEOUT,     // истекло время пребывания в состоянии
    LE_RESET,       // сброс (команда reset)
    LE_COUNT
};

// Номера настраиваемых параметров (тайм-аутов) автомата
enum LedFsmParam : uint8_t
{
    LP_ON_TIMEOUT = 0,
    LP_WARN_TIME = 1
};

extern const FsmState LED_FSM_TABLE[LS_COUNT] PROGMEM;

// Имя события (строка PROGMEM на AVR)
const char *ledFsmEventName(uint8_t event);

#endif
