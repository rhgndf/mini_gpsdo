
#include "stm32g4xx_hal.h"
#include "tusb.h"

#include "usb.h"

bool USBInit(void) {

  /* Enable CRS Clock */
  RCC_CRSInitTypeDef RCC_CRSInitStruct = {0};
  __HAL_RCC_CRS_CLK_ENABLE();

  /* Default Synchro Signal division factor (not divided) */
  RCC_CRSInitStruct.Prescaler = RCC_CRS_SYNC_DIV1;

  /* Set the SYNCSRC[1:0] bits according to CRS_Source value */
  RCC_CRSInitStruct.Source = RCC_CRS_SYNC_SOURCE_USB;

  /* HSI48 is synchronized with USB SOF at 1KHz rate */
  RCC_CRSInitStruct.ReloadValue =  __HAL_RCC_CRS_RELOADVALUE_CALCULATE(48000000, 1000);
  RCC_CRSInitStruct.ErrorLimitValue = RCC_CRS_ERRORLIMIT_DEFAULT;

  /* Set the TRIM[5:0] to the default value */
  RCC_CRSInitStruct.HSI48CalibrationValue = RCC_CRS_HSI48CALIBRATION_DEFAULT;

  /* Start automatic synchronization */
  HAL_RCCEx_CRSConfig(&RCC_CRSInitStruct);

  RCC_PeriphCLKInitTypeDef RCC_PeriphClkInitStruct = {0};
  RCC_PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USB;
  RCC_PeriphClkInitStruct.UsbClockSelection    = RCC_USBCLKSOURCE_HSI48;
  HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInitStruct);

  __HAL_RCC_USB_CLK_ENABLE();
  const tusb_rhport_init_t dev_init = {
    .role = TUSB_ROLE_DEVICE,
    .speed = TUSB_SPEED_FULL
  };
  return tusb_init(0, &dev_init);
}

int _read(int file, char *ptr, int len)
{
  (void)file;
  return len;
}

int _write(int file, char *ptr, int len)
{
  (void)file;
  tud_cdc_n_write(1, (uint8_t *)ptr, len);
  tud_cdc_n_write_flush(1);
  return len;
}

void USB_HP_IRQHandler(void) {
  tud_int_handler(0);
}

void USB_LP_IRQHandler(void) {
  tud_int_handler(0);
}

void USBWakeUP_IRQHandler(void) {
  tud_int_handler(0);
}