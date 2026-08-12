#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#define BASE_MEMORIA_RANGO3  0xD0000//0xF00000
#define TAMANO_BLOQUE        0x1000  // 4 KB (tamaño de página)

int main() {
    printf("\n==================================================\n");
    printf("     TEST MVBCS1 VIA MMAP: ASIC MVBCS1\n");
    printf("==================================================\n");
    printf("[INFO] Abriendo /dev/mem...\n");

    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) {
        perror("❌ ERROR: No se pudo abrir /dev/mem (¿Usaste sudo?)");
        return 1;
    }

    // Mapear la página de 4 KB que contiene la dirección 0xF00000
    printf("[INFO] Mapeando memoria física en 0xF00000...\n");
    void *mem_ptr = mmap(NULL, TAMANO_BLOQUE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, BASE_MEMORIA_RANGO3);
    if (mem_ptr == MAP_FAILED) {
        perror("❌ ERROR: mmap falló (¿El kernel bloquea esta zona?)");
        close(fd);
        return 1;
    }

    // Ahora podemos acceder a la memoria directamente como un puntero
    unsigned short *mem_16 = (unsigned short *)mem_ptr;

    unsigned short dato_envio1 = 0x5555;
    unsigned short dato_envio2 = 0xAAA1;
    unsigned short dato_leido1 = 0;
    unsigned short dato_leido2 = 0;

    printf("[PASO 1] Escribiendo 0x5555 en 0xF00000...\n");
    *mem_16 = dato_envio1;
    dato_leido1 = *mem_16;

    printf("[PASO 2] Escribiendo 0xAAAA en 0xF00000...\n");
    *mem_16 = dato_envio2;
    dato_leido2 = *mem_16;

    // Reportar resultados
    printf("\n==================================================\n");
    printf("📊 RESULTADO DEL ESCANEO DE MEMORIA ALTA\n");
    printf("==================================================\n");
    printf("-> Patrón 1 | Enviado: 0x5555 | Recuperado: 0x%04X\n", dato_leido1);
    printf("-> Patrón 2 | Enviado: 0xAAAA | Recuperado: 0x%04X\n", dato_leido2);

    if (dato_leido1 == 0x5555 && dato_leido2 == 0xAAAA) {
        printf("\n✅ ¡VICTORIA! El bus de 16 bits de la tarjeta Siemens respondió impecable.\n");
    } else {
        printf("\n❌ FALLA: Los datos no coinciden. El bus devolvió basura o está flotando.\n");
    }
    printf("==================================================\n");

    munmap(mem_ptr, TAMANO_BLOQUE);
    close(fd);
    return 0;
}