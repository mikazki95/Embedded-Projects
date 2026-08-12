#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#define PUERTO_SERIE "/dev/ttyS1"

int main() {
    printf("========================================================\n");
    printf("📊 EXCITANDO EXCLUSIVAMENTE %s A 9600 BAUDIOS\n", PUERTO_SERIE);
    printf("========================================================\n");
    printf("Inyectando ráfagas continuas del comando Query (10 Bytes)...\n");
    printf("Conecta tu osciloscopio en Pin 31 (Tx2), Pin 32 (Rx2) o Pin 35 (CTS2).\n");
    printf("Presiona Ctrl+C para detener el bucle y salir.\n\n");

    // 1. Abrir el puerto en modo Lectura/Escritura, No Bloqueante
    int fd = open(PUERTO_SERIE, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0) {
        perror("❌ Error grave: No se pudo abrir /dev/ttyS1 (¿Usaste sudo?)");
        return 1;
    }

    // 2. Configurar termios estructural de forma rígida a 9600 8N1
    struct termios opciones;
    tcgetattr(fd, &opciones);

    cfsetispeed(&opciones, B9600);
    cfsetospeed(&opciones, B9600);

    opciones.c_cflag &= ~PARENB;        // Sin paridad
    opciones.c_cflag &= ~CSTOPB;        // 1 bit de parada
    opciones.c_cflag &= ~CSIZE;         
    opciones.c_cflag |= CS8;            // 8 bits de datos

    // APAGAR EL CONTROL DE FLUJO POR HARDWARE (RTS/CTS)
    // Esto obliga a la CPU a mandar los datos aunque el pin 35 (CTS2) esté dañado
    opciones.c_cflag &= ~CRTSCTS;
    opciones.c_cflag |= (CLOCAL | CREAD);

    // Modo RAW para transmisión limpia de bytes binarios puros
    opciones.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    opciones.c_iflag &= ~(IXON | IXOFF | IXANY);
    opciones.c_oflag &= ~OPOST;

    tcsetattr(fd, TCSANOW, &opciones);

    // Paquete oficial de ID Request (SmartSet/Philips)
    unsigned char paquete[] = {0x55, 0x63, 0x49, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAA};


    // 3. Bucle infinito de excitación continua
    while(1) {
        // Forzar la escritura de los 10 bytes en el cable de transmisión
        write(fd, paquete, 10);
        
        // Limpiar el buffer inmediatamente para que no se sature de basura residual
        tcflush(fd, TCIOFLUSH);

        // Un pequeño retardo de 20 milisegundos entre ráfagas.
        // Es lo bastante lento para que el chip U105 respire,
        // pero lo bastante rápido para que tu osciloscopio pinte un tren estable.
        usleep(20000); 
    }

    close(fd);
    return 0;
}
