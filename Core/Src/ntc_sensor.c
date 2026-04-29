#include "ntc_sensor.h"
#include "math.h"
#include "main.h"

extern ADC_HandleTypeDef hadc1;

void NTC_Init(void) {
}

uint32_t NTC_ReadADC(void) {
    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK) {
        uint32_t value = HAL_ADC_GetValue(&hadc1);
        HAL_ADC_Stop(&hadc1);
        return value;
    }
    HAL_ADC_Stop(&hadc1);
    return 0;
}

uint32_t NTC_ReadAverageADC(uint16_t samples) {
    uint32_t sum = 0;
    uint16_t valid_samples = 0;
    
    for (uint16_t i = 0; i < samples; i++) {
        uint32_t value = NTC_ReadADC();
        if (value > 0 && value < 4095) {
            sum += value;
            valid_samples++;
        }
        HAL_Delay(ADC_SAMPLE_DELAY_MS);
    }
    
    return (valid_samples > 0) ? (sum / valid_samples) : 0;
}

float NTC_CalculateTemperature(uint32_t adc_raw) {
    if (adc_raw == 0 || adc_raw >= 4095) {
        return 25.0f;
    }
    
    float voltage = adc_raw * VREF / 4095.0f;
    float resistance = R_PULLUP * voltage / (VREF - voltage);
    
    if (resistance <= 0) {
        return 25.0f;
    }
    
    float lnR = logf(resistance / NTC_R0);
    float inv_T = (1.0f / 298.15f) + (1.0f / NTC_BETA) * lnR;
    float temp = 1.0f / inv_T - 273.15f;
    
    return temp;
}

SystemStatus NTC_ValidateTemperature(float temp) {
    if (temp < TEMP_MIN || temp > TEMP_MAX) {
        return STATUS_SENSOR_ERROR;
    }
    if (temp > TEMP_WARNING_HIGH || temp < TEMP_WARNING_LOW) {
        return STATUS_WARNING;
    }
    return STATUS_OK;
}

float NTC_ReadTemperature(void) {
    uint32_t avg_adc = NTC_ReadAverageADC(ADC_SAMPLES);
    return NTC_CalculateTemperature(avg_adc);
}

void NTC_UpdateTempData(TempData* data) {
    data->adc_raw = NTC_ReadAverageADC(ADC_SAMPLES);
    data->current_temp = NTC_CalculateTemperature(data->adc_raw);
    data->sample_count++;
    
    if (data->current_temp > data->temp_max) {
        data->temp_max = data->current_temp;
    }
    if (data->current_temp < data->temp_min) {
        data->temp_min = data->current_temp;
    }
    
    data->status = NTC_ValidateTemperature(data->current_temp);
}