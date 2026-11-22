#include "CommandParser.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef __AVR__
#include <avr/pgmspace.h>
#define NAME_COMPARE strcmp_P
#else
#define NAME_COMPARE strcmp
#endif

LineReader::LineReader() : length(0)
{
    buffer[0] = '\0';
}

bool LineReader::feed(char c)
{
    if (c == '\r' || c == '\n')
    {
        if (length == 0)
        {
            return false;   // пустые строки (например, \n после \r) игнорируем
        }
        buffer[length] = '\0';
        length = 0;
        return true;
    }
    if (length < COMMAND_LINE_MAX)
    {
        buffer[length++] = (char)tolower((unsigned char)c);
        buffer[length] = '\0';
    }
    return false;
}

ParsedCommand parseCommand(const char *line, const CommandDef *table, uint8_t count)
{
    ParsedCommand result = {CMD_NONE, false, 0};
    while (*line == ' ')
    {
        line++;
    }
    if (*line == '\0')
    {
        return result;
    }

    // Слово команды
    char word[COMMAND_LINE_MAX + 1];
    uint8_t n = 0;
    while (*line != '\0' && *line != ' ' && n < COMMAND_LINE_MAX)
    {
        word[n++] = *line++;
    }
    word[n] = '\0';

    result.id = CMD_UNKNOWN;
    for (uint8_t i = 0; i < count; i++)
    {
        if (NAME_COMPARE(word, table[i].name) == 0)
        {
            result.id = table[i].id;
            break;
        }
    }

    // Необязательный числовой аргумент
    while (*line == ' ')
    {
        line++;
    }
    if (*line != '\0')
    {
        char *end = 0;
        const long value = strtol(line, &end, 10);
        if (end != line)
        {
            result.hasArg = true;
            result.arg = value;
        }
    }
    return result;
}
