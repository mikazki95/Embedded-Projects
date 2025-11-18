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
#define low_Data		PORTA
#define high_Data		PORTJ
#define ctrl_port		PORTB
#define ctrl_pin		PINB

// Pines de control 
#define ce  0  // Chip Enable
#define oe  1  // Output Enable
#define we  2  // Write Enable
#define bye 3  // BYTE# (0=8bit, 1=16bit)
#define rst 4  // Reset
#define rby 5  // Ready/Busy (INPUT)

void Flash_init(); 


void Flash_init(){	
	DDRC = 0XFF;
	DDRD = 0XFF;
	DDRL = 0xFF;
	DDRD = 0x00;
	DDRL = 0x00;
	
	DDRB |= (1<<ce) | (1<<oe) | (1<<we) | (1<<bye) | (1<<rst);
	DDRB &= ~(1<<rby);  // RBY como entrada
	
	// Estado inicial
	ctrl_port |= (1<<ce) | (1<<oe) | (1<<we);  // Inactivos
	ctrl_port |= (1<<bye);  // Modo 16 bits
	
	// Reset
	ctrl_port &= ~(1<<rst);
	_delay_us(10);
	ctrl_port |= (1<<rst);
	_delay_ms(1);
	
}

int dir_dato(char funcion[5]){

	if (strcmp(funcion, "read") == 0)
	{
		DDRD = 0x00;
		DDRL = 0x00;
		return 1;
	} 
	else if (strcmp(funcion, "write") == 0)
	{
		DDRD = 0xFF;
		DDRL = 0xFF;
		return 1;
	}
	else
	{
		return 0;
	}
	
		
}


#endif /* S29GL256P_H_ */