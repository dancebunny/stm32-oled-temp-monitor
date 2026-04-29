#ifndef __OLED_DISPLAY_H
#define __OLED_DISPLAY_H

#include "app_config.h"
#include <stdint.h>

void OLED_Init(void);
void OLED_Clear(void);
void OLED_UpdateDisplay(const TempData* data, const AlarmConfig* alarm);
void OLED_ShowStartupScreen(void);
void OLED_ShowAlarmScreen(float temp, const char* message);
void OLED_DrawProgressBar(uint8_t x, uint8_t y, uint8_t width, uint8_t percentage);
void OLED_DrawTemperatureBar(uint8_t x, uint8_t y, uint8_t width, float temp);
void OLED_SetDisplayPage(uint8_t page);
void OLED_ToggleDisplayPage(void);

#endif