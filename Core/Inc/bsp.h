#ifndef BSP_H
#define BSP_H

#include "stm32f7xx_hal.h"
#include <stdint.h>

#define BSP_LED1_PWM_CHANNEL        TIM_CHANNEL_1
#define BSP_LED2_PWM_CHANNEL        TIM_CHANNEL_2
#define BSP_LED3_PWM_CHANNEL        TIM_CHANNEL_3
#define BSP_UART_RX_BUFFER_SIZE     32u

typedef enum {
    BSP_LED_1 = 0,
    BSP_LED_2,
    BSP_LED_3,
    BSP_LED_COUNT
} Bsp_LedId_t;


void Bsp_Init(void);

uint8_t Bsp_IsSampleTimeFlagSet(void);

void Bsp_ClearSampleTimeFlag(void);

uint16_t Bsp_AdcReadRaw(void);

void Bsp_SetLedDuty(Bsp_LedId_t ledId, uint8_t percent);

uint8_t Bsp_IsDebounceTimeFlagSet(void);

void Bsp_ClearDebounceTimeFlag(void);

void Bsp_StartDebounceTimer(void);

uint8_t Bsp_IsButtonEventFlagSet(void);

void Bsp_ClearButtonEventFlag(void);

uint8_t Bsp_IsUartLineReady(void);

void Bsp_GetUartLine(char *outBuffer, uint16_t maxLen);

void Bsp_UartPrint(const char *text);

#endif
