#include "gpsdo_averaging.h"
#include <stdint.h>
#include <string.h>

#define FREQUENCY_MEASUREMENTS 512

int16_t counts[FREQUENCY_MEASUREMENTS] = {0};
int32_t past4Counts = 0;
int32_t past16Counts = 0;
int32_t past64Counts = 0;
int32_t past256Counts = 0;
uint32_t curCountIdx = 0;

void GPSDOAverageAddCount(int16_t count)
{
    past4Counts += count - counts[(curCountIdx + FREQUENCY_MEASUREMENTS - 4) % FREQUENCY_MEASUREMENTS];
    past16Counts += count - counts[(curCountIdx + FREQUENCY_MEASUREMENTS - 16) % FREQUENCY_MEASUREMENTS];
    past64Counts += count - counts[(curCountIdx + FREQUENCY_MEASUREMENTS - 64) % FREQUENCY_MEASUREMENTS];
    past256Counts += count - counts[(curCountIdx + FREQUENCY_MEASUREMENTS - 256) % FREQUENCY_MEASUREMENTS];

    counts[curCountIdx] = count;
    curCountIdx = (curCountIdx + 1) % FREQUENCY_MEASUREMENTS;
}

int32_t GPSDOAverageGet4(void)
{
    return past4Counts;
}

int32_t GPSDOAverageGet16(void)
{
    return past16Counts;
}

int32_t GPSDOAverageGet64(void)
{
    return past64Counts;
}

int32_t GPSDOAverageGet256(void)
{
    return past256Counts;
}

uint32_t GPSDOAverageGetIdx(void)
{
    return curCountIdx;
}

void GPSDOAverageReset(void)
{
    past4Counts = 0;
    past16Counts = 0;
    past64Counts = 0;
    past256Counts = 0;
    curCountIdx = 0;
    memset(counts, 0, sizeof(counts));
}
