#include "ADC.h"
#include <math.h>   // ???? Steinhart-Hart ??????

/* ????? --------------------------------------------------------*/
// ???????????????????

// 1. ??????
#define VCC                    3.3f      // ??????(??:V)
#define ADC_RESOLUTION         4095.0f   // ADC???(12?)
#define SERIES_RESISTOR        10000.0f  // ??????????(??:O),??10k

// 2. ??????(???? B ?? R25 ?? Steinhart-Hart)
#define THERMISTOR_R25         10000.0f  // 25????????(??:O),??10k
#define B_VALUE                3950.0f   // ????? B ?,?? 3380, 3435, 3950 ?

// 3. ????????(???,??????)
// #define METHOD_LINEAR          // ???(????????)
#define METHOD_STEINHART       // Steinhart-Hart ???(??,?? B ?)
// #define METHOD_LUT             // ???(????????-ADC???)

// ???????(????????)
#ifdef METHOD_LINEAR
    #define LINEAR_SLOPE        20.0f   // ??(?/V)
    #define LINEAR_OFFSET       15.0f   // ??(0V ????)
#endif

// ????????(????:?,ADC?:0~4095)
#ifdef METHOD_LUT
    static const float temp_adc_table[][2] = {
        // {??(?), ADC???}
        {0.0,  3500},
        {25.0, 2400},
        {50.0, 1550},
        {75.0, 950}
    };
    #define TABLE_SIZE  (sizeof(temp_adc_table) / sizeof(temp_adc_table[0]))
#endif
/*-------------------------------------------------------------------*/

void ADC_Temp_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);
    
    ADC_Cmd(ADC1, ENABLE);
    
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
}

// ??ADC???(? 10 ?????)
static uint16_t ADC_GetValue(void)
{
    uint32_t sum = 0;
    // ????(??????,???????)
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5);
    
    for(int i = 0; i < 10; i++)
    {
        ADC_SoftwareStartConvCmd(ADC1, ENABLE);
        while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
        sum += ADC_GetConversionValue(ADC1);
    }
    return (uint16_t)(sum / 10);
}

// ?????(??????????????)
float Get_Temperature(void)
{
    uint16_t adc = ADC_GetValue();
    float vol = adc * VCC / ADC_RESOLUTION;   // ???
    
    #ifdef METHOD_LINEAR
        // ???(????? LINEAR_SLOPE ? LINEAR_OFFSET)
        float temp = vol * LINEAR_SLOPE + LINEAR_OFFSET;
        
    #elif defined(METHOD_STEINHART)
        // Steinhart-Hart ??(Beta ????)
        if(vol <= 0.001f) vol = 0.001f;  // ????
        float r_ntc = (vol * SERIES_RESISTOR) / (VCC - vol);
        float r_inf = r_ntc / THERMISTOR_R25;
        float steinhart = log(r_inf);              // ln(R/R25)
        steinhart = steinhart / B_VALUE;           // (1/B) * ln(R/R25)
        steinhart = steinhart + (1.0f / (25.0f + 273.15f)); // 1/T0
        steinhart = 1.0f / steinhart;              // ?????
        float temp = steinhart - 273.15f;          // ???
        
    #elif defined(METHOD_LUT)
        // ???(????)
        float temp = 25.0f;  // ???
        if(adc <= temp_adc_table[0][1]) {
            temp = temp_adc_table[0][0];
        } else if(adc >= temp_adc_table[TABLE_SIZE-1][1]) {
            temp = temp_adc_table[TABLE_SIZE-1][0];
        } else {
            for(int i = 0; i < TABLE_SIZE-1; i++) {
                if(adc <= temp_adc_table[i][1] && adc >= temp_adc_table[i+1][1]) {
                    float t1 = temp_adc_table[i][0];
                    float t2 = temp_adc_table[i+1][0];
                    float adc1 = temp_adc_table[i][1];
                    float adc2 = temp_adc_table[i+1][1];
                    temp = t1 + (t2 - t1) * (adc - adc1) / (adc2 - adc1);
                    break;
                }
            }
        }
    #else
        // ??:???????(?????)
        float temp = vol * 20.0f + 15.0f;
    #endif
    
    return temp;
}