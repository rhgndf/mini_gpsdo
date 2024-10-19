#include <stdbool.h>

#include "stm32g4xx_hal.h"
#include "tusb.h"

#include "gps.h"

UART_HandleTypeDef huart3;
void GPSRxCallback();

bool GPSInit(void)
{

    GPIO_InitTypeDef GPIO_InitStructure;

    // PB10, PB11
    GPIO_InitStructure.Pin = GPIO_PIN_10 | GPIO_PIN_11;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART3;
    PeriphClkInit.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        return false;
    }

    __HAL_RCC_USART3_CLK_ENABLE();

    huart3.Instance = USART3;
    huart3.Init.BaudRate = 115200;
    huart3.Init.WordLength = UART_WORDLENGTH_8B;
    huart3.Init.StopBits = UART_STOPBITS_1;     
    huart3.Init.Parity = UART_PARITY_NONE;
    huart3.Init.Mode = UART_MODE_TX_RX;
    huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart3.Init.OverSampling = UART_OVERSAMPLING_16;
    huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart3.Init.ClockPrescaler = UART_PRESCALER_DIV1;
    huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    huart3.RxISR = GPSRxCallback;

    HAL_UART_Init(&huart3);

    if (HAL_UARTEx_SetTxFifoThreshold(&huart3, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
    {
        return false;
    }
    if (HAL_UARTEx_SetRxFifoThreshold(&huart3, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
    {
        return false;
    }
    if (HAL_UARTEx_EnableFifoMode(&huart3) != HAL_OK)
    {
        return false;
    }
    USART3->CR1 |= USART_CR1_RXNEIE;
    USART3->CR3 |= USART_CR3_RXFTIE;
    NVIC_EnableIRQ(USART3_IRQn);
    return true;
}

void USART3_IRQHandler(void)
{
    uint8_t data;
    while(HAL_UART_Receive(&huart3, &data, 1, 0) == HAL_OK)
    {
        tud_cdc_n_write_char(0, data);
        if(data == '\n')
        {
            tud_cdc_n_write_flush(0);
        }
    }
}

void GPSRxCallback(UART_HandleTypeDef *huart)
{
}