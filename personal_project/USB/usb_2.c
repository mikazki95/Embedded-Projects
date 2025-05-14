///// c++
#INCLUDE <18f4550.h>
#USE DELAY(CLOCK=20000000)
#FUSES XT,PROTECT,NOWDT,NOBROWNOUT,PUT,NOLVP
#DEFINE SW PORTA,1
#DEFINE LED PORTB,0
#BYTE PORTA= 5
#BYTE PORTB= 6
MAIN()
{
SET_TRIS_E(0B000);          //Configura el puerto A
SET_TRIS_C(0x01);    //Configura el puerto B
WHILE(TRUE)                // Haga por siempre
{
	bit_set(PORTE.E0);
}

