/*#include <cstdint>

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
*/
