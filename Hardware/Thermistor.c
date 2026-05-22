#include "stm32f10x.h"

#define TH_PORT    GPIOA
#define TH_PIN     GPIO_Pin_0
#define TH_RCC     RCC_APB2Periph_GPIOA

void Thermistor_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(TH_RCC, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = TH_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(TH_PORT, &GPIO_InitStructure);
}

uint8_t Thermistor_Get(void)
{
    return GPIO_ReadInputDataBit(TH_PORT, TH_PIN);
}