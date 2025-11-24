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

char		comando		='0';
char		funcion_RW	= '0';
char		valor_m[15]	={0};
uint32_t	size		= 33554432;
char		texto_info[10]		= {"I:Flash,"};
char		version[10]	= {"1.0.0"};
int			error		=	0;
uint32_t	checksum	= 0;
uint16_t	Dato		= 0;
uint8_t		Aux_dato	= 0;
uint8_t		Top_addr	= 0;
uint32_t	base_addr	= 0;
uint32_t	Aux_addr	= 0;
uint8_t		char_dato	= 0;
uint32_t	size_flash	= 128;
uint32_t	offset		= 0X00; //0XEF7FFF; //0x7FFFEF;
uint8_t		flag_ok		= 0;
uint8_t		flag_val	= 1;

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
	else if (comando =='b')
	{
		flag_ok = 1;
	}
	else if (comando =='E')
	{
		flag_ok = 2;
	}
	else if (comando =='V')
	{
		flag_val ^= 1;
		send_string("validacion: ");
		dtostrf(flag_val,3,0,valor_m);
		send_string(valor_m);
		send_char('\n');
	}
	else 
	{
		send_char('X');
		send_char('\n');
		funcion_RW = 'E';
	}
	
}

void flash_read(uint32_t addr);
void flash_write(uint32_t addr, uint16_t data);
void write_command(uint32_t addr, uint16_t cmd);
void wait_ready(void);

int main(void)
{
    /* Replace with your application code */
	cli();
	
	usart_init();

	UCSR0B =  (1<<RXEN0) | (1<<TXEN0)|(1<<RXCIE0);
	
	sei();
	
	Flash_init();
	//size_flash = size/16;
	size_flash = 5;
	send_string(texto_info);
	send_string(version);
	send_char('\n');
	send_string("validacion: ");
	dtostrf(flag_val,3,0,valor_m);
	send_string(valor_m);
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
			base_addr = 0X0;		
			///////////////////////////////////////////////////////
			for(uint32_t pagina = 0; pagina < size_flash; pagina++) {
				base_addr = pagina * 8 + offset;  // Cada página son 8 palabras
				checksum = 0;				
				// ? INCLUIR DIRECCIÓN en el checksum (los 3 bytes)		
				mid_address = ((base_addr>>8) & 0xFF);
				high_address = ((base_addr>>16) & 0xFF);
				low_address = (base_addr & 0xFF);
				
				Aux_addr = base_addr<<1;
				//Aux_addr = base_addr>>3;
				checksum += (Aux_addr >> 24) & 0xFF;
				checksum += (Aux_addr >> 16) & 0xFF;
				checksum += (Aux_addr >> 8) & 0xFF;
				checksum += Aux_addr & 0xFF;
				char_dato = ((Aux_addr>>24) & 0xFF);
				send_char(char_dato);
				char_dato = ((Aux_addr>>16) & 0xFF);
				send_char(char_dato);
				char_dato = ((Aux_addr>>8) & 0xFF);
				send_char(char_dato);
				char_dato = ((Aux_addr) & 0xFF);
				send_char(char_dato);
				
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
				
				if (flag_val == 0)
				{
					flag_ok =1;
					_delay_us(.06);
				}
				while(flag_ok == 0)
				{
					if (flag_ok == 2)
					{
						pagina--;
					}
					//asm("nop");
				}
				/*
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				*/
				flag_ok = 0;
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
			base_addr = 0;
			Dato = 0xA5A5;
			flash_write(base_addr,Dato);
			while(flag_ok == 0)
			{
				if (flag_ok == 2)
				{
					funcion_RW == 0;
				}
				//asm("nop");
			}
			flag_ok = 0;
			base_addr = 1;
			Dato = 0x5A5A;
			flash_write(base_addr,Dato);
			//funcion_RW = '0';
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
	checksum+=Aux_dato;
	Dato = Aux_dato&0xFF;
	send_char((Aux_dato));
	Aux_dato = high_Data;
	checksum+=Aux_dato;
	Dato=(Dato>>8&0XFF);
	Dato += Aux_dato&0xFF;
	send_char((Aux_dato));
	Aux_dato = (Dato&0XFF);	
}

void flash_write(uint32_t addr, uint16_t data) {
	// Secuencia de escritura estándar
	error = dir_dato("write");
	if (error == 0)
	{
		send_string("error de comando");
		send_char('\n');
		send_char('\r');
		return;
	}  // Puerto como salida
	
	// Comando de escritura: 2 ciclos de unlock + program
	// Dirección 0x555 + 0xAA, Dirección 0x2AA + 0x55, Dirección 0x555 + 0xA0
	
	write_command(0x555, 0xAA);
	write_command(0x2AA, 0x55);
	write_command(0x555, 0xA0);
	
	// Escribir datos
	write_command(addr, data);
	
	// Esperar fin de escritura
	wait_ready();
	
	error = dir_dato("read");
	if (error == 0)
	{
		send_string("error de comando");
		send_char('\n');
		send_char('\r');
		return;
	} // Volver a lectura
}

void write_command(uint32_t addr, uint16_t cmd) {
	// Secuencia CE#
	uint8_t		aux_cmd = 0;
	ctrl_port &= ~(1 << ce);
	// Establecer dirección
	low_address = (addr & 0xFF);
	mid_address = ((addr >> 8) & 0xFF);
	high_address = ((addr >> 16) & 0xFF);
	
	// Establecer datos
	aux_cmd = (cmd & 0xFF);
	low_Data_O = aux_cmd;
	send_string("LOW: ");
	dtostrf(aux_cmd,3,0,valor_m);
	send_string(valor_m);
	send_char('\n');
	aux_cmd = ((cmd >> 8) & 0xFF);
	high_Data_O = aux_cmd;
	send_string("HIGTH: ");
	dtostrf(aux_cmd,3,0,valor_m);
	send_string(valor_m);
	send_char('\n');
	// Secuencia WE#
	ctrl_port &= ~(1 << we);
	//_delay_us(1);
	ctrl_port |= (1 << we);
	//_delay_us(1);
	ctrl_port |= (1 << ce);
}

void wait_ready(void) {
	uint32_t timeout = 1000000;  // Timeout por seguridad
	while(((ctrl_pin & (1 << rby)) == 0) && timeout--) {
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
	}
	if(timeout == 0) {
		// Error de timeout
		send_string("ERROR: Timeout escritura\n");
	}
}