// Модульные тесты логики ЛР 6.1 (запуск на ПК: pio test -e native)
#include <unity.h>

#include <ButtonLogic.h>
#include <CommandParser.h>
#include <Fsm.h>
#include <LedFsm.h>
#include <LedPattern.h>

void setUp() {}
void tearDown() {}

static Fsm makeFsm(uint32_t onTimeout = 30000, uint32_t warn = 5000)
{
    Fsm fsm(LED_FSM_TABLE, LS_COUNT, LE_TIMEOUT);
    fsm.setParam(LP_ON_TIMEOUT, onTimeout);
    fsm.setParam(LP_WARN_TIME, warn);
    fsm.begin(LS_OFF, 0);
    return fsm;
}

// ---------- Fsm + таблица LedFsm ----------

void test_initial_state_is_off_with_led_off()
{
    Fsm fsm = makeFsm();
    TEST_ASSERT_EQUAL_UINT8(LS_OFF, fsm.getState());
    TEST_ASSERT_EQUAL_UINT8(LED_MODE_OFF, fsm.getOutput());
    TEST_ASSERT_FALSE(fsm.hasTimeout());
}

void test_short_press_toggles_off_on()
{
    Fsm fsm = makeFsm();
    TEST_ASSERT_TRUE(fsm.dispatch(LE_SHORT, 100));
    TEST_ASSERT_EQUAL_UINT8(LS_ON, fsm.getState());
    TEST_ASSERT_EQUAL_UINT8(LED_MODE_ON, fsm.getOutput());
    TEST_ASSERT_TRUE(fsm.dispatch(LE_SHORT, 200));
    TEST_ASSERT_EQUAL_UINT8(LS_OFF, fsm.getState());
    TEST_ASSERT_EQUAL_UINT16(2, fsm.getTransitionCount());
}

void test_full_transition_table()
{
    // ожидаемое next[state][event]; 0xFF — событие игнорируется
    const uint8_t expected[LS_COUNT][LE_COUNT] = {
        /* OFF   */ {LS_ON, LS_BLINK, FSM_STAY, LS_OFF},
        /* ON    */ {LS_OFF, LS_BLINK, LS_WARN, LS_OFF},
        /* BLINK */ {LS_OFF, LS_ON, FSM_STAY, LS_OFF},
        /* WARN  */ {LS_ON, LS_BLINK, LS_OFF, LS_OFF},
    };
    for (uint8_t s = 0; s < LS_COUNT; s++)
    {
        for (uint8_t e = 0; e < LE_COUNT; e++)
        {
            Fsm fsm = makeFsm();
            fsm.forceState(s, 0);
            const bool moved = fsm.dispatch(e, 10);
            if (expected[s][e] == FSM_STAY)
            {
                TEST_ASSERT_FALSE(moved);
                TEST_ASSERT_EQUAL_UINT8(s, fsm.getState());
            }
            else
            {
                TEST_ASSERT_TRUE(moved);
                TEST_ASSERT_EQUAL_UINT8(expected[s][e], fsm.getState());
            }
        }
    }
}

void test_on_timeout_goes_to_warn_then_off()
{
    Fsm fsm = makeFsm(30000, 5000);
    fsm.dispatch(LE_SHORT, 1000);
    TEST_ASSERT_FALSE(fsm.update(30999));
    TEST_ASSERT_EQUAL_UINT32(1, fsm.getRemaining(30999));
    TEST_ASSERT_TRUE(fsm.update(31000));
    TEST_ASSERT_EQUAL_UINT8(LS_WARN, fsm.getState());
    TEST_ASSERT_EQUAL_UINT8(LED_MODE_BLINK_FAST, fsm.getOutput());
    TEST_ASSERT_FALSE(fsm.update(35999));
    TEST_ASSERT_TRUE(fsm.update(36000));
    TEST_ASSERT_EQUAL_UINT8(LS_OFF, fsm.getState());
}

void test_press_in_warn_extends_on()
{
    Fsm fsm = makeFsm(10000, 5000);
    fsm.dispatch(LE_SHORT, 0);
    fsm.update(10000);
    TEST_ASSERT_EQUAL_UINT8(LS_WARN, fsm.getState());
    fsm.dispatch(LE_SHORT, 12000);
    TEST_ASSERT_EQUAL_UINT8(LS_ON, fsm.getState());
    // таймер ON перезапущен от момента нажатия
    TEST_ASSERT_FALSE(fsm.update(21999));
    TEST_ASSERT_TRUE(fsm.update(22000));
}

void test_timeout_param_can_be_changed()
{
    Fsm fsm = makeFsm(30000, 5000);
    fsm.setParam(LP_ON_TIMEOUT, 5000);
    fsm.dispatch(LE_SHORT, 0);
    TEST_ASSERT_EQUAL_UINT32(5000, fsm.getTimeout());
    TEST_ASSERT_TRUE(fsm.update(5000));
    TEST_ASSERT_EQUAL_UINT8(LS_WARN, fsm.getState());
}

void test_blink_has_no_timeout_and_long_returns_on()
{
    Fsm fsm = makeFsm();
    fsm.dispatch(LE_LONG, 0);
    TEST_ASSERT_EQUAL_UINT8(LS_BLINK, fsm.getState());
    TEST_ASSERT_FALSE(fsm.update(1000000));
    fsm.dispatch(LE_LONG, 1000001);
    TEST_ASSERT_EQUAL_UINT8(LS_ON, fsm.getState());
}

void test_millis_overflow_is_handled()
{
    Fsm fsm = makeFsm(1000, 5000);
    fsm.dispatch(LE_SHORT, 0xFFFFFF00UL);
    TEST_ASSERT_FALSE(fsm.update(0x00000100UL));   // прошло 512 мс
    TEST_ASSERT_TRUE(fsm.update(0x000002F0UL));    // прошло 1008 мс
}

// ---------- Debouncer / PressDetector ----------

void test_debouncer_filters_bounces()
{
    Debouncer d(40);
    uint32_t t = 0;
    const bool bouncing[] = {true, false, true, false, true, true};
    int edges = 0;
    for (bool level : bouncing)
    {
        edges += d.update(level, t) ? 1 : 0;
        t += 2;
    }
    for (; t < 100; t += 5)
    {
        edges += d.update(true, t) ? 1 : 0;
    }
    TEST_ASSERT_EQUAL_INT(1, edges);
    TEST_ASSERT_TRUE(d.isPressed());
    TEST_ASSERT_EQUAL_UINT16(4, d.getBounceCount());
}

void test_debouncer_ignores_short_glitch()
{
    Debouncer d(40);
    TEST_ASSERT_FALSE(d.update(true, 0));
    TEST_ASSERT_FALSE(d.update(false, 10));   // импульс 10 мс < 40 мс
    for (uint32_t t = 15; t < 200; t += 5)
    {
        TEST_ASSERT_FALSE(d.update(false, t));
    }
    TEST_ASSERT_FALSE(d.isPressed());
}

void test_short_press_reported_on_release()
{
    PressDetector p(1000);
    TEST_ASSERT_EQUAL(PRESS_NONE, p.update(true, 0));
    TEST_ASSERT_EQUAL(PRESS_NONE, p.update(true, 300));
    TEST_ASSERT_EQUAL(PRESS_SHORT, p.update(false, 320));
    TEST_ASSERT_EQUAL_UINT32(320, p.getLastDurationMs());
}

void test_long_press_reported_once_without_short()
{
    PressDetector p(1000);
    p.update(true, 0);
    TEST_ASSERT_EQUAL(PRESS_NONE, p.update(true, 999));
    TEST_ASSERT_EQUAL(PRESS_LONG, p.update(true, 1000));
    TEST_ASSERT_EQUAL(PRESS_NONE, p.update(true, 3000));
    TEST_ASSERT_EQUAL(PRESS_NONE, p.update(false, 3010));
}

// ---------- LedPattern ----------

void test_led_patterns()
{
    TEST_ASSERT_FALSE(ledPatternLevel(LED_MODE_OFF, 123, 0));
    TEST_ASSERT_TRUE(ledPatternLevel(LED_MODE_ON, 123, 0));
    TEST_ASSERT_TRUE(ledPatternLevel(LED_MODE_BLINK_SLOW, 1000, 1000));
    TEST_ASSERT_TRUE(ledPatternLevel(LED_MODE_BLINK_SLOW, 1499, 1000));
    TEST_ASSERT_FALSE(ledPatternLevel(LED_MODE_BLINK_SLOW, 1500, 1000));
    TEST_ASSERT_TRUE(ledPatternLevel(LED_MODE_BLINK_SLOW, 2000, 1000));
    TEST_ASSERT_FALSE(ledPatternLevel(LED_MODE_BLINK_FAST, 125, 0));
    TEST_ASSERT_TRUE(ledPatternLevel(LED_MODE_BLINK_FAST, 250, 0));
}

// ---------- CommandParser ----------

static const CommandDef CMDS[] = {{"status", 1}, {"timeout", 2}, {"press", 3}};

void test_parse_command_with_argument()
{
    ParsedCommand c = parseCommand("timeout 15", CMDS, 3);
    TEST_ASSERT_EQUAL_UINT8(2, c.id);
    TEST_ASSERT_TRUE(c.hasArg);
    TEST_ASSERT_EQUAL_INT32(15, c.arg);
}

void test_parse_unknown_and_empty()
{
    TEST_ASSERT_EQUAL_UINT8(CMD_UNKNOWN, parseCommand("foo", CMDS, 3).id);
    TEST_ASSERT_EQUAL_UINT8(CMD_NONE, parseCommand("   ", CMDS, 3).id);
    TEST_ASSERT_FALSE(parseCommand("press", CMDS, 3).hasArg);
}

void test_line_reader_handles_crlf_and_case()
{
    LineReader r;
    const char *input = "STATUS\r\n";
    int lines = 0;
    for (const char *p = input; *p; p++)
    {
        if (r.feed(*p))
        {
            lines++;
            TEST_ASSERT_EQUAL_STRING("status", r.getLine());
        }
    }
    TEST_ASSERT_EQUAL_INT(1, lines);
}

int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_initial_state_is_off_with_led_off);
    RUN_TEST(test_short_press_toggles_off_on);
    RUN_TEST(test_full_transition_table);
    RUN_TEST(test_on_timeout_goes_to_warn_then_off);
    RUN_TEST(test_press_in_warn_extends_on);
    RUN_TEST(test_timeout_param_can_be_changed);
    RUN_TEST(test_blink_has_no_timeout_and_long_returns_on);
    RUN_TEST(test_millis_overflow_is_handled);
    RUN_TEST(test_debouncer_filters_bounces);
    RUN_TEST(test_debouncer_ignores_short_glitch);
    RUN_TEST(test_short_press_reported_on_release);
    RUN_TEST(test_long_press_reported_once_without_short);
    RUN_TEST(test_led_patterns);
    RUN_TEST(test_parse_command_with_argument);
    RUN_TEST(test_parse_unknown_and_empty);
    RUN_TEST(test_line_reader_handles_crlf_and_case);
    return UNITY_END();
}
