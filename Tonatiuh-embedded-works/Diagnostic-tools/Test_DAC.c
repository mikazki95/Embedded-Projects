/* 
 * File:   main.c
 * Author: TONA
 *
 * Created on 10 de junio de 2025, 10:30 AM
 */

#include <stdio.h>
#include <stdlib.h>
// En el archivo main.c (al inicio):
#include <xc.h>
#include <string.h>
#pragma config FNOSC = PRIPLL         // Usa oscilador primario (cristal)
//#pragma config NOSC = PRIPLL         // Usa oscilador primario (cristal)
#pragma config POSCMD = HS         // Cristal estándar (XT)
#pragma config OSCIOFNC = OFF      // OSC2 es salida de reloj
#pragma config FCKSM = CSECME      // Clock Switching habilitado
#pragma config PLLKEN = ON         // PLL habilitado
//#define FCY 70000000UL  // 70 MHz (ajusta según tu PLL), error de frecuencia 
#define _XTAL_FREQ 70000000UL
#define BAUDRATE 115200
#define BRGVAL ((_XTAL_FREQ/BAUDRATE)/16)-1
#define FCY _XTAL_FREQ
#include <libpic30.h>   // Necesario para __delay32()
#define LED _LATC9      // Usa el pin RC9 como LED (ajusta según tu tarjeta)
//#define TX1 _LATB11      // Usa el pin RC9 como LED (ajusta según tu tarjeta)
#pragma config FWDTEN = OFF   // Deshabilita el Watchdog Timer
// Asegura estos pragma config (adicionales a los que ya tienes):
//#pragma config BOREN = ON             // Brown-out Reset habilitado
//#pragma config BORV = 3               // Nivel de Brown-out (2.7V)
//#pragma config PWRTEN = ON            // Power-up Timer habilitado (equivalente a FPWRT)


// Definiciones para el DAC
#define DAC_CS0     _LATB5      // Pin CS0 del DAC 
#define DAC_CS1     _LATB6      // Pin CS0 del DAC 
#define DAC_CS2     _LATB7      // Pin CS0 del DAC 
#define DAC_CS3     _LATB8      // Pin CS0 del DAC 
#define DAC_SCK    _LATC3      // Pin SCK
#define DAC_SDI    _LATA4      // Pin SDI


#define BUFFER_SIZE 20  // Suficiente para "AD011025" + terminadores

// intento de lectura de configuracion 
// Para dsPIC33EV256GM104 (ver tabla 30-1 del datasheet)
#define _FWDT (*(volatile unsigned int*)0x02ABA0) // Fuse Word WDT

// Máscaras para los bits (ver sección 30.5)
#define WDT_ENABLE_MASK 0x8000
#define WDT_WINDOW_MASK 0x3000

//#pragma config WINDIS = ON         // Watchdog en modo no-ventana
//#pragma config WDTPRE = PR32       // Prescaler 1:32
//#pragma config WDTPS = PS1         // Postscaler 1:1 (el más lento)

char in_char = '\0';

char rx_buffer[BUFFER_SIZE];
unsigned int rx_index = 0;
volatile unsigned char cmd_ready = 0;

void SetupOscillator();
void Delay_ms(unsigned int ms);
void UART1_Init(unsigned long baud);
void UART1_WriteChar(char c);
void UART1_WriteString(const char* str);
void process_command(const char* cmd);
void UART1_Task();
void PPS_Config(void);
void clear_buffer();
void debug_print(const char* msg);
void DAC_Write(uint16_t value, uint8_t channel);

// Prototipo de función (colócalo AL INICIO del archivo o en el .h)
unsigned int ReadConfigWord(unsigned int address);

// Implementación (puede ir después de main())
unsigned int ReadConfigWord(unsigned int address) {
    unsigned int val;
    asm volatile("mov %1, _TBLPAG\n"   // Configura TBLPAG
                 "tblrdl [%1], %0"     // Lee palabra baja
                 : "=r"(val) : "r"(address));
    return val;
}

int main() 
{
     // Verificar causa de reinicio
    if (RCONbits.POR) {
        UART1_WriteString("Power-on Reset\r\n");
    }
    if (RCONbits.EXTR) {
        UART1_WriteString("External Reset\r\n");
    }
    if (RCONbits.WDTO) {
        UART1_WriteString("Watchdog Timeout Reset\r\n");  // Esto confirmaría el problema
    }
    
    RCON = 0;  // Limpiar flags de reset
    ///////////////////////////////////////////
    SetupOscillator();
    ANSELCbits.ANSC9 = 0; // Configura RC9 como digital (no analógico)
    _TRISC9 = 0;            // Configura RC9 como salida
    LED = 0;            // Inicia apagado
    PPS_Config();
    UART1_Init(BAUDRATE); // o la velocidad que se necesite
    Delay_ms(500);
    /*
    char msg[100];
    
    // Lee directamente de la memoria de configuración
    unsigned int fwdt = ReadConfigWord(0x2ABA0);  // Dirección exacta FWDT
    //unsigned int wdt_config = _FWDT; 
    sprintf(msg,
            "Configuración REAL del WDT:\n"
            "Address: 0x02ABA0\n"
            "Valor: 0x%04X\n"
            "WDTEN: %s\n",
            fwdt
            );
            */
    /*
    sprintf(msg, 
        "Configuración REAL del WDT:\n"
        "Address: 0x02ABA0\n"
        "Valor: 0x%04X\n"
        "WDTEN: %s\n"
        "WINDOW: %s\n",
        wdt_config,
        (wdt_config & WDT_ENABLE_MASK) ? "ACTIVO" : "inactivo",
        ((wdt_config & WDT_WINDOW_MASK) >> 12) == 0b11 ? "25%" : 
        ((wdt_config & WDT_WINDOW_MASK) >> 12) == 0b10 ? "50%" :
        ((wdt_config & WDT_WINDOW_MASK) >> 12) == 0b01 ? "75%" : "100%");
    
    UART1_WriteString(msg);
    */
    
    UART1_WriteString("Iniciando: \r\n");   
    
 
    //CheckResetCause();
    DAC_Init();
    DAC_Write(2048,0);  // 50% de 4095 (0x0800)
    while(1) {
        UART1_Task();
        asm("clrwdt");  // ¡Esto resetea el contador del WDT!
    }
    return 0;
}


void Delay_ms(unsigned int ms) {
    for (unsigned int i = 0; i < ms; i++) {
        //__delay32(16000); // Aprox. 1ms @ 16 MHz (ajusta según tu configuración)
        __delay32(70000); // Aprox. 1ms @ 70 MIPS (ajusta según tu configuración)
        
    }
}


void SetupOscillator() {
    // Configura PLL para 16 MHz ? 70 MIPS (PLL x8)
    PLLFBD = 33;                   // M = 40 (PLLDIV = 38 + 2)
    CLKDIVbits.PLLPOST = 0;        // N1 = 2
    CLKDIVbits.PLLPRE = 0;         // N2 = 2
    //CLKDIVbits.DOZE = 2;            // 1:8
    CLKDIVbits.DOZEN = 0;           // Habilita modo Doze
    //__builtin_write_OSCCONH(0x01); // Inicia switch a oscilador primario
    //__builtin_write_OSCCONL(OSCCON | 0x03);
    while (OSCCONbits.COSC != 0b011); // Espera cambio de reloj
    while (OSCCONbits.LOCK != 0b01);
}


void UART1_Init(unsigned long bauder) {
    U1MODEbits.UARTEN = 0;  // Desactiva UART
    //U1BRG = (FCY / (16 * bauder))-1;
    ///
    U1MODEbits.STSEL = 0;       // 1 bit de stop
    U1MODEbits.PDSEL = 0;       // 8 bits sin paridad
    U1MODEbits.ABAUD = 0;       // Auto-Baud disabled
    U1MODEbits.BRGH = 0;        // Standard-Speed mode
    U1BRG = BRGVAL;             // Baud Rate setting for

    U1MODEbits.UARTEN = 1; // Enable UART
    U1STAbits.UTXEN = 1; // Enable UART TX
    //U1STAbits.UTXEN = 1;        // Habilita transmisión
    //U1MODEbits.UARTEN = 1;      // Habilita UART
    
    // Verificación del baud rate real
    char msg[40];
    sprintf(msg, "Baudrate config: %lu, Real: %.1f\r\n", 
            bauder, 
            (double)_XTAL_FREQ/(16*(U1BRG+1)));
    UART1_WriteString(msg);
}
void UART1_WriteChar(char c) {
    while (U1STAbits.UTXBF); // Espera si buffer lleno
    U1TXREG = c;
    //TX1 = !TX1;     // Alterna LED
}

void UART1_WriteString(const char* str) {
    while (*str) UART1_WriteChar(*str++);
}

void process_command(const char* cmd) {
    // Verificar longitud mínima (debe ser al menos "AD" + CC + VVVV = 8 caracteres)
    if (strlen(cmd) < 7) {
        UART1_WriteString("1 ERROR: Comando demasiado corto\r\n");
        return;
    }
    
    // Verificar prefijo "AD"
    if (strncmp(cmd, "AD", 2) != 0) {
        UART1_WriteString("2 ERROR: Formato incorrecto (debe comenzar con AD)\r\n");
        return;
    }
    
    // Verificar canal (debe ser dígito 0-3)
    if (cmd[2] < '0' || cmd[2] > '3') {
        UART1_WriteString("3 ERROR: Canal inválido (debe ser 0-3)\r\n");
        return;
    }
    
    // Verificar que los siguientes 4 caracteres sean dígitos (VVVV)
    for (int i = 3; i < 7; i++) {
        if (cmd[i] < '0' || cmd[i] > '9') {
            UART1_WriteString("4 ERROR: Valor no numérico\r\n");
            return;
        }
    }
    
    // Convertir valor numérico (los últimos 4 dígitos)
    int valor = atoi(&cmd[3]);
    if (valor < 0 || valor > 4095) {
        UART1_WriteString("5 ERROR: Valor fuera de rango (0-4095)\r\n");
        return;
    }
    
    // Comando válido
    char response[32];
    sprintf(response, "OK: Canal=%c, Valor=%d\r\n", cmd[2], valor);
    uint8_t channel_num = cmd[2] - '0';
    //aplicar valor del DAC
    DAC_Write(valor,channel_num);  // 50% de 4095 (0x0800)
    UART1_WriteString(response);
    return;
}


// UART1_Task modificada
void UART1_Task() {
    while(U1STAbits.URXDA) {  // Procesa todos los caracteres disponibles
        char c = U1RXREG;
        
        // Eco opcional (comentar en producción)
        //UART1_WriteChar(c);
        //char response[32];
        //sprintf(response, "\n INDEX: %i",rx_index," \n");
        //UART1_WriteString(response);
        //if (rx_index < BUFFER_SIZE - 1) {
        if (rx_index < 7) {
            rx_buffer[rx_index++] = c;
        }
        if (c == '\r' || c == '\n' || rx_index >= 8) {
            if (rx_index > 0) {
                rx_buffer[rx_index] = '\0';
                cmd_ready = 1;  // Marca que hay comando listo
            }
        }
    }
    
    // Procesar comando si está listo
    if (cmd_ready) {
        process_command((const char*)rx_buffer);
        clear_buffer();
        cmd_ready = 0;
    }
}
void PPS_Config(void) {
    __builtin_write_OSCCONL(OSCCON & 0xBF); // Desbloquea PPS (OSCCONL<6> = 0)
    

    // 2. Configurar dirección
    _TRISB11 = 0; // RB11 como salida (U1TX)
    _TRISB12 = 1; // RB12 como entrada (U1RX)
    
   // 3. Asignar funciones PPS
    RPOR4bits.RP43R = 1;    // U1TX en RP43 (RB11)
    RPINR18bits.U1RXR = 44; // U1RX en RPI44 (RB12)

    __builtin_write_OSCCONL(OSCCON | 0x40); // Bloquea PPS (OSCCONL<6> = 1)
}

// Función para limpiar el buffer
void clear_buffer() {
    rx_index = 0;
    memset((void*)rx_buffer, 0, BUFFER_SIZE);
}

// Función de depuración
void debug_print(const char* msg) {
    UART1_WriteString("[DEBUG] ");
    UART1_WriteString(msg);
    UART1_WriteString("\r\n");
}

void DAC_Init() {
    _TRISB5 = 0; // CS como salida
    _TRISB6 = 0; // CS como salida
    _TRISB7 = 0; // CS como salida
    _TRISB8 = 0; // CS como salida
    _TRISC3 = 0; // SCK como salida
    _TRISA4 = 0; // SDI como salida
    
    DAC_CS0 = 1;   // Inicialmente deseleccionado
    DAC_CS1 = 1;   // Inicialmente deseleccionado
    DAC_CS2 = 1;   // Inicialmente deseleccionado
    DAC_CS3 = 1;   // Inicialmente deseleccionado
    DAC_SCK = 0;
}

void DAC_Write(uint16_t value, uint8_t channel) {
    // Aseguramos valor de 12 bits (0-4095)
    value &= 0x0FFF;
    // Modo normal (bit 15=0, bit 14=0)
    uint16_t data = value;
    // Luego selecciona solo el canal requerido
    switch(channel) {
        case 0: DAC_CS0 = 0; break;
        case 1: DAC_CS1 = 0; break;
        case 2: DAC_CS2 = 0; break;
        case 3: DAC_CS3 = 0; break;
        default: UART1_WriteString("ERROR: Canal inválido (debe ser 0-3)\r\n");
        return;  // Canal inválido
    }
    
    
    // Envía 16 bits (MSB first)
    for(int i=15; i>=0; i--) {
        DAC_SDI = (data >> i) & 0x1;
        DAC_SCK = 1;
        __builtin_nop(); __builtin_nop();  // Pequeña pausa
        DAC_SCK = 0;
        __builtin_nop(); __builtin_nop();  // Pequeña pausa
    }
    
    DAC_CS0 = 1;     // Deselecciona DAC
    DAC_CS1 = 1;     // Deselecciona DAC
    DAC_CS2 = 1;     // Deselecciona DAC
    DAC_CS3 = 1;     // Deselecciona DAC
}

// Para 50% de salida (2048 = 0x0800)
void Set_DAC_50Percent() {
    DAC_Write(2048,0);  // 50% de 4095 (0x0800)
}

