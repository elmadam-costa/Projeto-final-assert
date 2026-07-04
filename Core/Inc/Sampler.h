#ifndef SAMPLER_H
#define SAMPLER_H

#include <stdint.h>

#define SAMPLER_WINDOW_SIZE
#define SAMPLER_ADC_MAX_VALUE
#define SAMPLER_PERCENT_MAX


/**
 * @brief Inicializa o estado interno do Sampler, ou seja, incializa as leituras do potênciometro.
 * @retval Nenhum.
*/


void Sampler_init(void);

void Sampler_AddSample(uint16_t rawAdcValue);

uint8_t Sampler_IsWindowComplete(void);

uint8_t Sampler_ComputeFilteredPercent(void);

#endif
