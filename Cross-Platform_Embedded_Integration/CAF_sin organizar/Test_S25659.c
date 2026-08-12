#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/io.h>

// Ajusta esta base al segmento donde creas que los jumpers o el firmware ubicaron la tarjeta
#define SIEMENS_MVB_BASE  0x240  
#define MVB_REG_0         (SIEMENS_MVB_BASE + 0)
#define MVB_REG_2         (SIEMENS_MVB_BASE + 2)

void diagnostico_siemens_mvb(int segundos) {
    unsigned short leído_reg0 = 0;
    unsigned short leído_reg2 = 0;

    printf("\n==================================================\n");
    printf("  TEST DE HARDWARE: SIEMENS S25659 (MVB PC/104)   \n");
    printf("==================================================\n");
    printf("[INFO] Usando Dirección Base: 0x%03X\n", SIEMENS_MVB_BASE);
    printf("[INFO] Conecta el osciloscopio a las líneas de datos/direcciones.\n");
    printf("[INFO] Ejecutando bucle por %d segundos...\n\n", segundos);

    // Otorgar permisos de acceso a puertos I/O en x86 (requiere ejecutar como root)
    if (ioperm(SIEMENS_MVB_BASE, 16, 1) < 0) {
        perror("❌ ERROR: No se pudieron obtener permisos ioperm (¿Eres root?)");
        return;
    }

    for (int i = 0; i < segundos; i++) {
        // 1. Forzamos escrituras y lecturas alternadas de 16 bits para generar actividad física
        outw(0x5555, MVB_REG_0); 
        usleep(10);
        leído_reg0 = inw(MVB_REG_0);

        outw(0xAAAA, MVB_REG_0);
        usleep(10);
        leído_reg2 = inw(MVB_REG_2); // Leemos el registro de estado alterno

        // Imprimir en pantalla para ver actividad en tiempo real
        printf("[Ciclo %02d] Reg0 Leyó: 0x%04X | Reg2 Leyó: 0x%04X\n", i + 1, leído_reg0, leído_reg2);
        
        // Si el bus está flotando o no hay tarjeta en esa dirección, 
        // comúnmente leerás 0xFFFF de forma sostenida en x86.
        if (leído_reg0 == 0xFFFF && leído_reg2 == 0xFFFF) {
            printf("  ⚠ Alerta: Bus flotante (0xFFFF). El chip NO está respondiendo en esta base.\n");
        }

        usleep(990000); // Pausa cercana a 1 segundo para el siguiente ciclo
    }

    // Cerrar permisos
    ioperm(SIEMENS_MVB_BASE, 16, 0);
    printf("\n[OK] Bucle de diagnóstico completado.\n");
}

int main() {
    // Corre la prueba por 15 segundos para darte tiempo de revisar la pantalla y el osciloscopio
    diagnostico_siemens_mvb(15); 
    return 0;
}
