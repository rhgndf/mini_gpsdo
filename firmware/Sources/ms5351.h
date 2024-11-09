#ifndef MS5351_H
#define MS5351_H

bool MS5351Init(void);
void MS5351LoadSettings(void);
void MS5351I2CRead(uint8_t reg, uint8_t *buf, uint32_t len);

#endif