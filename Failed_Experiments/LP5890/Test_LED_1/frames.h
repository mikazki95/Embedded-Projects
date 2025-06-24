/*
 * frames.h
 *
 *  Created on: Mar 23, 2022
 */

#ifndef FRAMES_H_
#define FRAMES_H_

#include <stdint.h>
#include "led_driver.h"

#define FRAME_CNT 2
#if LP589X
    // LP5890/LP5891 EVM is 16x16 and can be cascaded
    #define SCREEN_SIZE_X 16*MAX_CASCADED_UNITS
    #define SCREEN_SIZE_Y 16
#else
    // TLC6983/TLC6984 EVM has screen size of 32x32 RGB
    #define SCREEN_SIZE_X 32
    #define SCREEN_SIZE_Y 32
#endif
#define FRAME_SIZE_X 48
#define FRAME_SIZE_Y 32
#define FRAME_RBG_CNT 72 // 3 time frame size of X direction
#define MAX_FRAMES_ARR 1

extern const uint16_t frame_size_x_dir[FRAME_CNT];
extern const uint16_t frame_size_y_dir[FRAME_CNT];

extern const uint16_t lookup_table_R[256];

extern const uint16_t lookup_table_G[256];

extern const uint16_t lookup_table_B[256];

extern const uint16_t (*pFrameDataArray[2]) [FRAME_SIZE_Y][FRAME_RBG_CNT];

#endif /* FRAMES_H_ */
