#include <stdbool.h>
#include <stdio.h>

#include "stm32g4xx_hal.h"
#include "tusb.h"

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

    return true;
}

void MS5351Scan(void) {
    // Scan I2C bus by reading first byte
    uint8_t data;
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&MS5351_i2c, 0xC0, 0, 1, &data, 1, 100);
    printf("I2C address %02X: %02X %d %ld\n", 0xC0, data, status, MS5351_i2c.ErrorCode);
}