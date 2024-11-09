#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "stm32g4xx_hal.h"
#include <string.h>

#include "gpsdo_averaging.h"

enum GPSState
{
    GPS_STATE_ADJUST_1,
    GPS_STATE_ADJUST_4,
    GPS_STATE_ADJUST_16,
    GPS_STATE_ADJUST_64,
    GPS_STATE_ADJUST_256,
};

enum GPSState GPSState = GPS_STATE_ADJUST_1;
uint32_t downgradeHysteresis = 0;
uint32_t frequencyTarget;

int64_t DAC_value = 0x80000000;

#define DAC_DMA_BUFFER_LENGTH 64
uint16_t DAC_dma[DAC_DMA_BUFFER_LENGTH] __attribute__((aligned(16)));
DAC_HandleTypeDef hdac;
DMA_HandleTypeDef hdma_dac_ch1;

int32_t DAC_error = 0;

uint32_t saturating_add(uint32_t a, uint32_t b)
{
    if (a > 0xFFFFFFFF - b)
    {
        return 0xFFFFFFFF;
    }
    return a + b;
}

uint32_t saturating_sub(uint32_t a, uint32_t b)
{
    if (a < b)
    {
        return 0;
    }
    return a - b;
}

void SetDACValue(int64_t value)
{
    for (int i = 0; i < DAC_DMA_BUFFER_LENGTH; i++)
    {
        uint32_t DAC_output = saturating_add(value, DAC_error);
        DAC_dma[i] = DAC_output >> 16;
        DAC_error = DAC_output & 0xFFFFF;
    }
}

uint32_t GetTIM2Frequency(void)
{
    RCC_ClkInitTypeDef RCC_ClkConfig;
    uint32_t latency;
    HAL_RCC_GetClockConfig(&RCC_ClkConfig, &latency);
    if (RCC_ClkConfig.APB1CLKDivider == RCC_HCLK_DIV1)
    {
        return HAL_RCC_GetPCLK1Freq();
    }
    else
    {
        return HAL_RCC_GetPCLK1Freq() * 2;
    }
}

uint32_t GetTIM15Frequency(void)
{
    RCC_ClkInitTypeDef RCC_ClkConfig;
    uint32_t latency;
    HAL_RCC_GetClockConfig(&RCC_ClkConfig, &latency);
    if (RCC_ClkConfig.APB2CLKDivider == RCC_HCLK_DIV1)
    {
        return HAL_RCC_GetPCLK2Freq();
    }
    else
    {
        return HAL_RCC_GetPCLK2Freq() * 2;
    }
}

bool GPSDOInit(void)
{

    GPIO_InitTypeDef GPIO_InitStructure = {0};
    GPIO_InitStructure.Pin = GPIO_PIN_5;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

    __HAL_RCC_TIM2_CLK_ENABLE();
    TIM_HandleTypeDef htim2 = {0};
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 0;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 0xFFFFFFFF;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.RepetitionCounter = 0;
    HAL_TIM_IC_Init(&htim2);
    TIM_ClockConfigTypeDef sClockSourceConfig;
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig);

    TIM_IC_InitTypeDef sConfigIC = {0};
    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
    sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
    sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
    sConfigIC.ICFilter = 0;
    HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_1);
    HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);

    GPIO_InitTypeDef GPIO_InitStructure_DAC = {0};
    GPIO_InitStructure_DAC.Pin = GPIO_PIN_2;
    GPIO_InitStructure_DAC.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStructure_DAC.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure_DAC);

    // Enable OpAmp1
    OPAMP_HandleTypeDef hopamp = {0};
    hopamp.Instance = OPAMP1;
    hopamp.Init.PowerMode = OPAMP_POWERMODE_HIGHSPEED;
    hopamp.Init.Mode = OPAMP_FOLLOWER_MODE;
    hopamp.Init.NonInvertingInput = OPAMP_NONINVERTINGINPUT_DAC;
    hopamp.Init.UserTrimming = OPAMP_TRIMMING_FACTORY;
    HAL_OPAMP_Init(&hopamp);
    HAL_OPAMP_Start(&hopamp);

    __HAL_RCC_DMA1_CLK_ENABLE();
    __HAL_RCC_DMAMUX1_CLK_ENABLE();
    hdma_dac_ch1.Instance = DMA1_Channel1;
    hdma_dac_ch1.Init.Request = DMA_REQUEST_DAC3_CHANNEL1;
    hdma_dac_ch1.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_dac_ch1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_dac_ch1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_dac_ch1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_dac_ch1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_dac_ch1.Init.Mode = DMA_CIRCULAR;
    hdma_dac_ch1.Init.Priority = DMA_PRIORITY_HIGH;
    HAL_DMA_Init(&hdma_dac_ch1);

    __HAL_LINKDMA(&hdac, DMA_Handle1, hdma_dac_ch1);

    __HAL_RCC_DAC3_CLK_ENABLE();
    hdac.Instance = DAC3;
    HAL_DAC_Init(&hdac);

    // Configure DAC3
    DAC_ChannelConfTypeDef sConfig = {0};
    sConfig.DAC_HighFrequency = DAC_HIGH_FREQUENCY_INTERFACE_MODE_AUTOMATIC;
    sConfig.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_ENABLE;
    sConfig.DAC_Trigger = DAC_TRIGGER_T15_TRGO;
    sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;
    sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_INTERNAL;
    sConfig.DAC_UserTrimming = DAC_TRIMMING_FACTORY;
    HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1);

    __HAL_RCC_TIM15_CLK_ENABLE();
    TIM_HandleTypeDef htim15 = {0};
    htim15.Instance = TIM15;
    htim15.Init.Prescaler = 0;
    htim15.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim15.Init.Period = 85 - 1;
    htim15.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim15.Init.RepetitionCounter = 0;
    HAL_TIM_Base_Init(&htim15);

    TIM_ClockConfigTypeDef sClockSourceConfigTIM15 = {0};
    sClockSourceConfigTIM15.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    HAL_TIM_ConfigClockSource(&htim15, &sClockSourceConfigTIM15);

    TIM_MasterConfigTypeDef sMasterConfig = {0};
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&htim15, &sMasterConfig);

    frequencyTarget = GetTIM2Frequency();

    SetDACValue(0x80000000);
    HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t *)DAC_dma, DAC_DMA_BUFFER_LENGTH, DAC_ALIGN_12B_L);
    HAL_TIM_Base_Start(&htim2);
    HAL_TIM_Base_Start(&htim15);

    NVIC_EnableIRQ(TIM2_IRQn);
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);
    NVIC_SetPriority(DMA1_Channel1_IRQn, 100);
    return true;
}

uint32_t last_cnt = 0;
int32_t I = 0;
void TIM2_IRQHandler(void)
{
    uint32_t cnt = TIM2->CCR1;
    uint32_t cnt_diff;
    if (last_cnt > cnt)
    {
        cnt_diff = 0xFFFFFFFF - last_cnt + cnt;
    }
    else
    {
        cnt_diff = cnt - last_cnt;
    }
    last_cnt = cnt;
    int32_t cnt_err = (int32_t)cnt_diff - frequencyTarget;

    // Reject invalid values
    if (abs(cnt_err) > 2000)
    {
        GPSState = GPS_STATE_ADJUST_1;
        return;
    }

    GPSDOAverageAddCount(cnt_err);
    uint32_t idx = GPSDOAverageGetIdx();
    switch (GPSState)
    {
    case GPS_STATE_ADJUST_1:
        // Adjust DAC value until PPS is within 2 Hz of target
        DAC_value -= cnt_err << 20;
        if (abs(cnt_err) < 2)
        {
            GPSState = GPS_STATE_ADJUST_4;
        }
        break;

    case GPS_STATE_ADJUST_4:
        if (idx % 4) {
            return;
        }
        cnt_err = GPSDOAverageGet4();
        DAC_value -= cnt_err << 18;
        if (abs(cnt_err) < 2)
        {
            GPSState = GPS_STATE_ADJUST_16;
        }
        else if (abs(cnt_err) > 4)
        {
            downgradeHysteresis++;
            if (downgradeHysteresis > 10)
            {
                GPSState = GPS_STATE_ADJUST_1;
                downgradeHysteresis = 0;
            }
        }
        break;
    case GPS_STATE_ADJUST_16:
        if (idx % 16) {
            return;
        }
        cnt_err = GPSDOAverageGet16();
        DAC_value -= cnt_err << 16;
        if (abs(cnt_err) < 4)
        {
            GPSState = GPS_STATE_ADJUST_64;
        }
        else if (abs(cnt_err) > 8)
        {
            downgradeHysteresis++;
            if (downgradeHysteresis > 10)
            {
                GPSState = GPS_STATE_ADJUST_4;
                downgradeHysteresis = 0;
            }
        }
        break;
    case GPS_STATE_ADJUST_64:
        if (idx % 64) {
            return;
        }
        cnt_err = GPSDOAverageGet64();
        DAC_value -= cnt_err << 14;
        if (abs(cnt_err) < 4)
        {
            GPSState = GPS_STATE_ADJUST_256;
        }
        else if (abs(cnt_err) > 16)
        {
            downgradeHysteresis++;
            if (downgradeHysteresis > 10)
            {
                GPSState = GPS_STATE_ADJUST_16;
                downgradeHysteresis = 0;
            }
        }
        break;
    case GPS_STATE_ADJUST_256:
        if (idx % 256) {
            return;
        }
        cnt_err = GPSDOAverageGet256();
        DAC_value -= cnt_err << 11;
        if (abs(cnt_err) > 128)
        {
            downgradeHysteresis++;
            if (downgradeHysteresis > 10)
            {
                GPSState = GPS_STATE_ADJUST_64;
                downgradeHysteresis = 0;
            }
        }
        break;
    }

    if (DAC_value < 0)
    {
        DAC_value = 0LL;
    }
    else if (DAC_value > 0xFFFFFFFFLL)
    {
        DAC_value = 0xFFFFFFFFLL;
    }
    SetDACValue(DAC_value);

    int divide = 1;
    switch(GPSState)
    {
        case GPS_STATE_ADJUST_1:
            divide = 1;
            break;
        case GPS_STATE_ADJUST_4:
            divide = 4;
            break;
        case GPS_STATE_ADJUST_16:
            divide = 16;
            break;
        case GPS_STATE_ADJUST_64:
            divide = 64;
            break;
        case GPS_STATE_ADJUST_256:
            divide = 256;
            break;
    }

    int16_t frequencyInteger = cnt_err / divide;
    int32_t frequencyFraction = (cnt_err + divide * 10000) % divide * 1000000 / divide;
    if (cnt_err < 0 && cnt_err % divide != 0)
    {
        frequencyInteger--;
    }
    printf("[PPS] State: %d, Current Frequency: %ld.%06ld, %ld, DAC value: %lX\n", divide, frequencyTarget + frequencyInteger, frequencyFraction, cnt_err, (uint32_t)(DAC_value & 0xFFFFFFFF));

}

void DMA1_CH1_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_dac_ch1);
}