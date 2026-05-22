#ifndef __SERIAL_H
#define __SERIAL_H

#include "stm32f10x.h"
#include <stdio.h>

void Serial_Init(uint32_t baudrate);
void Serial_SendChar(char ch);
void Serial_SendString(char* str);
void Serial_SendNum(int32_t num);
void Serial_SendFloat(float f, uint8_t decimalPlaces);

// ???? printf,????? fputc
int fputc(int ch, FILE *f);

#endif