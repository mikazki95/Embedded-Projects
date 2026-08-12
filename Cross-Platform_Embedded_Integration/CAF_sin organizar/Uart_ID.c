#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/io.h>
#include <time.h>

// Puertos de configuración del Super I/O validados por tu hardware
#define SIO_CONFIG_PORT 0x370
#define SIO_DATA_PORT   0x371

// Usaremos la dirección clásica de COM3 para la UART1
#define UART3_BASE       0x3E8  

// Usaremos la dirección clásica de COM4 para la UART2
#define UART4_BASE       0x2E8 



// 1. Funciones para el modo de configuración del Super I/O
// --------------------------------------------------------------

// Entrar en modo configuración (SMSC FDC37C669 requiere SOLO UNA VEZ el byte 0x55)
void habilitar_configuracion() {
    outb(0x55, SIO_CONFIG_PORT); 
}

// Salir del modo configuración (escribir 0xAA)
void deshabilitar_configuracion() {
    outb(0xAA, SIO_CONFIG_PORT);
}

// Escribir un valor en un registro de configuración del Super I/O
void escribir_registro_sio(unsigned char registro, unsigned char valor) {
    outb(registro, SIO_CONFIG_PORT);  // Seleccionar el registro
    outb(valor, SIO_DATA_PORT);       // Escribir el valor
}

// Leer un valor de un registro de configuración del Super I/O
unsigned char leer_registro_sio(unsigned char registro) {
    outb(registro, SIO_CONFIG_PORT);  // Seleccionar el registro
    return inb(SIO_DATA_PORT);        // Leer el valor
}

// --------------------------------------------------------------
// 3. Función para escribir en el LCR del UART
// --------------------------------------------------------------
void escribir_lcr(unsigned short base_addr, unsigned char valor) {
    unsigned short lcr_addr = base_addr + 3;  // El LCR siempre está en base+3
    printf("Escribiendo 0x%02X en el LCR (dirección 0x%04X)...\n", valor, lcr_addr);
    outb(valor, lcr_addr);
}

// --------------------------------------------------------------
// 4. Función para leer el LCR del UART (y verificar)
// --------------------------------------------------------------
unsigned char leer_lcr(unsigned short base_addr) {
    unsigned short lcr_addr = base_addr + 3;
    unsigned char valor = inb(lcr_addr);
    printf("Valor leído del LCR (0x%04X): 0x%02X\n", lcr_addr, valor);
    return valor;
}

// --------------------------------------------------------------
// 2. Función para configurar el UART1 en una dirección base
// --------------------------------------------------------------
int configurar_uart1_en(uint base_addr) {
    habilitar_configuracion();
    printf("Configurando UART1 en dirección base 0x%02X...\n", base_addr);

    // --- Paso 2: Seleccionar el dispositivo lógico UART1 (0x24) ---
    escribir_registro_sio(0x24, base_addr);
    
    // --- PAUSA DE DEBUEGO (0.1 SEGUNDOS) ---
    usleep(100000); 

    // --- LECTURA DE VERIFICACIÓN ---
    printf("[DEBUG] Verificando registro UART1...\n");
    unsigned char reg_24_validar = leer_registro_sio(0x24);
    
    if (reg_24_validar == base_addr) { 
        printf("[OK] Confirmado: CR24 retiene el valor 0x%02X con éxito.\n", reg_24_validar);
        //return 1; // Retorna Éxito
    } else {
        printf("[ER] Error de bus: CR24 leyó 0x%02X (Se esperaba 0x%02X).\n", reg_24_validar, base_addr);
        return 0; // Retorna Falla
    }
    deshabilitar_configuracion();
    printf("[UART-1] Activando bit DLAB en el LCR...\n");
    escribir_lcr(UART3_BASE, 0x83); 
    
    // Pausa de 0.1 seg para debugeo
    usleep(100000); 

    // Verificamos la lectura
    unsigned char lcr_validar = leer_lcr(UART3_BASE);
    if (lcr_validar == 0x83) {
        printf("[UART-OK] LCR retiene el valor 0x83 con éxito. Candado de velocidad abierto.\n");
        //return 1; // Retorna Éxito
    } else {
        printf("[UART-ER] Error: LCR devolvió 0x%02X. Abortando.\n", lcr_validar);
        return 0;
    }
    // ===================================================================
    // PASOS COMPLEMENTARIOS OBLIGATORIOS PARA REAVIVAR EL PIN FÍSICO
    // ===================================================================
    
    // PASO A: Cargar Divisor LSB para 4800 baudios (Valor = 24 -> 0x18)
    printf("[UART-1] Cargando Divisor LSB (0x18) en la dirección 0x%04X...\n", UART3_BASE);
    outb(0x18, UART3_BASE + 0); // Modifica el registro DLL
    
    // PASO B: Cargar Divisor MSB para 4800 baudios (Valor = 0 -> 0x00)
    printf("[UART-1] Cargando Divisor MSB (0x00) en la dirección 0x%04X...\n", UART3_BASE + 1);
    outb(0x00, UART3_BASE + 1); // Modifica el registro DLM
    
    // PASO C: Fijar trama 8N1 y APAGAR el bit DLAB (Valor = 0x03)
    printf("[UART-1] Fijando trama final 8N1 y cerrando DLAB...\n");
    escribir_lcr(UART3_BASE, 0x03); // El bit 7 se pone en 0. Libera el THR para datos.
    
    // Verificación final del estado operativo de la USART
    unsigned char lcr_final = leer_lcr(UART3_BASE);
    if (lcr_final == 0x03) {
        printf("[UART-READY] UART1 configurada a 4800 8N1 de forma exitosa. Lista para transmitir.\n");
        return 1; // Retorna Éxito real de transmisión
    } else {
        printf("[UART-ER] Error: El LCR no regresó a modo normal (Leyó: 0x%02X).\n", lcr_final);
        return 0;
    }
}

// --------------------------------------------------------------
// 2. Función para configurar el UART2 en una dirección base
// --------------------------------------------------------------
int configurar_uart2_en(uint base_addr) {
    habilitar_configuracion();
    printf("Configurando UART2 en dirección base 0x%02X...\n", base_addr);

    // --- Paso 2: Seleccionar el dispositivo lógico UART1 (0x24) ---
    escribir_registro_sio(0x25, base_addr);
    
    // --- PAUSA DE DEBUEGO (0.1 SEGUNDOS) ---
    usleep(100000); 

    // --- LECTURA DE VERIFICACIÓN ---
    printf("[DEBUG] Verificando registro UART2...\n");
    unsigned char reg_25_validar = leer_registro_sio(0x25);
    
    if (reg_25_validar == base_addr) { 
        printf("[OK] Confirmado: CR25 retiene el valor 0x%02X con éxito.\n", reg_25_validar);
        //return 1; // Retorna Éxito
    } else {
        printf("[ER] Error de bus: CR25 leyó 0x%02X (Se esperaba 0x%02X).\n", reg_25_validar, base_addr);
        return 0; // Retorna Falla
    }
    deshabilitar_configuracion();

    printf("[UART-1] Activando bit DLAB en el LCR...\n");
    escribir_lcr(UART4_BASE, 0x83); 
    
    // Pausa de 0.1 seg para debugeo
    usleep(100000); 

    // Verificamos la lectura
    unsigned char lcr_validar = leer_lcr(UART4_BASE);
    if (lcr_validar == 0x83) {
        printf("[UART-OK] LCR retiene el valor 0x83 con éxito. Candado de velocidad abierto.\n");
        //return 1; // Retorna Éxito
    } else {
        printf("[UART-ER] Error: LCR devolvió 0x%02X. Abortando.\n", lcr_validar);
        return 0;
    }
    // ===================================================================
    // PASOS COMPLEMENTARIOS OBLIGATORIOS PARA REAVIVAR EL PIN FÍSICO
    // ===================================================================
    
    // PASO A: Cargar Divisor LSB para 4800 baudios (Valor = 24 -> 0x18)
    printf("[UART-2] Cargando Divisor LSB (0x18) en la dirección 0x%04X...\n", UART4_BASE);
    outb(0x18, UART4_BASE + 0); // Modifica el registro DLL
    
    // PASO B: Cargar Divisor MSB para 4800 baudios (Valor = 0 -> 0x00)
    printf("[UART-1] Cargando Divisor MSB (0x00) en la dirección 0x%04X...\n", UART4_BASE + 1);
    outb(0x00, UART4_BASE + 1); // Modifica el registro DLM
    
    // PASO C: Fijar trama 8N1 y APAGAR el bit DLAB (Valor = 0x03)
    printf("[UART-1] Fijando trama final 8N1 y cerrando DLAB...\n");
    escribir_lcr(UART4_BASE, 0x03); // El bit 7 se pone en 0. Libera el THR para datos.
    
    // Verificación final del estado operativo de la USART
    unsigned char lcr_final = leer_lcr(UART4_BASE);
    if (lcr_final == 0x03) {
        printf("[UART-READY] UART2 configurada a 4800 8N1 de forma exitosa. Lista para transmitir.\n");
        return 1; // Retorna Éxito real de transmisión
    } else {
        printf("[UART-ER] Error: El LCR no regresó a modo normal (Leyó: 0x%02X).\n", lcr_final);
        return 0;
    }
}

// --------------------------------------------------------------
// 5. Función para enviar un solo byte por la UART (Modo Polling)
// --------------------------------------------------------------
void transmitir_byte(unsigned short base_addr, unsigned char dato) {
    unsigned short lsr_addr = base_addr + 5; // El LSR siempre está en Base + 5
    
    // Monitorear el Bit 5 (0x20). Esperamos mientras el bit sea 0 (Buffer ocupado)
    while ((inb(lsr_addr) & 0x20) == 0);
    
    // El buffer está libre, enviamos el dato al THR (Base + 0)
    outb(dato, base_addr + 0);
}

// --------------------------------------------------------------
// 6. Función para enviar una cadena de texto por la UART
// --------------------------------------------------------------
void transmitir_cadena(unsigned short base_addr, const char *cadena) {
    while (*cadena) {               // Mientras no lleguemos al final de la cadena
        transmitir_byte(base_addr, (unsigned char)*cadena);  // Enviar cada carácter
        cadena++;                   // Avanzar al siguiente carácter
    }
}

// --------------------------------------------------------------
// MAIN
// 


int main() {
    // Elevar privilegios en Tiny Linux
    if (iopl(3) < 0) {
        perror("Error grave: Requiere ejecutar con 'sudo' o privilegios root");
        return 1;
    }

    unsigned char id_leido = 0;
    uint UART3 = (UART3_BASE & 0xFF8) >> 2;
    uint UART4 = (UART4_BASE & 0xFF8) >> 2;
    printf("========================================================\n");
    printf("Prueba de UART3 y USART4 (FDC37C669) 📊\n");
    printf("========================================================\n\n");
    printf("[1] Despertando chip en 0x3F0...\n");
    // --- Paso 1: Entrar en modo configuración ---
    habilitar_configuracion();
    
       // El registro 0x0D en el FDC37C669 contiene el Device ID de fábrica
    id_leido = leer_registro_sio(0x0D);
    if (id_leido == 0x03)
    {
        printf("[2] ID correcto...\n");
    }
    else
    {
        printf("[ER] Error al leer ID termina prueba ...\n");
        deshabilitar_configuracion();
        return 1;
    }
    deshabilitar_configuracion();
    printf("[3] Preparar el Usart3...\n");
    if (configurar_uart1_en(UART3) == 1) 
    {
        printf("[4] Configuración de UART1 exitosa en hardware.\n");       
        // Aquí ya puedes continuar de frente con:
        // escribir_lcr(UART3_BASE, 0x83); -> Configurar Baudios
    } 
    else 
    {
        printf("[ER] Falla catastrófica al configurar la dirección de UART1. Terminando.\n");
        deshabilitar_configuracion(); // Cerramos el chip antes de abortar
        return 1;
    }
    printf("[5] Preparar el Usart4...\n");
    
    if (configurar_uart2_en(UART4) == 1) 
    {
        printf("[6] Configuración de UART2 exitosa en hardware.\n");       
        // Aquí ya puedes continuar de frente con:
        // escribir_lcr(UART3_BASE, 0x83); -> Configurar Baudios
    } 
    else 
    {
        printf("[ER] Falla catastrófica al configurar la dirección de UART2. Terminando.\n");
        deshabilitar_configuracion(); // Cerramos el chip antes de abortar
        return 1;
    }
    printf("[7] Configuracion exitosa...\n");
//////////////////////////fin de configuracion//////////////////////////////
    printf("\n🚀 EMPEZANDO TRANSMISIÓN CONTINUA PARA OSCILOSCOPIO... 🚀\n");
    printf("Conecta la sonda en el Pin 79 (UART1) o Pin 72 (UART2).\n");
    printf("Presiona Ctrl+C para detener la prueba.\n\n");

    unsigned long contador = 0;

        while (1) {
        // Enviar "Hola mundo" por UART1 (Pin 79)
        transmitir_cadena(UART3_BASE, "Hola mundo desde UART1 (0x3E8)\r\n");
        
        // Enviar "Hola mundo" por UART2 (Pin 72)
        transmitir_cadena(UART4_BASE, "Hola mundo desde UART2 (0x2E8)\r\n");

        contador++;
        
        // Imprimimos un eco local cada 10 mensajes para saber que el software avanza
        if (contador % 10 == 0) {
            printf("  [INFO] %lu mensajes enviados...\n", contador);
            fflush(stdout);
        }

        // Pequeña pausa de 1 segundo para no saturar
        sleep(1); 
        //usleep(5000); 
    }
    
    return 0;
}
