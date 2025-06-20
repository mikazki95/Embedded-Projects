/*
 * CCSI_Socket.h
 *
 *  Created on: Jan 21, 2022
 *
 */

#ifndef CCSI_SOCKET_H_
#define CCSI_SOCKET_H_

#include <stdint.h>

void CCSI_write(uint16_t headBytes, uint16_t *dataBytes, uint16_t length, unsigned int checkResponse, unsigned int bus);
void CCSI_read(uint16_t headBytes, unsigned int bus);

void processReadData(uint16_t headBytes, uint16_t length, unsigned int compareCheckBit, unsigned int bus);

#endif /* CCSI_SOCKET_H_ */
