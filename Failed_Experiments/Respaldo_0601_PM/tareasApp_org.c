/*  Programa desarrollado para la detección de arritmias
 *
 *  Se recibe el dato normalizado por el STM32
 *  Se procesa la información para detectar el complejo QRS
 *  Se detecta el punto R
 *  Se mide el segmento RR
 *  Se saca el promedio del mismo para determinar el ritmo cardiaco.
 *  Se define un codigo para diferentes tipos de arritmias.
 *  29/05/2023  Se identifica bradicardia, taquicardia y el latido perdido
 *  30/05/2023  Se implementa la segunda tarea
 *  31/05/2023  Se inicia implementación de red neuronal
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <kernel/dpl/ClockP.h>
#include <kernel/dpl/DebugP.h>              // Para acceder al API IPC RPMessage
#include <kernel/dpl/TaskP.h>               // para acceder a las tareas
#include <drivers/ipc_notify.h>
#include <drivers/ipc_rpmsg.h>
#include "ti_drivers_config.h"             // Para acceder al API UART
#include "ti_drivers_open_close.h"
#include "ti_board_open_close.h"


/*typedef enum {
    false = 0, true = 1
} bool;
typedef bool bolean;*/

/* Este es el servicio usado para el test de eco en el espacio de usuario */
#define IPC_RPMESSAGE_SERVICE_CHRDEV      "rpmsg_chrdev"                        // Nombre para que linux genere el remoteproc adecuado
#define IPC_RPMESSAGE_ENDPT_NUMBER        (14U)                                 // Dirección del EndPoint
#define IPC_RPMESSAGE_MAX_MSG_SIZE        (96u)                                 // Tamaño máximo que un mensaje puede tener en este ejemplo

/* Los objetos RPMessage DEBEN SER globales*/
RPMessage_Object gIpcRecvMsgObject;                                             // Objeto RPMessage usado para recibir y enviar mensajes

/* Prioridad de la tarea, stack, tamaño del stack y objetos de tareas, estos DEBEN SER globales */
#define IPC_RPMESSAFE_TASK_PRI            (8U)
#define IPC_RPMESSAFE_TASK_STACK_SIZE     (8*1024U)                             // 8KiB
uint8_t gIpcTaskStack[IPC_RPMESSAFE_TASK_STACK_SIZE] __attribute__((aligned(32)));
TaskP_Object gIpcTask;

/* Task priority, stack, stack size and task objects, these MUST be global's */
#define RED_NEURO_TASK_PRI         (8U)
#define RED_NEURO_TASK_STACK_SIZE (512*1024U)
uint8_t gRedNeuroStack[RED_NEURO_TASK_STACK_SIZE] __attribute__((aligned(32)));
TaskP_Object gRedNeuroTask;

/* Application specific task arguments */
typedef struct {
    uint32_t value;
} RedNeuro_Args;

RedNeuro_Args gRedNeuro_args;


/* Definiciones para datos recibidos por UART */
#define APP_UART_BUFSIZE             (200U)
#define APP_UART_RECEIVE_BUFSIZE      (20U)      // Numero de bytes recibidos
#define BUFSIZE_ECG                   (10U)
#define MUESTRAS_DIEZ_SEG         (100U)
#define UMBRAL_EQUIPOTENCIAL         (100U)
//   constantes para tarea 2 //
#define NUM_POINTS          (210U)     // tamaño de cluster = NUM_POINTS_2 / Minimo de puntos mínimo
#define NUM_POINTS_2        (1000U)    // tamaño de array

static SemaphoreP_Object gUartWriteDoneSem;
static SemaphoreP_Object gUartReadDoneSem;

#define APP_UART_ASSERT_ON_FAILURE(transferOK, transaction) \
        do { \
            if((SystemP_SUCCESS != (transferOK)) || (UART_TRANSFER_STATUS_SUCCESS != transaction.status)) \
            { \
                DebugP_assert(FALSE); /* UART TX/RX failed!! */ \
        } \
} while(0) \

#define MAX(a,b) ((a)>(b)?(a):(b))


/********  VARIABLES GLOBALES ************/

uint8_t     gUartBuffer[APP_UART_BUFSIZE];
char        gUartReceiveBuffer[APP_UART_RECEIVE_BUFSIZE];
char        bufferdatos_ECG[APP_UART_RECEIVE_BUFSIZE];

volatile uint32_t gNumBytesRead = 0U, gNumBytesWritten = 0U;

bool    flag_continuar = false;
bool    flag_punto_R = false;
bool    flag_define_fs =  true;
bool    flag_reset_cta = false;
bool    flag_inicio_datos = false;

uint8_t valida_d1 = 0XA5;  //0x11;      //  validación dato derivación 1
uint8_t signo_dato = 0X00;       // cambiar a 0x00
uint8_t valida_d2 = 0x5A;        // validación dato derivación 2

uint8_t dato_in_d1[4];      // bytes del dato recibido
uint8_t dato_in_d2[4];      // bytes del dato recibido
int32_t dato_raw_d1,dato_raw_d2;        //dato concatenado de los bytes recibidos

int32_t segmento_RR[MUESTRAS_DIEZ_SEG];     // aún no se utilizan   25/05/2023
int32_t maximo_punto_R[MUESTRAS_DIEZ_SEG];  // aún no se utilizan   25/05/2023
int32_t valor_punto_R = 0,valor_pico=0;

int32_t ecg_filt_ind1 = 0,ecg_filt_ind2 = 0;
int32_t deriv_ecg_d1 = 0,deriv_ecg_d2 = 0;
int32_t integral_ecg_d1 = 0,integral_ecg_d2 = 0;


int     contador_aprendizaje = 0;
uint32_t    cont_ritmo_irregular = 0;
uint32_t    cont_ritmo_regular = 0;

// ******   Variables gloabales para funciones
static int ptr = 0;
static int64_t sum_integral = 0;

int     cta_negativo_S = 0;
int     inicio_muestreo = 0;

int32_t valor_x0=0,valor_x1=0,gradiente=0;
int32_t array_RR_inicial[9], array_RR_actual[9],array_RR_valido[9]={0};
int32_t suma_RR_valido = 0;
int32_t media_RR_valido = 0;
int   time_segRR_valido = 0;
int   time_segRR_actual = 0;
float   Thr_RR_alto = 0,T_RR_valido = 0;
float   Thr_RR_bajo = 0;
int32_t   variable00 = 0;

int32_t tiempo_RR_1=0,tiempo_RR_2=0;
int64_t tiempo_fs0=0,tiempo_fs1 =0;

int     contador_muestras = 0;      // para envio de identificador de inicio de ciclo
float   delta_fs = 0,media_fs = 0;     //frecuencia de muestreo
//************** variables para tarea 2
int     contador_tarea_2 = 0;

int point_in[2]={0};
int data[30][2]={0}; // Aquí debes inicializar los datos de entrenamiento
float eps = 2;
int minPts = 10;
int clusters[30][30][2] = {0};
int len_clusters[30] = {0};
int no_clusters[30][2] = {0};
int len_no_clusters = 0;

float eps_2 = 8;
int minPts_2 = 5;
int clusters_2[30][30][2] = {0};
int len_clusters_2[30] = {0};
int no_clusters_2[30][2] = {0};
int len_no_clusters_2 = 0;
int arritmia = 0;

bool flag_trained = false;      // indicar tiempo de entrenamiento par ala red
bool flag_dato_rdy = false;     // indica dato valido de entrada para la red

/* Prototipos de funciones ****************************************************/
int32_t FiltroPasaBanda(uint32_t data_in);
int32_t Derivativa(int32_t data_in);
int32_t MovingWindowIntegral(int32_t data_in);
void    EncontrarPuntoR(int32_t dato_in);
void    ProcesamientoSegmentoRR();
// funciones para red
float euclidean_distance(int point1[2], int point2[2]);
int get_neighbors(int data[30][2], int point[2], float eps, int neighbors[30][2]);
int expand_cluster(int data[30][2],int neighbors[30][2],float eps,bool visited[30],int cluster[30][2],int len_neighbors);
void dbscan(int data[30][2], float eps, int minPts, int clusters[30][30][2], int len_clusters[30], int no_clusters[30][2], int *len_no_clusters);
int find_cluster(int point[2], int clusters[30][30][2], int len_clusters[30]);

/*  Funciones principales      */
void generarTareas(void *args);
void tarea1(void *args);
void tarea2(void *args);

void generarTareas(void *args) /* función main */
{
    //Levantar los servicios que usarán las tareas
    int32_t status;

    Drivers_open();
    Board_driversOpen();

    /* Esta API DEBE SER llamada por las aplicaciones para comunicarse con linux */
    status = RPMessage_waitForLinuxReady(SystemP_WAIT_FOREVER);
    DebugP_assert(status==SystemP_SUCCESS);

    /* Iniciar el servicio RPMessage con el objeto RPMessage_Object. Este servicio se encargará de la recepción y envío de los mensajes IPC */
    RPMessage_CreateParams createParams;
    RPMessage_CreateParams_init(&createParams);                                 // Valores por default. Método recomendado por la API
    createParams.localEndPt = IPC_RPMESSAGE_ENDPT_NUMBER;                       // Cambiar el valor por default al que queremos
    status = RPMessage_construct(&gIpcRecvMsgObject, &createParams);            // Servicio RPMsg listo
    DebugP_assert(status==SystemP_SUCCESS);

    /* Anunciar el EndPoint al cliente linux. Linux no sabe que el servicio existe hasta que lo anunciamos. */
    status = RPMessage_announce(CSL_CORE_ID_A53SS0_0,                           // Núcleo corriendo linux
                                IPC_RPMESSAGE_ENDPT_NUMBER,                     // LocalEndPt
                                IPC_RPMESSAGE_SERVICE_CHRDEV);                  // Nombre       rpmsg_chrdev
    DebugP_assert(status==SystemP_SUCCESS);

    /* Crear la tarea que manejará el servicio de ping */
    TaskP_Params taskParams1;                                                    // Parámetros para generar la tarea
    TaskP_Params_init(&taskParams1);                                             // Iniciar parámetros. Recomendado por la API
    taskParams1.name = "TAREA_01";                                               // Nombre para Debug
    taskParams1.stackSize = IPC_RPMESSAFE_TASK_STACK_SIZE;                       // 8KiB
    taskParams1.stack = gIpcTaskStack;                                           // Puntero al stack (*uint8_t)
    taskParams1.priority = IPC_RPMESSAFE_TASK_PRI;                               // Prioridad de la tarea
    taskParams1.args = &gIpcRecvMsgObject;                                       // Acá estamos pasando el RPMessage_Object como argumento de la función
    taskParams1.taskMain = tarea1;                                               // Función que implementa la tarea

    /* Construir la tarea */
    status = TaskP_construct(&gIpcTask, &taskParams1);
    DebugP_assert(status == SystemP_SUCCESS);

    /* Crear la tarea que manejará el servicio de ping */
    TaskP_Params taskParams2;                                                // Parámetros para generar la tarea
    TaskP_Params_init(&taskParams2);                                         // Iniciar parámetros. Recomendado por la API
    taskParams2.name = "TAREA_02";                                           // Nombre para Debug
    taskParams2.stackSize = RED_NEURO_TASK_STACK_SIZE;                       // 8KiB
    taskParams2.stack = gRedNeuroStack;                                      // Puntero al stack (*uint8_t)
    taskParams2.priority = RED_NEURO_TASK_PRI;                               // Prioridad de la tarea
    taskParams2.args = &gRedNeuro_args;                                   // Acá estamos pasando el RPMessage_Object como argumento de la función
    taskParams2.taskMain = tarea2;                                          // Función que implementa la tarea

    /* Construir la tarea */
    status = TaskP_construct(&gRedNeuroTask, &taskParams2);
    DebugP_assert(status == SystemP_SUCCESS);

    /* Esperar que todos los núcleos No-Linuxs estén listos. Esto asegura que cuando
     * Enviemos los mensajes, ellos no se pierdan debido a que el EndPoint
     * no se haya creado en el núcleo remoto.
     */
    IpcNotify_syncAll(SystemP_WAIT_FOREVER);


    /* Salida de la tarea. vTaskDelete() se llama desde fuera de la función, así que simplemente acaba */

    //Board_driversClose();
    /* No cerramos los drivers porque otros procesos quedan en segundo plano */
    /* Drivers_close(); */
}


/* Tarea que recibe datos normalizados del STM32 y los procesa.
 *   */
void tarea1(void *args)
{
    char    recvMsg[IPC_RPMESSAGE_MAX_MSG_SIZE+1];                  // Buffer para recibir mensajes, +1 para caracter NULL en el peor caso
    uint16_t recvMsgSize, remoteCoreId, remoteCoreEndPt;            // Características del mensaje: tamaño, ProcID, EndPoint
    RPMessage_Object *pRpmsgObj = (RPMessage_Object *)args;         // Adquirir el RPMessage_Object (que era global) por medio de los argumentos de la función


    int32_t     transferOK,status;
    UART_Transaction trans;
    bool        flag_dato_1=false;
    int         i=0,k=0;    //,cta_envio=0;
    float       suma_fs = 0;


    status = SemaphoreP_constructBinary(&gUartWriteDoneSem, 0);
    DebugP_assert(SystemP_SUCCESS == status);

    status = SemaphoreP_constructBinary(&gUartReadDoneSem, 0);
    DebugP_assert(SystemP_SUCCESS == status);

    UART_Transaction_init(&trans);                                              // Inicializar la estructura UART_Transaction

    flag_continuar = true;


    while(1)
    {
        /* Establecer 'recvMsgSize' al tamaño del buffer de recepción, después
         * se devolverá 'recvMsgSize' con el tamaño actual de los datos válidos
         * recibidos */
        recvMsgSize = IPC_RPMESSAGE_MAX_MSG_SIZE;
        status = RPMessage_recv(pRpmsgObj,                                      // RPMessage_Object * obj
                                recvMsg,                                        // void *             data
                                &recvMsgSize,                                   // uint16_t *         dataLen
                                &remoteCoreId,                                  // uint16_t *         rmeoteCoreId
                                &remoteCoreEndPt,                               // uint16_t *         remoteEndPt
                                SystemP_WAIT_FOREVER);                          // uint32_t           timeout
        DebugP_assert(status==SystemP_SUCCESS);
        recvMsg[recvMsgSize] = 0;                                              // Caracter NULL al final del mensaje para el envío de cadenas

        gNumBytesWritten = 0U;
        trans.buf   = &gUartBuffer[0U];
        strncpy(trans.buf,"Esta en una prueba de eco en modo callback \r\n", APP_UART_BUFSIZE);
        trans.count = strlen(trans.buf);
        transferOK = UART_write(gUartHandle[CONFIG_UART0], &trans);
        APP_UART_ASSERT_ON_FAILURE(transferOK, trans);

        /* Wait for write completion */
        SemaphoreP_pend(&gUartWriteDoneSem, SystemP_WAIT_FOREVER);
        DebugP_assert(gNumBytesWritten == strlen(trans.buf));

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
                tiempo_fs0 = ClockP_getTimeUsec();
                if (inicio_muestreo == 0)
                    delta_fs = 6;
                else
                    delta_fs = (float) (tiempo_fs0 - tiempo_fs1)/1000;

                tiempo_fs1 = tiempo_fs0;
                inicio_muestreo++;
                suma_fs += delta_fs;

                if (inicio_muestreo > 50)
                {
                    media_fs = suma_fs / 49;
                    flag_define_fs = false;
                    inicio_muestreo = 0;
                    suma_fs = 0;
                    flag_inicio_datos = true;
                    flag_trained = true;
                }
            }


            for (i = 0; i <= APP_UART_RECEIVE_BUFSIZE; i++) // Validaciòn de inicio de cadena
            {
                if (gUartReceiveBuffer[i] == (char)valida_d1)
                {
                    if (gUartReceiveBuffer[i+1] == (char)signo_dato)
                    {
                        flag_dato_1 = true;
                        flag_dato_rdy = true;
                        k = i;
                        i = APP_UART_RECEIVE_BUFSIZE + 1;
                    }
                }
            }

            if (flag_dato_1)
            {
                for (i = 0; i <= BUFSIZE_ECG; i++)
                {
                    bufferdatos_ECG[i] = gUartReceiveBuffer[k+i];
                    if ((i > 0)&& (i < 5))      //if ((i > 1)&& (i < 5))
                    {
                        dato_in_d1[i-1] = (uint8_t)bufferdatos_ECG[i];//dato_in_d1[i-2] = (uint8_t)bufferdatos_ECG[i];
                    }
                    else if (i > 5)     //else if (i > 6)
                    {
                        dato_in_d2[i-6] = (uint8_t)bufferdatos_ECG[i];       //dato_in_d2[i-7] = (uint8_t)bufferdatos_ECG[i];
                    }
                }
                // solo para datos normalizados

                if (dato_in_d1[0] == 0)
                {
                    dato_raw_d1 = ((dato_in_d1[0]<<24)|(dato_in_d1[1]<<16)|(dato_in_d1[2]<<8)|(dato_in_d1[3]));
                    dato_raw_d2 = ((dato_in_d2[0]<<24)|(dato_in_d2[1]<<16)|(dato_in_d2[2]<<8)|(dato_in_d2[3]));
                }

                flag_dato_1= false;
            }

            /* Eco en TX de caracteres recibidos (Eliminar al finalizar)  */
            gNumBytesWritten = 0U;
            trans.buf   = &bufferdatos_ECG[0U];
            trans.count = BUFSIZE_ECG;
            transferOK = UART_write(gUartHandle[CONFIG_UART1], &trans);
            APP_UART_ASSERT_ON_FAILURE(transferOK, trans);

            /* Wait for write completion */
            SemaphoreP_pend(&gUartWriteDoneSem, SystemP_WAIT_FOREVER);
            DebugP_assert(gNumBytesWritten == BUFSIZE_ECG);

            /**** Detección QRS  ***/

            ecg_filt_ind1 = FiltroPasaBanda(dato_raw_d1);
            ecg_filt_ind2 = FiltroPasaBanda(dato_raw_d2);
            deriv_ecg_d1 = Derivativa(ecg_filt_ind1);
            deriv_ecg_d2 = Derivativa(ecg_filt_ind2);
            integral_ecg_d1 = MovingWindowIntegral(deriv_ecg_d1);
            integral_ecg_d2 = MovingWindowIntegral(deriv_ecg_d2);
            integral_ecg_d1 = integral_ecg_d1 >> 5;
            integral_ecg_d2 = integral_ecg_d2 >> 5;

            EncontrarPuntoR(integral_ecg_d1);

            if (flag_punto_R)     // si se detecto punto R
                ProcesamientoSegmentoRR();

            point_in[0] = contador_muestras;
            point_in[1] = integral_ecg_d1 >> 1;

           /**** envío de datos a la terminal **/

            gNumBytesWritten = 0U;
            trans.buf   = &gUartBuffer[0U];
            trans.count = sprintf(trans.buf,"d:%i,%i,%i,%i\r\n",dato_raw_d1,integral_ecg_d1,contador_tarea_2,arritmia);
            transferOK = UART_write(gUartHandle[CONFIG_UART0], &trans);
            APP_UART_ASSERT_ON_FAILURE(transferOK, trans);

            /* Wait for write completion */
            SemaphoreP_pend(&gUartWriteDoneSem, SystemP_WAIT_FOREVER);
            DebugP_assert(gNumBytesWritten == strlen(trans.buf));

            SemaphoreP_destruct(&gUartWriteDoneSem);
            SemaphoreP_destruct(&gUartReadDoneSem);

        } while (flag_continuar);

    }
            /* Tarea permanente */
}

void tarea2(void *args)
{
   // int i;

    while (1)
    {
        if (flag_dato_rdy)
        {
            flag_dato_rdy = false;
            if (flag_trained)
            {
                data[contador_tarea_2][0] = point_in[0];
                data[contador_tarea_2][1] = point_in[1];
              /*  no_clusters[contador_tarea_2][0] = point_in[0];
                no_clusters[contador_tarea_2][1] = point_in[1];*/

                contador_tarea_2++;
                if(contador_tarea_2 >= 30)
                {
                  //  int i,j;
                 /*   for (i=0;i<NUM_POINTS;i++)
                    {
                        for (j=0;j<NUM_POINTS_2;j++)
                        {
                            clusters[i][j][0]=2;
                            clusters[i][j][1]=3;
                            clusters_2[i][j][0]=4;
                            clusters_2[i][j][1]=5;
                            no_clusters[j][0]=6;
                            no_clusters[j][1]=7;
                        }
                    }*/
                    dbscan(data, eps, minPts, clusters, len_clusters, no_clusters, &len_no_clusters);
                    dbscan(no_clusters, eps_2, minPts_2, clusters_2, len_clusters_2, no_clusters_2, &len_no_clusters_2);
                    contador_tarea_2 = 0;
                    flag_trained = false;
                }
            }
            else
            {
                int point_to_search[2];
                point_to_search[0] = point_in[0];
                point_to_search[1] = point_in[1];
             //   scanf("%f", &point_to_search[1]);
                int cluster_index = find_cluster(point_to_search, clusters, len_clusters);
                if (cluster_index != -1)
                {
                    arritmia = 0;
                    //printf("El punto (%.1f, %.1f) se encuentra en el cluster %d\n", point_to_search[0], point_to_search[1], cluster_index + 1);
                }
                else
                {
                    arritmia = 1000;
                    //printf("El punto (%.1f, %.1f) no se encuentra en ningún cluster\n", point_to_search[0], point_to_search[1]);
                }
            }


        }

    }
}

int32_t FiltroPasaBanda(uint32_t data_in)
{
    static int yl1 = 0, yl2 = 0, xl[26], nl = 12;
    static int yh1 = 0, xh[66], nh = 32;
    int yl0;
    int yh0;

    xl[nl] = xl[nl + 13] = (int32_t)data_in;
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

    if(++ptr == 32)
        ptr = 0;
    sum_integral -= x[ptr];
    sum_integral += data_in;
    x[ptr] = data_in;
    ly = sum_integral >> 6;

    return(ly);
}

void EncontrarPuntoR(int32_t dato_in)
{       // función dedicada a encontrar el punto R en cada onda

    int y, i, pendiente_I;
    static int x_derv[4];
    int32_t valor_mayor = 0;
    int  UMBRAL = 30;

    contador_muestras++;
    // ***** Calculo derivada de la señal de entrada  ****
    y = (dato_in << 1) + x_derv[3] - x_derv[1] - ( x_derv[0] << 1);
    y >>= 3;
    for (i = 0; i < 3; i++)
        x_derv[i] = x_derv[i + 1];
    x_derv[3] = dato_in;
    pendiente_I = y;

    if (dato_in > UMBRAL)
    {
        if (flag_reset_cta)
        {
            contador_muestras = 0;
            flag_reset_cta = false;
        }
        if (pendiente_I > 20)       //pendiente positiva
        {
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
                }
                else if (cta_negativo_S > 2)
                {
                    if (flag_punto_R)
                        flag_punto_R = false;
                }
            }
            valor_punto_R = valor_pico;
        }
    }
    else
    {
        valor_pico = 0;
        valor_punto_R = 0;
        cta_negativo_S = 0;
        if (pendiente_I< 0)
            flag_reset_cta = true;
    }

    gradiente = pendiente_I;        // solo para debugueo, borrar después

}

void ProcesamientoSegmentoRR()
{
    int32_t delta_RR_actual;
    int64_t suma_RR_actual = 0;
    int32_t media_RR_actual = 0;
    int8_t i;
    float   T_RR_actual = 0;    //periodo de RR para referencia (milisegundos)
    float   delta_RR_real = 0;


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

            Thr_RR_bajo = T_RR_valido * 0.92;       // define valores iniciales del rango
            Thr_RR_alto = T_RR_valido * 1.16;
            cont_ritmo_regular = 0;
        }
    }
    else
    {
        if ((delta_RR_real >= Thr_RR_bajo) && (delta_RR_real <= Thr_RR_alto))
        {
            cont_ritmo_regular++;
            suma_RR_valido = 0;
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

                Thr_RR_bajo = T_RR_valido * 0.92;       // define valores iniciales del rango
                Thr_RR_alto = T_RR_valido * 1.16;
            }

        }
        else    // si un latido se encuentra fuera del rango normal...
        {
            cont_ritmo_irregular++;
        }
    }

    variable00 = cont_ritmo_irregular;

    time_segRR_actual = (int)T_RR_actual;     //(float) media_RR_actual / 1000;
    time_segRR_valido = (int)T_RR_valido;

}

float euclidean_distance(int point1[2], int point2[2])
{
    return sqrt(pow(point1[0] - point2[0], 2) + pow(point1[1] - point2[1], 2));
}

int get_neighbors(int data[30][2], int point[2], float eps, int neighbors[30][2])
{
    int other_point[2] = {0};
    int index = 0;
    int i=0;
    for (i = 0; i < 30; i++)
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

int expand_cluster(int data[30][2],int neighbors[30][2],float eps,bool visited[30],int cluster[30][2],int len_neighbors)//,int point[2],,   int minPts,  )
{
    int i,j;
    int len_neighbor_new;
    int len_neighbor_max=0;
    int new_neighbors[30][2];
    int point_aux[2];


    for(i=0;i<len_neighbors;i++)
    {
        int new_point[2] = {neighbors[i][0],neighbors[i][1]};
        len_neighbor_new = get_neighbors(data,new_point,eps,new_neighbors);
        if (len_neighbor_new > len_neighbor_max)
        {
            len_neighbor_max = len_neighbor_new;
            point_aux[0] = new_point[0];
            point_aux[1] = new_point[1];
        }
    }

    len_neighbor_new = get_neighbors(data,point_aux,eps,cluster);
    for(i=0;i<len_neighbor_new;i++)
    {
        //int point_aux2
        for(j=0;j<30;j++)
        {
            if ((cluster[i][0]==data[j][0])&&(cluster[i][1]==data[j][1])&&(!visited[j]))
            {
                visited[j] = true;
            }
        }


    }
    return len_neighbor_new;
}

void dbscan(int data[30][2], float eps, int minPts, int clusters[30][30][2], int len_clusters[30], int no_clusters[30][2], int *len_no_clusters)
{
    bool visited[30] = {false};
    int index=0;
    int i = 0;
    for (i = 0; i < 30; i++)
    {
        int point[2] = {data[i][0], data[i][1]};
        if (!visited[i])
        {
            visited[i] = true;
            int neighbors[30][2] = {0};
            int len_neighbors = get_neighbors(data, point, eps, neighbors);
            if (len_neighbors >= minPts)
            {
                index =  expand_cluster(data, neighbors,eps, visited, clusters[*len_no_clusters],len_neighbors);
                //index = expand_cluster(data, i, point, neighbors, len_neighbors, eps, minPts, visited, clusters[*len_no_clusters]);
                len_clusters[*len_no_clusters] = index;
                (*len_no_clusters)++;
            }
            else
            {
                no_clusters[*len_no_clusters-index][0] = point[0];
                no_clusters[*len_no_clusters-index][1] = point[1];
                (*len_no_clusters)++;
            }
        }
    }
    *len_no_clusters=*len_no_clusters-index;
}


int find_cluster(int point[2], int clusters[30][30][2], int len_clusters[30])
{
    int i = 0;
    for (i = 0; i < 30; i++)
    {
        if (len_clusters[i] > 0)
        {
            int j = 0;
            for (j = 0; j < len_clusters[i]; j++)
            {
                if (abs(clusters[i][j][0] - point[0]) < 2 && abs(clusters[i][j][1] - point[1]) < 2)
                {
                    return i;
                }
            }
        }
    }
    return -1;
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
