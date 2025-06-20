/*
 * system_info.c
 *
 *  Created on: Mar 14, 2022
 */

#include <stdint.h>
#include "system_info.h"

const uint16_t FRAME_PERIOD = 40000;      // 40ms = 25 Hz frames-per-second

struct chipStatus chip_status[CCSI_BUS_NUM][MAX_CASCADED_UNITS];

// The cascaded number of devices
#if (CCSI_BUS_NUM > 1)
    const uint16_t CASCADED_UNITS[CCSI_BUS_NUM] = {CASCADED_UNITS_CCSI1,CASCADED_UNITS_CCSI2};
#else
    const uint16_t CASCADED_UNITS[CCSI_BUS_NUM] = {CASCADED_UNITS_CCSI1};
#endif

