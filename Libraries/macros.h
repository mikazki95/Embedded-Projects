/*
 * macros.h
 *
 * Created: 13/09/2021 09:53:13 a.m.
 *  Author: Gaby Morales
 */ 


#ifndef MACROS_H_
#define MACROS_H_

void load_arreglo(int jr,char arreglo_1[],char arreglo_2[],int sub0);
void limpiar_reg(char nombre[5],int tamano);

void load_arreglo(int jr, char arreglo_1[],char arreglo_2[],int sub0)
{
	for (int i=0;i<jr;i++)
	{
		arreglo_1[i]=arreglo_2[i+sub0];
	}
}

void limpiar_reg(char nombre[5],int tamano)
{
	for (int k=0;k<tamano;k++)
	{
		nombre[k]=0;
	}
}



#endif /* MACROS_H_ */