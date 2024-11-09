#ifndef GPSDO_AVERAGING_H
#define GPSDO_AVERAGING_H

#include "stdint.h"

void GPSDOAverageAddCount(int16_t count);
int32_t GPSDOAverageGet4(void);
int32_t GPSDOAverageGet16(void);
int32_t GPSDOAverageGet64(void);
int32_t GPSDOAverageGet256(void);
uint32_t GPSDOAverageGetIdx(void);
void GPSDOAverageReset(void);

#endif