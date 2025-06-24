

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>

int aux_1 = 8500;
int aux_2 = 8500;
int array_aux_1[20] ={15,15,15,2800,15,15,15,-10,0,15,20,150,200,0,15,15,14,15,14,15};;
int array_aux_2[20] = { 0 };
int index_array = 0;
bool flag_marcapasos_norm = false;    

int long_array = 20;
bool flag_array = false;
int Dnorm_1;
int Dnorm_2;

int cont_max_1      = 0;
int cont_max_2      = 0;
int value_1_max     = -2147483648;
int value_2_max     = -2147483648;
int aux_1_min       = 2147483647;
int aux_1_max       = -2147483648;
int aux_2_min       = 2147483647;
int aux_2_max       = -2147483648;
int aux_1_prom      = 0;
int prom_1          = 0;
int aux_iarray_prom     =0;
int aux_2_prom      = 0;
int prom_2          = 0;
int aux_iarray_prom2    =0;
int aux_send        = 0;


int factor_x = 1000;

__UINT8_TYPE__ Dato_2[24];

//uint16_t ch1Raw;  //Valor instantáneo leído del canal 1 (respiración)
float aux_graph = 0, aux_graph_2 = 0;       //para debuggear, borrar al terminar

int i_array = 0;

/*   Normalizacion definido en tarea 5
 Necesita las variables de int 32 del adc 2 y 3 que corresponden a las derivaciones I y II
 Tiene como salida que va al analizador de arritmias por usart2 con un vector Dato_2 que convierte los int32 en uint8 para su envió
 */


int main()
{
    //aux_1 = ads_I_i;
    //aux_2 = ads_II_i;

    //array_aux_1[index_array] = aux_1;
    //array_aux_2[index_array] = aux_2;
    index_array++;
    if (index_array >= long_array-1)
        {
            index_array = 0;
        }
    value_1_max     = -2147483648;
    value_2_max     = -2147483648;
    aux_1_max       = -2147483648;
    aux_1_min       = 2147483647;
    aux_2_max       = -2147483648;
    aux_2_min       = 2147483647;
    aux_iarray_prom = 0;
    aux_1_prom      = 0;
    prom_1  = 0;
    aux_iarray_prom2 = 0;
    aux_2_prom      = 0;
    prom_2          = 0;


    for (i_array = 0; i_array <( long_array - 1); i_array++)
        {
            aux_1_prom=array_aux_1[i_array]+aux_1_prom;
            aux_iarray_prom++;
            if (aux_iarray_prom>9)
                {
                    prom_1=prom_1+(aux_1_prom/10);
                    aux_1_prom=0;
                    aux_iarray_prom=0;
                }
            aux_2_prom=array_aux_2[i_array]+aux_2_prom;
            aux_iarray_prom2++;
            if (aux_iarray_prom2>9)
                {
                    prom_2=prom_2+(aux_2_prom/10);
                    aux_2_prom=0;
                    aux_iarray_prom2=0;
                }
            if ((array_aux_1[i_array] > value_1_max))
                {
                    cont_max_1++;
                    if((array_aux_1[i_array] > aux_1_max)&&(cont_max_1>2))
                        {
                            aux_1_max = array_aux_1[i_array];
                        }
                    value_1_max = array_aux_1[i_array];
                }
            else
                {
                    value_1_max     = -2147483648;
                    cont_max_1=0;
                }
            if (array_aux_1[i_array] < aux_1_min)
                {
                    aux_1_min = array_aux_1[i_array];
                }
            if (array_aux_2[i_array] > value_2_max)
                {
                    cont_max_2++;
                    if((array_aux_2[i_array] > aux_2_max)&&(cont_max_2>2))
                        {
                            aux_2_max = array_aux_2[i_array];
                        }
                    value_2_max = array_aux_2[i_array];

                }
            else
                {
                    value_2_max     = -2147483648;
                    cont_max_2=0;
                }
            if (array_aux_2[i_array] < aux_2_min)
                {
                    aux_2_min = array_aux_2[i_array];
                }
        }
    prom_1 = prom_1/100;
    prom_2 = prom_2/100;

    Dnorm_1=0;//
    Dnorm_2=0;//

   // Dnorm_1=aux_1;
   // Dnorm_2=aux_2;
    //Dnorm_1=aux_1_max-aux_1_min;
    //Dnorm_2=aux_2_max-aux_2_min;



    if (((aux_2_max-aux_2_min)>4500)&&((aux_1_max-aux_1_min)>300))//10000
        {
            Dnorm_2 = (aux_2 - prom_2) * (factor_x) / (aux_2_max - prom_2);
            Dnorm_1=(aux_1 - prom_1)*(factor_x) / (aux_1_max - prom_1);
           // Dnorm_1=ads_delta;
        }

    Dato_2[10] = 0XAA ;
    Dato_2[11] = 0X00 ;


    Dato_2[0] = 0xA5;

    Dato_2[1] = ((Dnorm_1 & 0xFF000000) >> 24); // rec de los bytes r y convinarlos con la mÃ¡scara de comando
    Dato_2[2] = ((Dnorm_1 & 0xFF0000) >> 16); // rec de los bytes r y convinarlos con la mÃ¡scara de comando
    Dato_2[3] = ((Dnorm_1 & 0xFF00) >> 8); // rec de los bytes r y convinarlos con la mÃ¡scara de comando
    Dato_2[4] = (Dnorm_1 & 0xFF); // rec de los bytes r y convinarlos con la mÃ¡scara de comando

    Dato_2[5] = 0x0A; // rec de los bytes r y convinarlos con la mÃ¡scara de comando

    Dato_2[6] = ((Dnorm_2 & 0xFF000000) >> 24); // rec de los bytes r y convinarlos con la mÃ¡scara de comando
    Dato_2[7] = ((Dnorm_2 & 0xFF0000) >> 16); // rec de los bytes r y convinarlos con la mÃ¡scara de comando
    Dato_2[8] = ((Dnorm_2 & 0xFF00) >> 8); // rec de los bytes r y convinarlos con la mÃ¡scara de comando
    Dato_2[9] = (Dnorm_2 & 0xFF); // rec de los bytes r y convinarlos con la mÃ¡scara de comando

    //if (flag_marcapasos_int)
    if (flag_marcapasos_norm)
        {
            //flag_marcapasos_int     = false;
            flag_marcapasos_norm    = false;
            Dato_2[11] = 1;
        }
    //Dato_2[12] = 0x0A;
   // HAL_UART_Transmit_IT (&huart2, Dato_2, 13);

    Dato_2[12]=Dato_2[0];
    Dato_2[13]=Dato_2[1];
    Dato_2[14]=Dato_2[2];
    Dato_2[15]=Dato_2[3];
    Dato_2[16]=Dato_2[4];
    Dato_2[17]=Dato_2[5];
    Dato_2[18]=Dato_2[6];
    Dato_2[19]=Dato_2[7];
    Dato_2[20]=Dato_2[8];
    Dato_2[21]=Dato_2[9];
    Dato_2[22]=Dato_2[10];
    Dato_2[23]=Dato_2[11];
    
    //HAL_UART_Transmit_IT (&huart2, Dato_2, 24);


}

