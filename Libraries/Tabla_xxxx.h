/*
 * Tabla_xxxxr.h
 *
 */ 


#ifndef TABLA_xxxx_H_
#define TABLA_xxxx_H_


int pulso_xx=0;

int xxxx_pulsos(float diferencia);

int xxxx_pulsos(float diferencia)
{
	if (diferencia<0.030)
	{
		pulso_xx=1;
	}
	else if ((diferencia>=0.030)&&(diferencia<0.040))
	{
		pulso_xx=2;
	}
	else if ((diferencia>=0.040)&&(diferencia<0.050))
	{
		pulso_xx=3;
	}
	else if ((diferencia>=0.050)&&(diferencia<0.060))
	{
		pulso_xx=4;
	}
	else if ((diferencia>=0.060)&&(diferencia<0.070))
	{
		pulso_xx=5;
	}
	else if ((diferencia>=0.070)&&(diferencia<0.080))
	{
		pulso_xx=6;
	}
	else if ((diferencia>=0.080)&&(diferencia<0.100))
	{
		pulso_xx=7;
	}
// 	else if ((diferencia>=0.090)&&(diferencia<0.100))
// 	{
// 		pulso_xx=8;
// 	}
	else if ((diferencia>=0.100)&&(diferencia<0.125))
	{
		pulso_xx=8;		//9;
	}
	else if ((diferencia>=0.125)&&(diferencia<0.150))
	{
		pulso_xx=9;		//10;
	}
	else if ((diferencia>=0.150)&&(diferencia<0.175))
	{
		pulso_xx=10;		//11;
	}
	else if ((diferencia>=0.175)&&(diferencia<0.200))
	{
		pulso_xx=11;		//12;
	}
	else if ((diferencia>=0.200)&&(diferencia<0.230))
	{
		pulso_xx=12;		//13;
	}
	else if ((diferencia>=0.230)&&(diferencia<0.260))
	{
		pulso_xx=13;		//14;
	}
	else if ((diferencia>=0.260)&&(diferencia<0.290))
	{
		pulso_xx=14;		//15;
	}
	else if ((diferencia>=0.290)&&(diferencia<0.320))
	{
		pulso_xx=15;		//16;
	}
	else if (diferencia>=0.320)
	{
		pulso_xx=16;		//18;
	}
	
	return pulso_xx;
}



#endif /* TABLA_xxxx_H_ */
