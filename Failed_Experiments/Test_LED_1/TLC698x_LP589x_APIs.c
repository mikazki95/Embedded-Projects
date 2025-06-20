/*
 * TLC698x_LP589x_APIs.c
 *
 *  Created on: Jul 5, 2022
 *
 */

//
// Included Files
//
#include "TLC698x_LP589x_APIs.h"
#include "led_driver.h"
#include <CCSI_Socket.h>
#include "FC_settings.h"
#include "dma.h"

//
// Globals
//
uint16_t vsyncDone;
uint16_t data[CCSI_BUS_NUM][MAX_DATA_LENGTH];
#if MONOCHROMATIC
    uint8_t img_mono[SCREEN_SIZE_Y][SCREEN_SIZE_X][CCSI_BUS_NUM];
#else
    uint8_t img_R_RGB[SCREEN_SIZE_Y][SCREEN_SIZE_X][CCSI_BUS_NUM];
    uint8_t img_G_RGB[SCREEN_SIZE_Y][SCREEN_SIZE_X][CCSI_BUS_NUM];
    uint8_t img_B_RGB[SCREEN_SIZE_Y][SCREEN_SIZE_X][CCSI_BUS_NUM];
#endif

/* Function Name: LED_getStackableDevices
 *
 * Purpose: Get the number of stackable devices.
 *
 * Parameters:
 *
 */

uint16_t LED_getStackableDevices(void)
{
    uint16_t stack_dev = 0;
    uint16_t modSizeMasked;

#if (LP5890 | TLC6983)
    modSizeMasked = _FC0_2 & 0xC000;
    if(modSizeMasked == MOD_SIZE__1) {
        stack_dev = 1;
    }
    else {
        stack_dev = 2;
    }
#else
    modSizeMasked = _FC2_0 & 0x3800;
    if(modSizeMasked == MOD_SIZE__1) {
        stack_dev = 1;
    }
    else {
        if(modSizeMasked == MOD_SIZE__2) {
            stack_dev = 2;
        }
        else {
            if((modSizeMasked & MOD_SIZE__4_7) == MOD_SIZE__4_7) {
                stack_dev = 4;
            }
            else {
                stack_dev = 3;
            }
        }
    }
#endif
    return stack_dev;
}

/* Function Name: setData
 *
 * Purpose: Set data send buffer.
 *
 * Parameters:
 * data -> The pointer of data send buffer.
 * high, mid, low -> Data bytes, MSB first.
 * bias -> Bias in the date send buffer.
 *
 */
void setData(uint16_t *data, uint16_t high, uint16_t mid, uint16_t low, uint16_t bias)
{
    data[0 + bias] = high;
    data[1 + bias] = mid;
    data[2 + bias] = low;
}

/* Function Name: LED_Set_Chip_Index
 *
 * Purpose: Set Chip Index.
 *
 * Parameters:
 * checkResponse -> Perform read of returned data
 * bus -> Index of the CCSI bus to be written
 *
 */
void LED_Set_Chip_Index(unsigned int checkResponse, unsigned int bus)
{
    uint16_t data_int[1] = {0x0000};

    CCSI_write(W_CHIP_INDEX, data_int, 0, checkResponse, bus);
}

/* Function Name: LED_sendSRAM
 *
 * Purpose: Send frame data to LED driver.
 *
 * Parameters:
 * scan_lines -> Number of scan lines to send. A user might only want to send the first few scan lines.
 *
 */
void LED_sendSRAM(uint16_t scan_lines)
{
    uint16_t line_idx = 0;
    uint16_t ch_idx = 0;
    uint16_t x_idx = 0;
    uint16_t y_idx = 0;
    uint16_t bus_idx = 0;
    uint16_t chip_idx = 0;

    for(line_idx = 0; line_idx < scan_lines; line_idx++){
        for(ch_idx = 0 ; ch_idx < RGB_CHANNEL_CNT ; ch_idx++){
            for(bus_idx = 0 ; bus_idx < CCSI_BUS_NUM ; bus_idx++){
                for(chip_idx = 0; chip_idx < CASCADED_UNITS[bus_idx]; chip_idx++){
                    x_idx = X_COORDINATE(chip_idx, line_idx, ch_idx);
                    y_idx = Y_COORDINATE(chip_idx, line_idx, ch_idx);
                    // Ensure there are different data per bus in case transmission efficiency is fast enough to overwrite before all data is sent
                    setData(&data[bus_idx][0],
#if MONOCHROMATIC
                            lookup_table_B[img_mono[y_idx][x_idx + 2][bus_idx]],   // B
                            lookup_table_G[img_mono[y_idx][x_idx + 1][bus_idx]],   // G
                            lookup_table_R[img_mono[y_idx][x_idx][bus_idx]],   // R
#else
                            lookup_table_B[img_B_RGB[y_idx][x_idx][bus_idx]],   // B
                            lookup_table_G[img_G_RGB[y_idx][x_idx][bus_idx]],   // G
                            lookup_table_R[img_R_RGB[y_idx][x_idx][bus_idx]],   // R
#endif
                            (chip_idx << 1) + chip_idx);
                }
                CCSI_write(W_SRAM, &data[bus_idx][0], (CASCADED_UNITS[bus_idx] << 1) + CASCADED_UNITS[bus_idx], FALSE, bus_idx);
            }
        }
    }
}

/* Function Name: LED_Write_RGB_Custom_ALL
 *
 * Purpose: Send RGB data to all LED drivers.
 *
 * Parameters:
 * r_value -> Brightness for red-channels
 * g_value -> Brightness for green-channels
 * b_value -> Brightness for blue-channels
 *
 */
void LED_Write_RGB_A(uint16_t r_value, uint16_t g_value, uint16_t b_value)
{
    uint16_t line_idx = 0;
    uint16_t ch_idx = 0;
    uint16_t bus_idx = 0;
    uint16_t chip_idx = 0;

    for(line_idx = 0; line_idx < r_value; line_idx++)
    {
        setData(&data[bus_idx][0],
                b_value,   // B
                b_value,   // G
                b_value,   // R
                (chip_idx << 1) + chip_idx);

        CCSI_write(W_SRAM, &data[bus_idx][0], (CASCADED_UNITS[bus_idx] << 1) + CASCADED_UNITS[bus_idx], FALSE, bus_idx);

    }
}//*/
/* Function Name: LED_Write_RGB_Custom_ALL
 *
 * Purpose: Send RGB data to all LED drivers.
 *
 * Parameters:
 * r_value -> Brightness for red-channels
 * g_value -> Brightness for green-channels
 * b_value -> Brightness for blue-channels
 *
 */
void LED_Write_RGB_Custom_ALL(uint16_t r_value, uint16_t g_value, uint16_t b_value)
{
    uint16_t line_idx = 0;
    uint16_t ch_idx = 0;
    uint16_t bus_idx = 0;
    uint16_t chip_idx = 0;

    for(line_idx = 0; line_idx < TOTAL_SCAN_LINES; line_idx++){
        for(ch_idx = 0 ; ch_idx < RGB_CHANNEL_CNT ; ch_idx++){
            for(bus_idx = 0 ; bus_idx < CCSI_BUS_NUM ; bus_idx++){
                for(chip_idx = 0; chip_idx < CASCADED_UNITS[bus_idx]; chip_idx++){
                    // Ensure there are different data per bus in case transmission efficiency is fast enough to overwrite before all data is sent
                    setData(&data[bus_idx][0],
                            b_value,   // B
                            g_value,   // G
                            r_value,   // R
                            (chip_idx << 1) + chip_idx);
                }
                CCSI_write(W_SRAM, &data[bus_idx][0], (CASCADED_UNITS[bus_idx] << 1) + CASCADED_UNITS[bus_idx], FALSE, bus_idx);
            }
        }
    }
}

/* Function Name: LED_Write_Black_ALL
 *
 * Purpose: Set all channels to off
 *
 * Parameters:
 *
 */
void LED_Write_Black_ALL(void)
{
    LED_Write_RGB_Custom_ALL(0x0000, 0x0000, 0x0000);
}

/* Function Name: LED_Write_White_ALL
 *
 * Purpose: Set all channels to maximum brightness
 *
 * Parameters:
 *
 */
void LED_Write_White_ALL(void)
{
    LED_Write_RGB_Custom_ALL(0xFFFF, 0xFFFF, 0xFFFF);
}

/* Function Name: LED_Write_Red_ALL
 *
 * Purpose: Set all red-channels to maximum brightness
 *
 * Parameters:
 *
 */
void LED_Write_Red_ALL(void)
{
    LED_Write_RGB_Custom_ALL(0xFFFF, 0x0000, 0x0000);
}

/* Function Name: LED_Write_Green_ALL
 *
 * Purpose: Set all green-channels to maximum brightness
 *
 * Parameters:
 *
 */
void LED_Write_Green_ALL(void)
{
    LED_Write_RGB_Custom_ALL(0x0000, 0xFFFF, 0x0000);
}

/* Function Name: LED_Write_Blue_ALL
 *
 * Purpose: Set all blue-channels to maximum brightness
 *
 * Parameters:
 *
 */
void LED_Write_Blue_ALL(void)
{
    LED_Write_RGB_Custom_ALL(0x0000, 0x0000, 0xFFFF);
}

/* Function Name: LED_Write_RGB_Diagonal_Custom_ALL
 *
 * Purpose: Diagonal data to all LED drivers
 *
 * Parameters:
 * r_value -> Brightness for red-channels
 * g_value -> Brightness for green-channels
 * b_value -> Brightness for blue-channels
 *
 */
void LED_Write_RGB_Diagonal_Custom_ALL(uint16_t r_value, uint16_t g_value, uint16_t b_value) {
    uint16_t bus_idx = 0;
    uint16_t line_idx;
    uint16_t ch_idx;
    uint16_t chip_idx;
    uint16_t r_val;
    uint16_t g_val;
    uint16_t b_val;
    uint16_t stacked_devices = LED_getStackableDevices();
    uint16_t offset;

    // Only do something when there are more than 1 scan lines
    if(TOTAL_SCAN_LINES > 1) {
        for(line_idx = 0; line_idx < TOTAL_SCAN_LINES; line_idx++){
            for(ch_idx = 0 ; ch_idx < RGB_CHANNEL_CNT ; ch_idx++){
                for(bus_idx = 0 ; bus_idx < CCSI_BUS_NUM ; bus_idx++){
                    for(chip_idx = 0; chip_idx < CASCADED_UNITS[bus_idx]; chip_idx++){
                        offset = ((chip_idx % stacked_devices) * RGB_CHANNEL_CNT);
                        if(((ch_idx + line_idx + offset + 1U) % (uint16_t) TOTAL_SCAN_LINES) == 0) {
                            r_val = r_value;
                            g_val = g_value;
                            b_val = b_value;
                        }
                        else {
                            r_val = 0x0000;
                            g_val = 0x0000;
                            b_val = 0x0000;
                        }
                        // Ensure there are different data per bus in case transmission efficiency is fast enough to overwrite before all data is sent
                        setData(&data[bus_idx][0],
                                b_val,   // B
                                g_val,   // G
                                r_val,   // R
                                (chip_idx << 1) + chip_idx);
                    }
                    CCSI_write(W_SRAM, &data[bus_idx][0], (CASCADED_UNITS[bus_idx] << 1) + CASCADED_UNITS[bus_idx], FALSE, bus_idx);
                }
            }
        }
    }
}

/* Function Name: LED_Write_Red_Diagonal_ALL
 *
 * Purpose: Write diagonal pattern to red-channels with maximum brightness
 *
 * Parameters:
 *
 */
void LED_Write_Red_Diagonal_ALL(void)
{
    LED_Write_RGB_Diagonal_Custom_ALL(0xFFFF, 0x0000, 0x0000);
}

/* Function Name: LED_Write_Green_Diagonal_ALL
 *
 * Purpose: Write diagonal pattern to green-channels with maximum brightness
 *
 * Parameters:
 *
 */
void LED_Write_Green_Diagonal_ALL(void)
{
    LED_Write_RGB_Diagonal_Custom_ALL(0x0000, 0xFFFF, 0x0000);
}

/* Function Name: LED_Write_Blue_Diagonal_ALL
 *
 * Purpose: Write diagonal pattern to blue-channels with maximum brightness
 *
 * Parameters:
 *
 */
void LED_Write_Blue_Diagonal_ALL(void)
{
    LED_Write_RGB_Diagonal_Custom_ALL(0x0000, 0x0000, 0xFFFF);
}

/* Function Name: LED_Write_RGB_Coupling_Custom_ALL
 *
 * Purpose: Write pattern to check coupling issue
 *
 * Parameters:
 * r_coupling_value -> Brightness for red-channels of coupling value
 * g_coupling_value -> Brightness for green-channels of coupling value
 * b_coupling_value -> Brightness for blue-channels of coupling value
 * r_value -> Brightness for low grayscale of red-channels
 * g_value -> Brightness for low grayscale of green-channels
 * b_value -> Brightness for low grayscale of blue-channels
 *
 */
void LED_Write_RGB_Coupling_Custom_ALL(uint16_t r_coupling_value, uint16_t g_coupling_value, uint16_t b_coupling_value, uint16_t r_lg_value, uint16_t g_lg_value, uint16_t b_lg_value)
{
    uint16_t rows_above;
    uint16_t rows_middle;
    uint16_t rows_below;
    uint16_t columns_left;
    uint16_t bus_idx = 0;
    uint16_t line_idx;
    uint16_t ch_idx;
    uint16_t chip_idx;
    uint16_t r_val;
    uint16_t g_val;
    uint16_t b_val;
    uint16_t stacked_devices = LED_getStackableDevices();
    uint16_t offset;

    // Only do something when there are more than 1 scan lines
    if(TOTAL_SCAN_LINES > 1) {
        rows_middle = TOTAL_SCAN_LINES >> 1;
        rows_below = (TOTAL_SCAN_LINES - rows_middle) >> 1;
        rows_above = TOTAL_SCAN_LINES - rows_middle - rows_below;
        columns_left = (stacked_devices * RGB_CHANNEL_CNT) >> 2;

        for(line_idx = 0; line_idx < TOTAL_SCAN_LINES; line_idx++){
            for(ch_idx = 0 ; ch_idx < RGB_CHANNEL_CNT ; ch_idx++){
                for(bus_idx = 0 ; bus_idx < CCSI_BUS_NUM ; bus_idx++){
                    for(chip_idx = 0; chip_idx < CASCADED_UNITS[bus_idx]; chip_idx++){
                        offset = ((chip_idx % stacked_devices) * RGB_CHANNEL_CNT);
                        if((line_idx < rows_above) | (line_idx > (rows_above + rows_middle - 1))) {
                            // Send low grayscale data
                            r_val = r_lg_value;
                            g_val = g_lg_value;
                            b_val = b_lg_value;
                        }
                        else {
                            // It depends on the column
                            if((ch_idx + offset) < columns_left) {
                                // Send low grayscale data
                                r_val = r_lg_value;
                                g_val = g_lg_value;
                                b_val = b_lg_value;
                            }
                            else {
                                // Send coupling grayscale data
                                r_val = r_coupling_value;
                                g_val = g_coupling_value;
                                b_val = b_coupling_value;
                            }
                        }
                        // Ensure there are different data per bus in case transmission efficiency is fast enough to overwrite before all data is sent
                        setData(&data[bus_idx][0],
                                b_val,   // B
                                g_val,   // G
                                r_val,   // R
                                (chip_idx << 1) + chip_idx);
                    }
                    CCSI_write(W_SRAM, &data[bus_idx][0], (CASCADED_UNITS[bus_idx] << 1) + CASCADED_UNITS[bus_idx], FALSE, bus_idx);
                }
            }
        }
    }
}

void LED_Write_White_Dark_Coupling_ALL(void)
{
    LED_Write_RGB_Coupling_Custom_ALL(0x0000, 0x0000, 0x0000, 0x0200, 0x0200, 0x0800);
}

void LED_Write_Red_Dark_Coupling_ALL(void)
{
    LED_Write_RGB_Coupling_Custom_ALL(0x0000, 0x0000, 0x0000, 0x0200, 0x0000, 0x0000);
}

void LED_Write_Green_Dark_Coupling_ALL(void)
{
    LED_Write_RGB_Coupling_Custom_ALL(0x0000, 0x0000, 0x0000, 0x0000, 0x0200, 0x0000);
}

void LED_Write_Blue_Dark_Coupling_ALL(void)
{
    LED_Write_RGB_Coupling_Custom_ALL(0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0800);
}

void LED_Write_White_Bright_Coupling_ALL(void)
{
    LED_Write_RGB_Coupling_Custom_ALL(0xFFFF, 0xFFFF, 0xFFFF, 0x0200, 0x0200, 0x0800);
}

void LED_Write_Red_Bright_Coupling_ALL(void)
{
    LED_Write_RGB_Coupling_Custom_ALL(0xFFFF, 0x0000, 0x0000, 0x0200, 0x0000, 0x0000);
}

void LED_Write_Green_Bright_Coupling_ALL(void)
{
    LED_Write_RGB_Coupling_Custom_ALL(0x0000, 0xFFFF, 0x0000, 0x0000, 0x0200, 0x0000);
}

void LED_Write_Blue_Bright_Coupling_ALL(void)
{
    LED_Write_RGB_Coupling_Custom_ALL(0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0800);
}

/* Function Name: LED_Write_Reg_Broadcast
 *
 * Purpose: Send same data to all FC register(s) in the chain.
 *
 * Parameters:
 * fc_reg -> First FC register to be written.
 * data2 -> Data byte
 * data1 -> Data byte
 * data0 -> Data byte
 * checkResponse -> Perform read of returned data
 * bus -> Index of the CCSI bus to be written
 *
 */
void LED_Write_Reg_Broadcast(uint16_t fc_reg, uint16_t data2, uint16_t data1, uint16_t data0, unsigned int checkResponse, unsigned int bus)
{
    uint16_t data_int[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    
    setData(data_int, data2, data1, data0, 0);
    CCSI_write(fc_reg, data_int, 3, checkResponse, bus);
}

/* Function Name: LED_Read_Reg
 *
 * Purpose: Read FC register for specified chip index.
 *
 * Parameters:
 * fc_reg -> FC register to be read.
 * chip_idx -> Chip index that should be read
 * bus -> Index of the CCSI bus to be used
 *
 */
void LED_Read_Reg(uint16_t fc_reg, uint16_t chip_idx, unsigned int bus)
{
    //CCSI_read(CHIP_REG_READ, ((fc_reg & 0x1F) << 10) + (chip_idx & 0x3FF), bus);
}

/* Function Name: LED_lod_lsd_detection
 *
 * Purpose: Detect if any line and channel have LED open or short
 *          LOD and LSD can only work when channel is on. This means that the device needs to be in a
 *          scan cycle. In addition, when during this scan cycle a channel is not turned on, e.g.
 *          only Red is displayed and Green and Blue are turned off, the LED open and LED short cannot
 *          be detected for the channels that are off. Also for low grayscale it might not be possible
 *          to detect the LSD and LOD.
 *
 * Parameters:
 * LOD -> When 0, LED short detection is executed. All other numbers LED open detection is executed.
 *
 */
void LED_lod_lsd_detection(unsigned int LOD)
{
    uint16_t data[3] = {0x00, 0x00, 0x00}; // Data send buffer - to send FC registers
    uint16_t fc3_0_old = 0;
    uint16_t fc3_0 = 0;
    uint16_t fc3_1 = 0;
    uint16_t fc3_2 = 0;
    uint16_t lod_lsd_line_warn[MAX_CASCADED_UNITS][4];
    uint16_t line_warn_reg[2];
    uint16_t line_warn_reg_cnt = 1;
    uint16_t line_cmd_reg = 0;
    uint16_t ch_reg = 0;
    uint16_t idx = 0;
    uint16_t bus_idx = 0;
    uint16_t chip_idx;
    uint16_t scan_idx;
    uint16_t ch_idx;
    uint16_t line_warn_reg_idx;
    uint16_t found_chip_with_fault;
    uint16_t offset;

    if(LOD == TRUE){
        // LOD detection
#if (LP5890 | TLC6983)
        line_warn_reg[0] = R_FC12;
        line_cmd_reg = W_FC10;
        ch_reg = R_FC14;
#else
        line_warn_reg_cnt = 2;
        line_warn_reg[0] = R_FC17;
        line_warn_reg[1] = R_FC16;
        line_cmd_reg = W_FC14;
        ch_reg = R_FC20;
#endif
    }
    else{
        // LSD detection
#if (LP5890 | TLC6983)
        line_warn_reg[0] = R_FC13;
        line_cmd_reg = W_FC11;
        ch_reg = R_FC15;
#else
        line_warn_reg_cnt = 2;
        line_warn_reg[0] = R_FC19;
        line_warn_reg[1] = R_FC18;
        line_cmd_reg = W_FC15;
        ch_reg = R_FC21;
#endif
    }

    // Clear flags
    for(bus_idx = 0; bus_idx < CCSI_BUS_NUM; bus_idx++) {
        for(chip_idx = 0; chip_idx < CASCADED_UNITS[bus_idx]; chip_idx++) {
            if(LOD == TRUE) {
                chip_status[bus_idx][chip_idx].LOD = 0U;
                /*for(scan_idx = 0; scan_idx < TOTAL_SCAN_LINES; scan_idx++) {
                    for(ch_idx = 0; ch_idx < CHANNEL_CNT; ch_idx++) {
                        chip_status[bus_idx][chip_idx].LOD_channels[scan_idx][ch_idx] = 0U;
                    }
                }*/
            }
            else {
                chip_status[bus_idx][chip_idx].LSD = 0U;
                /*for(scan_idx = 0; scan_idx < TOTAL_SCAN_LINES; scan_idx++) {
                    for(ch_idx = 0; ch_idx < CHANNEL_CNT; ch_idx++) {
                        chip_status[bus_idx][chip_idx].LSD_channels[scan_idx][ch_idx] = 0U;
                    }
                }*/
            }
        }
    }

    for(bus_idx = 0; bus_idx < CCSI_BUS_NUM; bus_idx++) {
        // Read FC3 because we will need to set bit LOD_LSB_RB in this register
        CCSI_read(R_FC3, bus_idx);

        fc3_0 = ledRcvBuffer[bus_idx][3];
        fc3_0_old = ledRcvBuffer[bus_idx][3];
        fc3_1 = ledRcvBuffer[bus_idx][2];
        fc3_2 = ledRcvBuffer[bus_idx][1];

        // Set LOD_LSB_RB
        fc3_0 |= LOD_LSD_RB__1;
        setData(data, fc3_2, fc3_1, fc3_0, 0);
        CCSI_write(W_FC3, data, 3, FALSE, bus_idx);

        // Wait at least one sub-period time
        //DEVICE_DELAY_US (sub_period_in_us);

        // Read the failing lines
        line_warn_reg_idx = 0;

        while(line_warn_reg_idx < line_warn_reg_cnt) {
            CCSI_read(line_warn_reg[line_warn_reg_idx], bus_idx);

            // Process read data for all devices in the chain
            for(chip_idx = 0; chip_idx < CASCADED_UNITS[bus_idx]; chip_idx++) {
                if(line_warn_reg_idx == 0) {
                    // Scan line 0 to 15
                    lod_lsd_line_warn[chip_idx][0] = ledRcvBuffer[bus_idx][3 + chip_idx*3];
                    // Scan line 16 to 31
                    lod_lsd_line_warn[chip_idx][1] = ledRcvBuffer[bus_idx][2 + chip_idx*3];
                    // Scan line 32 to 47 - For LP5890 and TLC6983 this should be always 0
                    lod_lsd_line_warn[chip_idx][2] = ledRcvBuffer[bus_idx][1 + chip_idx*3];
                }
                else {
                    // Scan line 48 to 63 - For LP5890 and TLC6983 there is no second read
                    lod_lsd_line_warn[chip_idx][3] = ledRcvBuffer[bus_idx][3 + chip_idx*3];
                }
            }

            // Check and process failing scan lines
            for(scan_idx = 0; scan_idx < TOTAL_SCAN_LINES; scan_idx++){
                idx = (scan_idx >> 4) & 0x1;
                found_chip_with_fault = FALSE;
                for(chip_idx = 0; chip_idx < CASCADED_UNITS[bus_idx]; chip_idx++) {
                    if(lod_lsd_line_warn[chip_idx][idx] & 0x1){
                        found_chip_with_fault = TRUE;
                        if(LOD == TRUE) {
                            chip_status[bus_idx][chip_idx].LOD = 1U;
                        }
                        else {
                            chip_status[bus_idx][chip_idx].LSD = 1U;
                        }
                    }
                    lod_lsd_line_warn[chip_idx][idx] = lod_lsd_line_warn[chip_idx][idx] >> 1;
                }
                // If chip with fault is found need to find channel
                if(found_chip_with_fault == TRUE) {
                    // Every time read line warning register
                    CCSI_read(line_warn_reg[line_warn_reg_idx], bus_idx);

                    // This scan line failed. So need to get the failing channel(s)
                    setData(data, 0x0000, 0x0000, scan_idx, 0);
                    CCSI_write(line_cmd_reg, data, 3, FALSE, bus_idx);

                    // Wait at least one sub-period time
                    //DEVICE_DELAY_US (sub_period_in_us);

                    CCSI_read(ch_reg, bus_idx);

                    // Run over channels to see which one in the line has a fault
                    for(chip_idx = 0; chip_idx < CASCADED_UNITS[bus_idx]; chip_idx++) {
                        for(ch_idx = 0; ch_idx < CHANNEL_CNT; ch_idx++) {
                            offset = (ch_idx >> 4);
                            if(ledRcvBuffer[bus_idx][1 + (2-offset) + chip_idx*3] & (0x1 << (ch_idx & 0xF))) {
                                /*if(LOD == TRUE) {
                                    chip_status[bus_idx][chip_idx].LOD_channels[scan_idx][ch_idx] = 1U;
                                }
                                else {
                                    chip_status[bus_idx][chip_idx].LSD_channels[scan_idx][ch_idx] = 1U;
                                }*/
                            }
                        }
                    }
                }
            }
            line_warn_reg_idx++;
        }

        // Return FC3 to old setting
        setData(data, fc3_2, fc3_1, fc3_0_old, 0);
        CCSI_write(W_FC3, data, 3, FALSE, bus_idx);
    }

}

void LED_Update_LSD_channels(void)
{
    LED_lod_lsd_detection(FALSE);
}

void LED_Update_LOD_channels(void)
{
    LED_lod_lsd_detection(TRUE);
}

/* Function Name: LED_Update_Chip_Status
 *
 * Purpose: Update the chip status struct for all devices in the chain.
 *
 * Parameters:
 *
 */
void LED_Update_Chip_Status(void)
{
    // Update LOD and LSD for each channel
    LED_Update_LOD_channels();
    LED_Update_LSD_channels();
}

/* Function Name: sendSYNCinternal
 *
 * Purpose: Send VSYNC command to display next frame.
 *
 * Parameters:
 * waitForINT -> When TRUE wait for interrupt before sending VSYNC command
 *
 */
void sendSYNCinternal(unsigned int waitForINT)
{
    uint16_t data_int[1] = {0x00};
    uint16_t bus_idx = 0;
    uint16_t vsync_local = 0;

    // VSYNC needs to be send before next frame data can be send
    if(waitForINT == TRUE) {
        while(!vsync_local) {
            vsync_local = vsyncDone;
        }
    }

    for(bus_idx = 0 ; bus_idx < CCSI_BUS_NUM ; bus_idx++){
        // SYNC display at a fixed FPS
        CCSI_write(W_VSYNC, data_int, 0, FALSE, bus_idx);
    }

    // Reset VSYNC
    vsyncDone = 0;
}

/* Function Name: sendSYNC
 *
 * Purpose: Send VSYNC command but wait for interrupt to control timing.
 *
 * Parameters:
 *
 */
void sendSYNC()
{
    sendSYNCinternal(TRUE);
}

/* Function Name: sendSYNCnoWait
 *
 * Purpose: Send VSYNC command directly.
 *
 * Parameters:
 *
 */
void sendSYNCnoWait()
{
    sendSYNCinternal(FALSE);
}

//
// End of File
//
