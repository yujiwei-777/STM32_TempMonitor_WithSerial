#include "Serial.h"

// ???USART1: TX=PA9, RX=PA10, ??????
void Serial_Init(uint32_t baudrate)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    
    // ??USART1?GPIOA??
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
    
    // ??PA9(TX)???????
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // ??PA10(RX)?????
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // USART1 ??
    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART1, &USART_InitStructure);
    
    // ??USART1
    USART_Cmd(USART1, ENABLE);
}

// ??????
void Serial_SendChar(char ch)
{
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData(USART1, (uint8_t)ch);
}

// ?????
void Serial_SendString(char* str)
{
    while(*str)
    {
        Serial_SendChar(*str++);
    }
}

// ????(???)
void Serial_SendNum(int32_t num)
{
    char buf[12];
    sprintf(buf, "%ld", num);
    Serial_SendString(buf);
}

// ?????(??????)
void Serial_SendFloat(float f, uint8_t decimalPlaces)
{
    int32_t integerPart = (int32_t)f;
    int32_t decimalPart = (int32_t)((f - integerPart) * (decimalPlaces == 1 ? 10 : 100));
    if(decimalPart < 0) decimalPart = -decimalPart;
    
    Serial_SendNum(integerPart);
    Serial_SendChar('.');
    if(decimalPlaces == 1)
        Serial_SendNum(decimalPart);
    else if(decimalPlaces == 2)
    {
        if(decimalPart < 10) Serial_SendChar('0');
        Serial_SendNum(decimalPart);
    }
}

// ??? printf ???
int fputc(int ch, FILE *f)
{
    Serial_SendChar((char)ch);
    return ch;
}