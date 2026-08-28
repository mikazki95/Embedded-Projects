/*
 *  Copyright (C) 2021 Texas Instruments Incorporated
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

/*      DETECCI�N DE ARRITMIAS
 *
 *  Se realizan modificaciones necesarias para implementar el c�digo de detecci�n de arritmias
 *  para correr el c�digo en la memoria flash OSPI con la finalidad de correlo sin OS.
 *
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


/* Prioridad de la tarea, stack, tama�o del stack y objetos de tareas, estos DEBEN SER globales */
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

// definicion de nueva UART_0
/** \brief UART RX buffer where received data is put in ISR */
static uint8_t              gAppUartRxBuffer[APP_UART_BUFSIZE];
/** \brief UART TX buffer where TX data is put in Task context */
static uint8_t              gAppUartTxBuffer[APP_UART_BUFSIZE];
/** \brief UART app object */
static UART_AppPrms         gAppPrms;

// definicion de nueva UART_1
/** \brief UART RX buffer where received data is put in ISR */
static uint8_t              gAppUartRx0Buffer[APP_UART_BUFSIZE];
/** \brief UART TX0 buffer where TX data is put in Task context */
static uint8_t              gAppUartTx0Buffer[APP_UART_BUFSIZE];
/** \brief UART app object */
static UART_AppPrms         gAppPrms0;

/* Static Function Declarations */
static void App_uartTx(UART_AppPrms   *appPrms,
                       const uint8_t  *writeBuf,
                       uint32_t        writeSize);
static void App_uartRx(UART_AppPrms *appPrms, uint8_t *readBuf);
static void App_uartRxDisable(UART_AppPrms *appPrms, uint8_t *readBuf);

static void App_uartUserISR(void *arg);
static void App_uartInit(UART_AppPrms *appPrms);
static void App_uartDeInit(UART_AppPrms *appPrms);

////////////////////////////////////////////////////
// Se da de alta la UART0
static void App_uartTx0(UART_AppPrms   *appPrms0,
                       const uint8_t  *writeBuf0,
                       uint32_t        writeSize0);
static void App_uartRx0(UART_AppPrms *appPrms0, uint8_t *readBuf0);
static void App_uartUserISR0(void *arg0);
static void App_uartInit0(UART_AppPrms *appPrms0);
static void App_uartDeInit0(UART_AppPrms *appPrms0);

UART_AppPrms            *appPrms0 = &gAppPrms0;
UART_AppPrms            *appPrms = &gAppPrms;
const char              *printStr       = "Detectar Arritmias..\r\n";
const char              *printVersion   = "I:ARRITMIA,1.2\r\n";
const char              *Ok_Rx          = "Rx correcta\n";
const char              *Error_Rx       = "Error Rx\n";
char                    *printCadena    = "0";
int                     neighbors_G[NUM_POINTS][2] = {0};
int                     aux_int_flag=0;

/*  Funciones principales      */
//void generarTareas(void *args);
void tarea1(void *args);
//void tarea3(void *args);     // Tarea para recepci�n de comandos


void generarTareas(void *args)
{
    int32_t status;
    //int32_t statusT3;

    /* Open drivers to open the UART driver for console */
    Drivers_open();
    Board_driversOpen();

    DebugP_log("Inicio Arritmias !\r\n");
    /* Init App Prms */
    App_uartInit(appPrms);
    /* Init App Prms0 */
    App_uartInit0(appPrms0);//Inicializacion de UART0

    /* Send entry string and wait for Tx completion */
    App_uartTx(appPrms, (const uint8_t *) printStr, strlen(printStr));
    App_uartTx(appPrms0, (const uint8_t *) printVersion, strlen(printVersion));
    //UART_INTR_THR
    /* Send entry string and wait for Tx completion */
    App_uartTx(appPrms0, (const uint8_t *) printStr, strlen(printStr));

    /* Crear la tarea que recibe, procesa datos y detecta arrimias */
    TaskP_Params taskParams1;                                                    // Par�metros para generar la tarea
    TaskP_Params_init(&taskParams1);                                             // Iniciar par�metros. Recomendado por la API
    taskParams1.name = "TAREA_01";                                               // Nombre para Debug
    taskParams1.stackSize = IPC_RPMESSAFE_TASK_STACK_SIZE;                       // 8KiB
    taskParams1.stack = gIpcTaskStack;                                           // Puntero al stack (*uint8_t)
    taskParams1.priority = IPC_RPMESSAFE_TASK_PRI;                               // Prioridad de la tarea
    taskParams1.args = &gIpcRecvMsg_Args;                                       // Ac� estamos pasando el RPMessage_Object como argumento de la funci�n
    taskParams1.taskMain = tarea1;                                               // Funci�n que implementa la tarea

    /* Construir la tarea */
    status = TaskP_construct(&gIpcTask, &taskParams1);
    DebugP_assert(status == SystemP_SUCCESS);


    /* Tarea para establecer la comunicaci�n con la Raspberry */
    /*
    TaskP_Params taskParams3;                                           // Par�metros para generar la tarea
    TaskP_Params_init(&taskParams3);                                    // Iniciar par�metros. Recomendado por la API
    taskParams3.name = "TAREA_03";                                      // Nombre para Debug
    taskParams3.stackSize = TX_RX_APP_TASK_STACK_SIZE;                  // 8KiB
    taskParams3.stack = gTxRxAppStack;                                  // Puntero al stack (*uint8_t)
    taskParams3.priority = TX_RX_APP_TASK_PRI;                          // Prioridad de la tarea
    taskParams3.args = &gTxRxApp_args;
    taskParams3.taskMain = tarea3;                                      // Funci�n que implementa la tarea

    // * Construir la tarea // /
    statusT3 = TaskP_construct(&gTxRxAppTask, &taskParams3);
    DebugP_assert(statusT3 == SystemP_SUCCESS);
*/

    Board_driversClose();
    Drivers_close();
}

void tarea1(void *args)//Red
{
    /////////////////////////////Inicio Variables
    uint32_t                 numCharsRead, i, charRecvdCnt, num_Chars_Cmd, charRecvdCnt0;
    uint8_t         BuffCmdApp[APP_UART_BUFSIZE] = {0};
    char            parametro[APP_UART_BUFSIZE] = {0};

    //Bool            continua                = true;
    Bool            flag_version            = false;
    Bool            flag_ping               = false;
    Bool            flag_Rx_start           = false;

    uint8_t         flag_activar            = 0;
    uint8_t         flag_activar_m          = 0;
    uint8_t         flag_aviso_asystole     = 0;
    uint8_t         flag_aviso_fib_vent     = 0;
    uint8_t         flag_aviso_tach_vent    = 0;
    uint8_t         flag_aviso_tachy        = 0;
    uint8_t         flag_aviso_brady        = 0;
    uint8_t         flag_aviso_cvp_min      = 0;
    uint8_t         flag_aviso_paired       = 0;
    uint8_t         flag_aviso_tv_2         = 0;
    uint8_t         flag_aviso_bigeminy     = 0;
    uint8_t         flag_aviso_trigeminy    = 0;
    uint8_t         flag_aviso_missbeat     = 0;
    uint8_t         flag_aviso_RenT         = 0;
    uint8_t         flag_aviso_pnp          = 0;
    uint8_t         flag_aviso_pnc          = 0;
    uint8_t         flag_envio_activo       = 0;
    uint8_t         cont_Rx                 = 0;
    uint8_t         aux_cont_Rx             = 0;
    static uint8_t  Aux_In[APP_UART_BUFSIZE];
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
    //int tiempo_asistole =  limite_asistole * 250;   // numero de muestras
    int conta_fib = 0;
    int tiempo_fibr = 1000;

    uint16_t cont_raw_cero = 0;
    int8_t index_p_arritmia = 0;

    Bool flag_vent_tach = false;
    Bool flag_desborde = false;
    Bool flag_asistole = false;
    //Bool flag_no_pulso = false;
    Bool flag_normal_beat = true;
    Bool flag_conta_fib = false;
    Bool flag_arrit_pnp = false;
    Bool flag_arrit_pnc = false;
    Bool flag_dato_1 = false;

    int segs_asistole = 0;
    float  lim_TachVent = 15000/limite_TachVent;
    int    lim_TachVent_int = (int)lim_TachVent;
    int    derivada_d2_ant = 0;
    ///////////////////////////fin de variables

    Drivers_open();
    Board_driversOpen();

    /* Init Rx Prms and Enable Rx Interrupt */
    App_uartRx(appPrms, &gAppUartRxBuffer[0U]);
    /* Init Rx Prms and Enable Rx Interrupt */
    App_uartRx(appPrms0, &gAppUartRx0Buffer[0U]);

    charRecvdCnt = 0U;
    charRecvdCnt0 = 0U;
    /* Echo whatever is read */

    while(1)
    {
        //aux_int_flag=appPrms->baseAddr;
        ////////////////////////////////////////////Inicio de Tarea de comandos //////////////////////////////
        if(appPrms0->readCount > 0U)
        {
            uintptr_t   key;

            // Enter Critical section, disable interrupt //
            key = HwiP_disable();

            // copy data from RX buf to TX buf //
            num_Chars_Cmd = appPrms0->readCount;
            for(i = 0U; i < num_Chars_Cmd; i++)
            {
                BuffCmdApp[i] = gAppUartRx0Buffer[i];
                gAppUartTx0Buffer[i] = gAppUartRx0Buffer[i];
            }
            appPrms0->readCount = 0U; // Reset variable so that RX can start from first /

            // End critical section //
            HwiP_restore(key);
///////////////////////////////comandos

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
                //     }
                if ((BuffCmdApp[0] == '0') && (BuffCmdApp[1] == '1'))   // alarma Asístole
                {
                    flag_aviso_asystole = flag_activar;
                    if ((BuffCmdApp[3] >= 48) && (BuffCmdApp[3] <= 57))
                    {
                        for (int j = 0; j < 2; j++)
                            parametro[j] = BuffCmdApp[j+3];

                        limite_asistole = atoi(parametro);
                        //tiempo_asistole = limite_asistole * 250;
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
            }

            for (int j = 0; j < 4; j++)
                parametro[j] = 0;

            for (i = 0;i < num_Chars_Cmd;i++)
                BuffCmdApp[i] = 0;

            //App_uartTx(appPrms0,((const uint8_t *) flag_version), 1);

            if (flag_version)
            {
                App_uartTx(appPrms0, (const uint8_t *) printVersion, strlen(printVersion));
                flag_version = false;
            }
            else
            {
                App_uartTx(appPrms0, &gAppUartTx0Buffer[0U], num_Chars_Cmd);

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

            // Echo the characters received and wait for Tx completion //
            //App_uartTx(appPrms0, &gAppUartTxBuffer[0U], num_Chars_Cmd);

            charRecvdCnt0++;
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
            /*if (flag_aviso_learn)  // envío solo al detectar punto Q
            {
                //strcpy(printCadena,"L:0\n");
                sprintf(printCadena,"L,%i\n",flag_info_arr_01);
                App_uartTx(appPrms0, (const uint8_t *) printCadena, strlen(printCadena));
                flag_aviso_learn = false;
            }*/
        }
        else
        {
            //if ((flag_aux_delay) && (flag_info_arr_06 == 1))
            if (flag_aux_delay)
            {
                if (flag_cvp == 1)
                {
                    sprintf(printCadena, "B:06,%i\n",cuenta_cvp);
                    App_uartTx(appPrms0, (const uint8_t *) printCadena, strlen(printCadena));

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
                        App_uartTx(appPrms0, (const uint8_t *) printCadena, strlen(printCadena));
                        flag_beat_ok = false;
                    }
                }
                //  sprintf(printCadena,"T,%i,%i,%i,%i\n",time_RR_actual, variable_aux,flag_info_arr_11,tipo_arritmia);   // borrar
                // App_uartTx(appPrms0, (const uint8_t *) printCadena, strlen(printCadena));            // borrar
                flag_aux_delay = false;
            }
        }

        if (flag_detecta)       // Indica el tipo de arritmia detectada
        {
            if (tipo_arritmia < 10)
                sprintf(printCadena, "B:0%i\n",tipo_arritmia);
            else
                sprintf(printCadena, "B:%i\n",tipo_arritmia);

            App_uartTx(appPrms0, (const uint8_t *) printCadena, strlen(printCadena));
            flag_detecta = false;
        }


        // Error check //
        if(appPrms0->rxOverflow == TRUE)
        {
           DebugP_log("Rx overflow occurred aPPPrms0!!\r\n");
           App_uartTx(appPrms0, (const uint8_t *) Error_Rx, strlen(Error_Rx));
           break;
        }

        if (charRecvdCnt0 >= APP_UART_BUFSIZE)
        {
            charRecvdCnt0 = 0;
            //App_uartTx(appPrms0, (const uint8_t *) printExitStr, strlen(printExitStr));

            //break;
        }
////////////////////////////////////////////Fin de Tarea de comandos //////////////////////////////
//*/
////////////////////////////Fin recepcion de datos/////////////////////////////////////////
        if(appPrms->readCount > 0U)
        {
            //sprintf(printCadena,"Data_in\n");
            //App_uartTx(appPrms0, (const uint8_t *) printCadena, strlen(printCadena));

            uintptr_t   key;

            // Enter Critical section, disable interrupt //
            key = HwiP_disable();

            // copy data from RX buf to TX buf //
            numCharsRead = appPrms->readCount;
            for(i = 0U; i < numCharsRead; i++)
            {
                gAppUartTxBuffer[i] = gAppUartRxBuffer[i];
                if((gAppUartRxBuffer[i]== 0xA5)&&(flag_Rx_start == false ))
                {
                    flag_Rx_start= true;
                    cont_Rx = i;
                    aux_cont_Rx = 0;
                }
            }
            appPrms->readCount = 0U; /* Reset variable so that RX can start from first */

            // End critical section //
            HwiP_restore(key);

            if(flag_Rx_start == true)
            {

                for(i = cont_Rx; i < numCharsRead; i++)
                {
                    Aux_In[aux_cont_Rx] = gAppUartRxBuffer[i];
                    aux_cont_Rx++;
                    if(aux_cont_Rx>11)
                    {
                        if((Aux_In[0] == 0xA5)&&(Aux_In[5] == 0x5A)&& (Aux_In[10] == 0xAA))
                        {
                            //App_uartTx(appPrms0, (const uint8_t *) Ok_Rx, strlen(Ok_Rx));
                            aux_raw_d1 =((Aux_In[1]<<24)|(Aux_In[2]<<16)|(Aux_In[3]<<8)|(Aux_In[4]));
                            aux_raw_d2 =((Aux_In[6]<<24)|(Aux_In[7]<<16)|(Aux_In[8]<<8)|(Aux_In[9]));
                            pulso_marcapasos = Aux_In[11];        // señal de marcapasos
                            if (aux_raw_d1 < 1500)
                                dato_raw_d1 = aux_raw_d1;
                            if (aux_raw_d2 < 1500)
                                dato_raw_d2 = aux_raw_d2;
                            if ((pulso_marcapasos == 0x55) && ((flag_info_arr_13 == 1) || (flag_info_arr_14 == 1)))   // cambiar 85 = 0X55
                            {
                                flag_detec_mkpasos = true;
                                cta_mkpasos = 0;
                                flag_tipo_pulso = true;
                                cuenta_pulso_mk = 0;
                            }
                            flag_dato_1 = true;
                            flag_dato_rdy = true;
                        }

                        /*else
                        {
                            App_uartTx(appPrms0, (const uint8_t *) Error_Rx, strlen(Error_Rx));

                        }
                        //*/
                        //App_uartTx(appPrms0, &Aux_In[0U], aux_cont_Rx);

                        flag_Rx_start=false;
                        aux_cont_Rx = 0;
                        cont_Rx=0;
                    }
                }
            }

            // Echo the characters received and wait for Tx completion //
            App_uartTx(appPrms, &gAppUartTxBuffer[0U], numCharsRead);
            //App_uartTx(appPrms0, &gAppUartTxBuffer[0U], numCharsRead);

            charRecvdCnt++;
        }
////////////////////////////Fin recepcion de datos/////////////////////////////////////////
/////////////////////////Inicio de analisis de datos/////////////////////////////////////////
        if(flag_dato_1== true)
        {
            
            //sprintf(printCadena,"Data_out\n");
            //App_uartTx(appPrms0, (const uint8_t *) printCadena, strlen(printCadena));
            //aux_int_flag=(int)(appPrms->baseAddr, UART_INTR_RHR_CTI);
            //sprintf(printCadena,"estado int= %i\n",aux_int_flag);
            //App_uartTx(appPrms0, (const uint8_t *) printCadena, strlen(printCadena));

            flag_dato_1=false;
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
            //   **** Detecci�n QRS  ***
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
            if (flag_trained)
            {
                if (flag_aviso_learn)  // envío solo al detectar punto Q
                {
                    //strcpy(printCadena,"L:0\n");
                    sprintf(printCadena,"L,%i\n",contador_tarea_2);
                    App_uartTx(appPrms0, (const uint8_t *) printCadena, strlen(printCadena));
                    flag_aviso_learn = false;
                    wtc_pulsos++;
                    if (wtc_pulsos>NUM_Pulsos_Max)// en caso de que se trabe el entrenamiento revisar electrodos 
                    {
                        flag_trained = true;
                        flag_inicio_datos = true;
                        flag_learning_1 = true;
                        cont_latido = 0;
                        cont_init_frec = 0;
                        flag_relearning = false;
                        tipo_arritmia = 0;
                        cuenta_cvp = 0;
                        //contador_tarea_2 = 0; verificar si se necesita
                        wtc_pulsos = 0;
                    }
                    
                }
            }

            point_in[0] = contador_muestras;
            point_in[1] = integral_ecg_d1 >> 5;     //  */

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
                                    num_clusters_1 = dbscan(data_pre, eps, minPts, clusters, len_clusters, no_clusters, &len_no_clusters,NUM_POINTS); /// debug_cluster
                                    dbscan_2(dato_frec, eps_3, minPts_3, clusters_3, len_clusters_3, no_clusters_3, &len_no_clusters_3,NUM_POINTS_3);
                                    contador_tarea_2 = 0;
                                    wtc_pulsos = 0;
                                    flag_trained = false;
                                    k = cont_task_3 + 1;
                                    flag_training_ready = true;
                                }
                            }
                            for (int k = 0; k< 250; k++)
                            {
                                array_cvp [k][0] = 0;
                                array_cvp [k][1] = 0;
                            }
                            cont_ok = 0;
                            cuenta_cvp = 0;
                            index_p_arritmia = 0;
                            flag_RenT_RV = false;
                            flag_vent_tach = false;
                            tipo_arritmia = NORMAL;  //0;
                            flag_asistole = false;
                        //    flag_no_pulso = false;
                            flag_normal_beat = true;
                            cont_InitBeat = 0;
                        }
                        cont_task_3 = 0;
                    }
                }       // fin de entrenamiento
                else
                {
                    
                    //sprintf(printCadena,"Data_search0\n");
                    //App_uartTx(appPrms0, (const uint8_t *) printCadena, strlen(printCadena));
                    if ((flag_info_arr_13 == 1) || (flag_info_arr_14 == 1))
                        flag_aviso_mk = true;
                    else
                        flag_aviso_mk = false;

                    if ((flag_aviso_mk) && (flag_cvp == 3))
                        cuenta_pulso_mk++;

                    if (cuenta_pulso_mk > SPS_asistole)
                        cuenta_pulso_mk = SPS_asistole;

                    int point_to_search[2];
                    point_to_search[0] = point_in[0];
                    point_to_search[1] = point_in[1];
                    //sprintf(printCadena,"Data_search1\n");
                    //App_uartTx(appPrms0, (const uint8_t *) printCadena, strlen(printCadena));
                    
                    conta_fib++;
                    if (cuenta_edge > 2)
                        flag_conta_fib = true;
                    else
                    {
                        if (contador_muestras == 0)
                            conta_fib = 0;
                    }

                    if ((conta_fib > tiempo_fibr) && (flag_conta_fib))
                    {
                        flag_cvp = 2;
                        if (flag_info_arr_02 == 1)
                        {
                            tipo_arritmia = VENT_FIBR;           // fibrilaci�n ventricular
                            cont_ok = 6;
                        }
                        conta_fib = tiempo_fibr;
                    }

                    if (dato_raw_d2 == 0)
                    {
                        cont_raw_cero++;
                        if (cont_raw_cero >= 250)
                        {
                            cuenta_edge = 0;
                            cont_raw_cero = 0;
                            segs_asistole++;
                        }
                    }
                    else
                    {
                        cont_raw_cero = 0;
                        segs_asistole = 0;
                    }

                    if (flag_asistole)
                    {
                    //    punto_heartrate = SPS_asistole;
                        punto_heartrate = 1000;
                        flag_desborde = true;
                        flag_asistole = false;
                    }
                    pulso[contador_muestras][0]= point_in[0];    //point_to_search[0];
                    pulso[contador_muestras][1]= point_in[1];   //point_to_search[1];

                    if ((derivada_raw_d2 < -50) && (dato_raw_d2 < 0))
                        flag_cvp_mk = true;

                    if (derivada_raw_d2 > 70)
                    {
                        flag_cvp_mk = false;
                        flag_detec_mkpasos = false;
                    }
                    if ((flag_cvp_mk) && (derivada_raw_d2 > 10))
                    {
                        if (derivada_raw_d2 < (derivada_d2_ant + 15))
                            flag_tipo_pulso = true;

                    }

                    derivada_d2_ant = derivada_raw_d2;

                    if (contador_muestras == 40)
                        flag_cvp_mk = false;

                    if (flag_punto_Q)
                    {
                        cont_InitBeat++;
                        if (cont_InitBeat > 32700)
                            cont_InitBeat = 4;

                        point_to_search[0] = punto_heartrate;
                        pulso[punto_heartrate][0]= point_to_search[0];
                        flag_punto_Q = false;

                        if (flag_desborde)      // desborde del contador por detecci�n de asistole
                        {
                            if (punto_heartrate < 375)
                                cont_hr++;
                            else
                                cont_hr = 0;

                            if (cont_hr >= 5)
                            {
                                cont_hr = 0;
                                flag_desborde = false;
                            }
                            flag_cvp = 2;
                            index_p_arritmia = 0;
                        }
                        else
                        {
                            flag_cvp = find_arritmia(pulso,clusters,len_clusters,num_clusters_1,punto_heartrate); //debug_cluster

                            if (flag_tipo_pulso)
                                flag_cvp = 3;

                            flag_tipo_pulso = false;

                            if (cont_InitBeat < 3)
                                flag_cvp = 0;

                        }
                        //sprintf(printCadena,"Data_search2\n");
                        //App_uartTx(appPrms0, (const uint8_t *) printCadena, strlen(printCadena));
                    
                        if (flag_cvp < 2)
                            array_cvp[cont_0][0]= flag_cvp;
                        else
                            array_cvp[cont_0][0]= 0;

                        array_cvp[cont_0][1]= punto_heartrate;
                        valor_hr_k_1 = valor_hr_k_0;
                        valor_hr_k_0 = punto_heartrate;
                        cont_0++;

                        cont_1 += punto_heartrate;

                        if ((cont_1 > 15000) || (cont_0 >= 250))
                        {
                            cont_0 = 0;
                            cont_1 = 1;
                        }
                        int cont_3 = 0;
                        int x_a = 0;

                        promedio_hr = 0;
                        cuenta_cvp = 0;

                        for (x_a = 0; x_a < 250; x_a++)
                        {
                            if ((flag_cvp == 2) || (flag_detec_mkpasos))    //(flag_cvp == 3) ||
                                array_cvp[x_a][0] = 0;

                            cuenta_cvp += array_cvp[x_a][0];        // ******** CVP/min
                            cont_3 += array_cvp[x_a][1];

                            if (cont_3 > 15000)
                                break;
                        }
                        promedio_hr = time_RR_valido/4;

                        if (!flag_arritmia_sinus)
                        {
                            if (flag_cvp == 1)
                            {
                                flag_arritmia = true;
                                flag_normal_beat = false;
                                flag_beat_ok = false;
                                cont_beat = 0;
                            }
                            else if (flag_cvp == 0)
                            {
                                cont_beat++;
                                if (cont_beat > 3)
                                {
                                    if (flag_beat_miss)
                                    {
                                        if (flag_info_arr_11 == 1)
                                            tipo_arritmia = MISS_BEAT;

                                        flag_beat_miss = false;
                                        cont_ok = 0;
                                    }
                                    cont_beat = 0;
                                }
                            }
                        }
                        else
                        {
                            cont_beat++;
                            if (cont_beat > 3)
                            {
                                if (flag_bradicardia)
                                {
                                    if (flag_cvp == 0)
                                    {
                                        if (flag_info_arr_05 == 1)
                                            tipo_arritmia = BRADY;
                                    }
                                    else
                                        flag_arritmia_sinus = false;

                                    flag_bradicardia = false;
                                }
                                if (flag_taquicardia)
                                {
                                    if (flag_info_arr_04 == 1)
                                        tipo_arritmia = TACHY;

                                    flag_taquicardia = false;
                                }
                                cont_beat = 0;
                            }
                        }

                        if (flag_arritmia)
                        {
                            pulsos_arritmia[index_p_arritmia][0] = flag_cvp;
                            pulsos_arritmia[index_p_arritmia][1] = punto_heartrate;
                            index_p_arritmia++;
                            fin_array += punto_heartrate;

                            if (index_p_arritmia == 1)
                            {
                                if (valor_hr_k_1 < ((promedio_hr / 3) + 13))
                                {
                                    if (valor_hr_k_0 > (promedio_hr * 5/4))
                                    {
                                        fin_array = 0;
                                        index_p_arritmia = 0;
                                        flag_arritmia = false;
                                        if (flag_info_arr_12 == 1)
                                            tipo_arritmia = R_EN_T;

                                        cont_ok = 0;
                                        flag_RenT_RV = false;
                                    }
                                    else if (valor_hr_k_0 < promedio_hr/3)
                                        flag_RenT_RV = true;
                                }
                            }   // fin index_p_arritmia =1
                            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                            if (index_p_arritmia == 2)
                            {
                                if ((valor_hr_k_0 > (promedio_hr * 5/4)) && (flag_RenT_RV))
                                {
                                    fin_array = 0;
                                    index_p_arritmia = 0;
                                    flag_arritmia = false;
                                    if (flag_info_arr_12 == 1)
                                        tipo_arritmia = R_EN_T;

                                    cont_ok = 0;
                                    flag_RenT_RV = false;
                                }
                            }

                            if (index_p_arritmia == 6)       // PAIR & TV > 2
                                    {
                                int k_a = 0;
                                int flag_a = 0;
                                bool flag_evaluar = false;
                                bool flag_taq = true;

                                if (punto_heartrate > lim_TachVent_int)
                                    flag_taq = false;

                                for(k_a = 0;k_a <= 6;k_a++)
                                {
                                    flag_a += pulsos_arritmia[k_a][0];

                                    if (flag_a < k_a)
                                        flag_evaluar = true;

                                    if ((flag_evaluar)&&(pulsos_arritmia[k_a][0]==1))
                                        break;

                                    if(k_a==6)
                                    {
                                        if ((flag_evaluar) && (pulsos_arritmia[k_a][1] < (promedio_hr+13))
                                                && (pulsos_arritmia[k_a][1] > (promedio_hr-13)) && (!flag_vent_tach))
                                        {
                                            if (flag_a == 2)
                                            {
                                                if (flag_info_arr_07 == 1)
                                                    tipo_arritmia = PAIRED_CVP;

                                            }
                                            else if (flag_a > 2)
                                            {
                                                if (flag_info_arr_08 == 1)
                                                    tipo_arritmia = TV_2;

                                            }

                                            fin_array = 0;
                                            index_p_arritmia = 0;
                                            flag_arritmia = false;
                                            cont_ok = 0;
                                            flag_RenT_RV = false;
                                        }
                                        else
                                        {
                                            if (flag_taq)
                                            {
                                                fin_array = 0;
                                                index_p_arritmia = 0;
                                                flag_arritmia = false;
                                                flag_vent_tach = true;
                                                if (flag_info_arr_03 == 1)
                                                    tipo_arritmia = VENT_TACH;

                                                cont_ok = 0;
                                                flag_RenT_RV = false;
                                                flag_normal_beat = false;
                                            }
                                        }
                                    }
                                }
                                    } // fin index_p_arritmia =6
                                    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                            if (index_p_arritmia == 8)
                            {
                                int k_a = 0;
                                int flag_a;

                                for(k_a = 0;k_a <= 7;k_a++)
                                {
                                    flag_a = (k_a) & 0x0001;
                                    if (flag_a == pulsos_arritmia[k_a][0])
                                        break;

                                    if (k_a == 7)
                                    {
                                        fin_array = 0;
                                        index_p_arritmia = 0;
                                        flag_arritmia = false;
                                        if (flag_info_arr_09 == 1)
                                            tipo_arritmia = BIGEMINY;

                                        cont_ok = 0;
                                        flag_RenT_RV = false;
                                    }
                                }
                            }
                            if (index_p_arritmia == 10 )      // trigeminy
                            {
                                int k_a = 0;
                                int flag_a;

                                for(k_a = 0;k_a <= 9;k_a++)
                                {
                                    flag_a = (k_a)%3;
                                    if(flag_a == 0)
                                    {
                                        if ((pulsos_arritmia[k_a][0]) == 0)
                                            break;
                                    }
                                    else
                                    {
                                        if ((pulsos_arritmia[k_a][0]) == 1)
                                            break;
                                    }
                                    if(k_a == 9)
                                    {
                                        fin_array = 0;
                                        index_p_arritmia = 0;
                                        flag_arritmia = false;
                                        if (flag_info_arr_10 == 1)
                                            tipo_arritmia = TRIGEMINY;

                                        cont_ok = 0;
                                        flag_RenT_RV = false;
                                    }
                                }
                            }
                            // *****************  finalizaci�n de b�squeda de patrones v�lidos de arritmias
                            if (index_p_arritmia >=  12)
                            {
                                fin_array = 0;
                                index_p_arritmia = 0;
                                flag_arritmia = false;
                                //   tipo_arritmia = CVP_pMIN;
                                cont_ok = 0;
                                flag_RenT_RV = false;
                            }

                            if (fin_array > 7500)
                            {
                                fin_array = 0;
                                index_p_arritmia = 0;
                                flag_arritmia = false;
                                cont_ok = 0;
                                flag_RenT_RV = false;
                            }
                        }       // fin flag_arritmia
                        else
                        {
                            if ((flag_cvp == 0) && (!flag_arritmia_sinus))
                                cont_ok++;

                            if (cont_ok >= 8)
                            {
                                cont_ok = 0;
                                index_p_arritmia = 0;
                                flag_RenT_RV = false;
                                flag_vent_tach = false;
                                tipo_arritmia = NORMAL;
                                flag_asistole = false;
                          //      flag_no_pulso = false;
                                flag_conta_fib = false;
                                flag_normal_beat = true;
                                cta_mkpasos = 0;
                                flag_arrit_pnp = false;
                            }

                            if (flag_normal_beat)
                            {
                                if (cuenta_cvp_ant != cuenta_cvp)
                                    flag_beat_ok = true;

                                cuenta_cvp_ant = cuenta_cvp;
                            }
                        }
                        if ((!flag_arrit_pnc) && (!flag_arrit_pnp) && (flag_cvp == 3) && (!flag_arritmia) && (!flag_arritmia_sinus))
                            tipo_arritmia = NORMAL;

                        if ((flag_info_arr_13 == 1) && (flag_arrit_pnp) && (flag_cvp == 3))
                        {
                            tipo_arritmia = PNP;
                            flag_arrit_pnp = false;
                        }

                        if ((flag_info_arr_14 == 1) && (flag_arrit_pnc) && (flag_cvp == 3))
                        {
                            tipo_arritmia = PNC;
                            flag_arrit_pnc = false;
                        }
                    }       // fin flag_punto_Q
                    //sprintf(printCadena,"Data_search3\n");
                    //App_uartTx(appPrms0, (const uint8_t *) printCadena, strlen(printCadena));
                    
                    if ((cuenta_pulso_mk > (promedio_hr * 7 / 4)) && (flag_cvp == 3))
                        flag_arrit_pnp = true;

                    if ((flag_detec_mkpasos) && (flag_cvp == 3))
                    {
                        cta_mkpasos++;

                        if ((cta_mkpasos == 75) && (flag_detec_mkpasos))     // Equivale a 300 ms
                        {
                            cta_mkpasos = 75;
                            flag_arrit_pnc = true;
                            flag_detec_mkpasos = false;
                        }
                    }

                    if (segs_asistole >= limite_asistole)
                    {
                        if (flag_info_arr_01 == 1)
                        {
                            tipo_arritmia = ASYSTOLE;
                            cont_ok = 7;
                        }
                        flag_asistole = true;
                        contador_muestras = 0;
                        flag_normal_beat = false;
                        cont_ok = 0;
                    }

                    if (tipo_arritmia != tipo_ant)
                        flag_detecta = true;

                    tipo_ant = tipo_arritmia;
                    //sprintf(printCadena,"Data_search4\n");
                    //App_uartTx(appPrms0, (const uint8_t *) printCadena, strlen(printCadena));

                }   // analisis arritmias
            }  // (flag_dato_rdy)       */

        }
///////////////////////////analisis de arritmias///////////////////////////////////////////
        // Error check //
        if(appPrms->rxOverflow == TRUE)
        {
            sprintf(printCadena,"rxOverflow en appPrms\n");
            App_uartTx(appPrms0, (const uint8_t *) printCadena, strlen(printCadena));
            /* Init App Prms */
            App_uartInit(appPrms);
            /* Init Rx Prms and Enable Rx Interrupt */
            App_uartRx(appPrms, &gAppUartRxBuffer[0U]);

            //break;
        }

        if (charRecvdCnt >= APP_UART_BUFSIZE)
        {
            charRecvdCnt = 0;
            //App_uartTx(appPrms, (const uint8_t *) printExitStr, strlen(printExitStr));

            //break;
        }
//////////////////////Inicio Codigo/////////////////////////////////////////////
        TaskP_yield();
    }

    /////////////////////////////////////////////////Fin de cadena
    /* Disable Rx Interrupt */
    sprintf(printCadena,"un_code \n");
    App_uartTx(appPrms0, (const uint8_t *) printCadena, strlen(printCadena));
            
    UART_intrDisable(appPrms->baseAddr, UART_INTR_RHR_CTI);

    App_uartDeInit(appPrms);
    Board_driversClose();
    Drivers_close();

    return;

}

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
    appPrms->baseAddr = UART_getBaseAddr(gUartHandle[UART1]);
    DebugP_assert(appPrms->baseAddr != 0U); /* UART baseAddr Invalid!! */
    appPrms->rxTrigLvl          = gUartParams[UART1].rxTrigLvl;
    appPrms->txTrigLvl          = gUartParams[UART1].txTrigLvl;

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
    hwiPrms.intNum      = gUartParams[UART1].intrNum;
    hwiPrms.priority    = gUartParams[UART1].intrPriority;
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

static void App_uartRxDisable(UART_AppPrms *appPrms0, uint8_t *readBuf0)
{
    appPrms0->readBuf           = readBuf0;
    appPrms0->readCount         = 0U;

    /* Enable RHR interrupt */
    //UART_intrEnable(appPrms0->baseAddr, UART_INTR_RHR_CTI);
    UART_intr2Disable(appPrms0->baseAddr, UART_INTR_RHR_CTI);
    return;
}

////////////////////UART0_Config/////////////////////
static void App_uartTx0(UART_AppPrms   *appPrms0,
                       const uint8_t  *writeBuf0,
                       uint32_t        writeSize0)
{
    appPrms0->writeBuf           = (const uint8_t *)writeBuf0;
    appPrms0->writeSizeRemaining = writeSize0;
    appPrms0->writeCount         = 0U;
    appPrms0->txDataSent         = FALSE;

    // Enable THR interrupt //
    UART_intrEnable(appPrms0->baseAddr, UART_INTR_THR);

    SemaphoreP_pend(&appPrms0->writeDoneSem, SystemP_WAIT_FOREVER);

    return;
}

static void App_uartRx0(UART_AppPrms *appPrms0, uint8_t *readBuf0)
{
    appPrms0->readBuf           = readBuf0;
    appPrms0->readCount         = 0U;

    /* Enable RHR interrupt */
    UART_intrEnable(appPrms0->baseAddr, UART_INTR_RHR_CTI);
    UART_intr2Disable(appPrms0->baseAddr, UART_INTR_RHR_CTI);
    return;
}

static void App_uartUserISR0(void *arg0)
{
    UART_AppPrms  *appPrms0     = (UART_AppPrms *) arg0;
    uint32_t       intrType;

    intrType = UART_getIntrIdentityStatus(appPrms0->baseAddr);

    /* Check RX FIFO threshold is set */
    if((intrType & UART_INTID_RX_THRES_REACH) == UART_INTID_RX_THRES_REACH)
    {
        uint32_t       readSuccess;
        uint8_t       *readBuf, readChar;

        /* Read all data from RX FIFO */
        readBuf = (uint8_t *)appPrms0->readBuf + appPrms0->readCount;
        while(1)
        {
            readSuccess = UART_getChar(appPrms0->baseAddr, &readChar);
            if(readSuccess == TRUE)
            {
                if(appPrms0->readCount >= APP_UART_BUFSIZE)
                {
                    /* Rx buffer overflow */
                    appPrms0->rxOverflow = TRUE;
                }
                else
                {
                    /* Store data in buffer */
                    *readBuf = readChar;
                    readBuf++;
                    appPrms0->readCount++;
                }
            }
            else
            {
                break;      /* No data left in FIFO */
            }
        }

        if(appPrms0->rxOverflow == TRUE)
        {
            /* Stop processing further data */
            UART_intrDisable(appPrms0->baseAddr, UART_INTR_RHR_CTI);
        }
    }

    /* Check TX FIFO threshold is set */
    if((intrType & UART_INTID_TX_THRES_REACH) == UART_INTID_TX_THRES_REACH)
    {
        uint32_t       numBytesToTransfer;
        const uint8_t *writeBuf;

        if(appPrms0->writeSizeRemaining > 0U)
        {
            numBytesToTransfer = appPrms0->writeSizeRemaining;
            if(numBytesToTransfer > appPrms0->txTrigLvl)
            {
                /* Write only threshold level of data */
                numBytesToTransfer = appPrms0->txTrigLvl;
            }
            appPrms0->writeSizeRemaining -= numBytesToTransfer;

            /* Send characters upto FIFO threshold level or until done */
            writeBuf = appPrms0->writeBuf + appPrms0->writeCount;
            while(numBytesToTransfer != 0U)
            {
                UART_putChar(appPrms0->baseAddr, *writeBuf);
                writeBuf++;
                numBytesToTransfer--;
                appPrms0->writeCount++;
            }

            if(appPrms0->writeSizeRemaining == 0U)
            {
                /* Write complete  - disable TX interrupts */
                appPrms0->txDataSent = TRUE;
                UART_intrDisable(appPrms0->baseAddr, UART_INTR_THR);
                UART_intr2Enable(appPrms0->baseAddr, UART_INTR2_TX_EMPTY);
            }
        }
        else
        {
            /* Disable interrupt */
            UART_intrDisable(appPrms0->baseAddr, UART_INTR_THR);
        }
    }

    /* Check if TX FIFO is empty */
    if(appPrms0->txDataSent == TRUE)
    {
        intrType = UART_getIntr2Status(appPrms0->baseAddr);
        if((intrType & UART_INTR2_TX_EMPTY) != 0U)
        {
            UART_intr2Disable(appPrms0->baseAddr, UART_INTR2_TX_EMPTY);
            appPrms0->txDataSent = FALSE;
            SemaphoreP_post(&appPrms0->writeDoneSem);
        }
    }

    return;
}

static void App_uartInit0(UART_AppPrms *appPrms0)
{
    int32_t         status;
    HwiP_Params     hwiPrms;

    /* Get UART Instance Base Address */
    appPrms0->baseAddr = UART_getBaseAddr(gUartHandle[UART0]);
    DebugP_assert(appPrms0->baseAddr != 0U); /* UART baseAddr Invalid!! */
    appPrms0->rxTrigLvl          = gUartParams[UART0].rxTrigLvl;
    appPrms0->txTrigLvl          = gUartParams[UART0].txTrigLvl;

    /* Reset other variables */
    appPrms0->writeBuf           = NULL;
    appPrms0->writeSizeRemaining = 0U;
    appPrms0->writeCount         = 0U;
    appPrms0->txDataSent         = FALSE;
    appPrms0->readBuf            = NULL;
    appPrms0->readCount          = 0U;
    appPrms0->rxOverflow         = FALSE;

    status = SemaphoreP_constructBinary(&appPrms0->writeDoneSem, 0);
    DebugP_assert(SystemP_SUCCESS == status);

    /* Register interrupt */
    HwiP_Params_init(&hwiPrms);
    hwiPrms.intNum      = gUartParams[UART0].intrNum;
    hwiPrms.priority    = gUartParams[UART0].intrPriority;
    hwiPrms.callback    = &App_uartUserISR0;
    hwiPrms.args        = (void *) appPrms0;
    status              = HwiP_construct(&appPrms0->hwiObject, &hwiPrms);
    DebugP_assert(status == SystemP_SUCCESS);

    return;
}

static void App_uartDeInit0(UART_AppPrms *appPrms0)
{
    HwiP_destruct(&appPrms0->hwiObject);
    SemaphoreP_destruct(&appPrms0->writeDoneSem);

    return;
}
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

void EncontrarPuntoR(int dato_in)
{       // funci�n dedicada a encontrar el punto R en cada onda

    int y, i, pendiente_I;
    static int x_derv[4];
    int valor_mayor = 0;
    int  umbral_asc = 12;
    int  umbral_desc = 6;

    contador_muestras++;        // incrementa el contador de muestras en cada acceso
    // ***** Calculo derivada de la se�al de entrada  ****
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
    return;

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

    int lim_asistole =  (limite_asistole + 3) * 1000;

    tiempo_RR_1 = ClockP_getTimeUsec();
    delta_RR_actual = (tiempo_RR_1 - tiempo_RR_2);
    delta_RR_real = (float)delta_RR_actual / 1000;
    tiempo_RR_2 = tiempo_RR_1;

    // promedio de �ltimos 8 pulsos detectados
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
            if (cont_ritmo_regular == 4)    // despu�s de 4 pulsos en ritmo regular
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
                // no entra en rango aceptable, pero tampoco est� en el rango de latido perdido
                else if ((delta_RR_real < Thr_RR_miss) || (delta_RR_real < lim_brady_int))
                    flag_arritmia_sinus = false;
            }
            else if (cont_ritmo_irregular > 2)
            {
                if ((delta_RR_real > lim_brady_int) && (delta_RR_real < lim_asistole) && (flag_cvp < 1))    // DETECCI�N DE LATID PERDIDO
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
    return;
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
    return;
}

float euclidean_distance(int point1[2], int point2[2])
{
    return sqrt(pow(point1[0] - point2[0], 2) + pow(point1[1] - point2[1], 2));
}

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
    uintptr_t   key;
    // Enter Critical section, disable interrupt //
    key = HwiP_disable();

    Bool visited[NUM_POINTS] = {false};
    int index=0;
    int idb = 0;
    int no_cluster_aux=0;
    int aux_cont=0;

    /* Init Rx Prms and Disable Rx Interrupt */
    //App_uartRxDisable(appPrms, &gAppUartRxBuffer[0U]);
    /* Init Rx Prms and Disable Rx Interrupt */
    //App_uartRxDisable(appPrms0, &gAppUartRx0Buffer[0U]);

    for (idb = 0; idb < len; idb++)
    {
        int point[2] = {data[idb][0], data[idb][1]};
        if (!visited[idb])
        {
            //int neighbors[NUM_POINTS][2] = {0};// es posible que se tenga que vaciar el arreglo antes de usar Debug_neighbors
            int len_neighbors = get_neighbors(data, point, eps, neighbors_G,visited);
            if (len_neighbors >= minPts)
            {
                index =  expand_cluster(data, neighbors_G,eps, visited, clusters[*len_no_clusters],len_neighbors);
                len_clusters[*len_no_clusters] = index;
                (*len_no_clusters)++;
                aux_cont++;
            }
            else
            {
                no_clusters[no_cluster_aux][0] = point[0];
                no_clusters[no_cluster_aux][1] = point[1];
                (no_cluster_aux)++;
            }//*/
        }
        //*/
    }
    *len_no_clusters = no_cluster_aux;

    sprintf(printCadena,"Saliendo de DBscan \n");
    App_uartTx(appPrms0, (const uint8_t *) printCadena, strlen(printCadena));

    /* Init Rx Prms and Enable Rx Interrupt */
    //App_uartRx(appPrms, &gAppUartRxBuffer[0U]);
    /* Init Rx Prms and Enable Rx Interrupt */
    //App_uartRx(appPrms0, &gAppUartRx0Buffer[0U]);

    // End critical section //
    HwiP_restore(key);

    return aux_cont;
}

void dbscan_2(int data_2[NUM_POINTS_3][2], float eps, int minPts, int clusters[NUM_POINTS_3][NUM_POINTS_3][2],
              int len_clusters[NUM_POINTS_3], int no_clusters[NUM_POINTS_3][2], int *len_no_clusters, int len)
{
    sprintf(printCadena,"Entrando a DBscan2\n");
    App_uartTx(appPrms0, (const uint8_t *) printCadena, strlen(printCadena));

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
    sprintf(printCadena,"Saliendo a DBscan2\n");
    App_uartTx(appPrms0, (const uint8_t *) printCadena, strlen(printCadena));
    return;

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
    //sprintf(printCadena,"Entrando a buscar arritmias\n");
    //App_uartTx(appPrms0, (const uint8_t *) printCadena, strlen(printCadena));

     
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
    //*/
    //sprintf(printCadena,"Saliendo de buscar arritmias\n");
    //App_uartTx(appPrms0, (const uint8_t *) printCadena, strlen(printCadena));

     resultado = (aux_cont_2 - aux_cont);
     if (resultado <= 1)
         return 0;

     return 1;

}

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

 void uart_echo_write_callback_1(UART_Handle handle, UART_Transaction *trans)
{
    DebugP_assertNoLog(UART_TRANSFER_STATUS_SUCCESS == trans->status);
    gNumBytesWritten_1 = trans->count;
    SemaphoreP_post(&gUartWriteDoneSem_1);

    return;
}       //  */

void uart_echo_read_callback_1(UART_Handle handle, UART_Transaction *trans)
{
    DebugP_assertNoLog(UART_TRANSFER_STATUS_SUCCESS == trans->status);
    gNumBytesRead_1 = trans->count;
    SemaphoreP_post(&gUartReadDoneSem_1);

    return;
}
