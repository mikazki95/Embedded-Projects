/*
 * I2C.h
 *
 *  Author: F1
 */ 


#ifndef I2C_H_
#define I2C_H_

#include <avr/io.h>
//#include <stdint.h>
#include <util/delay.h>

int dt,r_dato;

uint8_t i2c_inicia_com();
void i2c_detener();
uint8_t i2c_envia_dato(unsigned char dato);
uint8_t i2c_read_ack ();
uint8_t i2c_read_nack ();


uint8_t i2c_inicia_com()
{
	//sda1
	DDRD&=~(1<<PD1);		//=0	IN
	PORTD|=(1<<PD1);		//=1
	
	//scl1
	//DDRD&=~(1<<PD0);		//=0	IN
	DDRD|=(1<<PD0);
	PORTD|=(1<<PD0);		//=1

	_delay_us(0.5);

	//sda0
	DDRD|=(1<<PD1);			//=1    OUT
	PORTD&=~(1<<PD1);		//=0

	return 1;
}

void i2c_detener()
{
	//scl0
	DDRD|=(1<<PD0);			//=1
	PORTD&=~(1<<PD0);		//=0
	//sda0
	DDRD|=(1<<PD1);			//=1	OUT
	PORTD&=~(1<<PD1);		//=0
	//scl1
	DDRD&=~(1<<PD0);		//=0	IN
	PORTD|=(1<<PD0);		//=1
	_delay_us(0.5);
	//sda1
	DDRD&=~(1<<PD1);		//=0	IN
	PORTD|=(1<<PD1);		//=1

}


uint8_t i2c_envia_dato(unsigned char dato)
{
	int b_wdt_int,dt_aux=0;
	int In_dato_ed=0;

	for (dt=0 ; dt<8 ; dt ++)
	{
		//scl0
		DDRD|=(1<<PD0);		//=1
		PORTD&=~(1<<PD0);	//=0
		
		dt_aux=dato;
		dt_aux=dt_aux&0x80;
		if (dt_aux==0x80)
		{
			//sda1
			DDRD&=~(1<<PD1);		//=0
			PORTD|=(1<<PD1);		//=1
		}
		else
		{
			//sda0
			DDRD|=(1<<PD1);		//=1
			PORTD&=~(1<<PD1);	//=0
		}
		//scl1
		DDRD&=~(1<<PD0);		//=0
		PORTD|=(1<<PD0);		//=1
		
		dato <<= 1;
	}
	//scl0
	DDRD|=(1<<PD0);			//=1
	PORTD&=~(1<<PD0);		//=0
	_delay_us(0.3);
	//sda1
	DDRD&=~(1<<PD1);		//=0
	PORTD|=(1<<PD1);		//=1
	
	//scl1
	DDRD&=~(1<<PD0);		//=0
	PORTD|=(1<<PD0);		//=1
		
	_delay_us(0.3);
	//DDRD&=~(1<<PD1);
	In_dato_ed=PIND;
	In_dato_ed=In_dato_ed&0X02;
	if (In_dato_ed==2)
	{
		b_wdt_int=0;
	}
	else
	{
		b_wdt_int=1;
	}
	
	//scl0
	//	DDRD|=(1<<PD0);			//=1
	PORTD&=~(1<<PD0);		//=0
	_delay_us(0.1);
	return b_wdt_int;
}

unsigned char i2c_read_ack ()
{
	int in_dato_a=0;
	r_dato=0;
	DDRD&=~(1<<PD1);

	DDRD|=(1<<PD0);		//=1		out
	PORTD&=~(1<<PD0);	//=0

	for (dt=0 ; dt<8 ; dt ++)
	{
		//scl1
		DDRD&=~(1<<PD0);		//=0
		PORTD|=(1<<PD0);		//=1
		_delay_us(0.15);
		
		in_dato_a=PIND;		
		in_dato_a=in_dato_a&0X02;
		if (in_dato_a==2)
		{
			r_dato=r_dato+1;
		}
		//scl0
		DDRD|=(1<<PD0);			//=1
		PORTD&=~(1<<PD0);		//=0
			
		if (dt<7)
		{
			r_dato=r_dato << 1;
		}
	}

	//sda0
	DDRD|=(1<<PD1);				//=1
	PORTD&=~(1<<PD1);			//=0
	//scl0
	//DDRD|=(1<<PD0);				//=1
	PORTD&=~(1<<PD0);			//=0

	_delay_us(0.2);

	//scl1
	DDRD&=~(1<<PD0);			//=0
	PORTD|=(1<<PD0);			//=1
	
	_delay_us(0.5);

	//scl0
	DDRD|=(1<<PD0);				//=1
	PORTD&=~(1<<PD0);			//=0
	
	return r_dato; 
		
}

unsigned char i2c_read_nack ()
{
	int in_dato_na=0;
	r_dato=0;
	DDRD&=~(1<<PD1);
	DDRD|=(1<<PD0);		//=1
	PORTD&=~(1<<PD0);	//=0 

	for (dt=0 ; dt<8 ; dt ++)
	{
		//scl1
		DDRD&=~(1<<PD0);		//=0
		PORTD|=(1<<PD0);		//=1
		_delay_us(0.15);

		in_dato_na=PIND;
		in_dato_na=in_dato_na&0X02;
		if (in_dato_na==2)
		{
			r_dato=r_dato+1;
		}
		//scl0
		DDRD|=(1<<PD0);		//=1
		PORTD&=~(1<<PD0);	//=0
		
		if (dt<7)
		{
			r_dato=r_dato << 1;
		}
	}
		
	//sda1
	DDRD&=~(1<<PD1);		//=0
	PORTD|=(1<<PD1);		//=1

	//scl0
//	DDRD|=(1<<PD0);			//=1
	PORTD&=~(1<<PD0);		//=0
	_delay_us(0.2);

	//scl1
	DDRD&=~(1<<PD0);		//=0
	PORTD|=(1<<PD0);		//=1
	_delay_us(0.5);
	
	//scl0
	DDRD|=(1<<PD0);			//=1
	PORTD&=~(1<<PD0);		//=0

	return r_dato;
}

#endif /* I2C_H_ */

