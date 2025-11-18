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
#include "S29GL256P.h"

char		comando='0';
char		funcion_RW = '0';
char		valor_m[15]={0};
uint32_t	size = 33554432;
char		texto_info[10]		= {"I:Flash,"};
char		version[10]	= {"1.0.0"};
int			error =	0;

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

uint16_t flash_read(uint32_t addr);

int main(void)
{
    /* Replace with your application code */
	cli();
	
	usart_init();

	UCSR0B =  (1<<RXEN0) | (1<<TXEN0)|(1<<RXCIE0);
	
	sei();
	
	Flash_init();
	
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
			///////////////////////////////////////////////////////
			for(uint8_t pagina = 0; pagina < 16; pagina++) {
				uint32_t base_addr = pagina * 8;  // Cada página son 8 palabras
				
				send_string("Página ");
				dtostrf(pagina,10,0,valor_m);
				send_string(valor_m);
				send_char('\n');
				send_string("Pos\t| Addr\t| Data\n");
				send_string("-----|-------|------\n");
				
				// Leer las 8 palabras de la página
				for(uint8_t pos = 0; pos < 8; pos++) {
					uint32_t addr = base_addr + pos;
					uint16_t data = flash_read(addr);
										
					dtostrf(pos,10,0,valor_m);
					send_string(valor_m);
					send_string(" | ");
					dtostrf(addr,10,0,valor_m);
					send_string(valor_m);
					send_string(" | ");
					dtostrf(data,10,0,valor_m);
					send_string(valor_m);
					send_string(" | ");
					send_char('\n');
										
				}
				send_char('\n');
			}
			//////////////////////////////////////////
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

uint16_t flash_read(uint32_t addr)
{
	error = dir_dato("read");
	if (error == 0)
	{
		send_string("error de comando");
		send_char('\n');
		send_char('\r');
		return 0;
	}
	low_address = (addr & 0x0000FF);
	mid_address = ((addr>>8) & 0x0000FF);
	high_address = ((addr>>16) & 0x0000FF);
	
	
}