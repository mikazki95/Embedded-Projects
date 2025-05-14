#include <18f4550.h>          // la librería del PIC
#Fuses HSPLL, NOWDT, NOPROTECT, NOLVP, NODEBUG, USBDIV, PLL2, CPUDIV1, VREGEN
#use delay (clock=48M)        //Seleccionamos la frecuencia de reloj de 48MHz
#use standard_io(C)        //Preconfiguramos el puerto C
#use standard_io(e)        //Preconfiguramos el puerto E
void main(){               //Inicio de la función principal
   set_tris_e(0x00);       //Configuramos puerto E como salida 
   set_tris_C(0x01);       //Configuramos pin C0 como entrada
   output_e(0x07);         //Apagamos el LED RGB
   while(true){            //Inicia ciclo infinito
      if (input_state(PIN_C0)){  //Revisamos el estado del PIN_C0
         output_low(PIN_E1);     //Encender LED
         delay_ms(100);          //Esperamos 100 milisegundos
      }
      else{                      //Si no se presionó el pulsador
         output_high(PIN_E1);    //Apagar LED
      }
   }
}
