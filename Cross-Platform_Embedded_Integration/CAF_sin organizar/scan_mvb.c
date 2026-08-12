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

    printf("Escaneando offsets en 0xD0000 (palabras de 16 bits)...\n");
    for (int i = 0; i < 32; i++) {
        unsigned short val = regs[i];
        // Si no es 0xFFFF (vacío) ni 0x0000 (nulo), es un registro vivo
        if (val != 0xFFFF && val != 0x0000) {
            printf("Offset 0x%02X (índice %d) -> 0x%04X\n", i*2, i, val);
        }
    }

    munmap(ptr, SIZE);
    close(fd);
    return 0;
}