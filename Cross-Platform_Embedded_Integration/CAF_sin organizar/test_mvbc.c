#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/io.h>

void barrido_mvbc() {
    unsigned short base_inicio = 0x200;
    unsigned short base_fin = 0x3FF;
    unsigned char test_val = 0x55;

    printf("=== BARRIDO PARA MVBC01 A TRAVÉS DEL FPGA ===\n");
    printf("Buscando en el rango 0x%03X - 0x%03X...\n", base_inicio, base_fin);

    for (unsigned short base = base_inicio; base <= base_fin; base += 8) {
        // Escribir valor de prueba
        outb(test_val, base);
        usleep(10);
        unsigned char read_val = inb(base);

        if (read_val == test_val) {
            printf("✅ Posible MVBC01/FPGA en 0x%03X (respuesta: 0x%02X)\n", base, read_val);
        }
    }
}

int main() {
    if (iopl(3) < 0) {
        perror("Error: Ejecutar con 'sudo'");
        return 1;
    }

    barrido_mvbc();
    return 0;
}