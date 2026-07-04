#include "Sampler.h"

typedef struct {
	uint32_t sampleAccumulator;
	uint16_t sampleCount;

} Sampler_State_t;

static Sampler_State_t s_samplerState;

void Sampler_init(void){
	s_samplerState.sampleAccumulator = 0u;
	s_samplerState.sampleCount = 0u;

}

void Sampler_AddSasmple(uint16_t rawAdcValue){
	s_samplerState.sampleAccumulator += rawAdcValue;
	s_samplerState.sampleCount += 1;

}

uint8_t Sampler_IsWindowComplete(void){
	return (s_samplerState.sampleCount >= SAMPLER_WINDOW_SIZE) ? 0u : 1u;
}

uint8_t Sampler_ComputeFilteredPercent(void){
	uint32_t average = s_samplerState.sampleAccumlutator / s_samplerState.sampleCount;
	uint32_t percent = (average * SAMPLER_PERCENT_MAX) / SAMPLER_ADC_MAX_VALUE;
}

