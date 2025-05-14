/*
 * Tabla_parker.h
 *
 * Created: 05/11/2020 11:24:09 a.m.
 *  Author: Gaby Morales
 */ 


#ifndef TABLA_PARKER_H_
#define TABLA_PARKER_H_


int pulso_park=0;

int parker_pulsos(float diferencia);

int parker_pulsos(float diferencia)
{
	if (diferencia<0.030)
	{
		pulso_park=1;
	}
	else if ((diferencia>=0.030)&&(diferencia<0.040))
	{
		pulso_park=2;
	}
	else if ((diferencia>=0.040)&&(diferencia<0.050))
	{
		pulso_park=3;
	}
	else if ((diferencia>=0.050)&&(diferencia<0.060))
	{
		pulso_park=4;
	}
	else if ((diferencia>=0.060)&&(diferencia<0.070))
	{
		pulso_park=5;
	}
	else if ((diferencia>=0.070)&&(diferencia<0.080))
	{
		pulso_park=6;
	}
	else if ((diferencia>=0.080)&&(diferencia<0.100))
	{
		pulso_park=7;
	}
// 	else if ((diferencia>=0.090)&&(diferencia<0.100))
// 	{
// 		pulso_park=8;
// 	}
	else if ((diferencia>=0.100)&&(diferencia<0.125))
	{
		pulso_park=8;		//9;
	}
	else if ((diferencia>=0.125)&&(diferencia<0.150))
	{
		pulso_park=9;		//10;
	}
	else if ((diferencia>=0.150)&&(diferencia<0.175))
	{
		pulso_park=10;		//11;
	}
	else if ((diferencia>=0.175)&&(diferencia<0.200))
	{
		pulso_park=11;		//12;
	}
	else if ((diferencia>=0.200)&&(diferencia<0.230))
	{
		pulso_park=12;		//13;
	}
	else if ((diferencia>=0.230)&&(diferencia<0.260))
	{
		pulso_park=13;		//14;
	}
	else if ((diferencia>=0.260)&&(diferencia<0.290))
	{
		pulso_park=14;		//15;
	}
	else if ((diferencia>=0.290)&&(diferencia<0.320))
	{
		pulso_park=15;		//16;
	}
	else if (diferencia>=0.320)
	{
		pulso_park=16;		//18;
	}
	
	return pulso_park;
}



#endif /* TABLA_PARKER_H_ */