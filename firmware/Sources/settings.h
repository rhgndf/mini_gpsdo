#ifndef SETTINGS_H
#define SETTINGS_H

#include "stm32g4xx_hal.h"

#include <stdbool.h>
#include <stdint.h>

#define SETTINGS_PAGE_0 ((uint32_t)0x0801F000) /* Base @ of Page 62, 2 Kbytes */
#define SETTINGS_PAGE_1 ((uint32_t)0x0801F800) /* Base @ of Page 63, 2 Kbytes */
#define SETTINGS_SIZE (FLASH_PAGE_SIZE / sizeof(uint32_t))
#define SETTINGS_MAGIC_VALUE 0x12345678

enum SettingsU8Offset {
    SETTINGS_CLK0_SOURCE,
    SETTINGS_CLK0_DRIVE,
    SETTINGS_CLK0_PHASE,
    SETTINGS_CLK0_INTEGER,
    SETTINGS_CLK0_RDIV,
    SETTINGS_CLK1_SOURCE,
    SETTINGS_CLK1_DRIVE,
    SETTINGS_CLK1_PHASE,
    SETTINGS_CLK1_INTEGER,
    SETTINGS_CLK1_RDIV,
    SETTINGS_CLK2_SOURCE,
    SETTINGS_CLK2_DRIVE,
    SETTINGS_CLK2_PHASE,
    SETTINGS_CLK2_INTEGER,
    SETTINGS_CLK2_RDIV,
    SETTINGS_SIZE_U8,
};

enum SettingsU16Offset {
    SETTINGS_USB_VID,
    SETTINGS_USB_PID,
    SETTINGS_SIZE_U16,
};

enum SettingsU32Offset {
    SETTINGS_PLL_A_MULT,
    SETTINGS_PLL_A_NUM,
    SETTINGS_PLL_A_DENOM,
    SETTINGS_PLL_B_MULT,
    SETTINGS_PLL_B_NUM,
    SETTINGS_PLL_B_DENOM,

    SETTINGS_CLK0_DIV,
    SETTINGS_CLK0_NUM,
    SETTINGS_CLK0_DENOM,
    SETTINGS_CLK1_DIV,
    SETTINGS_CLK1_NUM,
    SETTINGS_CLK1_DENOM,
    SETTINGS_CLK2_DIV,
    SETTINGS_CLK2_NUM,
    SETTINGS_CLK2_DENOM,
    SETTINGS_SIZE_U32,
};


struct Settings {
    union {
        struct {
            uint32_t magic;
            uint32_t format;
            uint32_t version;
            uint32_t u32[SETTINGS_SIZE_U32];
            uint16_t u16[SETTINGS_SIZE_U16];
            uint8_t u8[SETTINGS_SIZE_U8];
        };
        struct {
            uint32_t raw[SETTINGS_SIZE - 1];
        };
    };
    uint32_t crc;
};

_Static_assert(sizeof(struct Settings) == FLASH_PAGE_SIZE, "Settings struct size must match flash page size");


bool SettingsInit(void);
bool SettingsCommit(void);
void SettingsReset(void);

uint32_t SettingsGetRaw(uint32_t offset, void* dest, uint32_t len);

uint32_t SettingsGetU32(enum SettingsU32Offset offset);
uint16_t SettingsGetU16(enum SettingsU16Offset offset);
uint8_t SettingsGetU8(enum SettingsU8Offset offset);

void SettingsSetU32(enum SettingsU32Offset offset, uint32_t value);
void SettingsSetU16(enum SettingsU16Offset offset, uint16_t value);
void SettingsSetU8(enum SettingsU8Offset offset, uint8_t value);
#endif