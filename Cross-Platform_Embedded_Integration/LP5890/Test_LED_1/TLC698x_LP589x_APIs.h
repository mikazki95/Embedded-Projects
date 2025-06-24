/*
 * TLC698x_LP589x_APIs.h
 *
 *  Created on: Jul 5, 2022
 *
 */

//
// Included Files
//
#include "system_info.h"
#include "frames.h"

//
// Globals
//
extern uint16_t vsyncDone;

//
// Function Prototypes
//
void setData(uint16_t *data, uint16_t high, uint16_t mid, uint16_t low, uint16_t bias);

void LED_Set_Chip_Index(unsigned int checkResponse, unsigned int bus);

void LED_sendSRAM(uint16_t scan_lines);

void LED_Write_RGB_Custom_ALL(uint16_t r_value, uint16_t g_value, uint16_t b_value);
void LED_Write_RGB_A(uint16_t r_value, uint16_t g_value, uint16_t b_value);
void LED_Write_Black_ALL(void);
void LED_Write_White_ALL(void);
void LED_Write_Red_ALL(void);
void LED_Write_Green_ALL(void);
void LED_Write_Blue_ALL(void);
void LED_Write_Red_Custom_ALL(uint16_t brt_value);
void LED_Write_Green_Custom_ALL(uint16_t brt_value);
void LED_Write_Blue_Custom_ALL(uint16_t brt_value);

void LED_Write_RGB_Diagonal_Custom_ALL(uint16_t r_value, uint16_t g_value, uint16_t b_value);
void LED_Write_Red_Diagonal_ALL(void);
void LED_Write_Green_Diagonal_ALL(void);
void LED_Write_Blue_Diagonal_ALL(void);

void LED_Write_RGB_Coupling_Custom_ALL(uint16_t r_coupling_value, uint16_t g_coupling_value, uint16_t b_coupling_value, uint16_t r_lg_value, uint16_t g_lg_value, uint16_t b_lg_value);

void LED_Write_White_Dark_Coupling_ALL(void);
void LED_Write_Red_Dark_Coupling_ALL(void);
void LED_Write_Green_Dark_Coupling_ALL(void);
void LED_Write_Blue_Dark_Coupling_ALL(void);

void LED_Write_White_Bright_Coupling_ALL(void);
void LED_Write_Red_Bright_Coupling_ALL(void);
void LED_Write_Green_Bright_Coupling_ALL(void);
void LED_Write_Blue_Bright_Coupling_ALL(void);

void LED_Write_Reg_Broadcast(uint16_t fc_reg, uint16_t data2, uint16_t data1, uint16_t data0, unsigned int checkResponse, unsigned int bus);

void LED_lod_lsd_detection(unsigned int LOD);
void LED_Update_LSD_channels(void);
void LED_Update_LOD_channels(void);
void LED_Update_Chip_Status(void);

void sendSYNC();
void sendSYNCnoWait();

#if MONOCHROMATIC
    extern uint8_t img_mono[SCREEN_SIZE_Y][SCREEN_SIZE_X][CCSI_BUS_NUM];
#else
    extern uint8_t img_R_RGB[SCREEN_SIZE_Y][SCREEN_SIZE_X][CCSI_BUS_NUM];
    extern uint8_t img_G_RGB[SCREEN_SIZE_Y][SCREEN_SIZE_X][CCSI_BUS_NUM];
    extern uint8_t img_B_RGB[SCREEN_SIZE_Y][SCREEN_SIZE_X][CCSI_BUS_NUM];
#endif

//
// End of File
//
