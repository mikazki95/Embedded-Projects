#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/io.h>

// Direcciones de configuración del Super I/O SMSC (según hoja de datos)
#define SIO_CONFIG_PORT 0x3F0
#define SIO_DATA_PORT   0x3F1

// --------------------------------------------------------------
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
// 2. Función para configurar el UART1 en una dirección base
// --------------------------------------------------------------
void configurar_uart1_en(unsigned short base_addr) {
    printf("Configurando UART1 en dirección base 0x%04X...\n", base_addr);

    // --- Paso 1: Entrar en modo configuración ---
    habilitar_configuracion();

    // --- Paso 2: Seleccionar el dispositivo lógico UART1 (0x04) ---
    escribir_registro_sio(0x07, 0x04);

    // --- Paso 3: Asignar la dirección base (dos bytes) ---
    // Registro 0x60: Byte alto de la dirección
    escribir_registro_sio(0x60, (base_addr >> 8) & 0xFF);
    // Registro 0x61: Byte bajo de la dirección
    escribir_registro_sio(0x61, base_addr & 0xFF);

    // --- Paso 4: Asignar una IRQ (usaremos 4, estándar de COM1/COM3) ---
    escribir_registro_sio(0x70, 0x04);

    // --- Paso 5: Habilitar el UART1 (registro 0x30 = 0x01) ---
    escribir_registro_sio(0x30, 0x01);

    // --- Paso 6: Salir del modo configuración ---
    deshabilitar_configuracion();

    printf("UART1 mapeado en el bus x86 en 0x%04X.\n", base_addr);
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
// MAIN
// --------------------------------------------------------------
int main() {
    // --- Obtener permisos de E/S del Kernel de Linux (Requiere root) ---
    if (iopl(3) < 0) {
        perror("iopl falló. Intenta ejecutar con 'sudo'");
        return 1;
    }

    printf("=== Programa para configurar y verificar UART del FDC37C669 ===\n\n");

    // Usamos 0x3E8 (COM3) tal como lo definiste
    unsigned short base_uart = 0x3E8;  

    // 1. Configurar el hardware interno de la UART1 a través del Super I/O
    configurar_uart1_en(base_uart);

    // 2. Escribir el valor de prueba (8 bits, 1 stop, sin paridad, DLAB=1)
    escribir_lcr(base_uart, 0x83);

    // 3. Leer el registro LCR de vuelta desde el bus físico para validar
    unsigned char leido = leer_lcr(base_uart);

    // 4. Diagnóstico de resultados
    if (leido == 0x83) {
        printf("\n✅ ¡ÉXITO TOTAL! El LCR guardó el estado y respondió correctamente.\n");
        printf("El chip SMSC FDC37C669 ha mapeado la UART en 0x%04X de forma exitosa.\n", base_uart);
    } else {
        printf("\n❌ Fallo catastrófico: El LCR no retiene el valor. Leído: 0x%02X\n", leido);
        printf("Causas probables de error:\n");
        printf(" 1. El chip real de tu placa usa la dirección base secundaria: 0x370 / 0x371.\n");
        printf(" 2. El BIOS del sistema tiene un candado de hardware en las líneas LPC/ISA.\n");
    }

    return 0;
}
