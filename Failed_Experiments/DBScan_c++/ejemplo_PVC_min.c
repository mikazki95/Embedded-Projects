#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#define num_puls 250
#define NUM_POINTS_2 11

int main()
{
 
//   int conta_arrit = 0;
  //  bool flag_inicia_cta = false;
    int tipo_arritmia = 0;
    int array_cvp [250][2] = {0};
    int pulsos_arritmia [90][2] = {0};
    int pulso [250][2] = {0};
    int cont_0 = 0;
    int cont_1 = 0;
    int promedio_hr = 0;
    int cont_ok = 0;
    bool flag_punto_Q=false, flag_arritmia=false, flag_data_aux=false;
    int punto_heartrate=0;
    int input_0=0, input_1=0, flag_cvp;
    int cuenta_cvp=0;
    int index_p_arritmia = 0 , limite_taq = 110 , variable_aux =0, fin_array=0;

    while (1)
    {
       

        if (flag_punto_Q)
        {
            pulso[punto_heartrate][0]= punto_heartrate;
            flag_punto_Q = false;
            pulso[punto_heartrate][0]= input_1;
            //flag_cvp = find_arritmia(pulso,clusters,len_clusters,num_clusters_1,punto_heartrate);
        //       flag_imprimir_cl = true;

            array_cvp[cont_0][0]= flag_cvp;
            array_cvp[cont_0][1]= punto_heartrate;
            cont_0++;

            cont_1 += punto_heartrate;

            if ((cont_1 > 15000) || (cont_0 >= 250))
            {
                cont_0 = 0;
                cont_1 = 1;
            }
            int cont_3 = 0;
            int x_a = 0;
            cuenta_cvp = 0;
            int cont_4 = 2;
            promedio_hr = 0;

            for (x_a = 0; x_a < 250; x_a++)
            {
                cuenta_cvp += array_cvp[x_a][0];        // ******** CVP/min
                cont_3 += array_cvp[x_a][1];

                if (cont_3 > 15000)
                {
                    cont_4 = x_a;
                    break;
                }
                }
            //promedio_hr = time_segRR_valido/4;

            if (flag_cvp == 1)
            {
                flag_arritmia = true;

            }

            if (flag_arritmia)
            {
                pulsos_arritmia[index_p_arritmia][0] = flag_cvp;
                pulsos_arritmia[index_p_arritmia][1] = punto_heartrate;
                /*   array_aux_1[index_p_arritmia][0]=flag_a;
                array_aux_1[index_p_arritmia][1]=pulsos_arritmia[k_a][0];   */
                index_p_arritmia++;
                fin_array += punto_heartrate;
                //  variable_aux = index_p_arritmia;        // solo para imprimir


                if (index_p_arritmia == 1)
                {


                    if (cont_0 >= 2)
                    {
                        variable_aux = array_cvp[cont_0-2][1];
                        if ((array_cvp[cont_0-2][1]) < ((promedio_hr/3) + 13))     // checar el lunes, limite de condición
                        {
                            tipo_arritmia=array_cvp[cont_0-1][1];
                            if ((array_cvp[cont_0-1][1]) > (promedio_hr*5/4))
                            {
                                fin_array = 0;
                                index_p_arritmia = 0;
                                flag_arritmia = false;
                                tipo_arritmia = 6;            // R en T
                                cont_ok = 0;
                                //break;
                            }

                        }

                    }
                    else
                    {
                        tipo_arritmia = array_cvp[cont_4-2+cont_0][1];
                        if ((array_cvp[cont_4-2+cont_0][1]) < ((promedio_hr/3) + 13))     // checar el lunes, limite de condición
                        {
                            //  tipo_arritmia=array_cvp[cont_4-1+cont_0][1];
                            if ((array_cvp[cont_4-1+cont_0][1]) > (promedio_hr*5/4))
                            {
                                fin_array = 0;
                                index_p_arritmia = 0;
                                flag_arritmia = false;
                                //   tipo_arritmia = 7;            // R en T
                                cont_ok = 0;
                                //break;
                            }

                        }
                    }

                }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////


                if (index_p_arritmia == 8)        ////  BIGEMINY tipo_arritmia = 1
                {
                    int k_a=0;
                    int flag_a;
                    for(k_a=0;k_a<=7;k_a++)
                    {
                        flag_a = (k_a)&0x0001;
                        if(flag_a==pulsos_arritmia[k_a][0])
                        {
                            //   tipo_arritmia=80;
                            break;

                        }
                        if(k_a == 7)
                        {
                            fin_array = 0;
                            // variable_aux = index_p_arritmia;
                            index_p_arritmia = 0;
                            flag_arritmia = false;
                            tipo_arritmia=1;
                            cont_ok = 0;
                        }
                    }
                }
                if (index_p_arritmia == 10)        // trigeminy
                {
                    int k_a = 0;
                    int flag_a;

                    for(k_a = 0;k_a <= 9;k_a++)
                    {
                        flag_a = (k_a)%3;
                        if(flag_a==0)
                        {
                            if ((pulsos_arritmia[k_a][0]) == 0)
                            {
                                tipo_arritmia=55;
                                break;
                            }
                        }
                        else
                        {
                            if ((pulsos_arritmia[k_a][0]) == 1)
                            {
                                tipo_arritmia=25;
                                break;
                            }
                        }
                        if(k_a==9)
                        {
                            fin_array = 0;
                            index_p_arritmia = 0;
                            flag_arritmia = false;
                            tipo_arritmia=2;
                            cont_ok = 0;
                        }
                    }
                }
                if (index_p_arritmia == 6)        // PAIR & TV > 2
                {
                    int k_a = 0;
                    int flag_a = 0;
                    bool flag_evaluar = false;
                    bool flag_taq = true;
                    //  int frec=0;

                    for(k_a=0;k_a<=6;k_a++)
                    {
                        flag_a += pulsos_arritmia[k_a][0];
                        if (pulsos_arritmia[k_a][1] > limite_taq)
                            flag_taq = false;

                        if (flag_a < (k_a + 1))
                        {
                            flag_evaluar = true;
                        }

                        if ((flag_evaluar)&&(pulsos_arritmia[k_a][0]==1))
                            break;

                        if(k_a==6)
                        {
                            if((flag_evaluar)&&(flag_a == 2))
                            {
                                fin_array = 0;
                                index_p_arritmia = 0;
                                flag_arritmia = false;
                            //      tipo_arritmia=3;        // par CVP
                                cont_ok = 0;
                            }
                            else if (flag_evaluar)
                            {
                                fin_array = 0;
                                index_p_arritmia = 0;
                                flag_arritmia = false;
                                tipo_arritmia=4;
                                cont_ok = 0;
                            }
                            else
                            {
                                if (flag_taq)
                                {
                                    fin_array = 0;
                                    index_p_arritmia = 0;
                                    flag_arritmia = false;
                                    tipo_arritmia=5;        // Taquicardia ventricular
                                    cont_ok = 0;
                                }
                                else
                                {
                                    fin_array = 0;
                                    index_p_arritmia = 0;
                                    flag_arritmia = false;
                                    tipo_arritmia=20;
                                    cont_ok = 0;
                                }

                            }

                        }

                    }

                }
                // *****************  finalización de búsqueda de patrones de arritmias
                if (index_p_arritmia >= 12)    //&&(flag_cvp == 0))
                {
                    fin_array = 0;
                    index_p_arritmia = 0;
                    flag_arritmia = false;
                    tipo_arritmia=99;
                    cont_ok = 0;
                }


                if (fin_array > 7500)
                {
                    fin_array = 0;
                    index_p_arritmia = 0;
                    flag_arritmia = false;
                    tipo_arritmia=99;
                    cont_ok = 0;
                }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

            }       // fin flag_arritmia
            else
            {
                cont_ok++;
                if (cont_ok >= 8)
                {
                    cont_ok = 0;
                    tipo_arritmia = 0;
                }
            }

        }       // fin flag_punto S
            
        
    }
}

    