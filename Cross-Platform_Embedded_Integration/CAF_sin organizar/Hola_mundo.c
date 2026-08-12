#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/io.h>

// Puertos de configuración del Super I/O validados por tu hardware
#define SIO_CONFIG_PORT 0x370
#define SIO_DATA_PORT   0x371

// Usaremos la dirección clásica de COM1 para la UART1
#define UART_BASE       0x3F8  

// Direcciones del mapa de registros de la UART (Offsets de la pág. 72)
#define UART_THR        (UART_BASE + 0) // Transmit Holding Register (DLAB=0)
#define UART_DLL        (UART_BASE + 0) // Divisor Latch LSB (DLAB=1)
#define UART_DLM        (UART_BASE + 1) // Divisor Latch MSB (DLAB=1)
#define UART_LCR        (UART_BASE + 3) // Line Control Register 
#define UART_LSR        (UART_BASE + 5) // Line Status Register

// ---------- Función para enviar un solo byte por la UART ----------
void enviar_byte(unsigned char dato) {
    // Monitorear el bit 5 de LSR (THRE: Transmitter Holding Register Empty)
    // Esperamos mientras el bit sea 0 (buffer ocupado)
    while ((inb(UART_LSR) & 0x20) == 0);
    
    // El buffer está vacío, enviamos el dato al THR
    outb(dato, UART_THR);
}

// ---------- Función para enviar una cadena de texto ----------
void enviar_cadena(const char *str) {
    while (*str) {
        enviar_byte((unsigned char)*str);
        str++;
    }
}

int main() {
    // 1. Solicitar privilegios de acceso al bus de E/S de Linux
    if (iopl(3) < 0) {
        perror("Error de privilegios. Ejecuta con 'sudo'");
        return 1;
    }

    printf("=== ACTIVACIÓN COMPLETA Y TRANSMISIÓN: FDC37C669 ===\n\n");

    // 2. Despertar al chip SMSC en 0x370
    outb(0x55, SIO_CONFIG_PORT);

    // 3. Seleccionar Logical Device 4 (UART1)
    outb(0x07, SIO_CONFIG_PORT);
    outb(0x04, SIO_DATA_PORT);

    // 4. Mapear UART1 en la dirección base 0x03F8 (COM1)
    outb(0x60, SIO_CONFIG_PORT);
    outb((UART_BASE >> 8) & 0xFF, SIO_DATA_PORT); // 0x03
    outb(0x61, SIO_CONFIG_PORT);
    outb(UART_BASE & 0xFF, SIO_DATA_PORT);        // 0xF8

    // 5. Asignar la línea de interrupción estándar IRQ 4
    outb(0x70, SIO_CONFIG_PORT);
    outb(0x04, SIO_DATA_PORT);

    // 6. Activar la UART eléctricamente (Registro 0x30 = 0x01)
    outb(0x30, SIO_CONFIG_PORT);
    outb(0x01, SIO_DATA_PORT);

    // 7. Cerrar de forma segura el modo configuración del Super I/O
    outb(0xAA, SIO_CONFIG_PORT);
    printf("[1] UART1 mapeada y encendida en 0x%04X.\n", UART_BASE);

    // 8. Configurar velocidad (Página 72): Forzar DLAB=1 (0x83)
    outb(0x83, UART_LCR);

    // Cargar el divisor para 4800 baudios (Divisor = 24 -> 0x0018)
    outb(0x18, UART_DLL); // Escribir LSB
    outb(0x00, UART_DLM); // Escribir MSB
    printf("[2] Velocidad configurada a 4800 Baudios.\n");

    // Fijar trama final 8N1 y apagar DLAB (0x03) para liberar buffers de datos
    outb(0x03, UART_LCR);

    // 9. Verificación final de lectura del LCR
    unsigned char verificacion_lcr = inb(UART_LCR);
    if (verificacion_lcr != 0x03) {
        printf("\n❌ Error grave: La UART no retiene la configuración de línea.\n");
        return 1;
    }
    
    printf("[3] Verificación exitosa. LCR responde con 0x03.\n");
    printf("\n🚀 EMPEZANDO TRANSMISIÓN CONTINUA... (Presiona Ctrl+C para salir)\n");

	unsigned long contador = 0;
    char buffer_mensaje[64];

    // 10. Bucle de alta velocidad con datos variables
    while (1) {
        // Creamos un mensaje que cambia en cada iteración para validar la transmisión
        snprintf(buffer_mensaje, sizeof(buffer_mensaje), "Mensaje nro: %lu - Transmitiendo OK\r\n", contador);
        
        // Enviamos la cadena formateada por el puerto serie
        enviar_cadena(buffer_mensaje); 
        
        // Imprimimos un eco local en la consola para saber que el bucle progresa
        if (contador % 50 == 0) {
            printf("  [INFO] Enviados %lu paquetes acumulados...\n", contador);
            fflush(stdout); // Forzar la salida en la terminal de Linux
        }

        contador++;
        
        // Reducimos el retraso a solo 10 milisegundos (10,000 microsegundos)
        // Esto mantendrá el Pin 79 (TXD1) operando casi a su máxima capacidad
        usleep(50000); 
    }
	

    return 0; // El programa nunca llegará aquí a menos que lo abortes
}
