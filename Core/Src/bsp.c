#include "Bsp.h"
#include "main.h"
#include <string.h>

#define BSP_BUTTON_GPIO_PIN          GPIO_PIN_13

typedef struct {
    volatile uint8_t sampleTimeFlag;
    volatile uint8_t debounceTimeFlag;
    volatile uint8_t buttonEventFlag;
    volatile uint8_t uartLineReadyFlag;
    char uartRxLineBuffer[BSP_UART_RX_BUFFER_SIZE];
    volatile uint16_t uartRxIndex;
    uint8_t uartRxByte;
} Bsp_State_t;

static Bsp_State_t s_bspState;

static void Bsp_HandleUartByteReceived(uint8_t receivedByte);

void Bsp_Init(void){
    memset(&s_bspState, 0, sizeof(s_bspState));

    HAL_TIM_Base_Start_IT(&htim6);

    HAL_TIM_PWM_Start(&htim3, BSP_LED1_PWM_CHANNEL);
    HAL_TIM_PWM_Start(&htim3, BSP_LED2_PWM_CHANNEL);
    HAL_TIM_PWM_Start(&htim3, BSP_LED3_PWM_CHANNEL);

    HAL_UART_Receive_IT(&huart3, &s_bspState.uartRxByte, 1);
}

uint8_t Bsp_IsSampleTimeFlagSet(void){
	return s_bspState.sampleTimeFlag;
}

void Bsp_ClearSampleTimeFlag(void){
	s_bspState.sampleTimeFlag = 0u;
}

uint16_t Bsp_AdcReadRaw(void){
    uint16_t adcValue = 0u;
    HAL_ADC_Start(&hadc1);

    if (HAL_ADC_PollForConversion(&hadc1, 10u) == HAL_OK) {
        adcValue = (uint16_t)HAL_ADC_GetValue(&hadc1);
    }

    HAL_ADC_Stop(&hadc1);
    return adcValue;
}

void Bsp_SetLedDuty(Bsp_LedId_t ledId, uint8_t percent){
    uint32_t channel;
    uint32_t pulse;
    uint32_t period = htim3.Init.Period;

    switch (ledId) {
        case BSP_LED_1: channel = BSP_LED1_PWM_CHANNEL; break;
        case BSP_LED_2: channel = BSP_LED2_PWM_CHANNEL; break;
        case BSP_LED_3: channel = BSP_LED3_PWM_CHANNEL; break;
        default: return;
    }

    if (percent > 100u){
    	percent = 100u;
    }

    pulse = (period * percent) / 100u;
    __HAL_TIM_SET_COMPARE(&htim3, channel, pulse);
}

uint8_t Bsp_IsDebounceTimeFlagSet(void) {
	return s_bspState.debounceTimeFlag; }

void Bsp_ClearDebounceTimeFlag(void) { s_bspState.debounceTimeFlag = 0u; }

void Bsp_StartDebounceTimer(void){
    __HAL_TIM_SET_COUNTER(&htim7, 0u);
    HAL_TIM_Base_Start_IT(&htim7);
}

uint8_t Bsp_IsButtonEventFlagSet(void){
	return s_bspState.buttonEventFlag;
}

void Bsp_ClearButtonEventFlag(void){
	s_bspState.buttonEventFlag = 0u;
}

uint8_t Bsp_IsUartLineReady(void){
	return s_bspState.uartLineReadyFlag;
}

void Bsp_GetUartLine(char *outBuffer, uint16_t maxLen){
    uint16_t copyLen = (uint16_t)strlen(s_bspState.uartRxLineBuffer);

    if (copyLen >= maxLen) { copyLen = maxLen - 1u; }

    memcpy(outBuffer, s_bspState.uartRxLineBuffer, copyLen);
    outBuffer[copyLen] = '\0';

    s_bspState.uartLineReadyFlag = 0u;
    s_bspState.uartRxIndex = 0u;
    memset(s_bspState.uartRxLineBuffer, 0, BSP_UART_RX_BUFFER_SIZE);
}


void Bsp_UartPrint(const char *text){
    HAL_UART_Transmit(&huart3, (uint8_t *)text, (uint16_t)strlen(text), 100u);
}

static void Bsp_HandleUartByteReceived(uint8_t receivedByte){
    if ((receivedByte == '\n') || (receivedByte == '\r')) {
        if (s_bspState.uartRxIndex > 0u) {
            s_bspState.uartRxLineBuffer[s_bspState.uartRxIndex] = '\0';
            s_bspState.uartLineReadyFlag = 1u;
            s_bspState.uartRxIndex = 0u;
        }
    } else if (s_bspState.uartRxIndex < (BSP_UART_RX_BUFFER_SIZE - 1u)) {
        s_bspState.uartRxLineBuffer[s_bspState.uartRxIndex] = (char)receivedByte;
        s_bspState.uartRxIndex++;
    } else {
        s_bspState.uartRxIndex = 0u;
    }
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
    if (htim->Instance == htim6.Instance) {
        s_bspState.sampleTimeFlag = 1u;
    } else if (htim->Instance == htim7.Instance) {
        s_bspState.debounceTimeFlag = 1u;
        HAL_TIM_Base_Stop_IT(&htim7);
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
    if (GPIO_Pin == BSP_BUTTON_GPIO_PIN) {
        s_bspState.buttonEventFlag = 1u;
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
    if (huart->Instance == huart3.Instance) {
        Bsp_HandleUartByteReceived(s_bspState.uartRxByte);
        HAL_UART_Receive_IT(&huart3, &s_bspState.uartRxByte, 1);
    }
}
