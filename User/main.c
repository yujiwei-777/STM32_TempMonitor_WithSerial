#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "Buzzer.h"
#include "ADC.h"
#include "Serial.h"
#include "Motor.h"
#include "LED.h"

// ??????
static uint16_t tempThreshold = 30;

int main(void)
{
    OLED_Init();
    Buzzer_Init();
    ADC_Temp_Init();
    Serial_Init(115200);      // ???,???
    Motor_Init();
    LED_Init();

    OLED_ShowString(1, 1, "Temp:");
    OLED_ShowString(1, 12, "C");

    uint8_t sendCount = 0;

    while (1)
    {
        float temp = Get_Temperature();

        // OLED ????(??+????)
        int integer_part = (int)temp;
        int decimal_part = (int)(temp * 10) % 10;
        OLED_ShowNum(1, 6, integer_part, 2);
        OLED_ShowChar(1, 8, '.');
        OLED_ShowNum(1, 9, decimal_part, 1);

        uint8_t isOverTemp = (temp > tempThreshold);

        // ========== ?????(???????) ==========
        if (isOverTemp)
        {
            Buzzer_ON();
        }
        else
        {
            Buzzer_OFF();
        }

        // ========== LED ??(???????,???;???????,???) ==========
        if (isOverTemp)
        {
            LED1_OFF();     // ???
            LED2_ON();      // ????
        }
        else
        {
            LED1_ON();      // ????
            LED2_OFF();     // ???
        }

        // ========== ????(????) ==========
        if (isOverTemp)
            Motor_SetSpeed(70);     // ????? 70%
        else
            Motor_SetSpeed(0);      // ??

        // ========== OLED ??????? ==========
        if (isOverTemp)
            OLED_ShowString(2, 1, "Warning");
        else
            OLED_ShowString(2, 1, "Normal  ");

        // ========== ??????(????) ==========
        sendCount++;
        if (sendCount >= 5)     // 200ms * 5 = 1s
        {
            sendCount = 0;
            Serial_SendString("Temp: ");
            Serial_SendFloat(temp, 1);
            Serial_SendString(" C\r\n");
        }

        Delay_ms(200);
    }
}