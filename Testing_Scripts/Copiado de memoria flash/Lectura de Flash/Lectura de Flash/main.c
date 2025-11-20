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
uint8_t		checksum = 0;
uint16_t	Dato = 0;
uint8_t		Aux_dato = 0;

ISR(USART0_RX_vect)
{
	comando=UDR0;
	if (comando =='R')
	{
		//send_char('R');
		//send_char('\n');
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

void flash_read(uint32_t addr);

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
	// Verificar estado WP#
	if(ctrl_port & (1 << wp)) {
		printf("SA0 DESPROTEGIDO\n");
		} else {
		printf("SA0 PROTEGIDO - no se puede escribir en 0x000000-0x01FFFF\n");
	}
	
    while (1) 
    {
		if (funcion_RW == 'R')
		{

			funcion_RW = '0';		
			///////////////////////////////////////////////////////
			for(uint8_t pagina = 0; pagina < 40; pagina++) {
				uint32_t base_addr = pagina * 8;  // Cada página son 8 palabras
				checksum = 0;				
				// ? INCLUIR DIRECCIÓN en el checksum (los 3 bytes)
				checksum += (base_addr >> 16) & 0xFF;
				checksum += (base_addr >> 8) & 0xFF;
				checksum += base_addr & 0xFF;
				
				
				mid_address = ((base_addr>>8) & 0xFF);
				high_address = ((base_addr>>16) & 0xFF);
				low_address = (base_addr & 0xFF);
				
				send_char(high_address);
				send_char(mid_address);
				send_char(low_address);
				
				ctrl_port &= ~(1 << ce);
				ctrl_port &= ~(1 << oe);
				
				// Leer las 8 palabras de la página
				for(uint8_t pos = 0; pos < 8; pos++) {
					uint32_t addr = base_addr + pos;
					flash_read(addr);									
				}
				ctrl_port |= (1 << oe);
				ctrl_port |= (1 << ce);
				checksum=(0x100 - checksum) & 0xFF;
				send_char(checksum);
				//send_char('\n');							
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

void flash_read(uint32_t addr)
{
	error = dir_dato("read");
	if (error == 0)
	{
		send_string("error de comando");
		send_char('\n');
		send_char('\r');
		return 0;
	}
	low_address = (addr & 0xFF);
	
	//Dato = low_Data | (high_Data << 8); 
	//Dato = 0x3031
	Aux_dato = low_Data;
	Dato = Aux_dato&0xFF;
	send_char((Aux_dato));
	Aux_dato = high_Data;
	Dato=(Dato>>8&0XFF);
	Dato += Aux_dato&0xFF;
	send_char((Aux_dato));
	checksum+=Aux_dato;
	Aux_dato = (Dato&0XFF);
	checksum+=Aux_dato;	
}