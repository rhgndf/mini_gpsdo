#include <stdbool.h>

#include "stm32g4xx_hal.h"
#include "tusb.h"

#include "settings.h"

struct Settings *settings_page_0 = (struct Settings*)SETTINGS_PAGE_0;
struct Settings *settings_page_1 = (struct Settings*)SETTINGS_PAGE_1;

struct Settings settings = {0};

const struct Settings default_settings = {
    .magic = SETTINGS_MAGIC_VALUE,
    .format = 1,
    .version = 1,
    .u16 = {
        [SETTINGS_USB_VID] = 0xCAFE,
        [SETTINGS_USB_PID] = 0x4012,
    },
    .crc = 0,
};

CRC_HandleTypeDef hcrc;

void SettingsWrite(uint32_t page) {
    uint32_t address = page == 0 ? SETTINGS_PAGE_0 : SETTINGS_PAGE_1;

    uint32_t crc = HAL_CRC_Calculate(&hcrc, (uint32_t*)settings.raw, SETTINGS_SIZE - 1);
    settings.crc = crc;

    HAL_FLASH_Unlock();
    FLASH_EraseInitTypeDef erase = {0};
    erase.TypeErase = FLASH_TYPEERASE_PAGES;
    erase.Page = address / FLASH_PAGE_SIZE;
    erase.NbPages = 1;
    uint32_t error;
    HAL_FLASHEx_Erase(&erase, &error);

    uint64_t* data = (uint64_t*)&settings;
    for (int i = 0; i < sizeof(settings) / sizeof(uint64_t); i++)
    {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address + i * 8, data[i]);
    }
}

bool SettingsCommit(void) {
    settings.version++;
    SettingsWrite(0);
    SettingsWrite(1);
    return true;
}

void SettingsReset(void) {
    memcpy(&settings, &default_settings, sizeof(settings));
}

uint32_t SettingsGetRaw(uint32_t offset, void* dest, uint32_t len) {
    uint8_t* src = (uint8_t*)&settings;
    if (offset + len > sizeof(struct Settings)) {
        len = sizeof(struct Settings) - offset;
        memcpy(dest, src + offset, len);
        return len;
    }
    memcpy(dest, &settings, len);
    return len;
}

uint32_t SettingsGetU32(enum SettingsU32Offset offset) {
    if (offset < SETTINGS_SIZE_U32) {
        return settings.u32[offset];
    }
    return 0; // or some error value
}


uint16_t SettingsGetU16(enum SettingsU16Offset offset) {
    if (offset < SETTINGS_SIZE_U16) {
        return settings.u16[offset];
    }
    return 0; // or some error value
}

uint8_t SettingsGetU8(enum SettingsU8Offset offset) {
    if (offset < SETTINGS_SIZE_U8) {
        return settings.u8[offset];
    }
    return 0; // or some error value
}

void SettingsSetU32(enum SettingsU32Offset offset, uint32_t value) {
    if (offset < SETTINGS_SIZE_U32) {
        settings.u32[offset] = value;
    }
}

void SettingsSetU16(enum SettingsU16Offset offset, uint16_t value) {
    if (offset < SETTINGS_SIZE_U16) {
        settings.u16[offset] = value;
    }
}

void SettingsSetU8(enum SettingsU8Offset offset, uint8_t value) {
    if (offset < SETTINGS_SIZE_U8) {
        settings.u8[offset] = value;
    }
}

bool SettingsInit(void)
{
    __HAL_RCC_CRC_CLK_ENABLE();
    hcrc.Instance = CRC;
    hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_WORDS;
    hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE;
    hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
    hcrc.Init.CRCLength = CRC_POLYLENGTH_32B;
    hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
    hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
    HAL_CRC_Init(&hcrc);
    
    bool is_page_0_valid = HAL_CRC_Calculate(&hcrc, (uint32_t*)settings_page_0, SETTINGS_SIZE - 1) == settings_page_0->crc;
    bool is_page_1_valid = HAL_CRC_Calculate(&hcrc, (uint32_t*)settings_page_1, SETTINGS_SIZE - 1) == settings_page_1->crc;

    uint32_t page_0_version = settings_page_0->version;
    uint32_t page_1_version = settings_page_1->version;

    is_page_0_valid = is_page_0_valid && settings_page_0->magic == SETTINGS_MAGIC_VALUE;
    is_page_1_valid = is_page_1_valid && settings_page_1->magic == SETTINGS_MAGIC_VALUE;

    if (is_page_0_valid && is_page_1_valid)
    {   
        if (page_0_version > page_1_version)
        {
            memcpy(&settings, settings_page_0, sizeof(settings));
            SettingsWrite(1);
        }
        else if (page_1_version > page_0_version)
        {
            memcpy(&settings, settings_page_1, sizeof(settings));
            SettingsWrite(0);
        }
        else
        {
            memcpy(&settings, settings_page_0, sizeof(settings));
        }
    }
    else if (is_page_0_valid)
    {
        memcpy(&settings, settings_page_0, sizeof(settings));
        SettingsWrite(1);
    }
    else if (is_page_1_valid)
    {
        memcpy(&settings, settings_page_1, sizeof(settings));
        SettingsWrite(0);
    }
    else
    {
        memcpy(&settings, &default_settings, sizeof(settings));
        SettingsWrite(0);
        SettingsWrite(1);
    }
    return true;
}