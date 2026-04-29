<<<<<<< HEAD
#include "main.h"
#include "app_config.h"
#include "ntc_sensor.h"
#include "oled_display.h"

ADC_HandleTypeDef hadc1;

static void MX_ADC1_Init(void);
static void MX_GPIO_Init(void);
void SystemClock_Config(void);
void Error_Handler(void);

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_ADC1_Init();

    NTC_Init();
    OLED_Init();
    OLED_ShowStartupScreen();

    TempData temp_data = {
        .current_temp = 0.0f,
        .adc_raw = 0,
        .sample_count = 0,
        .status = STATUS_OK,
        .temp_min = TEMP_MAX,
        .temp_max = TEMP_MIN
    };

    AlarmConfig alarm_config = {
        .warning_enabled = 1,
        .warning_high = TEMP_WARNING_HIGH,
        .warning_low = TEMP_WARNING_LOW
    };

    uint32_t last_page_switch = HAL_GetTick();
    
    while (1)
    {
        NTC_UpdateTempData(&temp_data);
        OLED_UpdateDisplay(&temp_data, &alarm_config);
        
        uint32_t current_time = HAL_GetTick();
        if (current_time - last_page_switch >= 2000) {
            OLED_ToggleDisplayPage();
            last_page_switch = current_time;
        }
        
        HAL_Delay(TEMP_UPDATE_INTERVAL_MS);
    }
}

static void MX_ADC1_Init(void)
{
    hadc1.Instance = ADC1;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;
    if (HAL_ADC_Init(&hadc1) != HAL_OK)
    {
        Error_Handler();
    }

    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = ADC_CHANNEL_0;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }
}

static void MX_GPIO_Init(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
}

void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
    }
}
=======
/* USER CODE BEGIN Header */
/* USER CODE END Header */

#include "main.h"
#include "stm32f1xx_hal.h"
#include <stdio.h>
#include <math.h>

/* ==================== 温度参数 ==================== */
#define NTC_BETA 3950.0f
#define NTC_R0   10000.0f
#define R_PULLUP 10000.0f
#define VREF     3.3f

uint32_t adc_value = 0;
float temperature = 25.0f;

/* ==================== 5x7字体（已包含小数点） ==================== */
const uint8_t font5x7[128][5] = {
  [32] = {0x00,0x00,0x00,0x00,0x00},
  [46] = {0x00,0x00,0x60,0x60,0x00},
  [48] = {0x3E,0x51,0x49,0x45,0x3E}, [49] = {0x00,0x42,0x7F,0x40,0x00},
  [50] = {0x42,0x61,0x51,0x49,0x46}, [51] = {0x21,0x41,0x45,0x4B,0x31},
  [52] = {0x18,0x14,0x12,0x7F,0x10}, [53] = {0x27,0x45,0x45,0x45,0x39},
  [54] = {0x3C,0x4A,0x49,0x49,0x30}, [55] = {0x01,0x71,0x09,0x05,0x03},
  [56] = {0x36,0x49,0x49,0x49,0x36}, [57] = {0x06,0x49,0x49,0x29,0x1E},
  [65] = {0x7E,0x09,0x09,0x09,0x7E}, [67] = {0x3E,0x41,0x41,0x41,0x22},
  [68] = {0x7F,0x41,0x41,0x41,0x3E}, [69] = {0x7F,0x49,0x49,0x49,0x41},
  [72] = {0x7F,0x08,0x08,0x08,0x7F}, [76] = {0x7F,0x40,0x40,0x40,0x40},
  [77] = {0x7F,0x02,0x04,0x02,0x7F}, [79] = {0x3E,0x41,0x41,0x41,0x3E},
  [80] = {0x7F,0x09,0x09,0x09,0x06}, [83] = {0x46,0x49,0x49,0x49,0x31},
  [84] = {0x01,0x01,0x7F,0x01,0x01},
};

/* ==================== 软件I2C (PB8 SCL, PB9 SDA) ==================== */
#define OLED_W_SCL(x)   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, (x))
#define OLED_W_SDA(x)   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, (x))

void OLED_I2C_Init(void);
void OLED_I2C_Start(void);
void OLED_I2C_Stop(void);
void OLED_I2C_SendByte(uint8_t Byte);
void OLED_WriteCommand(uint8_t cmd);
void OLED_WriteData(uint8_t data);
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawChar(uint8_t x, uint8_t y, char ch);
void OLED_DrawString(uint8_t x, uint8_t y, const char* str);
float Calculate_Temperature(uint32_t adc_raw);
void SystemClock_Config(void);
void Error_Handler(void);

static void MX_ADC1_Init(void);
static void MX_GPIO_Init(void);

/* ==================== OLED驱动函数 ==================== */
void OLED_I2C_Init(void) {
  GPIO_InitTypeDef GPIO_InitStructure = {0};
  __HAL_RCC_GPIOB_CLK_ENABLE();
  GPIO_InitStructure.Pin = GPIO_PIN_8 | GPIO_PIN_9;
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
  OLED_W_SCL(1); OLED_W_SDA(1);
}

void OLED_I2C_Start(void) { OLED_W_SDA(1); OLED_W_SCL(1); OLED_W_SDA(0); OLED_W_SCL(0); }
void OLED_I2C_Stop(void)  { OLED_W_SDA(0); OLED_W_SCL(1); OLED_W_SDA(1); }
void OLED_I2C_SendByte(uint8_t Byte) {
  for (uint8_t i = 0; i < 8; i++) {
    OLED_W_SDA(!!(Byte & (0x80 >> i)));
    OLED_W_SCL(1); OLED_W_SCL(0);
  }
  OLED_W_SCL(1); OLED_W_SCL(0);
}

void OLED_WriteCommand(uint8_t cmd) {
  OLED_I2C_Start();
  OLED_I2C_SendByte(0x78);
  OLED_I2C_SendByte(0x00);
  OLED_I2C_SendByte(cmd);
  OLED_I2C_Stop();
}

void OLED_WriteData(uint8_t data) {
  OLED_I2C_Start();
  OLED_I2C_SendByte(0x78);
  OLED_I2C_SendByte(0x40);
  OLED_I2C_SendByte(data);
  OLED_I2C_Stop();
}

void OLED_Init(void) {
  OLED_I2C_Init();
  HAL_Delay(200);
  OLED_WriteCommand(0xAE);
  OLED_WriteCommand(0xD5); OLED_WriteCommand(0x80);
  OLED_WriteCommand(0xA8); OLED_WriteCommand(0x3F);
  OLED_WriteCommand(0xD3); OLED_WriteCommand(0x00);
  OLED_WriteCommand(0x40);
  OLED_WriteCommand(0xA1);
  OLED_WriteCommand(0xC8);
  OLED_WriteCommand(0xDA); OLED_WriteCommand(0x12);
  OLED_WriteCommand(0x81); OLED_WriteCommand(0xCF);
  OLED_WriteCommand(0xD9); OLED_WriteCommand(0xF1);
  OLED_WriteCommand(0xDB); OLED_WriteCommand(0x30);
  OLED_WriteCommand(0xA4);
  OLED_WriteCommand(0xA6);
  OLED_WriteCommand(0x8D); OLED_WriteCommand(0x14);
  OLED_WriteCommand(0xAF);
  OLED_Clear();
}

void OLED_Clear(void) {
  for (uint8_t page = 0; page < 8; page++) {
    OLED_WriteCommand(0xB0 + page);
    OLED_WriteCommand(0x00);
    OLED_WriteCommand(0x10);
    for (uint8_t i = 0; i < 128; i++) OLED_WriteData(0x00);
  }
}

void OLED_DrawChar(uint8_t x, uint8_t y, char ch) {
  if (ch < 32 || ch > 127) ch = ' ';
  for (uint8_t i = 0; i < 5; i++) {
    OLED_WriteCommand(0xB0 + y);
    OLED_WriteCommand(0x00 + ((x + i) & 0x0F));
    OLED_WriteCommand(0x10 + ((x + i) >> 4));
    OLED_WriteData(font5x7[(uint8_t)ch][i]);
  }
}

void OLED_DrawString(uint8_t x, uint8_t y, const char* str) {
  while (*str) {
    OLED_DrawChar(x, y, *str++);
    x += 6;
  }
}

/* 温度计算 */
float Calculate_Temperature(uint32_t adc_raw) {
  if (adc_raw == 0 || adc_raw == 4095) return 25.0f;
  float voltage = adc_raw * VREF / 4095.0f;
  float resistance = R_PULLUP * (voltage / (VREF - voltage));
  float lnR = logf(resistance / NTC_R0);
  float inv_T = (1.0f / 298.15f) + (1.0f / NTC_BETA) * lnR;
  return 1.0f / inv_T - 273.15f;
}

/* ==================== 手动初始化 ==================== */
ADC_HandleTypeDef hadc1;

static void MX_ADC1_Init(void) {
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  HAL_ADC_Init(&hadc1);

  ADC_ChannelConfTypeDef sConfig = {0};
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  HAL_ADC_ConfigChannel(&hadc1, &sConfig);
}

static void MX_GPIO_Init(void) {
  __HAL_RCC_GPIOB_CLK_ENABLE();
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_ADC1_Init();

  OLED_Init();
  OLED_DrawString(0, 0, "HELLO STM32!");

  HAL_ADC_Start(&hadc1);

  while (1)
  {
	  HAL_ADC_Start(&hadc1);
	  if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK) {
      adc_value = HAL_ADC_GetValue(&hadc1);
      temperature = Calculate_Temperature(adc_value);
    }
	  HAL_ADC_Stop(&hadc1);

    int temp_int = (int)temperature;
    int temp_dec = (int)((temperature - temp_int) * 10 + 0.5f);

    char buf[32];
    sprintf(buf, "TEMP:%2d.%1dC ", temp_int, temp_dec);
    OLED_DrawString(0, 2, buf);

    sprintf(buf, "ADC:%4lu", adc_value);
    OLED_DrawString(0, 4, buf);

    HAL_Delay(800);
  }
}

/* CubeMX 生成的函数 */
void SystemClock_Config(void) { /* 保留你项目里原来的 SystemClock_Config */ }
void Error_Handler(void) { __disable_irq(); while(1) {} }
>>>>>>> 12bd0d659590962a20f2406d5e3e0c07e81f53bd
