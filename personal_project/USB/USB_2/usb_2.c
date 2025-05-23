#include <18F4550.h>
#device ADC=10
#device ADC=10
#fuses HSPLL, NOWDT, NOPROTECT, NODEBUG, USBDIV, PLL2, CPUDIV1, VREGEN
#use delay(clock=8000000)

//#byte porta = 0xf80 // Identificador para el puerto A. 
//#byte portb = 0xf81 // Identificador para el puerto B. 
//#byte portc = 0xf82 // Identificador para el puerto C. 
//#byte portd = 0xf83 // Identificador para el puerto D. 
#byte porte = 0xf84 // Identificador para el puerto E.
 
//#define  USB_CONFIG_PID       0x000A
//#define  USB_CONFIG_VID       0x04D8
 
#define __USB_PIC_PERIF__ 1 
// if USB_CDC_ISR is defined, then this function will be called
// by the USB ISR when there incoming CDC (virtual com port) data.
// this is useful if you want to port old RS232 code that was use
//#int_rda to CDC.
#define USB_CDC_ISR() lectura()
 
// in order for handle_incoming_usb() to be able to transmit the entire
// USB message in one pass, we need to increase the CDC buffer size from
// the normal size and use the USB_CDC_DELAYED_FLUSH option.
// failure to do this would cause some loss of data.
#define USB_CDC_DELAYED_FLUSH
#define USB_CDC_DATA_LOCAL_SIZE  128
 
static void lectura(void);
int bandera=0;
int bandera1=0;
// Includes all USB code and interrupts, as well as the CDC API
#include <usb_cdc.h>
#include <stdlib.h>
#include <string.h>
 
//#define USB_CON_SENSE_PIN PIN_B2 //No usado cuando alimentado desde el USB
#define LED1 PIN_E0
#define LED2 PIN_E1
#define LED3 PIN_E2
 
 
 
int deg=0;
char dat;
char msg[32];

//Define la interrupción por recepción Serial
static void lectura(void)
{  
output_toggle(LED3);
bandera1=0;
return;
 
/*
 while(usb_cdc_kbhit())
   {
      bandera1=0;
      dat=usb_cdc_getc();
      if(dat=='S'){
      output_toggle(LED1);
     }
     else{
      
      output_toggle(LED2);
     }
   }*/
}
 
 
void main(){
   set_tris_e(0x00);          //Configuramos el puerto E como salida
   output_e(0x07); 
   set_tris_C(0x01);
   usb_cdc_init();
   usb_init();
   while(true){
      bandera1++;
      //static void bandera(bandera);
      usb_task();
      if(usb_enumerated()) {
      if (input_state(PIN_C0)){
            output_toggle(LED1);
            //sprintf(msg,"toggle led %i \n",bandera1);
            sprintf(msg,"%i \n",bandera1);
            printf(usb_cdc_putc,"%s",msg);
       }
       ///sprintf(msg,"%i",bandera1);
      //sprintf(msg,"hola tona v2 \n");
      //printf(usb_cdc_putc,"%s",msg); 
      delay_ms(10);
      }
   }
}


