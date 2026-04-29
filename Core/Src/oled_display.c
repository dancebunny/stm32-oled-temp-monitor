#include "oled_display.h"
#include "ntc_sensor.h"
#include <stdio.h>
#include <string.h>

#define OLED_I2C_ADDR 0x78

#define OLED_SCL_PIN GPIO_PIN_8
#define OLED_SDA_PIN GPIO_PIN_9
#define OLED_PORT GPIOB

#define OLED_W_SCL(x) HAL_GPIO_WritePin(OLED_PORT, OLED_SCL_PIN, (x))
#define OLED_W_SDA(x) HAL_GPIO_WritePin(OLED_PORT, OLED_SDA_PIN, (x))

const uint8_t font5x7[128][5] = {
    [32] = {0x00,0x00,0x00,0x00,0x00},
    [33] = {0x00,0x00,0x5F,0x00,0x00},
    [34] = {0x00,0x07,0x00,0x07,0x00},
    [35] = {0x14,0x7F,0x14,0x7F,0x14},
    [36] = {0x24,0x2A,0x7F,0x2A,0x12},
    [37] = {0x23,0x13,0x08,0x64,0x62},
    [38] = {0x36,0x49,0x55,0x22,0x50},
    [39] = {0x00,0x05,0x03,0x00,0x00},
    [40] = {0x00,0x1C,0x22,0x41,0x00},
    [41] = {0x00,0x41,0x22,0x1C,0x00},
    [42] = {0x08,0x2A,0x1C,0x2A,0x08},
    [43] = {0x08,0x08,0x3E,0x08,0x08},
    [44] = {0x00,0x50,0x30,0x00,0x00},
    [45] = {0x08,0x08,0x08,0x08,0x08},
    [46] = {0x00,0x60,0x60,0x00,0x00},
    [47] = {0x20,0x10,0x08,0x04,0x02},
    [48] = {0x3E,0x51,0x49,0x45,0x3E},
    [49] = {0x00,0x42,0x7F,0x40,0x00},
    [50] = {0x42,0x61,0x51,0x49,0x46},
    [51] = {0x21,0x41,0x45,0x4B,0x31},
    [52] = {0x18,0x14,0x12,0x7F,0x10},
    [53] = {0x27,0x45,0x45,0x45,0x39},
    [54] = {0x3C,0x4A,0x49,0x49,0x30},
    [55] = {0x01,0x71,0x09,0x05,0x03},
    [56] = {0x36,0x49,0x49,0x49,0x36},
    [57] = {0x06,0x49,0x49,0x29,0x1E},
    [58] = {0x00,0x36,0x36,0x00,0x00},
    [59] = {0x00,0x56,0x36,0x00,0x00},
    [60] = {0x00,0x08,0x14,0x22,0x41},
    [61] = {0x14,0x14,0x14,0x14,0x14},
    [62] = {0x00,0x41,0x22,0x14,0x08},
    [63] = {0x02,0x01,0x51,0x09,0x06},
    [64] = {0x32,0x49,0x79,0x41,0x3E},
    [65] = {0x7E,0x11,0x11,0x11,0x7E},
    [66] = {0x7F,0x49,0x49,0x49,0x36},
    [67] = {0x3E,0x41,0x41,0x41,0x22},
    [68] = {0x7F,0x41,0x41,0x22,0x1C},
    [69] = {0x7F,0x49,0x49,0x49,0x41},
    [70] = {0x7F,0x09,0x09,0x01,0x01},
    [71] = {0x3E,0x41,0x49,0x49,0x7A},
    [72] = {0x7F,0x08,0x08,0x08,0x7F},
    [73] = {0x00,0x41,0x7F,0x41,0x00},
    [74] = {0x20,0x40,0x41,0x3F,0x01},
    [75] = {0x7F,0x08,0x14,0x22,0x41},
    [76] = {0x7F,0x40,0x40,0x40,0x40},
    [77] = {0x7F,0x02,0x04,0x02,0x7F},
    [78] = {0x7F,0x02,0x0C,0x02,0x7F},
    [79] = {0x3E,0x41,0x41,0x41,0x3E},
    [80] = {0x7F,0x09,0x09,0x09,0x06},
    [81] = {0x3E,0x41,0x51,0x21,0x5E},
    [82] = {0x7F,0x09,0x19,0x29,0x46},
    [83] = {0x46,0x49,0x49,0x49,0x31},
    [84] = {0x01,0x01,0x7F,0x01,0x01},
    [85] = {0x3F,0x40,0x40,0x40,0x3F},
    [86] = {0x1F,0x20,0x40,0x20,0x1F},
    [87] = {0x7F,0x20,0x18,0x20,0x7F},
    [88] = {0x63,0x14,0x08,0x14,0x63},
    [89] = {0x03,0x04,0x78,0x04,0x03},
    [90] = {0x61,0x51,0x49,0x45,0x43},
    [91] = {0x00,0x00,0x7F,0x41,0x41},
    [92] = {0x02,0x04,0x08,0x10,0x20},
    [93] = {0x41,0x41,0x7F,0x00,0x00},
    [94] = {0x04,0x02,0x01,0x02,0x04},
    [95] = {0x40,0x40,0x40,0x40,0x40},
    [96] = {0x00,0x01,0x02,0x04,0x00},
    [97] = {0x20,0x54,0x54,0x54,0x78},
    [98] = {0x7F,0x48,0x44,0x44,0x38},
    [99] = {0x38,0x44,0x44,0x44,0x20},
    [100] = {0x38,0x44,0x44,0x48,0x7F},
    [101] = {0x38,0x54,0x54,0x54,0x18},
    [102] = {0x08,0x7E,0x09,0x01,0x02},
    [103] = {0x08,0x14,0x54,0x54,0x3C},
    [104] = {0x7F,0x08,0x04,0x04,0x78},
    [105] = {0x00,0x44,0x7D,0x40,0x00},
    [106] = {0x20,0x40,0x44,0x3D,0x00},
    [107] = {0x00,0x7F,0x10,0x28,0x44},
    [108] = {0x00,0x41,0x7F,0x40,0x00},
    [109] = {0x7C,0x04,0x18,0x04,0x78},
    [110] = {0x7C,0x08,0x04,0x04,0x78},
    [111] = {0x38,0x44,0x44,0x44,0x38},
    [112] = {0xFC,0x18,0x24,0x24,0x18},
    [113] = {0x18,0x24,0x24,0x18,0xFC},
    [114] = {0x7C,0x08,0x04,0x04,0x08},
    [115] = {0x48,0x54,0x54,0x54,0x20},
    [116] = {0x04,0x3F,0x44,0x40,0x20},
    [117] = {0x3C,0x40,0x40,0x20,0x7C},
    [118] = {0x1C,0x20,0x40,0x20,0x1C},
    [119] = {0x3C,0x40,0x30,0x40,0x3C},
    [120] = {0x44,0x28,0x10,0x28,0x44},
    [121] = {0x0C,0x50,0x50,0x50,0x3C},
    [122] = {0x44,0x64,0x54,0x4C,0x44},
    [123] = {0x00,0x08,0x36,0x41,0x00},
    [124] = {0x00,0x00,0x7F,0x00,0x00},
    [125] = {0x00,0x41,0x36,0x08,0x00},
    [126] = {0x10,0x08,0x08,0x10,0x08},
    [127] = {0x00,0x00,0x00,0x00,0x00}
};

const uint8_t degree_symbol[5] = {0x0E, 0x11, 0x11, 0x0E, 0x00};

static void OLED_I2C_Init(void);
static void OLED_I2C_Start(void);
static void OLED_I2C_Stop(void);
static void OLED_I2C_SendByte(uint8_t Byte);
static void OLED_WriteCommand(uint8_t cmd);
static void OLED_WriteData(uint8_t data);
static void OLED_SetPosition(uint8_t x, uint8_t y);
static void OLED_DrawChar(uint8_t x, uint8_t y, char ch);
static void OLED_DrawString(uint8_t x, uint8_t y, const char* str);
static void OLED_DrawHLine(uint8_t x, uint8_t y, uint8_t width);

static void OLED_I2C_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitStructure.Pin = OLED_SCL_PIN | OLED_SDA_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(OLED_PORT, &GPIO_InitStructure);
    OLED_W_SCL(1);
    OLED_W_SDA(1);
}

static void OLED_I2C_Start(void) {
    OLED_W_SDA(1);
    OLED_W_SCL(1);
    OLED_W_SDA(0);
    OLED_W_SCL(0);
}

static void OLED_I2C_Stop(void) {
    OLED_W_SDA(0);
    OLED_W_SCL(1);
    OLED_W_SDA(1);
}

static void OLED_I2C_SendByte(uint8_t Byte) {
    for (uint8_t i = 0; i < 8; i++) {
        OLED_W_SDA(!!(Byte & (0x80 >> i)));
        OLED_W_SCL(1);
        OLED_W_SCL(0);
    }
    OLED_W_SCL(1);
    OLED_W_SCL(0);
}

static void OLED_WriteCommand(uint8_t cmd) {
    OLED_I2C_Start();
    OLED_I2C_SendByte(OLED_I2C_ADDR);
    OLED_I2C_SendByte(0x00);
    OLED_I2C_SendByte(cmd);
    OLED_I2C_Stop();
}

static void OLED_WriteData(uint8_t data) {
    OLED_I2C_Start();
    OLED_I2C_SendByte(OLED_I2C_ADDR);
    OLED_I2C_SendByte(0x40);
    OLED_I2C_SendByte(data);
    OLED_I2C_Stop();
}

static void OLED_SetPosition(uint8_t x, uint8_t y) {
    OLED_WriteCommand(0xB0 + y);
    OLED_WriteCommand(0x00 + (x & 0x0F));
    OLED_WriteCommand(0x10 + (x >> 4));
}

static void OLED_DrawChar(uint8_t x, uint8_t y, char ch) {
    if (ch == 176) {
        for (uint8_t i = 0; i < 5; i++) {
            OLED_SetPosition(x + i, y);
            OLED_WriteData(degree_symbol[i]);
        }
        return;
    }
    
    if (ch < 32 || ch > 127) {
        ch = ' ';
    }
    
    for (uint8_t i = 0; i < 5; i++) {
        OLED_SetPosition(x + i, y);
        OLED_WriteData(font5x7[(uint8_t)ch][i]);
    }
}

static void OLED_DrawString(uint8_t x, uint8_t y, const char* str) {
    uint8_t pos = x;
    while (*str && pos < OLED_WIDTH - 5) {
        OLED_DrawChar(pos, y, *str++);
        pos += 6;
    }
}

static void OLED_DrawHLine(uint8_t x, uint8_t y, uint8_t width) {
    OLED_SetPosition(x, y);
    for (uint8_t i = 0; i < width && (x + i) < OLED_WIDTH; i++) {
        OLED_WriteData(0xFF);
    }
}

void OLED_Init(void) {
    OLED_I2C_Init();
    HAL_Delay(200);
    
    OLED_WriteCommand(0xAE);
    OLED_WriteCommand(0xD5);
    OLED_WriteCommand(0x80);
    OLED_WriteCommand(0xA8);
    OLED_WriteCommand(0x3F);
    OLED_WriteCommand(0xD3);
    OLED_WriteCommand(0x00);
    OLED_WriteCommand(0x40);
    OLED_WriteCommand(0xA1);
    OLED_WriteCommand(0xC8);
    OLED_WriteCommand(0xDA);
    OLED_WriteCommand(0x12);
    OLED_WriteCommand(0x81);
    OLED_WriteCommand(0xCF);
    OLED_WriteCommand(0xD9);
    OLED_WriteCommand(0xF1);
    OLED_WriteCommand(0xDB);
    OLED_WriteCommand(0x30);
    OLED_WriteCommand(0xA4);
    OLED_WriteCommand(0xA6);
    OLED_WriteCommand(0x8D);
    OLED_WriteCommand(0x14);
    OLED_WriteCommand(0xAF);
    
    OLED_Clear();
}

void OLED_Clear(void) {
    for (uint8_t page = 0; page < OLED_PAGES; page++) {
        OLED_SetPosition(0, page);
        for (uint8_t i = 0; i < OLED_WIDTH; i++) {
            OLED_WriteData(0x00);
        }
    }
}

void OLED_ShowStartupScreen(void) {
    OLED_Clear();
    OLED_DrawString(28, 1, "STM32");
    OLED_DrawString(18, 3, "TEMP MONITOR");
    OLED_DrawString(30, 5, "v2.0");
    HAL_Delay(2000);
}

void OLED_DrawTemperatureBar(uint8_t x, uint8_t y, uint8_t width, float temp) {
    float temp_range = TEMP_MAX - TEMP_MIN;
    float temp_normalized = (temp - TEMP_MIN) / temp_range;
    
    if (temp_normalized < 0) temp_normalized = 0;
    if (temp_normalized > 1) temp_normalized = 1;
    
    uint8_t filled_width = (uint8_t)(width * temp_normalized);
    
    for (uint8_t page = 0; page < 2; page++) {
        OLED_SetPosition(x, y + page);
        for (uint8_t i = 0; i < width; i++) {
            if (i < filled_width) {
                OLED_WriteData(0xFF);
            } else {
                OLED_WriteData(0x81);
            }
        }
    }
}

void OLED_ShowAlarmScreen(float temp, const char* message) {
    OLED_Clear();
    OLED_DrawString(10, 1, "!! WARNING !!");
    
    char temp_str[32];
    int temp_int = (int)temp;
    int temp_dec = (int)((temp - temp_int) * 10 + 0.5f);
    sprintf(temp_str, "TEMP:%d.%d%cC", temp_int, temp_dec, 176);
    OLED_DrawString(20, 3, temp_str);
    
    if (message) {
        OLED_DrawString(0, 5, message);
    }
}

static uint8_t display_page = 0;
static uint32_t last_screen_refresh = 0;
static uint8_t pending_refresh = 0;

void OLED_SetDisplayPage(uint8_t page) {
    display_page = page % 2;
    pending_refresh = 1;
}

void OLED_ToggleDisplayPage(void) {
    display_page = (display_page + 1) % 2;
    pending_refresh = 1;
}

void OLED_UpdateDisplay(const TempData* data, const AlarmConfig* alarm) {
    uint32_t current_time = HAL_GetTick();
    
    if (alarm->warning_enabled && 
        (data->current_temp > alarm->warning_high || data->current_temp < alarm->warning_low)) {
        OLED_ShowAlarmScreen(data->current_temp, 
            (data->current_temp > alarm->warning_high) ? "HIGH TEMP!" : "LOW TEMP!");
        last_screen_refresh = current_time;
        pending_refresh = 0;
        return;
    }
    
    if (!pending_refresh) {
        return;
    }
    
    if (display_page == 0) {
        OLED_Clear();
        OLED_DrawString(0, 0, "TEMP MONITOR");
        
        char temp_str[32];
        int temp_int = (int)data->current_temp;
        int temp_dec = (int)((data->current_temp - temp_int) * 10 + 0.5f);
        sprintf(temp_str, "T:%d.%d%cC", temp_int, temp_dec, 176);
        OLED_DrawString(0, 2, temp_str);
        
        OLED_DrawTemperatureBar(0, 4, 128, data->current_temp);
        
        char status_str[16];
        switch (data->status) {
            case STATUS_OK: strcpy(status_str, "STATUS: OK"); break;
            case STATUS_WARNING: strcpy(status_str, "STATUS: WARN"); break;
            case STATUS_ERROR: strcpy(status_str, "STATUS: ERR"); break;
            default: strcpy(status_str, "STATUS: ???"); break;
        }
        OLED_DrawString(0, 7, status_str);
        
    } else {
        OLED_Clear();
        OLED_DrawString(0, 0, "STATISTICS");
        
        char min_str[32], max_str[32], adc_str[32];
        
        int min_int = (int)data->temp_min;
        int min_dec = (int)((data->temp_min - min_int) * 10 + 0.5f);
        int max_int = (int)data->temp_max;
        int max_dec = (int)((data->temp_max - max_int) * 10 + 0.5f);
        
        sprintf(min_str, "MIN:%d.%d%cC", min_int, min_dec, 176);
        sprintf(max_str, "MAX:%d.%d%cC", max_int, max_dec, 176);
        sprintf(adc_str, "ADC:%4lu", data->adc_raw);
        
        OLED_DrawString(0, 2, min_str);
        OLED_DrawString(0, 3, max_str);
        OLED_DrawString(0, 4, adc_str);
        
        char samples_str[32];
        sprintf(samples_str, "SAMPLES:%lu", data->sample_count);
        OLED_DrawString(0, 6, samples_str);
    }
    
    last_screen_refresh = HAL_GetTick();
    pending_refresh = 0;
}