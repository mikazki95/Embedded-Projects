/*
 *  Copyright (C) 2018-2021 Texas Instruments Incorporated
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <drivers/gpio.h>
#include "ti_drivers_config.h"
#include "ti_drivers_open_close.h"
#include "ti_board_open_close.h"
#include <kernel/dpl/DebugP.h>
#include <kernel/dpl/SemaphoreP.h>
#include <kernel/dpl/TaskP.h>
#include <kernel/dpl/ClockP.h>
#include <kernel/dpl/AddrTranslateP.h>
#include "Definiciones.h"

/* Prioridad de la tarea, stack, tamaño del stack y objetos de tareas, estos DEBEN SER globales */
#define IPC_RPMESSAFE_TASK_PRI            (8U)
#define IPC_RPMESSAFE_TASK_STACK_SIZE     (8*1024U)                             // 8KiB
uint8_t gIpcTaskStack[IPC_RPMESSAFE_TASK_STACK_SIZE] __attribute__((aligned(32)));
TaskP_Object gIpcTask;


/* Task priority, stack, stack size and task objects, these MUST be global's */
#define TX_RX_APP_TASK_PRI         (8U)
#define TX_RX_APP_TASK_STACK_SIZE  (8*1024U)
uint8_t gTxRxAppStack[TX_RX_APP_TASK_STACK_SIZE] __attribute__((aligned(32)));
TaskP_Object gTxRxAppTask;

/** \brief App state structure to share data between main fxn and ISR */
typedef struct
{
    /*  UART variables - Set once     */
    uint32_t                baseAddr;      /*< UART base address */
    uint32_t                txTrigLvl;      /**< TX FIFO Trigger Level */
    uint32_t                rxTrigLvl;      /**< RX FIFO Trigger Level */

    /*  UART objects     */
    HwiP_Object             hwiObject;      /**< HWI object */
    SemaphoreP_Object       writeDoneSem;      /**< Write done semaphore */

    /* * UART write state variables - set for every transfer     */
    const uint8_t          *writeBuf;     /**< Buffer data pointer */
    volatile uint32_t       writeCount;     /**< Number of Chars sent */
    volatile uint32_t       writeSizeRemaining;     /**< Chars remaining in buffer */
    volatile uint32_t       txDataSent;     /**< Flag to indicate all the data written to the TX FIFO */

    /* * UART receive state variables - set for every transfer      */
    uint8_t                *readBuf;     /**< Read Buffer data pointer */
    volatile uint32_t       readCount;     /**< Number of Chars read */
    volatile uint32_t       rxOverflow;     /**< Flag to indicate if num of chars read more than the given size */
} UART_AppPrms;

/* Application specific task arguments */
typedef struct {
    uint32_t value;
} IpcRecvMsg_Args;

IpcRecvMsg_Args gIpcRecvMsg_Args;

typedef struct {
    uint32_t value_App;
} TxRxApp_Args;

TxRxApp_Args gTxRxApp_args;

/** \brief UART RX buffer where received data is put in ISR */
static uint8_t              gAppUartRxBuffer[APP_UART_BUFSIZE];
/** \brief UART TX buffer where TX data is put in Task context */
static uint8_t              gAppUartTxBuffer[APP_UART_BUFSIZE];
/** \brief UART app object */

static UART_AppPrms         gAppPrms;

// Static Function Declarations
static void App_uartTx(UART_AppPrms   *appPrms,
                       const uint8_t  *writeBuf,
                       uint32_t        writeSize);
static void App_uartRx(UART_AppPrms *appPrms, uint8_t *readBuf);
static void App_uartUserISR(void *arg);
static void App_uartInit(UART_AppPrms *appPrms);
static void App_uartDeInit(UART_AppPrms *appPrms);

/*  Funciones principales      */
//void generarTareas(void *args);
void tarea1(void *args);
void tarea3(void *args);     // Tarea para recepción de comandos


void generarTareas(void *args)
{
    int32_t status;
    int32_t statusT3;

    /* Open drivers to open the UART driver for console */
    Drivers_open();
    Board_driversOpen();

    DebugP_log("Inicio Arritmias !\r\n");

    /* Crear la tarea que recibe, procesa datos y detecta arrimias */
    TaskP_Params taskParams1;                                                    // Parámetros para generar la tarea
    TaskP_Params_init(&taskParams1);                                             // Iniciar parámetros. Recomendado por la API
    taskParams1.name = "TAREA_01";                                               // Nombre para Debug
    taskParams1.stackSize = IPC_RPMESSAFE_TASK_STACK_SIZE;                       // 8KiB
    taskParams1.stack = gIpcTaskStack;                                           // Puntero al stack (*uint8_t)
    taskParams1.priority = IPC_RPMESSAFE_TASK_PRI;                               // Prioridad de la tarea
    taskParams1.args = &gIpcRecvMsg_Args;                                       // Acá estamos pasando el RPMessage_Object como argumento de la función
    taskParams1.taskMain = tarea1;                                               // Función que implementa la tarea

    /* Construir la tarea */
    status = TaskP_construct(&gIpcTask, &taskParams1);
    DebugP_assert(status == SystemP_SUCCESS);


    /* Tarea para establecer la comunicación con la Raspberry */
    TaskP_Params taskParams3;                                           // Parámetros para generar la tarea
    TaskP_Params_init(&taskParams3);                                    // Iniciar parámetros. Recomendado por la API
    taskParams3.name = "TAREA_03";                                      // Nombre para Debug
    taskParams3.stackSize = TX_RX_APP_TASK_STACK_SIZE;                  // 8KiB
    taskParams3.stack = gTxRxAppStack;                                  // Puntero al stack (*uint8_t)
    taskParams3.priority = TX_RX_APP_TASK_PRI;                          // Prioridad de la tarea
    taskParams3.args = &gTxRxApp_args;
    taskParams3.taskMain = tarea3;                                      // Función que implementa la tarea

    // * Construir la tarea * /
    statusT3 = TaskP_construct(&gTxRxAppTask, &taskParams3);
    DebugP_assert(statusT3 == SystemP_SUCCESS);


    //  Board_driversClose();
    //  Drivers_close();
}

void tarea1(void *args)
{
    /* Tarea que recibe datos normalizados del STM32 y los procesa. */
    int32_t     transferOK,status;
    UART_Transaction trans;

    uint8_t     gUartBuffer[APP_UART_BUFSIZE];
    char        gUartReceiveBuffer[APP_UART_RECEIVE_BUFSIZE] = {0};
    char        bufferdatos_ECG[APP_UART_RECEIVE_BUFSIZE];

    #define APP_UART_ASSERT_ON_FAILURE(transferOK, transaction) \
        do { \
            if((SystemP_SUCCESS != (transferOK)) || (UART_TRANSFER_STATUS_SUCCESS != transaction.status)) \
            { \
                DebugP_assert(FALSE); /* UART TX/RX failed!! */ \
        } \
        } while(0) \

    int     i=0,k=0;
    int8_t  valor_ecg_act = 0;
    int8_t  ecg_ant = 0,flanco_ecg = 0;
    int32_t aux_raw_d1 = 0;
    int32_t aux_raw_d2 = 0;

    int array_cvp [250][2] = {0};
    int cont_0 = 0;
    int cont_1 = 0;
    int cont_beat = 0;
    int promedio_hr = 0;
    int valor_hr_k_1 = 0,valor_hr_k_0 = 0;
    int tipo_ant = 0;

    int cont_hr =0;
    int cuenta_cvp_ant = 0;
    int tiempo_asistole =  limite_asistole * 250;   // numero de muestras
    int conta_fib = 0;
    int tiempo_fibr = 1000;

    uint16_t cont_raw_cero = 0;
    int8_t index_p_arritmia = 0;

    Bool flag_vent_tach = false;
    Bool flag_desborde = false;
    Bool flag_asistole = false;
    Bool flag_no_pulso = false;
    Bool flag_normal_beat = true;
    Bool flag_conta_fib = false;
    Bool flag_arrit_pnp = false;
    Bool flag_arrit_pnc = false;
    Bool flag_dato_1 = false;

    float  lim_TachVent = 15000/limite_TachVent;
    int    lim_TachVent_int = (int)lim_TachVent;
    int derivada_d2_ant = 0;

    status = SemaphoreP_constructBinary(&gUartWriteDoneSem, 0);
    DebugP_assert(SystemP_SUCCESS == status);

    status = SemaphoreP_constructBinary(&gUartReadDoneSem, 0);
    DebugP_assert(SystemP_SUCCESS == status);

    UART_Transaction_init(&trans);      // Inicializar la estructura UART_Transaction

    flag_continuar = true;
    flag_start_ready = true;
    while(1)
    {
        gNumBytesWritten = 0U;        // secuencia para probar puerto UART de STM32
        trans.buf   = &gUartBuffer[0U];
        trans.count = sprintf(trans.buf,"Tarea 1: \n");
        transferOK = UART_write(gUartHandle[CONFIG_UART1], &trans);
        APP_UART_ASSERT_ON_FAILURE(transferOK, trans);

        // Wait for write completion
        SemaphoreP_pend(&gUartWriteDoneSem, SystemP_WAIT_FOREVER);
        DebugP_assert(gNumBytesWritten == BUFSIZE_ECG);
        do {
            /* Lectura de 10 caracteres enviados por STM32 */
            gNumBytesRead = 0U;
            trans.buf   = &gUartReceiveBuffer[0U];
            trans.count = APP_UART_RECEIVE_BUFSIZE;
            transferOK = UART_read(gUartHandle[CONFIG_UART1], &trans);
            APP_UART_ASSERT_ON_FAILURE(transferOK, trans);

            /* Wait for read completion */
            SemaphoreP_pend(&gUartReadDoneSem, SystemP_WAIT_FOREVER);
            DebugP_assert(gNumBytesRead == APP_UART_RECEIVE_BUFSIZE);
            //calculo de periodo de muestreo
            if (flag_define_fs)
            {
                inicio_muestreo++;
                if (inicio_muestreo == 10)
                {
                    cont_latido = 0;
                    flag_learning_1 = true;
                }
                if (inicio_muestreo > 50)
                {
                    flag_define_fs = false;
                    inicio_muestreo = 0;
                    flag_inicio_datos = true;
                    flag_trained = true;
                    cont_init_frec = 0;
                    int i0 = 0;
                    for (i0 = 0; i0 < 20; i0++)
                    {
                        dato_frec[i0][0]=0;
                        dato_frec[i0][1]=0;
                    }
                }
            }

            for (i = 0; i <= APP_UART_RECEIVE_BUFSIZE; i++) // Validaciòn de inicio de cadena
            {
                if ((gUartReceiveBuffer[i] == (char)valida_d1) && (flag_initTraining))
                {
                    flag_dato_1 = true;
                    flag_dato_rdy = true;
                    k = i;
                    i = APP_UART_RECEIVE_BUFSIZE + 1;
                }
            }

            if (flag_dato_1)
            {
                for (i = 0; i < BUFSIZE_ECG; i++)
                {
                    bufferdatos_ECG[i] = gUartReceiveBuffer[k+i];
                    if ((i > 0)&& (i < 5))
                        dato_in_d1[i-1] = (uint8_t)bufferdatos_ECG[i];
                    else if (i > 5)
                        dato_in_d2[i-6] = (uint8_t)bufferdatos_ECG[i];
                }
                pulso_marcapasos = (uint8_t) gUartReceiveBuffer[k + BUFSIZE_ECG +1];        // señal de marcapasos

                if (pulso_marcapasos > 100)
                    pulso_marcapasos = 0;
                else if ((pulso_marcapasos == valido_pace) && ((flag_info_arr_13 == 1) || (flag_info_arr_14 == 1)))   // cambiar 85 = 0X55
                {
                    flag_detec_mkpasos = true;
                    cta_mkpasos = 0;
                    flag_tipo_pulso = true;
                    cuenta_pulso_mk = 0;
                }

                if ((dato_in_d1[0] == 0x00)||(dato_in_d1[0] == 0xFF))
                {
                    aux_raw_d1 = ((dato_in_d1[0]<<24)|(dato_in_d1[1]<<16)|(dato_in_d1[2]<<8)|(dato_in_d1[3]));
                    aux_raw_d2 = ((dato_in_d2[0]<<24)|(dato_in_d2[1]<<16)|(dato_in_d2[2]<<8)|(dato_in_d2[3]));
                    if (aux_raw_d1 < 1500)
                        dato_raw_d1 = aux_raw_d1;
                    if (aux_raw_d2 < 1500)
                        dato_raw_d2 = aux_raw_d2;
                }
                flag_dato_1 = false;
            }

            /* Eco en TX de caracteres recibidos hacia STM32 */
            gNumBytesWritten = 0U;
            trans.buf   = &bufferdatos_ECG[0U];
            trans.count = BUFSIZE_ECG;
            transferOK = UART_write(gUartHandle[CONFIG_UART1], &trans);
            APP_UART_ASSERT_ON_FAILURE(transferOK, trans);

            // Wait for write completion
            SemaphoreP_pend(&gUartWriteDoneSem, SystemP_WAIT_FOREVER);
            DebugP_assert(gNumBytesWritten == BUFSIZE_ECG);     // */

            SemaphoreP_destruct(&gUartWriteDoneSem);
            SemaphoreP_destruct(&gUartReadDoneSem);

            valor_ecg_act = 1;
            if (dato_raw_d1 < -150)       // cambio 27/07/2023
                valor_ecg_act = 0;
            flanco_ecg = valor_ecg_act - ecg_ant;
            ecg_ant = valor_ecg_act;

            if (flanco_ecg == 1)   //flanco de subida
            {
                cuenta_edge++;
                if (cuenta_edge >= 150)
                    cuenta_edge = 150;
            }
            //   **** Detección QRS  ***
            ecg_filt_ind1 = FiltroPasaBanda(dato_raw_d1);
            ecg_filt_ind2 = FiltroPasaBanda(dato_raw_d2);
            deriv_ecg_d1 = Derivativa(ecg_filt_ind1);
            deriv_ecg_d2 = Derivativa(ecg_filt_ind2);
            integral_ecg_d1 = MovingWindowIntegral(deriv_ecg_d1);
            integral_ecg_d2 = MovingWindowIntegral(deriv_ecg_d2);

            EncontrarPuntoR(integral_ecg_d1);

            derivada_raw_d2 = DerivadaFuncion(dato_raw_d2);

            if (flag_punto_R)     // si se detecto punto R
                    {
                ProcesamientoSegmentoRR();
                DefinicionUmbrales(valor_punto_R);
                flag_punto_R = false;
                cuenta_edge = 0;
                flag_aux_delay = true;
                flag_aviso_learn = true;
                    }

            point_in[0] = contador_muestras;
            point_in[1] = integral_ecg_d1 >> 5;
            //-------- entrenamiento de red
            if (flag_dato_rdy)
            {
                flag_dato_rdy = false;
                if (flag_trained)
                {
                    if (!flag_data_aux)
                    {
                        data_aux[cont_task_3] = point_in[1];
                        cont_task_3++;
                        if (cont_task_3 > 399)
                            cont_task_3 = 0;
                    }
                    else
                    {
                        flag_data_aux = false;
                        if (flag_heartbeat)
                        {
                            if (cont_init_frec < 20)
                            {
                                dato_frec[cont_init_frec][0] = cont_task_3;
                                dato_frec[cont_init_frec][1] = point_in[1];
                                cont_init_frec++;
                            }
                            flag_heartbeat = false;

                            int k;
                            for (k = 0; k <= cont_task_3; k++)
                            {
                                data_pre[contador_tarea_2][0] = k;
                                data_pre[contador_tarea_2][1] = data_aux[k];

                                contador_tarea_2++;
                                if(contador_tarea_2 >= NUM_POINTS)
                                {
                                    //num_clusters_1 = dbscan(data_pre, eps, minPts, clusters, len_clusters, no_clusters, &len_no_clusters,NUM_POINTS);
                                    dbscan_2(dato_frec, eps_3, minPts_3, clusters_3, len_clusters_3, no_clusters_3, &len_no_clusters_3,NUM_POINTS_3);
                                    contador_tarea_2 = 0;
                                    flag_trained = false;
                                    k = cont_task_3 + 1;
                                    flag_training_ready = true;
                                }
                            }

                        }
                    }
                }
            }

        }    while (flag_continuar);
    }

}
void tarea3(void *args)
{
    UART_AppPrms    *appPrms = &gAppPrms;
    const char      *printStr = "Detección de arritmias..\r\n";
    const char      *printExitStr = "\r\nAll tests have passed!!\r\n";
    const char      *printVersion = "I:ARRITMIA,1.2\r\n";
    char            *printCadena = "0";
    uint32_t        numCharsRead, i, charRecvdCnt;
    uint8_t         BuffCmdApp[APP_UART_BUFSIZE] = {0};
    char            parametro[APP_UART_BUFSIZE] = {0};

    Bool            continua = true;
    Bool            flag_version = false;
    Bool            flag_ping = false;

    uint8_t         flag_activar = false;
    uint8_t         flag_activar_m = false;
    uint8_t         flag_aviso_asystole = 0;
    uint8_t         flag_aviso_fib_vent = 0;
    uint8_t         flag_aviso_tach_vent = 0;
    uint8_t         flag_aviso_tachy = 0;
    uint8_t         flag_aviso_brady = 0;
    uint8_t         flag_aviso_cvp_min = 0;
    uint8_t         flag_aviso_paired = 0;
    uint8_t         flag_aviso_tv_2 = 0;
    uint8_t         flag_aviso_bigeminy = 0;
    uint8_t         flag_aviso_trigeminy = 0;
    uint8_t         flag_aviso_missbeat = 0;
    uint8_t         flag_aviso_RenT = 0;
    uint8_t         flag_aviso_pnp = 0;
    uint8_t         flag_aviso_pnc = 0;
    uint8_t         flag_envio_activo = 0;


    DebugP_log("[UART] Echo Low Latency interrupt mode example started ...\r\n");
    /* Init App Prms */
    App_uartInit(appPrms);
    /* Send entry string and wait for Tx completion */
    App_uartTx(appPrms, (const uint8_t *) printStr, strlen(printStr));
    /* Init Rx Prms and Enable Rx Interrupt */
    App_uartRx(appPrms, &gAppUartRxBuffer[0U]);

    charRecvdCnt = 0U;
    /* Echo whatever is read */
    while(1)
    {
        do{
            if(appPrms->readCount > 0U)
            {
                uintptr_t   key;

                /* Enter Critical section, disable interrupt */
                key = HwiP_disable();

                /* copy data from RX buf to TX buf */
                numCharsRead = appPrms->readCount;
                for(i = 0U; i < numCharsRead; i++)
                {
                    BuffCmdApp[i] = gAppUartRxBuffer[i];
                    gAppUartTxBuffer[i] = gAppUartRxBuffer[i];
                }

                appPrms->readCount = 0U; /* Reset variable so that RX can start from first */

                /* End critical section */
                HwiP_restore(key);

                if (BuffCmdApp[0] == 'I')
                    flag_version = true;
                else if (BuffCmdApp[0] == 'R') {
                    flag_relearning = true;
                    flag_initTraining = true;
                }
                else if (BuffCmdApp[0] == 'P') {
                    flag_ping = true;
                }
                else if (BuffCmdApp[0] == 'D')  // D:A activar envio de informe de alarmas
                {
                    if (BuffCmdApp[2] == 'A')
                        flag_envio_activo = 1;
                    else if (BuffCmdApp[2] == 'I') {    // T:I desactivar el análisis de arritmias
                        flag_envio_activo = 0;
                        flag_initTraining = false;
                        flag_continuar = false;
                    }
                }
                else if (BuffCmdApp[0] == 'T')   // activación de alarmas
                {
                    if (BuffCmdApp[2] == 'A')   // T:A para activar todas las alarmas excepto marcapasos
                        flag_activar = 1;
                    else if (BuffCmdApp[2] == 'I') {    // T:I desactivar todas las alarmas
                        flag_activar = 0;
                        flag_activar_m = 0;
                    }
                    else if  (BuffCmdApp[2] == 'M') { // T:M para activar alarmas de marcapasos
                        flag_activar = 1;
                        flag_activar_m = 1;  // para alarmas de marcapasos
                    }
                    flag_aviso_asystole = flag_activar;
                    flag_aviso_fib_vent = flag_activar;
                    flag_aviso_tach_vent = flag_activar;
                    flag_aviso_tachy = flag_activar;
                    flag_aviso_brady = flag_activar;
                    flag_aviso_cvp_min = flag_activar;
                    flag_aviso_paired = flag_activar;
                    flag_aviso_tv_2 = flag_activar;
                    flag_aviso_bigeminy = flag_activar;
                    flag_aviso_trigeminy = flag_activar;
                    flag_aviso_missbeat = flag_activar;
                    flag_aviso_RenT = flag_activar;
                    flag_aviso_pnc = flag_activar_m;
                    flag_aviso_pnp = flag_activar_m;

                }
                else if ((BuffCmdApp[0] == '0') || (BuffCmdApp[0] == '1'))      // activación alarmas individuales
                {
                    if (BuffCmdApp[3] == 'A')
                        flag_activar = 1;
                    else if (BuffCmdApp[3] == 'I')
                        flag_activar = 0;
                }
                if ((BuffCmdApp[0] == '0') && (BuffCmdApp[1] == '1'))   // alarma Asístole
                {
                    flag_aviso_asystole = flag_activar;
                    if ((BuffCmdApp[3] >= 48) && (BuffCmdApp[3] <= 57))
                    {
                        for (int j = 0; j < 2; j++)
                            parametro[j] = BuffCmdApp[j + 3];

                        limite_asistole = atoi(parametro);
                    }
                }
                else if ((BuffCmdApp[0] == '0') && (BuffCmdApp[1] == '2')){
                    flag_aviso_fib_vent = flag_activar;
                }
                else if ((BuffCmdApp[0] == '0') && (BuffCmdApp[1] == '3'))
                {
                    flag_aviso_tach_vent = flag_activar;
                    if ((BuffCmdApp[3] >= 48) && (BuffCmdApp[3] <= 57))
                    {
                        for (int j = 0; j < 3; j++)
                            parametro[j] = BuffCmdApp[j + 3];
                        limite_TachVent = atoi(parametro);
                    }
                }
                else if ((BuffCmdApp[0] == '0') && (BuffCmdApp[1] == '4'))
                {
                    flag_aviso_tachy = flag_activar;
                    if ((BuffCmdApp[3] >= 48) && (BuffCmdApp[3] <= 57))
                    {
                        for (int j = 0; j < 3; j++)
                            parametro[j] = BuffCmdApp[j + 3];
                        limite_Tachy = atoi(parametro);
                    }
                }
                else if ((BuffCmdApp[0] == '0') && (BuffCmdApp[1] == '5'))
                {
                    flag_aviso_brady = flag_activar;
                    if ((BuffCmdApp[3] >= 48) && (BuffCmdApp[3] <= 57))
                    {
                        for (int j = 0; j < 2; j++)
                            parametro[j] = BuffCmdApp[j + 3];
                        limite_brady = atoi(parametro);
                    }
                }
                else if ((BuffCmdApp[0] == '0') && (BuffCmdApp[1] == '6')) {
                    flag_aviso_cvp_min = flag_activar;
                }
                else if ((BuffCmdApp[0] == '0') && (BuffCmdApp[1] == '7')) {
                    flag_aviso_paired = flag_activar;
                }
                else if ((BuffCmdApp[0] == '0') && (BuffCmdApp[1] == '8')) {
                    flag_aviso_tv_2 = flag_activar;
                }
                else if ((BuffCmdApp[0] == '0') && (BuffCmdApp[1] == '9')) {
                    flag_aviso_bigeminy = flag_activar;
                }
                else if ((BuffCmdApp[0] == '1') && (BuffCmdApp[1] == '0')) {
                    flag_aviso_trigeminy = flag_activar;
                }
                else if ((BuffCmdApp[0] == '1') && (BuffCmdApp[1] == '1')) {
                    flag_aviso_missbeat = flag_activar;
                }
                else if ((BuffCmdApp[0] == '1') && (BuffCmdApp[1] == '2')) {
                    flag_aviso_RenT = flag_activar;
                }
                else if ((BuffCmdApp[0] == '1') && (BuffCmdApp[1] == '3')){
                    flag_aviso_pnc = flag_activar;
                }
                else if ((BuffCmdApp[0] == '1') && (BuffCmdApp[1] == '4')) {
                    flag_aviso_pnp = flag_activar;
                }

                for (int j = 0; j < 4; j++)
                    parametro[j] = 0;

                for (i = 0;i < numCharsRead;i++)
                    BuffCmdApp[i] = 0;

                if (flag_version)
                {
                    App_uartTx(appPrms, (const uint8_t *) printVersion, strlen(printVersion));
                    flag_version = false;
                }
                else
                {
                    App_uartTx(appPrms, &gAppUartTxBuffer[0U], numCharsRead);
                }

                flag_info_arr_01 = flag_envio_activo & flag_aviso_asystole;
                flag_info_arr_02 = flag_envio_activo & flag_aviso_fib_vent;
                flag_info_arr_03 = flag_envio_activo & flag_aviso_tach_vent;
                flag_info_arr_04 = flag_envio_activo & flag_aviso_tachy;
                flag_info_arr_05 = flag_envio_activo & flag_aviso_brady;
                flag_info_arr_06 = flag_envio_activo & flag_aviso_cvp_min;
                flag_info_arr_07 = flag_envio_activo & flag_aviso_paired;
                flag_info_arr_08 = flag_envio_activo & flag_aviso_tv_2;
                flag_info_arr_09 = flag_envio_activo & flag_aviso_bigeminy;
                flag_info_arr_10 = flag_envio_activo & flag_aviso_trigeminy;
                flag_info_arr_11 = flag_envio_activo & flag_aviso_missbeat;
                flag_info_arr_12 = flag_envio_activo & flag_aviso_RenT;
                flag_info_arr_13 = flag_envio_activo & flag_aviso_pnc;
                flag_info_arr_14 = flag_envio_activo & flag_aviso_pnp;

                charRecvdCnt++;
            }

            if (flag_relearning)
            {
                flag_trained = true;
                flag_inicio_datos = true;
                flag_learning_1 = true;
                cont_latido = 0;
                cont_init_frec = 0;
                flag_relearning = false;
                tipo_arritmia = 0;
                cuenta_cvp = 0;
            }

            if (flag_trained)
            {
                if (flag_aviso_learn)  // envío solo al detectar punto Q
                {
                    strcpy(printCadena,"L:0\n");
                    //sprintf(printCadena,"L,%i\n",flag_info_arr_01);
                    App_uartTx(appPrms, (const uint8_t *) printCadena, strlen(printCadena));
                    flag_aviso_learn = false;
                }
            }
            else
            {
                //if ((flag_aux_delay) && (flag_info_arr_06 == 1))
                if (flag_aux_delay)
                {
                    if (flag_cvp == 1)
                    {
                        sprintf(printCadena, "B:06,%i\n",cuenta_cvp);
                        App_uartTx(appPrms, (const uint8_t *) printCadena, strlen(printCadena));

                    }
                    else
                    {
                        if (flag_beat_ok)       // cambio de estado en cuenta CVP's
                        {
                            if (tipo_arritmia < 10)
                                sprintf(printCadena, "B:0%i,%i\n",tipo_arritmia,cuenta_cvp);
                            else
                                sprintf(printCadena, "B:%i,%i\n",tipo_arritmia,cuenta_cvp);
                            //   sprintf(trans.buf, "t:%i,%i,%i,%i,%i\n",tipo_arritmia,cont_ritmo_irregular,time_RR_actual,(int)Thr_RR_miss,variable_aux);
                            App_uartTx(appPrms, (const uint8_t *) printCadena, strlen(printCadena));
                            flag_beat_ok = false;
                        }
                    }
                    //  sprintf(printCadena,"T,%i,%i,%i,%i\n",time_RR_actual, variable_aux,flag_info_arr_11,tipo_arritmia);   // borrar
                    // App_uartTx(appPrms, (const uint8_t *) printCadena, strlen(printCadena));            // borrar
                    flag_aux_delay = false;
                }
            }

            if (flag_detecta)       // Indica el tipo de arritmia detectada
            {
                if (tipo_arritmia < 10)
                    sprintf(printCadena, "B:0%i\n",tipo_arritmia);
                else
                    sprintf(printCadena, "B:%i\n",tipo_arritmia);

                App_uartTx(appPrms, (const uint8_t *) printCadena, strlen(printCadena));
                flag_detecta = false;
            }
            /* Error check */
            if(appPrms->rxOverflow == TRUE)
            {
                DebugP_log("Rx overflow occurred!!\r\n");
                break;
            }

            if (charRecvdCnt >= APP_UART_BUFSIZE)
            {
                break;
            }

            TaskP_yield();

        }while(continua);
    }

}
///////////////////////INICIO USART  //////////////////////////////////
static void App_uartTx(UART_AppPrms   *appPrms,
                       const uint8_t  *writeBuf,
                       uint32_t        writeSize)
{
    appPrms->writeBuf           = (const uint8_t *)writeBuf;
    appPrms->writeSizeRemaining = writeSize;
    appPrms->writeCount         = 0U;
    appPrms->txDataSent         = FALSE;

    /* Enable THR interrupt */
    UART_intrEnable(appPrms->baseAddr, UART_INTR_THR);

    SemaphoreP_pend(&appPrms->writeDoneSem, SystemP_WAIT_FOREVER);

    return;
}

static void App_uartRx(UART_AppPrms *appPrms, uint8_t *readBuf)
{
    appPrms->readBuf           = readBuf;
    appPrms->readCount         = 0U;

    /* Enable RHR interrupt */
    UART_intrEnable(appPrms->baseAddr, UART_INTR_RHR_CTI);

    return;
}

static void App_uartUserISR(void *arg)
{
    UART_AppPrms  *appPrms     = (UART_AppPrms *) arg;
    uint32_t       intrType;

    intrType = UART_getIntrIdentityStatus(appPrms->baseAddr);

    /* Check RX FIFO threshold is set */
    if((intrType & UART_INTID_RX_THRES_REACH) == UART_INTID_RX_THRES_REACH)
    {
        uint32_t       readSuccess;
        uint8_t       *readBuf, readChar;

        /* Read all data from RX FIFO */
        readBuf = (uint8_t *)appPrms->readBuf + appPrms->readCount;
        while(1)
        {
            readSuccess = UART_getChar(appPrms->baseAddr, &readChar);
            if(readSuccess == TRUE)
            {
                if(appPrms->readCount >= APP_UART_BUFSIZE)
                {
                    /* Rx buffer overflow */
                    appPrms->rxOverflow = TRUE;
                }
                else
                {
                    /* Store data in buffer */
                    *readBuf = readChar;
                    readBuf++;
                    appPrms->readCount++;
                }
            }
            else
            {
                break;      /* No data left in FIFO */
            }
        }

        if(appPrms->rxOverflow == TRUE)
        {
            /* Stop processing further data */
            UART_intrDisable(appPrms->baseAddr, UART_INTR_RHR_CTI);
        }
    }

    /* Check TX FIFO threshold is set */
    if((intrType & UART_INTID_TX_THRES_REACH) == UART_INTID_TX_THRES_REACH)
    {
        uint32_t       numBytesToTransfer;
        const uint8_t *writeBuf;

        if(appPrms->writeSizeRemaining > 0U)
        {
            numBytesToTransfer = appPrms->writeSizeRemaining;
            if(numBytesToTransfer > appPrms->txTrigLvl)
            {
                /* Write only threshold level of data */
                numBytesToTransfer = appPrms->txTrigLvl;
            }
            appPrms->writeSizeRemaining -= numBytesToTransfer;

            /* Send characters upto FIFO threshold level or until done */
            writeBuf = appPrms->writeBuf + appPrms->writeCount;
            while(numBytesToTransfer != 0U)
            {
                UART_putChar(appPrms->baseAddr, *writeBuf);
                writeBuf++;
                numBytesToTransfer--;
                appPrms->writeCount++;
            }

            if(appPrms->writeSizeRemaining == 0U)
            {
                /* Write complete  - disable TX interrupts */
                appPrms->txDataSent = TRUE;
                UART_intrDisable(appPrms->baseAddr, UART_INTR_THR);
                UART_intr2Enable(appPrms->baseAddr, UART_INTR2_TX_EMPTY);
            }
        }
        else
        {
            /* Disable interrupt */
            UART_intrDisable(appPrms->baseAddr, UART_INTR_THR);
        }
    }

    /* Check if TX FIFO is empty */
    if(appPrms->txDataSent == TRUE)
    {
        intrType = UART_getIntr2Status(appPrms->baseAddr);
        if((intrType & UART_INTR2_TX_EMPTY) != 0U)
        {
            UART_intr2Disable(appPrms->baseAddr, UART_INTR2_TX_EMPTY);
            appPrms->txDataSent = FALSE;
            SemaphoreP_post(&appPrms->writeDoneSem);
        }
    }

    return;
}

static void App_uartInit(UART_AppPrms *appPrms)
{
    int32_t         status;
    HwiP_Params     hwiPrms;

    /* Get UART Instance Base Address */
    appPrms->baseAddr = UART_getBaseAddr(gUartHandle[CONFIG_UART0]);
    DebugP_assert(appPrms->baseAddr != 0U); /* UART baseAddr Invalid!! */
    appPrms->rxTrigLvl          = gUartParams[CONFIG_UART0].rxTrigLvl;
    appPrms->txTrigLvl          = gUartParams[CONFIG_UART0].txTrigLvl;

    /* Reset other variables */
    appPrms->writeBuf           = NULL;
    appPrms->writeSizeRemaining = 0U;
    appPrms->writeCount         = 0U;
    appPrms->txDataSent         = FALSE;
    appPrms->readBuf            = NULL;
    appPrms->readCount          = 0U;
    appPrms->rxOverflow         = FALSE;

    status = SemaphoreP_constructBinary(&appPrms->writeDoneSem, 0);
    DebugP_assert(SystemP_SUCCESS == status);

    /* Register interrupt */
    HwiP_Params_init(&hwiPrms);
    hwiPrms.intNum      = gUartParams[CONFIG_UART0].intrNum;
    hwiPrms.priority    = gUartParams[CONFIG_UART0].intrPriority;
    hwiPrms.callback    = &App_uartUserISR;
    hwiPrms.args        = (void *) appPrms;
    status              = HwiP_construct(&appPrms->hwiObject, &hwiPrms);
    DebugP_assert(status == SystemP_SUCCESS);

    return;
}

static void App_uartDeInit(UART_AppPrms *appPrms)
{
    HwiP_destruct(&appPrms->hwiObject);
    SemaphoreP_destruct(&appPrms->writeDoneSem);

    return;
}
/////////////////////////FIN USART ///////////////////////////////////
//////////////////////INICIO FILTROS ///////////////////////////////////
int32_t FiltroPasaBanda(int32_t data_in)
{
    static int yl1 = 0, yl2 = 0, xl[26], nl = 12;
    static int yh1 = 0, xh[66], nh = 32;
    int yl0;
    int yh0;

    xl[nl] = xl[nl + 13] = data_in;
    yl0 = (yl1 << 1) - yl2 + xl[nl] - (xl[nl + 6] << 1) + xl[nl + 12];
    yl2 = yl1;
    yl1 = yl0;
    yl0 >>= 5;

    if(--nl < 0)
        nl = 12;

    xh[nh] = xh[nh + 33] = yl0;
    yh0 = yh1 + xh[nh] - xh[nh + 32];
    yh1 = yh0;
    if(--nh < 0)
        nh = 32;

    return(xh[nh + 16] - (yh0 >> 5));

}

int32_t Derivativa(int32_t data_in)
{
    int y, i;
    static int x_derv[4];
    int32_t sqr_y;
    /*y = 1/8 (2x( nT) + x( nT - T) - x( nT - 3T) - 2x( nT - 4T))*/
    y = (data_in << 1) + x_derv[3] - x_derv[1] - ( x_derv[0] << 1);
    y >>= 3;
    for (i = 0; i < 3; i++)
        x_derv[i] = x_derv[i + 1];
    x_derv[3] = data_in;

    sqr_y = y * (y >> 2);

    return(sqr_y);
}

int32_t MovingWindowIntegral(int32_t data_in)
{
    static int x[32];
    int64_t ly;
    int32_t y;

    if(++ptr == 32)
        ptr = 0;
    sum_integral -= x[ptr];
    sum_integral += data_in;
    x[ptr] = data_in;
    ly = sum_integral >> 5;
    if(ly > 10000)
        y = 10000;
    else
    y = (int) ly;

    return(y);
}

int32_t DerivadaFuncion (int derivacion)
{
    int y, i;
    static int fx_derv[4];

    /*y = 1/8 (2x( nT) + x( nT - T) - x( nT - 3T) - 2x( nT - 4T))*/
    y = (derivacion << 1) + fx_derv[3] - fx_derv[1] - (fx_derv[0] << 1);
    y >>= 3;
    for (i = 0; i < 3; i++)
        fx_derv[i] = fx_derv[i + 1];
    fx_derv[3] = derivacion;

    return(y);
}
///////////////////////// FIN FILTROS ////////////////////////////////
void EncontrarPuntoR(int dato_in)
{       // función dedicada a encontrar el punto R en cada onda

    int y, i, pendiente_I;
    static int x_derv[4];
    int valor_mayor = 0;
    int  umbral_asc = 12;
    int  umbral_desc = 6;

    contador_muestras++;
    // ***** Calculo derivada de la señal de entrada  ****
    y = (dato_in << 1) + x_derv[3] - x_derv[1] - ( x_derv[0] << 1);
    y >>= 3;
    for (i = 0; i < 3; i++)
        x_derv[i] = x_derv[i + 1];
    x_derv[3] = dato_in;
    pendiente_I = y;

    if (dato_in >= umbral_asc)
    {
        if (pendiente_I > 2)
        {
            if (flag_reset_cta)
            {
                punto_heartrate = contador_muestras;
                flag_reset_cta = false;
                flag_punto_Q = true;
                contador_muestras = 0;
                flag_data_aux  = flag_trained;

                if (!flag_trained)
                    flag_heartbeat = false;
            }
            valor_mayor = MAX(dato_in,valor_pico);
            valor_pico = valor_mayor;
        }
        else
        {
            if (dato_in < valor_pico)
            {
                cta_negativo_S++;
                if (cta_negativo_S == 2)
                {
                    flag_punto_R = true;
                    valor_punto_R = valor_pico;
                }
                else if (cta_negativo_S > 2)
                {
                    if (flag_punto_R)
                        flag_punto_R = false;

                }
            }
        }
    }
    else if (dato_in < umbral_desc)
    {
        valor_pico = 0;
        cta_negativo_S = 0;
        if (abs(pendiente_I)< 5)
            flag_reset_cta = true;

    }
    gradiente = pendiente_I;

}

void ProcesamientoSegmentoRR()
{
    int32_t delta_RR_actual;
    int64_t suma_RR_actual = 0;
    int32_t media_RR_actual = 0;
    int8_t i;
    float   T_RR_actual = 0;    //periodo de RR para referencia (milisegundos)
    float   delta_RR_real = 0;

    float lim_taquicardia = 60000/limite_Tachy;       // [milisegundos]
    float lim_bradicardia = 60000/limite_brady;       // [milisegundos]

    int lim_tachy_int = (int)lim_taquicardia;
    int lim_brady_int = (int)lim_bradicardia;

    int lim_asistole =  limite_asistole * 1000;

    tiempo_RR_1 = ClockP_getTimeUsec();
    delta_RR_actual = (tiempo_RR_1 - tiempo_RR_2);
    delta_RR_real = (float)delta_RR_actual / 1000;
    tiempo_RR_2 = tiempo_RR_1;

    // promedio de últimos 8 pulsos detectados
    array_RR_actual[0] = delta_RR_actual;
    for (i = 8; i > 0; i--)
    {
        suma_RR_actual += array_RR_actual[i-1];
        array_RR_actual[i] = array_RR_actual[i-1];
    }
    media_RR_actual = suma_RR_actual >> 3;
    T_RR_actual = (float)media_RR_actual / 1000;   // periodo en milisegundos

    if (flag_inicio_datos)
    {
        contador_aprendizaje++;
        if (contador_aprendizaje >= 9)
        {
            flag_inicio_datos = false;
            contador_aprendizaje = 0;
            for (i = 8; i > 0; i--)
                array_RR_valido[i] = array_RR_actual[i];

            T_RR_valido = T_RR_actual;   // periodo en milisegundos

            Thr_RR_bajo = T_RR_valido * 0.92;       // define valores iniciales del rango T_RR_valido
            Thr_RR_alto = T_RR_valido * 1.16;
            cont_ritmo_regular = 0;
            cont_ritmo_irregular = 0;
        }
    }
    else    // despues de los primeros 8 latidos analizados
    {
        if ((delta_RR_real >= Thr_RR_bajo) && (delta_RR_real <= Thr_RR_alto))
        {
            cont_ritmo_regular++;
            suma_RR_valido = 0;

            cont_ritmo_irregular = 0;
            if (cont_ritmo_regular == 4)    // después de 4 pulsos en ritmo regular
            {
                flag_arritmia_sinus = false;
                flag_bradicardia = false;
                flag_taquicardia = false;
            }

            for (i = 8; i > 0; i--)
            {
                suma_RR_valido += array_RR_valido[i-1];
                array_RR_valido[i] = array_RR_valido[i-1];
            }
            array_RR_valido[0] = delta_RR_actual;
            if (cont_ritmo_regular > 7)
            {
                media_RR_valido = suma_RR_valido >> 3;
                T_RR_valido = (float)media_RR_valido / 1000;   // periodo en milisegundos

                Thr_RR_bajo = T_RR_valido * 0.90;       // define valores iniciales del rango
                Thr_RR_alto = T_RR_valido * 1.18;
                Thr_RR_miss = T_RR_valido * 1.66;
            }
        }
        else    // si un latido se encuentra fuera del rango normal de entrenamiento...
        {
            cont_ritmo_irregular++;
            cont_ritmo_regular = 0;
            if (cont_ritmo_irregular >= 3)
                flag_arritmia_sinus = true;
            if (cont_ritmo_irregular == 1)
            {
                if ((delta_RR_real > Thr_RR_miss) && (delta_RR_real < lim_asistole) && (flag_cvp < 1))
                    flag_beat_miss = true;
                // no entra en rango aceptable, pero tampoco está en el rango de latido perdido
                else if ((delta_RR_real < Thr_RR_miss) || (delta_RR_real < lim_brady_int))
                    flag_arritmia_sinus = false;
            }
            else if (cont_ritmo_irregular > 2)
            {
                if ((delta_RR_real > lim_brady_int) && (delta_RR_real < lim_asistole) && (flag_cvp < 1))    // DETECCIÓN DE LATID PERDIDO
                {
                    flag_bradicardia = true;
                    flag_beat_miss = false;
                    flag_taquicardia = false;

                }
                else if ((delta_RR_real <= lim_tachy_int) && (flag_cvp < 1))
                {
                    flag_taquicardia = true;
                    flag_bradicardia = false;
                }
                else
                {
                    flag_arritmia_sinus = false;
                    flag_taquicardia = false;
                    flag_bradicardia = false;
                }
            }
        }
    }
    time_RR_actual = (int)delta_RR_real;
    time_RR_valido = (int)T_RR_valido;
    variable_aux = (int) Thr_RR_miss;
}


void DefinicionUmbrales(int valor_R)       // cuando detecta pico R
{
    int pico_actual;

    pico_actual =  valor_R;
    if (flag_learning_1)
    {
        cont_latido++;

        if (cont_latido == 2)
            suma_pico = pico_actual;
        else if (cont_latido > 2)
        {
            umbral_ref = (suma_pico + pico_actual) >> 1;
            cont_latido = 0;
            flag_learning_1 = false;
            flag_actualiza = true;
        }
    }
    else
    {
        if (flag_actualiza)
            flag_actualiza = false;
        else
        {
            if ((pico_actual > ((umbral_ref >> 1) - 10)) && (pico_actual < (2 * umbral_ref)))
            {
                umbral_ref = (umbral_ref >> 3) + (pico_actual >> 3) + (pico_actual >> 2) + (pico_actual >> 1);
                flag_heartbeat = true;
            }
        }
    }
}

///////////////////////////////////INICIO RED NEURONAL//////////////////////////////
int get_neighbors(int data[NUM_POINTS][2], int point[2], float eps, int neighbors[NUM_POINTS][2], Bool visited[NUM_POINTS])
{
    int other_point[2] = {0};
    int index = 0;
    int i=0;
    int delta_abs=0;
    int delta_abs2 =0;

    for (i = 0; i < NUM_POINTS; i++)
    {
        other_point[0] = data[i][0];
        other_point[1] = data[i][1];
        delta_abs = abs(point[1]-other_point[1]);
        delta_abs2 = abs(point[0]-other_point[0]);

        if ((!visited[i]) && (delta_abs <= eps)&& ((delta_abs2 < width_neigh)))
        {
            neighbors[index][0] = data[i][0];
            neighbors[index][1] = data[i][1];
            index++;
        }
    }
    return index;
}

int get_neighbors_2(int data[NUM_POINTS_3][2], int point[2], float eps, int neighbors[NUM_POINTS_3][2])
{
    int other_point[2] = {0};
    int index = 0;
    int i=0;
    for (i = 0; i < NUM_POINTS_3; i++)
    {
        other_point[0] = data[i][0];
        other_point[1] = data[i][1];
        if ((euclidean_distance(point, other_point) <= eps))
        {
            neighbors[index][0] = data[i][0];
            neighbors[index][1] = data[i][1];
            index++;
        }
    }
    return index;
}

int expand_cluster(int data[NUM_POINTS][2],int neighbors[NUM_POINTS][2],float eps,Bool visited[NUM_POINTS],
                   int cluster[NUM_POINTS][2],int len_neighbors)
{
    int i,j;
    int len_neighbor_new=0;
    int len_neighbor_max=0;
    int new_neighbors[NUM_POINTS][2]={0};
    int point_aux[2];


    for(i=0;i<len_neighbors;i++)
    {
        int new_point[2] = {neighbors[i][0],neighbors[i][1]};
        len_neighbor_new = get_neighbors(data,new_point,eps,new_neighbors,visited);
        if (len_neighbor_new > len_neighbor_max)
        {
            len_neighbor_max = len_neighbor_new;
            point_aux[0] = new_point[0];
            point_aux[1] = new_point[1];
        }
    }

    len_neighbor_new = get_neighbors(data,point_aux,eps,cluster,visited);
    for(i=0;i<len_neighbor_new;i++)
    {
        for(j=0;j<NUM_POINTS;j++)
        {
            if ((cluster[i][0]==data[j][0])&&(cluster[i][1]==data[j][1])&&(!visited[j]))
            {
                visited[j] = true;
            //    break;
            }
        }

    }
    return len_neighbor_new;
}

int expand_cluster_2(int data[NUM_POINTS_3][2],int neighbors[NUM_POINTS_3][2],float eps,Bool visited[NUM_POINTS_3],
                     int cluster[NUM_POINTS_3][2],int len_neighbors)
{
    int i,j;
    int len_neighbor_new=0;
    int len_neighbor_max=0;
    int new_neighbors[NUM_POINTS_3][2]={0};
    int point_aux[2];

    for(i=0;i<len_neighbors;i++)
    {
        int new_point[2] = {neighbors[i][0],neighbors[i][1]};
        len_neighbor_new = get_neighbors_2(data,new_point,eps,new_neighbors);
        if (len_neighbor_new > len_neighbor_max)
        {
            len_neighbor_max = len_neighbor_new;
            point_aux[0] = new_point[0];
            point_aux[1] = new_point[1];
        }
    }

    len_neighbor_new = get_neighbors_2(data,point_aux,eps,cluster);
    for(i=0;i<len_neighbor_new;i++)
    {
        for(j=0;j<NUM_POINTS_3;j++)
        {
            if ((cluster[i][0]==data[j][0])&&(cluster[i][1]==data[j][1])&&(!visited[j]))
            {
                visited[j] = true;
            }
        }
    }
    return len_neighbor_new;
}

int dbscan(int data[NUM_POINTS][2], float eps, int minPts, int clusters[NUM_POINTS_2][NUM_POINTS][2],
           int len_clusters[NUM_POINTS_2], int no_clusters[NUM_POINTS][2], int *len_no_clusters, int len)
{
    Bool visited[NUM_POINTS] = {false};
    int index=0;
    int i = 0;
    int no_cluster_aux=0;
    int aux_cont=0;

    for (i = 0; i < len; i++)
    {
        int point[2] = {data[i][0], data[i][1]};
        if (!visited[i])
        {
            int neighbors[NUM_POINTS][2] = {0};
            int len_neighbors = get_neighbors(data, point, eps, neighbors,visited);
            if (len_neighbors >= minPts)
            {
                index =  expand_cluster(data, neighbors,eps, visited, clusters[*len_no_clusters],len_neighbors);
                len_clusters[*len_no_clusters] = index;
                (*len_no_clusters)++;
                aux_cont++;
            }
            else
            {
                no_clusters[no_cluster_aux][0] = point[0];
                no_clusters[no_cluster_aux][1] = point[1];
                (no_cluster_aux)++;
            }
        }
    }
    *len_no_clusters = no_cluster_aux;
    return aux_cont;
}

void dbscan_2(int data_2[NUM_POINTS_3][2], float eps, int minPts, int clusters[NUM_POINTS_3][NUM_POINTS_3][2],
              int len_clusters[NUM_POINTS_3], int no_clusters[NUM_POINTS_3][2], int *len_no_clusters, int len)
{
    Bool visited[NUM_POINTS_3] = {false};
    int index=0;
    int i = 0;
    int no_cluster_aux=0;

    for (i = 0; i < len; i++)
    {
        int point[2] = {data_2[i][0], data_2[i][1]};
        if (!visited[i])
        {
            int neighbors[NUM_POINTS_3][2] = {0};
            int len_neighbors = get_neighbors_2(data_2,point, eps, neighbors);
            if (len_neighbors >= minPts)
            {
                index =  expand_cluster_2(data_2, neighbors,eps, visited, clusters[*len_no_clusters],len_neighbors);
                len_clusters[*len_no_clusters] = index;
                (*len_no_clusters)++;
            }
            else
            {
                no_clusters[no_cluster_aux][0] = point[0];
                no_clusters[no_cluster_aux][1] = point[1];
                (no_cluster_aux)++;
            }
        }
    }
    *len_no_clusters=no_cluster_aux;

}

int find_cluster(int point[2], int clusters[NUM_POINTS_2][NUM_POINTS][2], int len_clusters[NUM_POINTS_2])
{
    int i = 0;
    for (i = 0; i < NUM_POINTS_2; i++)
    {
        if (len_clusters[i] > 0)
        {
            int j = 0;
            for (j = 0; j < len_clusters[i]; j++)
            {
                if ((euclidean_distance(clusters[i][j],point) <= 10))
                {
                    return i;
                }
            }
        }
    }
    return -1;
}

int find_arritmia(int pulso[250][2], int clusters[NUM_POINTS_2][NUM_POINTS][2], int len_clusters[NUM_POINTS_2],
                  int long_clusters, int long_pulso)
{
    int aux_cont=0;
    int resultado=0;
    int aux_cont_2 = 0;
    int aux_cont_3 = 0;
    int lim_int = 0;

    for (int i = 0; i < long_pulso; i++)
    {
        if (pulso[i][1] > 5)
            aux_cont_3++;
    }
    for (int i = 0; i < long_clusters; i++)
    {
        if (clusters[i][1][1] > 5)
        {
            int min_cluster0 = 20000;
            int max_cluster0 = 0;
            int min_cluster1 = 20000;
            int max_cluster1 = 0;
            aux_cont_2++;
            for (int j = 0; j < len_clusters[i]; j++)
            {
                if (clusters[i][j][0] > max_cluster0)
                {
                    max_cluster0 = clusters[i][j][0];
                    if (max_cluster0 > lim_int)
                        lim_int = max_cluster0;
                 }
                if (clusters[i][j][0] < min_cluster0)
                    min_cluster0 = clusters[i][j][0];

                if (clusters[i][j][1] > max_cluster1)
                    max_cluster1 = clusters[i][j][1];

                if (clusters[i][j][1] < min_cluster1)
                    min_cluster1 = clusters[i][j][1];
            }

            for (int j = 0; j < long_pulso; j++)
            {
                if ((pulso[j][0] < max_cluster0) && (pulso[j][0] > min_cluster0) &&
                        (pulso [j][1] < max_cluster1) && (pulso[j][1] > min_cluster1))
                {
                    aux_cont++;
                    break;
                }
            }
        }
    }

     resultado = (aux_cont_2 - aux_cont);
     if (resultado <= 1)
         return 0;

     return 1;

}

float euclidean_distance(int point1[2], int point2[2])
{
    return sqrt(pow(point1[0] - point2[0], 2) + pow(point1[1] - point2[1], 2));
}


/////////////////////////////////////FIN RED NEURONAL//////////////////////////////

//////////////////////////////////////INICIO USAR1//////////////////////////////
void uart_echo_write_callback(UART_Handle handle, UART_Transaction *trans)
{
    DebugP_assertNoLog(UART_TRANSFER_STATUS_SUCCESS == trans->status);
    gNumBytesWritten = trans->count;
    SemaphoreP_post(&gUartWriteDoneSem);

    return;
}

void uart_echo_read_callback(UART_Handle handle, UART_Transaction *trans)
{
    DebugP_assertNoLog(UART_TRANSFER_STATUS_SUCCESS == trans->status);
    gNumBytesRead = trans->count;
    SemaphoreP_post(&gUartReadDoneSem);

    return;
}
///////////////////////////////////////FIN USAR1//////////////////////////////
