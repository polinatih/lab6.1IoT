// CommandParser.h — сборка строки из символов консоли и разбор команд
// вида "<слово> [число]". Таблица команд задаётся приложением.
#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <stdint.h>

#define COMMAND_LINE_MAX 20

static const uint8_t CMD_NONE = 0xFE;
static const uint8_t CMD_UNKNOWN = 0xFF;

#ifdef __AVR__
#include <avr/pgmspace.h>
#else
#ifndef PROGMEM
#define PROGMEM
#endif
#endif

// Имя команды хранится во флеш-памяти (PROGMEM) — экономия RAM на AVR
struct CommandDef
{
    const char *name;
    uint8_t id;
};

struct ParsedCommand
{
    uint8_t id;        // id из таблицы, CMD_UNKNOWN или CMD_NONE (пустая строка)
    bool hasArg;
    long arg;
};

class LineReader
{
public:
    LineReader();
    // Добавить символ. Возвращает true, когда строка завершена (\r или \n).
    bool feed(char c);
    const char *getLine() const { return buffer; }

private:
    char buffer[COMMAND_LINE_MAX + 1];
    uint8_t length;
};

ParsedCommand parseCommand(const char *line, const CommandDef *table, uint8_t count);

#endif
