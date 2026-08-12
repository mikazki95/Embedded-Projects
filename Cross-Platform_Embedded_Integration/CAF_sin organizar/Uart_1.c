#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/io.h>
#include <string.h>

#define SIO_CONFIG_PORT 0x2E
#define SIO_DATA_PORT   0x2F

// Funciones para Super I/O
void escribir_sio(unsigned char registro, unsigned char valor) {
    outb(registro, SIO_CONFIG_PORT);
    outb(valor, SIO_DATA_PORT);
}

unsigned char leer_sio(unsigned char registro) {
    outb(registro, SIO_CONFIG_PORT);
    return inb(SIO_DATA_PORT);
}

void habilitar_configuracion() {
    outb(0x87, SIO_CONFIG_PORT);
    outb(0x01, SIO_CONFIG_PORT);
    outb(0x55, SIO_CONFIG_PORT);
    outb(0x55, SIO_CONFIG_PORT);
}

void deshabilitar_configuracion() {
    outb(0xAA, SIO_CONFIG_PORT);
}

void configurar_uart_personalizado(int uart_num, unsigned short direccion, int irq) {
    habilitar_configuracion();
    escribir_sio(0x07, uart_num);
    escribir_sio(0x60, (direccion >> 8) & 0xFF);
    escribir_sio(0x61, direccion & 0xFF);
    escribir_sio(0x70, irq);
    escribir_sio(0x30, 0x01);
    deshabilitar_configuracion();
    printf("UART%d configurado en 0x%04X con IRQ%d\n", uart_num - 0x03, direccion, irq);
}

// Enviar un carácter por el UART (acceso directo a puertos)
void enviar_caracter(unsigned short base, unsigned char c) {
    // Esperar a que el buffer de transmisión esté vacío (bit 5 del LSR)
    while ((inb(base + 5) & 0x20) == 0) {
        // Pequeña pausa para no saturar el bus
        usleep(1);
    }
    // Escribir el carácter en el registro de transmisión
    outb(c, base);
}

// Enviar una cadena completa
void enviar_cadena(unsigned short base, const char *cadena) {
    while (*cadena) {
        enviar_caracter(base, *cadena);
        cadena++;
    }
}

// Inicializar el UART (9600, 8N1)
void inicializar_uart(unsigned short base) {
    // Deshabilitar interrupciones
    outb(0x00, base + 1);
    
    // Configurar velocidad (9600 bps)
    outb(0x80, base + 3);  // Activar DLAB
    outb(0x0C, base);      // Divisor bajo (9600 = 115200/12)
    outb(0x00, base + 1);  // Divisor alto
    outb(0x03, base + 3);  // 8N1, DLAB desactivado
    
    // Habilitar FIFO y limpiar buffers
    outb(0xC7, base + 2);  // Activar FIFO, limpiar RX y TX
    
    // Configurar RTS/DTR (opcional)
    outb(0x03, base + 4);  // Activar RTS y DTR
}

int main() {
    if (iopl(3) < 0) {
        perror("iopl (necesitas permisos de root)");
        return 1;
    }
    
    printf("=== Programa directo para FDC37C669 ===\n\n");
    
    // 1. Configurar UART1 en 0x3E8
    printf("Configurando UART en 0x3E8...\n");
    configurar_uart_personalizado(0x04, 0x3E8, 4);
    
    // 2. Inicializar el UART
    printf("Inicializando UART en 0x3E8...\n");
    inicializar_uart(0x3E8);
    
    // 3. Enviar mensaje
    printf("\nEnviando mensaje por 0x3E8...\n");
    enviar_cadena(0x3E8, "Hola desde TinyCore via FDC37C669 (directo)\r\n");
    
    // 4. Verificar estado
    printf("\nEstado del UART (LSR): 0x%02X\n", inb(0x3E8 + 5));
    
    printf("\n¡Datos enviados directamente por los puertos ISA!\n");
    printf("Verifica actividad en los pines 78-79 (SOUT1/SIN1) del FDC37C669.\n");
    
    return 0;
}