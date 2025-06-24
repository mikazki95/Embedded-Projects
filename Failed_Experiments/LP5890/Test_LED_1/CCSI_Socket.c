/*
 * CCSI_Socket.c
 *
 *  Created on: Jan 11, 2022
 *
 */

//
// Included Files
//
#include <stdint.h>
#include <msp430.h>
#include "CCSI_Socket.h"
#include "frames.h"
#include "dma.h"
#include "led_driver.h"
#include "system_info.h"

/* Function Name: CCSI_write
 *
 * Purpose: Write data bytes
 *
 * Parameters:
 * headBytes -> Head bytes of the command.
 * dataBytes -> Data bytes, MSB first.
 * length -> Length of data bytes.
 * checkResponse -> For CLB solution this is a dummy argument
 *
 */
void CCSI_write(uint16_t headBytes, uint16_t *dataBytes, uint16_t length, unsigned int checkResponse, unsigned int bus)
{
    uint16_t i = 1;
    uint16_t dataIdx = 0;
    uint16_t tempData = 0x0000;
    uint16_t leftBits = 0;

    // For length is 6 have hardcoded calculation because this is sending data to SRAM which
    // happens most of the time and should reduce calculation time to improve frame rate
    if(length == 6){
        // Clear first item to have the start bit
        ledXmtBuffer[bus][0] = 0x0000;
        // Head bytes
        ledXmtBuffer[bus][0] |= (headBytes >> 1) & 0x7FFF; // 15 bits
        ledXmtBuffer[bus][0] = __swap_bytes(ledXmtBuffer[bus][0]);
        ledXmtBuffer[bus][1] = (headBytes << 15) & 0x8000; // 1 bits
        // Check bit - Only need to set to 1 when index is all 0
        if (!(headBytes & 0x0001)) {
            ledXmtBuffer[bus][1] |= 0x4000;
        }

        // dataBytes[0]
        ledXmtBuffer[bus][1] |= (dataBytes[0] >> 2) & 0x3FFF; // 14 bits
        ledXmtBuffer[bus][1] = __swap_bytes(ledXmtBuffer[bus][1]);
        ledXmtBuffer[bus][2] = (dataBytes[0] << 14) & 0xC000; // 2 bits
        // Check bit
        if (!(ledXmtBuffer[bus][2] & 0x4000)) {
            ledXmtBuffer[bus][2] |= 0x2000;
        }

        //dataBytes[1]
        ledXmtBuffer[bus][2] |= (dataBytes[1] >> 3) & 0x1FFF; // 13 bits
        ledXmtBuffer[bus][2] = __swap_bytes(ledXmtBuffer[bus][2]);
        ledXmtBuffer[bus][3] = (dataBytes[1] << 13) & 0xE000; // 3 bits
        // Check bit
        if (!(ledXmtBuffer[bus][3] & 0x2000)) {
            ledXmtBuffer[bus][3] |= 0x1000;
        }

        //dataBytes[2]
        ledXmtBuffer[bus][3] |= (dataBytes[2] >> 4) & 0x0FFF; // 12 bits
        ledXmtBuffer[bus][3] = __swap_bytes(ledXmtBuffer[bus][3]);
        ledXmtBuffer[bus][4] = (dataBytes[2] << 12) & 0xF000; // 4 bits
        // Check bit
        if (!(ledXmtBuffer[bus][4] & 0x1000)) {
            ledXmtBuffer[bus][4] |= 0x0800;
        }

        //dataBytes[3]
        ledXmtBuffer[bus][4] |= (dataBytes[3] >> 5) & 0x07FF; // 11 bits
        ledXmtBuffer[bus][4] = __swap_bytes(ledXmtBuffer[bus][4]);
        ledXmtBuffer[bus][5] = (dataBytes[3] << 11) & 0xF800; // 5 bits
        // Check bit
        if (!(ledXmtBuffer[bus][5] & 0x0800)) {
            ledXmtBuffer[bus][5] |= 0x0400;
        }

        //dataBytes[4]
        ledXmtBuffer[bus][5] |= (dataBytes[4] >> 6) & 0x03FF; // 10 bits
        ledXmtBuffer[bus][5] = __swap_bytes(ledXmtBuffer[bus][5]);
        ledXmtBuffer[bus][6] = (dataBytes[4] << 10) & 0xFC00; // 6 bits
        // Check bit
        if (!(ledXmtBuffer[bus][6] & 0x0400)) {
            ledXmtBuffer[bus][6] |= 0x0200;
        }

        //dataBytes[5]
        ledXmtBuffer[bus][6] |= (dataBytes[5] >> 7) & 0x01FF; // 9 bits
        ledXmtBuffer[bus][6] = __swap_bytes(ledXmtBuffer[bus][6]);
        ledXmtBuffer[bus][7] = (dataBytes[5] << 9) & 0xFE00; // 7 bits
        // Check bit
        if (!(ledXmtBuffer[bus][7] & 0x0200)) {
            ledXmtBuffer[bus][7] |= 0x0100;
        }

        //End bytes
        ledXmtBuffer[bus][7] |= 0x00FF; // 8 bits
        ledXmtBuffer[bus][7] = __swap_bytes(ledXmtBuffer[bus][7]);
        ledXmtBuffer[bus][8] = 0xFFFF; // 10 bits + fill remaining of byte

        // Start transfer - Size is 2*9
        dmaTransfer(18, 21, checkResponse);
    }
    else{
        // Clear first item to have the start bit
        ledXmtBuffer[bus][0] = 0x0000;
        // Head bytes
        ledXmtBuffer[bus][0] |= (headBytes >> 1) & 0x7FFF; // 15 bits
        ledXmtBuffer[bus][1] = (headBytes << 15) & 0x8000; // 1 bits
        // Check bit - Only need to set to 1 when index is all 0
        if (!(headBytes & 0x0001)) {
            ledXmtBuffer[bus][1] |= 0x4000;
        }

        // Data bytes
        leftBits = 14;
        for(dataIdx = 0; dataIdx < length; dataIdx++) {
            if(leftBits == 0){
                // Last data exactly filled 16 bits in the buffer
                leftBits = 16;
                i++;
            }
            if(leftBits == 16) {
                // Only data fits and check bit will go to next data
                ledXmtBuffer[bus][i] = dataBytes[dataIdx] & 0xFFFF;
                i++;
                if (!(dataBytes[dataIdx] & 0x0001)) {
                    ledXmtBuffer[bus][i] = 0x8000;
                } //Check bit
                leftBits = 15;
            }
            else {
                // Only part of the data will fit
                tempData = dataBytes[dataIdx];
                tempData = tempData >> (16 - leftBits);
                ledXmtBuffer[bus][i] |= tempData;
                i++;
                tempData = dataBytes[dataIdx];
                tempData = tempData << leftBits;
                if (!(dataBytes[dataIdx] & 0x0001)) {
                    tempData |= (0x0001 << (leftBits - 1));
                } //Check bit
                ledXmtBuffer[bus][i] = tempData;
                leftBits--;
            }
        }

        // End bytes - First fill remaining bits of last word
        tempData = 0xFFFF >> (16 - leftBits);
        ledXmtBuffer[bus][i] |= tempData;
        i++;
        // Add 16 bits to next data
        ledXmtBuffer[bus][i] = 0xFFFF;
        // Check if we already reached 18 bits
        if((32 - leftBits) < 18){
            i++;
            // Add another 16 bits to next data
            ledXmtBuffer[bus][i] = 0xFFFF;
        }

        // Convert arrayTempData to dmaTxData with swapped bytes
        for(dataIdx = 0; dataIdx <= i; dataIdx++){
            ledXmtBuffer[bus][dataIdx] = __swap_bytes(ledXmtBuffer[bus][dataIdx]);
        }
        
        // Start transfer - Size is 2*(i+1)
        dmaTransfer((i+1)<<1, ((i+1)<<1)+3, checkResponse);
    }

    if(checkResponse)
    {
        processReadData(headBytes, 0, FALSE, bus);
    }
}

/* Function Name: processReadData
 *
 * Purpose: Process the RX data buffer from SPI
 *
 * Parameters:
 * length -> Length of data bytes requested.
 * compareCheckBit -> Check the buffer if CHECK bit was successfully received.
 *
 */
void processReadData(uint16_t headBytes, uint16_t length, unsigned int compareCheckBit, unsigned int bus)
{
    // TODO: Would we need a timeout to prevent endless loop
    uint16_t RxSize;
    uint16_t idx;
#if LP589X
    uint16_t startData = 1;
    uint16_t leftBits = 2;
    uint16_t offset = 1;
#else
    uint16_t startData = 3;
    uint16_t leftBits = 14;
    uint16_t offset = 3;
#endif

    RxSize = (DMA1SZ >> 1) + (DMA1SZ & 0x1);

    // First swap receive bytes for easier calculation
    for(idx = startData; idx < RxSize; idx++){
        RxRingBuff[bus][idx] = __swap_bytes(RxRingBuff[bus][idx]);
    }

#if LP589X
    ledRcvBuffer[bus][0] = ((RxRingBuff[bus][startData] & 0x0007) << 13) + ((RxRingBuff[bus][startData + 1] & 0xFFF8) >> 3);
#else
    ledRcvBuffer[bus][0] = ((RxRingBuff[bus][startData] & 0x7FFF) << 1) + ((RxRingBuff[bus][startData + 1] & 0x8000) >> 15);
#endif

    // Retrieve data bytes
    for(idx = startData + 1; idx < RxSize; idx++){
        if(leftBits == 0){
            leftBits = 16;
        }
        if(leftBits == 16){
            ledRcvBuffer[bus][idx - offset] = RxRingBuff[bus][idx + 1];
            leftBits = 15;
        }
        else {
            ledRcvBuffer[bus][idx - offset] = (RxRingBuff[bus][idx] << (16 - leftBits)) + (RxRingBuff[bus][idx + 1] >> leftBits);
            leftBits--;
        }
    }
    // Fill last word with all ones
    ledRcvBuffer[bus][idx - offset] = 0xFFFF;
}

/* Function Name: CCSI_read
 *
 * Purpose: Read data bytes
 *
 * Parameters:
 * headBytes -> Head bytes of the command.
 *
 */
void CCSI_read(uint16_t headBytes, unsigned int bus)
{
    // Clear first item to have the start bit
    ledXmtBuffer[bus][0] = 0x0000;
    // Head bytes
    ledXmtBuffer[bus][0] |= (headBytes >> 1) & 0x7FFF; // 15 bits
    ledXmtBuffer[bus][0] = __swap_bytes(ledXmtBuffer[bus][0]);
    ledXmtBuffer[bus][1] = (headBytes << 15) & 0x8000; // 1 bits
    // Check bit - Only need to set to 1 when index is all 0
    if (!(headBytes & 0x0001)) {
        ledXmtBuffer[bus][1] |= 0x4000;
    }
    //End bytes
    ledXmtBuffer[bus][1] |= 0x3FFF; // 14 bits
    ledXmtBuffer[bus][1] = __swap_bytes(ledXmtBuffer[bus][1]);
    ledXmtBuffer[bus][2] = 0xFFFF; // 4 bits + fill remaining of byte
    
    // Start transfer
    // Transmit size is 5 bytes
    // Receive size is 17 * 3 * cascaded + 17 bits + send data
    dmaTransfer(6, 22, TRUE);

    processReadData(headBytes, 0, FALSE, bus);
}
