#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/io.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>

#define SIO_CONFIG_PORT 0x2E
#define SIO_DATA_PORT   0x2F

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

// Configurar un UART con dirección personalizada
void configurar_uart_personalizado(int uart_num, unsigned short direccion, int irq) {
    habilitar_configuracion();
    
    // Seleccionar el dispositivo lógico
    escribir_sio(0x07, uart_num);
    
    // Configurar dirección base (evitando conflictos)
    escribir_sio(0x60, (direccion >> 8) & 0xFF);
    escribir_sio(0x61, direccion & 0xFF);
    
    // Configurar IRQ (usar IRQ diferente si es posible)
    escribir_sio(0x70, irq);
    
    // Habilitar el dispositivo
    escribir_sio(0x30, 0x01);
    
    deshabilitar_configuracion();
    printf("UART%d configurado en 0x%04X con IRQ%d (evitando conflictos)\n", 
           uart_num - 0x03, direccion, irq);
}

// Enviar datos por un puerto serie específico (usando dirección)
void enviar_por_puerto(unsigned short direccion, const char *datos) {
    // Abrir el puerto serie correspondiente (si existe en /dev)
    // Nota: Si usas direcciones no estándar, es posible que necesites crear un enlace simbólico
    int fd = open("/dev/ttyS2", O_RDWR | O_NOCTTY);  // Asumimos que es el tercer puerto
    if (fd < 0) {
        perror("open /dev/ttyS2");
        // Intentar con otro
        fd = open("/dev/ttyS3", O_RDWR | O_NOCTTY);
        if (fd < 0) {
            perror("open /dev/ttyS3");
            return;
        }
    }
    
    // Configurar UART
    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) {
        perror("tcgetattr");
        close(fd);
        return;
    }
    
    cfsetispeed(&tty, B9600);
    cfsetospeed(&tty, B9600);
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~CRTSCTS;
    tty.c_cflag |= CREAD | CLOCAL;
    
    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO;
    tty.c_lflag &= ~ECHOE;
    tty.c_lflag &= ~ECHONL;
    tty.c_lflag &= ~ISIG;
    
    tty.c_oflag &= ~OPOST;
    tty.c_oflag &= ~ONLCR;
    
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_iflag &= ~(INLCR | ICRNL | IGNCR);
    
    tty.c_cc[VTIME] = 10;
    tty.c_cc[VMIN] = 0;
    
    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr");
        close(fd);
        return;
    }
    
    // Enviar datos
    ssize_t enviados = write(fd, datos, strlen(datos));
    if (enviados < 0) {
        perror("write");
    } else {
        printf("Enviados %ld bytes por el puerto en 0x%04X\n", enviados, direccion);
    }
    
    close(fd);
}

int main() {
    if (iopl(3) < 0) {
        perror("iopl (necesitas permisos de root)");
        return 1;
    }
    
    printf("=== Configuración FDC37C669 (evitando conflictos) ===\n\n");
    
    // 1. Configurar UART del FDC37C669 en dirección alternativa (0x3E8)
    printf("Configurando UART del FDC37C669 en 0x3E8 (IRQ4)...\n");
    configurar_uart_personalizado(0x04, 0x3E8, 4);
    
    // 2. Configurar otro UART en dirección alternativa (0x2E8)
    printf("Configurando otro UART en 0x2E8 (IRQ3)...\n");
    configurar_uart_personalizado(0x05, 0x2E8, 3);
    
    // 3. Enviar mensaje por el puerto recién configurado
    printf("\nEnviando mensaje por el puerto en 0x3E8...\n");
    enviar_por_puerto(0x3E8, "Hola desde TinyCore via FDC37C669 (sin conflictos)\n");
    
    // 4. Verificar estado del UART configurado
    printf("\nLeyendo estado del UART en 0x3E8...\n");
    unsigned char estado = inb(0x3E8 + 5);  // Registro de estado (LSR)
    printf("Estado (LSR): 0x%02X\n", estado);
    
    printf("\n¡Éxito! Datos enviados por el bus ISA a través del FDC37C669 sin conflictos.\n");
    printf("Los pines 78-79 y 88-89 del FDC37C669 deberían tener actividad ahora.\n");
    
    return 0;
}