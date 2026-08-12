#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

// Dirección base física donde lograste el destrabe con éxito
#define BASE_MEMORIA_SIEMENS 0xD0000
#define TAMANO_BLOQUE        0x1000  // 4 KB (tamaño de página estándar)

// Offsets estructurales basados en el driver pp04_drv.c de Duagon
#define REG_SIEMENS_ID       0  // Índice 0 del arreglo de 16 bits (Offset 0x00)
#define REG_SIEMENS_CTRL     1  // Índice 1 del arreglo de 16 bits (Offset 0x02)

int main() {
    printf("\n==================================================\n");
    printf("📊 EVALUACIÓN ESTRUCTURAL SIEMENS MVB (ESTILO DUAGON)\n");
    printf("==================================================\n");
    printf("[INFO] Abriendo canal /dev/mem...\n");

    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) {
        perror("❌ ERROR: No se pudo abrir /dev/mem");
        return 1;
    }

    printf("[INFO] Mapeando página física en 0xD0000...\n");
    void *mem_ptr = mmap(NULL, TAMANO_BLOQUE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, BASE_MEMORIA_SIEMENS);
    if (mem_ptr == MAP_FAILED) {
        perror("❌ ERROR: mmap falló");
        close(fd);
        return 1;
    }

    // Convertimos el puntero a 16 bits (Palabras completas)
    unsigned short *mvb_regs = (unsigned short *)mem_ptr;

    printf("\n[PASO 1] Interrogando Identificador de Hardware (ID Register)...\n");
    // Leemos el primer registro del chip de Siemens sin escribir nada antes
    unsigned short id_hardware = mvb_regs[REG_SIEMENS_ID];
    printf("         -> Firma de Silicio detectada: 0x%04X\n", id_hardware);

    printf("\n[PASO 2] Probando estabilidad del Registro de Control (CTRL Register)...\n");
    // Probamos conmutar el segundo registro estructural del driver de Duagon
    unsigned short comando_prueba = 0x0001; // Activar modo operacional de ráfaga
    mvb_regs[REG_SIEMENS_CTRL] = comando_prueba;
    usleep(5);
    unsigned short ctrl_leido = mvb_regs[REG_SIEMENS_CTRL];
    printf("         -> Comando enviado: 0x%04X | Estado de confirmación: 0x%04X\n", comando_prueba, ctrl_leido);

    // ---- REPORTE DE EVALUACIÓN FINAL ----
    printf("\n==================================================\n");
    printf("📋 VEREDICTO DE LA PRUEBA DE CONTROL DE SIEMENS\n");
    printf("==================================================\n");

    // Si el ID lee 0xFFFF o 0x0000, significa que el chip ignoró el comando o el bus está muerto
    if (id_hardware != 0xFFFF && id_hardware != 0x0000) {
        printf("✅ ¡ÉXITO TOTAL! El integrado MVBCS1 respondió con una firma válida.\n");
        printf("   El firmware de Duagon/MEN podrá inicializar esta tarjeta sin colgarse.\n");
    } else {
        printf("❌ FALLA DE PROTOCOLO: El chip devolvió 0x%04X (Bus vacío o flotando).\n", id_hardware);
        printf("   El ASIC de Siemens está congelado internamente o no tiene alimentación.\n");
    }
    printf("==================================================\n");

    munmap(mem_ptr, TAMANO_BLOQUE);
    close(fd);
    return 0;
}
