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

char	comando='0';
char	funcion_RW = '0';
char	valor_m[10]={0};
int		size = 32584256;

ISR (USART_RX_vect)
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
	///////////////////////////////////////////debug
	
}

int main(void)
{
    /* Replace with your application code */
    while (1) 
    {
		DDRB = 0b00000111;		

		DDRD	= 0b00100000;
		PORTD	= 0x00;
		TCCR0A	= 0x23;
		TCCR0B	= 0x01;
		TCCR2A	= 0x02;
		OCR2A	= 230;
		TCCR2B	= 0x05;
		TIMSK2	= 0x02;
		OCR0B=0x01;
		PORTB = 0x03;//0x02;
		cli();
		
		usart_init();

		UCSR0B =  (1<<RXEN0) | (1<<TXEN0)|(1<<RXCIE0);
		
		sei();
		if (funcion_RW == 'R')
		{
			send_string("preparando para leer: ");
			dtostrf(size,3,0,valor_m);
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

