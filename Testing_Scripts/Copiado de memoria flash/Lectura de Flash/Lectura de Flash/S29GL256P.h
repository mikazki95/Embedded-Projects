/*
 * S29GL256P.h
 *
 * Created: 18/11/2025 10:17:44 a. m.
 *  Author: TONA
 */ 


#ifndef S29GL256P_H_
#define S29GL256P_H_
#define SIZE			(32 * 1024 * 1024)
#define SECTOR_SIZE		(128 * 1024) 
#define low_address		PORTC
#define mid_address		PORTD
#define high_address	PORTL
#define low_Data		PINA
#define high_Data		PINJ
#define low_Data_O		PORTA
#define high_Data_O		PORTJ
#define ctrl_port		PORTH
#define ctrl_pin		PINH

// Pines de control 
#define ce  0  // Chip Enable
#define oe  1  // Output Enable
#define we  2  // Write Enable
#define bye 3  // BYTE# (0=8bit, 1=16bit)
#define rst 4  // Reset
#define rby 5  // Ready/Busy (INPUT)
#define wp   6  // ? AGREGADO: WP#/ACC

void Flash_init(); 


void Flash_init(){	
	DDRC = 0XFF;
	DDRD = 0XFF;
	DDRL = 0xFF;
	PORTA = 0xFF; 
	PORTJ = 0xFF;
	DDRA = 0x00;
	DDRJ = 0x00;
	
	DDRH |= (1 << ce) | (1 << oe) | (1 << we) | (1 << bye) | (1 << rst) | (1 << wp);
	DDRH &= ~(1<<rby);  // RBY como entrada
	PORTH |= (1 << rby);  // ? ACTIVAR PULL-UP interno en RBY#
	// Estado inicial
	ctrl_port |= (1<<ce) | (1<<oe) | (1<<we);  // Inactivos
	ctrl_port |= (1<<bye);  // Modo 16 bits
	ctrl_port |= (1 << wp);   // ? WP#
	
	// Reset
	ctrl_port &= ~(1<<rst);
	_delay_us(10);
	ctrl_port |= (1<<rst);
	_delay_ms(1);
	
}

int dir_dato(char funcion[5]){

	if (strcmp(funcion, "read") == 0)
	{
		DDRA = 0x00;
		DDRJ = 0x00;
		low_Data_O = 0XFF;
		high_Data_O = 0XFF;
		return 1;
	} 
	else if (strcmp(funcion, "write") == 0)
	{
		DDRA = 0xFF;
		DDRJ = 0xFF;
		return 1;
	}
	else
	{
		return 0;
	}
	
		
}


#endif /* S29GL256P_H_ */