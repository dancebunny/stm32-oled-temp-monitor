#ifndef __NTC_SENSOR_H
#define __NTC_SENSOR_H

#include "app_config.h"
#include <stdint.h>

void NTC_Init(void);
float NTC_ReadTemperature(void);
uint32_t NTC_ReadADC(void);
float NTC_CalculateTemperature(uint32_t adc_raw);
uint32_t NTC_ReadAverageADC(uint16_t samples);
SystemStatus NTC_ValidateTemperature(float temp);
void NTC_UpdateTempData(TempData* data);

#endif