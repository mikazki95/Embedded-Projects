/*
 * UCS.c
 *
 *  Created on: Jan 21, 2020
 *      Author: a0222913
 */

#include <msp430.h>
#include "ucs.h"
#include "frames.h"
#include "led_driver.h"
#include "system_info.h"

void ucsInitial()
{
    //ucs setting
    P5SEL |= (BIT2| BIT3| BIT4 | BIT5);//OPEN XTI1 AND XT2
    UCSCTL6 |= (XT2OFF |XT2DRIVE_0 | XT1DRIVE_3 | XTS);//configuration XT1 and XT2
    UCSCTL6 &= ~(XT2OFF);
    UCSCTL6 &= ~(XT1OFF);
    do{
        UCSCTL7 &= ~XT1LFOFFG;
    }while(UCSCTL7&XT1LFOFFG);
    UCSCTL6 |= XT1DRIVE_0;

    //increase voltage of VCC
    PMMCTL0_H = 0xA5;
    SVSMLCTL |= SVSMLRRL_1 + SVMLE;
    PMMCTL0 = PMMPW + PMMCOREV_3;// configurate the internal core voltage PMMCOREV_3 25MHz
    while((PMMIFG & SVSMLDLYIFG )==3);
    PMMIFG &= ~(SVMLVLRIFG + SVMLIFG + SVSMLDLYIFG);
   if((PMMIFG & SVMLIFG)==1)
    while((PMMIFG & SVMLVLRIFG)==0);
   SVSMLCTL &= ~SVMLE;
   PMMCTL0_H = 0X00;

   __bis_SR_register(SCG0); //enable FLL
    UCSCTL3 = (SELREF__XT2CLK | FLLREFDIV__8);//XT2CLK as the reference clock
    UCSCTL1 = DCORSEL_6;
    UCSCTL2 = FLLD_3 | 5; //4M/8*6*8=24
    __bic_SR_register(SCG0);
    UCSCTL4 = (SELA__XT1CLK | SELS__DCOCLK | SELM__DCOCLK);//SMCLK=DCOCLK

   // Loop until XT1,XT2 & DCO stabilizes - in this case loop until XT1 settles
   do
   {
       UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
                                            // Clear XT2,XT1,DCO fault flags
   }while (UCSCTL7&DCOFFG);                   // Test oscillator fault flag

   //Output ACLK
   //P1DIR |= BIT0;
   //P1SEL |= BIT0;
   //Output SMCLK
   //P2DIR |= BIT2;
   //P2SEL |= BIT2;
}

void timerInitial()
{
    TA2CTL |= TASSEL_2 + MC_0; //select SMCLK clock source, select /1 =24MHz, divider, select stop mode
    // First trial for LP589x to increase SCLK to 12 MHz
    TA2CCTL1 |= OUTMOD_2; //TA2.1 select toggle/reset mode//OUTMOD_2
#ifdef TLC698X
    TA2CCR0 = 3;
    TA2CCR1 = 2;
#endif
#ifdef LP589X
    TA2CCR0 = 1;
    TA2CCR1 = 1;
#endif

    P2DIR |= BIT4; //P2.4 set as output
    P2SEL |= BIT4;//P2.4 -> TA2.1
    TA2CTL |= MC_1; //select up mode to start timer

    // Timer 0 setup for frame rate control
    TA0CTL = TASSEL__SMCLK + ID_2 + MC_0; //select SMCLK clock source, select /4 = 6MHz, divider, select stop mode
    TA0CCTL0 = OUTMOD_0;
    TA0CCR0 = FRAME_PERIOD - 1;
    TA0EX0 = TAIDEX_5; // Select /6 --> 1MHz
    TA0CTL |= MC_1; //select up mode to start timer
    TA0CCTL0 |= CCIE; //enable TA0CCR1 CCIFG interrupt
}

