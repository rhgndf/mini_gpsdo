#ifndef GPS_H
#define GPS_H

bool GPSInit(void);
bool GPSSetBaudRate(uint32_t baudrate);
bool GPSSend(uint8_t *data, uint32_t len);

#endif