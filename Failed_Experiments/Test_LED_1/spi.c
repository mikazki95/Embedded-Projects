/*
 * SPI.c
 *
 *  Created on: Jan 21, 2020
 *      Author: a0222913
 */

#include <msp430.h>
#include "spi.h"
#include "led_driver.h"

void spiInitial()
{

    UCB0CTL1 |= UCSWRST; //USCI rest software
    UCB0CTL0 |= UCCKPL + UCMSB + UCMST;//UCCKPH = 0, UCCKPL = 1;
    UCB0CTL1 |= UCSSEL__SMCLK ; //UCLK = SMCLK = 24MHz
    //UCB0BR0 = 2;
#ifdef TLC698X
    UCB0BR0 = 2;//fBitClock = 24/2 = 12MHz --> With double edge this means SCLK is 6 MHz
#endif
#ifdef LP589X
    UCB0BR0 = 2;//fBitClock = 24/2 = 12MHz --> With rising-edge this means SCLK is 12 MHz
#endif
    UCB0BR1 = 0;
    P3DIR |= BIT0 + BIT2; //P3.0 -> SIMO, P3.2 -> SPICLK;
    P3DIR &= ~ BIT1; //P3.1 -> SOMI;
    P3SEL |= BIT0 + BIT1 + BIT2;
    UCB0CTL0 &= ~UC7BIT; //8-bit character length
    UCB0CTL1 &= ~UCSWRST; //USCI rest released for operation
}

