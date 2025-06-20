/*
 * dma.h
 *
 *  Created on: Dec 27, 2021
 *      Author: a0219226
 */
 
#ifndef DMA_H_
#define DMA_H_

#include "system_info.h"

extern uint16_t ledXmtBuffer[CCSI_BUS_NUM][MAX_DATA_LENGTH]; // Stores the data bytes to be sent
extern uint16_t ledRcvBuffer[CCSI_BUS_NUM][MAX_DATA_LENGTH]; // Processed received bytes
extern uint16_t RxRingBuff[CCSI_BUS_NUM][MAX_DATA_LENGTH]; //Stores the data bytes to be received

void dmaInitial();
void dmaTransfer(unsigned int dmaTxSize, unsigned int dmaRxSize, unsigned int checkResponse);

#endif /* DMA_H_ */
