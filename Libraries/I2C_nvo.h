/*
 * I2C.h
 *
 * Created: 30/11/2018 13:11:03
 *  Author: F1
 */ 


#ifndef I2C_H_
#define I2C_H_

#include <util/twi.h>
#include <stdint.h>

//#define F_I2C	400000UL// clock i2c

#define I2C_READ	0x01
#define I2C_WRITE	0x00
#define I2C_READACK	3
#define I2C_BYTE	2		// bit 2: timeout byte-transmission


void i2c_inicio()	//Inicializa módulo TWI	//		i2c_init
{
	TWBR=10;		//Para vel 400kHz,Fosc 14.74568MHz, prescaler de 1  antes 10
	TWCR|=(1<<TWEN);
}

uint8_t i2c_inicia_com()
{
	TWCR=(1<<TWEN)|(1<<TWINT)|(1<<TWSTA);
	while (!(TWCR & (1<<TWINT)));
	
	if((TWSR & 0xF8) != TW_START)
	{
		return 0;
	}
	
	return 1;
}

void i2c_detener()
{
	TWCR=(1<<TWEN)|(1<<TWINT)|(1<<TWSTO);
}

uint8_t i2c_envia_dato(unsigned char dato)
{
	TWDR=dato;
	TWCR=(1<<TWEN)|(1<<TWINT);
	while (!(TWCR & (1<<TWINT)));

	uint8_t twst = TW_STATUS & 0xF8;
	
	if ( (twst != TW_MT_SLA_ACK) && (twst != TW_MR_SLA_ACK) ) return 1;
	
	return 0;

}

uint8_t i2c_read_ack ()
{
	TWCR=(1<<TWEN)|(1<<TWINT)|(1<<TWEA);
	while (!(TWCR & (1<<TWINT)));
	return TWDR;
}


uint8_t i2c_read_nack ()			//uint8_t i2c_read_nack(void)
{
	TWCR=(1<<TWEN)|(1<<TWINT);
	while (!(TWCR & (1<<TWINT)));
	return TWDR;
}

#endif /* I2C_H_ */

