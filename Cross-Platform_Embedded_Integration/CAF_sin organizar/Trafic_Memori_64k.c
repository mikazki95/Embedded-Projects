#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#define BASE 0xD0000
#define SIZE 0x10000 // 64 KB
#define STEP 2       // Palabras de 16 bits

int main() {
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) { perror("open"); return 1; }

    void *ptr = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, BASE);
    if (ptr == MAP_FAILED) { perror("mmap"); close(fd); return 1; }

    unsigned short *mem = (unsigned short *)ptr;
    unsigned short test_vals[] = {0xAAAA, 0x5555};
    int errors = 0;

    printf("=== PRUEBA DE BUS DE DATOS EN TRAFFIC MEMORY (0xD0000-0xDFFFF) ===\n\n");

    for (int t = 0; t < 2; t++) {
        unsigned short val = test_vals[t];
        printf("Probando patrón 0x%04X...\n", val);
        // Escribir
        for (int i = 0; i < SIZE / STEP; i++) {
            mem[i] = val;
        }
        // Leer y verificar
        for (int i = 0; i < SIZE / STEP; i++) {
            if (mem[i] != val) {
                printf("  ❌ Error en offset 0x%04X: esperado 0x%04X, leído 0x%04X\n", i*STEP, val, mem[i]);
                errors++;
                break; // No saturar la salida
            }
        }
        if (errors == 0) {
            printf("  ✅ Patrón 0x%04X verificado correctamente.\n", val);
        }
    }

    printf("\n--- Prueba completada. Errores encontrados: %d ---\n", errors);

    munmap(ptr, SIZE);
    close(fd);
    return 0;
}