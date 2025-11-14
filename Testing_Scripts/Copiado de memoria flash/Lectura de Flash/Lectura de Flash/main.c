/*
 * Lectura de Flash.c
 *
 * Created: 14/11/2025 11:05:06 a. m.
 * Author : TONA
 */ 

#define F_CPU 14745600UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "USART0.h"

char		comando='0';
char		funcion_RW = '0';
char		valor_m[15]={0};
uint32_t	size = 33554432;
char		texto_info[10]		= {"I:Flash,"};
char		version[10]	= {"1.0.0"};

ISR(USART0_RX_vect)
{
	comando=UDR0;
	if (comando =='R')
	{
		send_char('R');
		send_char('\n');
		funcion_RW = 'R';
	}
	else if (comando =='W')
	{
		send_char('W');
		send_char('\n');
		funcion_RW = 'W';
	}
	else 
	{
		send_char('E');
		send_char('\n');
		funcion_RW = 'E';
	}
	
}

int main(void)
{
    /* Replace with your application code */
	cli();
	
	usart_init();

	UCSR0B =  (1<<RXEN0) | (1<<TXEN0)|(1<<RXCIE0);
	
	sei();
	
	send_string(texto_info);
	send_string(version);
	send_char('\n');
	
    while (1) 
    {
		if (funcion_RW == 'R')
		{
			send_string("preparando para leer: ");
			dtostrf(size,10,0,valor_m);
			send_string(valor_m);
			send_string("bytes");
			send_char('\n');
			send_char('\r');
			funcion_RW = '0';
		} 
		else if (funcion_RW == 'W')
		{
			send_string("preparando para escribir: ");
			dtostrf(size,3,0,valor_m);
			send_string(valor_m);
			send_string("bytes");
			send_char('\n');
			send_char('\r');
			funcion_RW = '0';
		}
		else if(funcion_RW == 'E')
		{
			send_string("Comando no reconocido, intente con R o W");
			send_char('\n');
			send_char('\r');
			funcion_RW = '0';
		}
		

    }
}

