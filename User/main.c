#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "Buzzer.h"
#include "ADC.h"
#include "Serial.h"

#define TEMP_THRESHOLD 30

int main(void)
{
    OLED_Init();
    Buzzer_Init();
    ADC_Temp_Init();
    Serial_Init(115200);           // ?????,???115200
    
    OLED_ShowString(1, 1, "Temp:");
    OLED_ShowString(1, 12, "C");
    
    uint8_t sendCount = 0;          // ????,5??? = 1?
    
    while (1)
    {
        float temp = Get_Temperature();
        
        // ========== OLED ?? ==========
        int integer_part = (int)temp;
        int decimal_part = (int)(temp * 10) % 10;
        OLED_ShowNum(1, 6, integer_part, 2);
        OLED_ShowChar(1, 8, '.');
        OLED_ShowNum(1, 9, decimal_part, 1);
        
        // ========== ????? ==========
        if (temp > TEMP_THRESHOLD)
        {
            Buzzer_ON();
            OLED_ShowString(2, 1, "Warning");
        }
        else
        {
            Buzzer_OFF();
            OLED_ShowString(2, 1, "Normal ");
        }
        
        // ========== ??????(?1????)==========
        sendCount++;
        if (sendCount >= 5)          // 200ms * 5 = 1000ms
        {
            sendCount = 0;
            Serial_SendString("Temperature: ");
            Serial_SendFloat(temp, 1);
            Serial_SendString(" C\r\n");
        }
        
        Delay_ms(200);               // ?????200ms
    }
}