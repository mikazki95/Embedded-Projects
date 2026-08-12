#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#define BASE 0xD0000
#define SIZE 0x1000

int main() {
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) { perror("open"); return 1; }

    void *ptr = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, BASE);
    if (ptr == MAP_FAILED) { perror("mmap"); close(fd); return 1; }

    unsigned short *regs = (unsigned short *)ptr;

    // Probaremos a escribir en el offset 0x04 (índice 2) y ver si cambia
    unsigned short valor_original = regs[2];
    unsigned short valor_prueba = 0x1234;

    printf("Offset 0x04 (índice 2) -> Original: 0x%04X\n", valor_original);
    regs[2] = valor_prueba;
    usleep(10);
    unsigned short valor_leido = regs[2];

    printf("-> Escribimos: 0x%04X\n", valor_prueba);
    printf("-> Leemos:     0x%04X\n", valor_leido);

    if (valor_leido == valor_prueba) {
        printf("✅ ¡ÉXITO! El registro 0x04 es de Lectura/Escritura (RW).\n");
    } else {
        printf("❌ El registro es de solo lectura, o tiene bits de estado.\n");
    }

    // Restauramos el valor original para no dañar la tarjeta
    regs[2] = valor_original;

    munmap(ptr, SIZE);
    close(fd);
    return 0;
}