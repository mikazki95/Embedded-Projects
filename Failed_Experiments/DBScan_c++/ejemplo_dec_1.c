#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h> // para uint8_t

unsigned char x;
uint8_t y;
          
int BUFSIZE_ECG = 10, APP_UART_RECEIVE_BUFSIZE= 10;
int i,k;
char gUartReceiveBuffer[10]={{0xFF},{0xFF},{0xFF},{0xa5},{0xFF},{0xFF},{0xFF},{0xCE},{0xFF}};
char bufferdatos_ECG[10];
uint8_t valida_d1 = 0XA5;  //0x11;      //  validación dato derivación 1
uint8_t signo_dato = 0X00;       // cambiar a 0x00
uint8_t valida_d2 = 0x5A;  
bool        flag_dato_1=false;
uint8_t dato_in_d1[4];      // bytes del dato recibido
uint8_t dato_in_d2[4];      // bytes del dato recibido
int32_t dato_raw_d1,dato_raw_d2;        //dato concatenado de los bytes recibidos

int main()
{
        
    for (i = 0; i <= APP_UART_RECEIVE_BUFSIZE; i++) // Validaciòn de inicio de cadena
    {
        if (gUartReceiveBuffer[i] == (char)valida_d1)
        {
            // if (gUartReceiveBuffer[i+1] == (char)signo_dato)
            // {
                flag_dato_1 = true;
                k = i;
                i = APP_UART_RECEIVE_BUFSIZE + 1;
            // }
        }
    }
    if (flag_dato_1)
    {
        for (int i = 0; i <= BUFSIZE_ECG; i++)
        {
            
            bufferdatos_ECG[i] = gUartReceiveBuffer[k+i];
            if ((i > 0)&& (i < 5))      //if ((i > 1)&& (i < 5))
            {
                dato_in_d1[i-1] = (uint8_t)bufferdatos_ECG[i];//dato_in_d1[i-2] = (uint8_t)bufferdatos_ECG[i];
            }
            /*
            else if (i > 5)     //else if (i > 6)
            {
                dato_in_d2[i-6] = (uint8_t)bufferdatos_ECG[i];       //dato_in_d2[i-7] = (uint8_t)bufferdatos_ECG[i];
            }
              */  
                
        }
        // solo para datos normalizados

        if ((dato_in_d1[0] == 0)||(dato_in_d1[0] == 255))
        {
            
            dato_raw_d1 = ((dato_in_d1[0]<<24)|(dato_in_d1[1]<<16)|(dato_in_d1[2]<<8)|(dato_in_d1[3]));
            // dato_raw_d1 &= 0x0000FFFF;
            dato_raw_d2 = ((dato_in_d2[0]<<24)|(dato_in_d2[1]<<16)|(dato_in_d2[2]<<8)|(dato_in_d2[3]));
            //  dato_raw_d2 &= 0x0000FFFF;
            
        }
        flag_dato_1= false;
    }
}
