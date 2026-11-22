#include "SerialStdio.h"

static FILE serialStream;

static int serialPutChar(char c, FILE *stream)
{
    (void)stream;
    if (c == '\n')
    {
        Serial.write('\r');
    }
    Serial.write(c);
    return 0;
}

static int serialGetChar(FILE *stream)
{
    (void)stream;
    const int c = Serial.read();
    return c < 0 ? _FDEV_EOF : c;
}

void serialStdioInit(unsigned long baudRate)
{
    Serial.begin(baudRate);
    fdev_setup_stream(&serialStream, serialPutChar, serialGetChar, _FDEV_SETUP_RW);
    stdout = &serialStream;
    stdin = &serialStream;
}

bool serialStdioAvailable()
{
    return Serial.available() > 0;
}
