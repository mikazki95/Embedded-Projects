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
uint8_t		count_recived= 69;//21;
uint8_t		Top_addr	= 0;
uint32_t	base_addr	= 0;
uint32_t	Aux_addr	= 0;
uint8_t		char_dato	= 0;
uint32_t	size_flash	= 128;
uint32_t	offset		= 0x00; //0X770000; //0XEF7FFF; //0x7FFFEF;
uint8_t		flag_ok		= 0;
uint8_t		flag_val	= 1;
uint8_t     flag_impr	= 0;
uint8_t		trama_count = 0;
volatile uint8_t reception_mode		= 0;
volatile uint32_t data_buffer	[80]={0};    // Buffer para trama
volatile uint8_t data_index			= 0;     // Índice del buffer
volatile uint8_t expected_bytes		= 0; // Bytes esperados

ISR(USART0_RX_vect)
{
	comando=UDR0;
	if(reception_mode == 0)
	{
		if (comando =='R')
		{
			//send_char('R');
			//send_char('\n');
			funcion_RW = 'R';
			reception_mode = 0;
		}
		else if (comando =='W')
		{
			//send_char('W');
			//send_char('\n');
			funcion_RW = 'W';
			reception_mode = 1;
			data_index = 0;
		}
		else if (comando =='X')
		{
			send_char('X');
			send_char('\n');
			funcion_RW = 'X';
			reception_mode = 0;
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
			flag_val = 1;
			/*send_string("validacion: ");
			dtostrf(flag_val,3,0,valor_m);
			send_string(valor_m);
			send_char('\n');*/
		}
		else if (comando =='v')
		{
			flag_val = 0;
			/*send_string("validacion: ");
			dtostrf(flag_val,3,0,valor_m);
			send_string(valor_m);
			send_char('\n');*/
		}
		else 
		{
			send_char('X');
			send_char('\n');
			funcion_RW = 'E';
		}
	}
	else
	{
		data_buffer[data_index++] = comando;
		if (data_index >= count_recived)
		{
			reception_mode = 2;
		}
	}
	
}

void flash_read(uint32_t addr);
void flash_write(uint32_t addr, uint16_t data);
void buffer_write(uint32_t addr, uint16_t data[32]);
void write_command(uint32_t addr, uint16_t cmd);
void wait_ready(void);
void flash_erase(uint8_t Sector);
void process_received_frame(void);

int main(void)
{
    /* Replace with your application code */
	cli();
	
	usart_init();

	UCSR0B =  (1<<RXEN0) | (1<<TXEN0)|(1<<RXCIE0);
	
	sei();
	
	Flash_init();
	size_flash = size/16;
	//size_flash = 4095;
	size_flash = 25 ;
	//offset = 0x20000/2;
	send_string(texto_info);
	send_string(version);
	send_char('\n');
	send_string("validacion: ");
	dtostrf(flag_val,3,0,valor_m);
	send_string(valor_m);
	send_char('\n');
	// Verificar estado WP#
	/*
	if(ctrl_port & (1 << wp)) {
		printf("SA0 DESPROTEGIDO\n");
		} else {
		printf("SA0 PROTEGIDO - no se puede escribir en 0x000000-0x01FFFF\n");
	}
	*/
	trama_count = ((count_recived-5)/2); // tamaño de buffer de datos
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
				Aux_dato =	(base_addr & 0xFF);
				low_address = Aux_dato;
				Aux_dato = ((base_addr>>8) & 0xFF);
				mid_address = Aux_dato;
				Aux_dato = ((base_addr>>16) & 0xFF);
				high_address = Aux_dato;
				/*
				send_string("adress: ");
				dtostrf(base_addr,3,0,valor_m);
				send_string(valor_m);
				send_char('\n');
				*/
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
				//send_string("datos: ");
				// Leer las 8 palabras de la página
				flag_impr = 0;
				//flash_read(pagina);
				
				for(uint8_t pos = 0; pos < 8; pos++) {
					uint32_t addr = 0;
					addr = base_addr + pos;
					low_address = (addr & 0xFF);
					//_delay_ms(100);
					flash_read(addr);
												
				}
				
				//send_string("check sum: ");
				//_delay_us(1);
				ctrl_port |= (1 << oe);
				ctrl_port |= (1 << ce);
				checksum=(0x100 - checksum) & 0xFF;
				send_char(checksum);
				
				if (flag_val == 0)
				{
					flag_ok =1;
					_delay_us(2);
				}
				while(flag_ok == 0)
				{
					if (flag_ok == 2)
					{
						pagina--;
					}
					//asm("nop");
				}
				flag_ok = 0;					
			}
			//////////////////////////////////////////
		} 
		else if (funcion_RW == 'W')
		{
			flag_ok = 0;
			while (reception_mode == 1)
			{
				//send_string("esperando trama ");
				//_delay_ms(100);
				asm("nop");
			}
			process_received_frame();
			//flash_write(0,0x0123);
			Dato = (data_buffer[4]<<8)|data_buffer[5];
			Aux_addr = (base_addr>>1);//&(0x01FFFFFF);
			/*
			send_string("Direccion de array: ");
			dtostrf(base_addr,3,0,valor_m);
			send_string(valor_m);
			send_string(", en la direccion: ");
			dtostrf(Aux_addr,3,0,valor_m);
			send_string(valor_m);
			send_char('\n');
			*/
			uint16_t aux_dato_or [32] = {0};
			if (flag_ok == 1)
			{
				//send_string("enviar trama \n");
				for (uint8_t i =0; i<trama_count; i++)
				{
					uint8_t aux_index = 4 + 2*i;
					uint32_t aux2_addr = Aux_addr +i;
					Dato = (data_buffer[aux_index]<<8)|data_buffer[aux_index + 1];
					if (aux2_addr<0XFFE010)
					{
						//flash_write(aux2_addr,Dato);
						aux_dato_or[i]= Dato;
						// buffer_write(aux2_addr,aux_dato_or);
						//asm("nop");
					}
					else
					{						
						send_string("E");
						break;			
					}					
				}
				buffer_write(Aux_addr,aux_dato_or);
				send_string("b");
			} 			
			
			funcion_RW = '0';
			reception_mode = 0;
		}
		else if(funcion_RW == 'X')
		{
			send_string("Preparando para borrar \n");
			
			for (uint8_t i1=0;i1<255;i1++)
			{
				flash_erase(i1);
				send_string("borrado n \n");
			}
			
			flash_erase(0xFF);
			send_string("borrado \n");
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
		return;
	}
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
	uint16_t Aux_send = 0;
	uint16_t aux_high = 0;
	//uint16_t aux_low = 0;
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
	
	//addr = 0x0;
	Aux_send = data;
	//Aux_send = 0x1234;
	aux_high = (Aux_send>>8)|(Aux_send<<8);
	// Escribir datos
	/*
	send_string("dato: ");
	dtostrf(Aux_send,3,0,valor_m);
	send_string(valor_m);
	send_string(", direccion: ");
	dtostrf(addr,3,0,valor_m);
	send_string(valor_m);
	send_char('\n');
	*/
	write_command(addr, aux_high);
	//_delay_us(1);
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
	uint8_t		aux_cmd = 0;
	// Secuencia OE#
	ctrl_port |= (1 << oe);
	// Establecer dirección
	low_address = (addr & 0xFF);
	mid_address = ((addr >> 8) & 0xFF);
	high_address = ((addr >> 16) & 0xFF);
	asm("nop");
	asm("nop");
	
	// Establecer datos
	aux_cmd = (cmd & 0xFF);
	low_Data_O = aux_cmd;
	aux_cmd = ((cmd >> 8) & 0xFF);
	high_Data_O = aux_cmd;
	// Secuencia CE#
	ctrl_port &= ~(1 << ce);
	// Secuencia WE#
	ctrl_port &= ~(1 << we);
	ctrl_port |= (1 << we);
	ctrl_port |= (1 << ce);
}

void wait_ready(void) {
	uint32_t timeout = 1000000;  // Timeout por seguridad

	//send_string("espera");
	//send_char('\n');
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
	//send_string("Listo");
	//send_char('\n');
}

void flash_erase(uint8_t Sector){
	error = dir_dato("write");
	if (error == 0)
	{
		send_string("error de comando");
		send_char('\n');
		send_char('\r');
		return;
	}  // Puerto como salida
	
	// 2. Secuencia de 6 ciclos para Chip Erase
	write_command(0x555, 0x00AA);  // Unlock 1
	write_command(0x2AA, 0x0055);  // Unlock 2
	write_command(0x555, 0x0080);  // Setup erase
	write_command(0x555, 0x00AA);  // Unlock 3
	write_command(0x2AA, 0x0055);  // Unlock 4
	//write_command(0x555, 0x0010);  // Chip Erase command

	//write_command(0x00, 0x30);	//555 10   // borrado por seccion 
	write_command(Sector, 0x30);
	_delay_us(100);
	// Esperar fin de escritura
	wait_ready();
}

void process_received_frame(void) {
	// 1. Extraer dirección (primeros 4 bytes: 00 00 00 10)
	base_addr = 0;
		
	base_addr = (data_buffer[0] << 24) | (data_buffer[1] << 16) |
	(data_buffer[2] << 8) | data_buffer[3];
	
	// 2. Calcular checksum
	uint8_t calculated_checksum = 0;
	for(uint8_t i = 0; i < data_index - 1; i++) {
		calculated_checksum = calculated_checksum+data_buffer[i]; // 
	}
	calculated_checksum=(0x100 - calculated_checksum) & 0xFF;
	// 3. Verificar checksum
	uint8_t received_checksum = data_buffer[data_index - 1];
	//uint8_t received_checksum = data_buffer[68];
	
	if(calculated_checksum != received_checksum) {
		send_string("E");
		/*
		send_string("ERROR: Checksum incorrecto\n\r");
		send_string("calculated_checksum: ");
		dtostrf(calculated_checksum,3,0,valor_m);
		send_string(valor_m);
		send_char('\n');
		send_string("received_checksum: ");
		dtostrf(received_checksum,3,0,valor_m);
		send_string(valor_m);
		send_char('\n');
		*/
		return;
	}
	flag_ok = 1;
}

void buffer_write(uint32_t addr, uint16_t data[32])
{
	uint32_t Aux_SA = addr&0xFFFE0000;
	uint8_t Aux_send = trama_count-1;
	uint16_t aux_high = (addr&0x0000001F)-Aux_send;
	//uint16_t aux_low = 0;
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
	
	if (aux_high>=1)
	{
		write_command(0x555, 0xAA);
		write_command(0x2AA, 0x55);
		write_command(Aux_SA, 0x25);
		write_command(addr, Aux_send);
		
		//send_string("escribir 8 palabras\n\r");
		
		for (uint8_t i2 = 0; i2 <=Aux_send; i2++)
		{
			uint16_t flash_data = (data[i2] << 8) | (data[i2] >> 8);
			write_command(addr+i2, flash_data);
			/*
			send_string("datos: \n\r");
			dtostrf(data[i2],3,0,valor_m);
			send_string(valor_m);
			send_char('\n');
			*/
		}
		
		write_command(Aux_SA, 0x29);
	} 
	else
	{
		send_string("error de comando");
		send_char('\n');
	}
	
}