#include <stdbool.h>
#include <stdio.h>

#include "stm32g4xx_hal.h"
#include "tusb.h"
#include "settings.h"
#include "ms5351.h"

#include "si5351.h"

I2C_HandleTypeDef MS5351_i2c;
#define MS5351_ADDRESS 0xC0

bool MS5351Init(void) {
    HAL_StatusTypeDef status;
    GPIO_InitTypeDef GPIO_InitStructure;

    // PB7 I2C1_SDA
    GPIO_InitStructure.Pin =  GPIO_PIN_7;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

    // PA15 I2C1_SCL
    GPIO_InitStructure.Pin =  GPIO_PIN_15;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

    RCC_PeriphCLKInitTypeDef  RCC_PeriphCLKInitStruct;
    RCC_PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
    RCC_PeriphCLKInitStruct.I2c1ClockSelection = RCC_I2C1CLKSOURCE_SYSCLK;
    HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct);
    
    __HAL_RCC_I2C1_CLK_ENABLE();
    MS5351_i2c.Instance = I2C1;
    MS5351_i2c.Init.Timing =
        (0xF << I2C_TIMINGR_PRESC_Pos) |
        (0xC8 << I2C_TIMINGR_SCLL_Pos) |
        (0xA0 << I2C_TIMINGR_SCLH_Pos) |
        (0x14 << I2C_TIMINGR_SDADEL_Pos) |
        (0x32 << I2C_TIMINGR_SCLDEL_Pos);
    MS5351_i2c.Init.OwnAddress1 = 0;
    MS5351_i2c.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    MS5351_i2c.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    MS5351_i2c.Init.OwnAddress2 = 0;
    MS5351_i2c.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    MS5351_i2c.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    MS5351_i2c.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    
    if((status = HAL_I2C_Init(&MS5351_i2c)) != HAL_OK) {
        return false;
    }

    HAL_I2CEx_AnalogFilter_Config(&MS5351_i2c, I2C_ANALOGFILTER_ENABLE);

    __HAL_SYSCFG_FASTMODEPLUS_ENABLE(I2C_FASTMODEPLUS_I2C1);

    MS5351LoadSettings();
    return true;
}

void MS5351LoadSettings(void) {
    si5351_Init(0);

    uint8_t outputEnable = 0;
    si5351_EnableOutputs(0);

    si5351OutputConfig_t clk0Config;
    clk0Config.allowIntegerMode = SettingsGetU8(SETTINGS_CLK0_INTEGER);
    clk0Config.div = SettingsGetU32(SETTINGS_CLK0_DIV);
    clk0Config.num = SettingsGetU32(SETTINGS_CLK0_NUM);
    clk0Config.denom = SettingsGetU32(SETTINGS_CLK0_DENOM);
    clk0Config.rdiv = SettingsGetU8(SETTINGS_CLK0_RDIV);
    si5351PLL_t clk0Source = SettingsGetU8(SETTINGS_CLK0_SOURCE);
    si5351DriveStrength_t clk0Drive = SettingsGetU8(SETTINGS_CLK0_DRIVE);
    uint8_t clk0Phase = SettingsGetU8(SETTINGS_CLK0_PHASE);
    if (clk0Config.div != 0 && clk0Config.denom != 0 && clk0Drive < SI5351_DRIVE_STRENGTH_NONE) {
        si5351_SetupOutput(0, clk0Source, clk0Drive, &clk0Config, clk0Phase);
        outputEnable |= 1 << 0;
    }
    
    si5351OutputConfig_t clk1Config;
    clk1Config.allowIntegerMode = SettingsGetU8(SETTINGS_CLK1_INTEGER);
    clk1Config.div = SettingsGetU32(SETTINGS_CLK1_DIV);
    clk1Config.num = SettingsGetU32(SETTINGS_CLK1_NUM);
    clk1Config.denom = SettingsGetU32(SETTINGS_CLK1_DENOM);
    clk1Config.rdiv = SettingsGetU8(SETTINGS_CLK1_RDIV);
    si5351PLL_t clk1Source = SettingsGetU8(SETTINGS_CLK1_SOURCE);
    si5351DriveStrength_t clk1Drive = SettingsGetU8(SETTINGS_CLK1_DRIVE);
    uint8_t clk1Phase = SettingsGetU8(SETTINGS_CLK1_PHASE);
    if (clk1Config.div != 0 && clk1Config.denom != 0 && clk1Drive < SI5351_DRIVE_STRENGTH_NONE) {
        si5351_SetupOutput(1, clk1Source, clk1Drive, &clk1Config, clk1Phase);
        outputEnable |= 1 << 1;
    }

    si5351OutputConfig_t clk2Config;
    clk2Config.allowIntegerMode = SettingsGetU8(SETTINGS_CLK2_INTEGER);
    clk2Config.div = SettingsGetU32(SETTINGS_CLK2_DIV);
    clk2Config.num = SettingsGetU32(SETTINGS_CLK2_NUM);
    clk2Config.denom = SettingsGetU32(SETTINGS_CLK2_DENOM);
    clk2Config.rdiv = SettingsGetU8(SETTINGS_CLK2_RDIV);
    si5351PLL_t clk2Source = SettingsGetU8(SETTINGS_CLK2_SOURCE);
    si5351DriveStrength_t clk2Drive = SettingsGetU8(SETTINGS_CLK2_DRIVE);
    uint8_t clk2Phase = SettingsGetU8(SETTINGS_CLK2_PHASE);
    if (clk2Config.div != 0 && clk2Config.denom != 0 && clk2Drive < SI5351_DRIVE_STRENGTH_NONE) {
        si5351_SetupOutput(2, clk2Source, clk2Drive, &clk2Config, clk2Phase);
        outputEnable |= 1 << 2;
    }

    si5351PLLConfig_t pllAConfig;
    pllAConfig.mult = SettingsGetU32(SETTINGS_PLL_A_MULT);
    pllAConfig.num = SettingsGetU32(SETTINGS_PLL_A_NUM);
    pllAConfig.denom = SettingsGetU32(SETTINGS_PLL_A_DENOM);
    if (pllAConfig.mult != 0 && pllAConfig.denom != 0) {
        si5351_SetupPLL(SI5351_PLL_A, &pllAConfig);
    }

    si5351PLLConfig_t pllBConfig;
    pllBConfig.mult = SettingsGetU32(SETTINGS_PLL_B_MULT);
    pllBConfig.num = SettingsGetU32(SETTINGS_PLL_B_NUM);
    pllBConfig.denom = SettingsGetU32(SETTINGS_PLL_B_DENOM);
    if (pllBConfig.mult != 0 && pllBConfig.denom != 0) {
        si5351_SetupPLL(SI5351_PLL_B, &pllBConfig);
    }

    si5351_EnableOutputs(outputEnable);
}

void MS5351I2CRead(uint8_t reg, uint8_t *buf, uint32_t len) {
    HAL_I2C_Master_Transmit(&MS5351_i2c, MS5351_ADDRESS, &reg, 1, 1000);
    HAL_I2C_Master_Receive(&MS5351_i2c, MS5351_ADDRESS, buf, len, 1000);
}