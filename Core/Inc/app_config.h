#ifndef __APP_CONFIG_H
#define __APP_CONFIG_H

#include "stm32f1xx_hal.h"

#define APP_VERSION "2.0"
#define APP_NAME "OLED_TEMP_MONITOR"

#define NTC_BETA 3950.0f
#define NTC_R0 10000.0f
#define R_PULLUP 10000.0f
#define VREF 3.3f

#define ADC_SAMPLES 32
#define ADC_SAMPLE_DELAY_MS 1

#define TEMP_MIN -20.0f
#define TEMP_MAX 100.0f
#define TEMP_WARNING_HIGH 80.0f
#define TEMP_WARNING_LOW 0.0f

#define TEMP_UPDATE_INTERVAL_MS 500

#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_PAGES 8

typedef enum {
    STATUS_OK = 0,
    STATUS_ERROR,
    STATUS_WARNING,
    STATUS_SENSOR_ERROR
} SystemStatus;

typedef struct {
    float current_temp;
    uint32_t adc_raw;
    uint32_t sample_count;
    SystemStatus status;
    float temp_min;
    float temp_max;
} TempData;

typedef struct {
    uint8_t warning_enabled;
    float warning_high;
    float warning_low;
} AlarmConfig;

#endif