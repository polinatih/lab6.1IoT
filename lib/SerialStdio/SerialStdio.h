// SerialStdio.h — подключение стандартного ввода-вывода (printf/getchar)
// к UART через fdev_setup_stream() библиотеки avr-libc.
#ifndef SERIAL_STDIO_H
#define SERIAL_STDIO_H

#include <Arduino.h>
#include <stdio.h>

void serialStdioInit(unsigned long baudRate);
// Есть ли в буфере UART принятые символы (getchar() не заблокируется)
bool serialStdioAvailable();

// printf со строкой формата во флеш-памяти (экономия RAM)
#define PRINT_F(fmt, ...) printf_P(PSTR(fmt), ##__VA_ARGS__)

#endif
