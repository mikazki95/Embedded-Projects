/*
 * Sensores.h
 *
 * Created: 22/02/2021 09:37:29 a.m.
 */ 


#ifndef SENSORES_H_
#define SENSORES_H_

#define FS			1875		//Full Scale sensor/0.4
#define T_ST		273.15
#define	KTE_BAR		101.325	
#define N_CERO		8192

int		high_byte=0,low_byte=0,serialMS,serialLS;
int		vf_dt=0;

void initHAF (unsigned char ID_mux,unsigned char channel,unsigned char ID_haf);
float read_HAF_std(unsigned char ID_mux,unsigned char channel,unsigned char ID_haf,float temper,float atmosf,float flujo_0);
void read_NPA700est (unsigned char ID_npa);
float read_NPA700 (unsigned char ID_npa);
float read_T_NPA700 (unsigned char ID_npa);

void initHAF (unsigned char ID_mux,unsigned char channel,unsigned char ID_haf)
{
	i2c_inicia_com();
	i2c_envia_dato(ID_mux-1);
	i2c_envia_dato(channel);
	i2c_detener();

	i2c_inicia_com();
	i2c_envia_dato(ID_mux);
	i2c_read_nack();
	i2c_inicia_com();
	i2c_envia_dato(ID_haf);
	serialMS=(((int16_t)i2c_read_ack()<<8) | (int16_t)i2c_read_ack());
	serialLS=(((int16_t)i2c_read_ack()<<8) | (int16_t)i2c_read_nack());
	i2c_detener();
}

 float read_HAF_std(unsigned char ID_mux,unsigned char channel,unsigned char ID_haf,float temper,float atmosf,float flujo_0)
 {
 	float	fl_val=0,Q_std=0,flujo_ccm=0,Q_haf=0;
	float	temp_act,difer_flujo;	
 	int16_t	DT_Q=0;

	temp_act=T_ST+temper;
 	
	for (vf_dt=0 ; vf_dt<1 ; vf_dt=vf_dt )
	{
		DT_Q=0;

		i2c_inicia_com();
		i2c_envia_dato(ID_mux-1);
		i2c_envia_dato(channel);
		i2c_detener();

 		i2c_inicia_com();
 		i2c_envia_dato(ID_mux);
 		i2c_read_nack();
 		i2c_inicia_com();
 		vf_dt=i2c_envia_dato(ID_haf);

 		DT_Q=(((int16_t)i2c_read_ack()<<8) | (int16_t)i2c_read_nack());
 		i2c_detener();
		if (DT_Q==0x7FFF)
		{
			vf_dt=0;
		}
 		fl_val=(float)DT_Q;
 		Q_std=FS*((fl_val-N_CERO)/16384);		//Valor de flujo [SCCM]
		flujo_ccm=(Q_std*temp_act*KTE_BAR)/(T_ST*atmosf);	//flujo (ccm)
		Q_haf=flujo_ccm/1000;		//[lpm] litros por minuto
		
		difer_flujo=Q_haf-flujo_0;
		if ((difer_flujo>7)||(difer_flujo<-7))
		{
			vf_dt=0;
			flujo_0=Q_haf;
		}
	}
 	
 	return Q_haf;
 }

void read_NPA700est (unsigned char ID_npa)
{
	i2c_inicia_com();
	i2c_envia_dato(ID_npa);
	high_byte = i2c_read_ack();
	low_byte = i2c_read_nack();
	i2c_detener();
}

float read_NPA700 (unsigned char ID_npa)
{
	float	Pr_npa=0;
	int		lectura=0,h=1,sprs1,estado;		//,sprs2=0
	int		lect_0=0,difer=0;
	uint32_t acumula = 0;
	uint32_t aux_presion=0;
	
	for (vf_dt=0 ; vf_dt<1 ; vf_dt=vf_dt )
	{
		lectura=0;
		h=1;
		acumula = 0;
	
		while (h<5)
		{
			i2c_inicia_com();
			vf_dt=i2c_envia_dato(ID_npa);
			lectura=(int)i2c_read_ack();
			estado=lectura;
			lectura=lectura<<8;
			sprs1=(int)i2c_read_nack();
			lectura=lectura|sprs1;
			i2c_detener();
			if ((lectura==0x7FFF)||((lectura>0x2008)&&(lectura<0x3FFF)))
			{
				vf_dt=0;
			}
			
			lectura=lectura&0x3FFF;
			if (estado<0x3F)
			{
				if (h==1)
				{
					acumula+=lectura;
					lect_0=lectura;
					h++;
				}
				else
				{
					difer=lectura-lect_0;
					if ((difer<76)&&(difer>-76))
					{
						acumula=acumula+lectura;
						h++;
					}
					lect_0=lectura;
				}
			}
		}
		aux_presion=acumula/4;
		Pr_npa=-0.0525*((float)aux_presion-N_CERO);
	}
	return Pr_npa;
}
float read_T_NPA700(unsigned char ID_npa)
{
	uint8_t Hi_P_byte=0,Lo_P_byte=0,Hi_T_byte=0,Lo_T_byte=0;
	uint16_t ent_pres=0,ent_temp=0;
	float temperatura=0;
	
	i2c_inicia_com();
	i2c_envia_dato(ID_npa);
	Hi_P_byte=i2c_read_ack();
	Lo_P_byte=i2c_read_ack();
	Hi_T_byte=i2c_read_ack();
	Lo_T_byte=i2c_read_nack();
	i2c_detener();
	ent_pres=Hi_P_byte;
	ent_pres=(ent_pres<<8|Lo_P_byte);
	ent_temp=Hi_T_byte;
	ent_temp=(ent_temp<<8|Lo_T_byte);
	ent_temp=ent_temp>>5;
	temperatura=(float)ent_temp;
	temperatura=(temperatura/10.24)-50;

	return temperatura;
}


#endif /* SENSORES_H_ */