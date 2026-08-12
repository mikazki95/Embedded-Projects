#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/io.h>

void escanear_todo_el_bus_io() {
    printf("\n========================================================\n");
    printf("🔍 INICIANDO ESCÁNER DE PUERTOS DE E/S (RANGO 0x100 - 0x3FF)\n");
    printf("========================================================\n");
    printf("Buscando qué dirección física responde al espejo de datos...\n\n");
    printf("DIRECCIÓN\t| ESC_1: 0x55 -> LEY_1\t| ESC_2: 0xAA -> LEY_2\t| ESTADO\n");
    printf("--------------------------------------------------------\n");

    int encontrados = 0;
    unsigned char prueba1 = 0x55;
    unsigned char prueba2 = 0xAA;

    // Escaneamos el rango clásico de periféricos de la arquitectura PC-104 / ISA
    for (unsigned short puerto = 0x100; puerto <= 0x3FF; puerto++) {
        
        // OMITIR PUERTOS CRÍTICOS DE LA PC PARA EVITAR CONGELAR EL ARRANQUE DEL SO
        if (puerto >= 0x000 && puerto <= 0x1F0) continue; 
        if (puerto == 0x370 || puerto == 0x371) continue; // Super I/O
        if (puerto >= 0x3F8 && puerto <= 0x3FF) continue; // COM1 estándar

        // 1. Guardar el valor original del puerto por seguridad
        unsigned char valor_original = inb(puerto);

        // 2. Prueba de Espejo 1: Escribir y leer 0x55
        outb(prueba1, puerto);
        usleep(2); // Retardo corto de estabilidad
        unsigned char leido1 = inb(puerto);

        // 3. Prueba de Espejo 2: Escribir y leer 0xAA
        outb(prueba2, puerto);
        usleep(2); 
        unsigned char leido2 = inb(puerto);

        // 4. Restaurar el valor original del puerto inmediatamente
        outb(valor_original, puerto);

        // 5. Evaluar si encontramos un registro de Lectura/Escritura real
        if (leido1 == prueba1 && leido2 == prueba2) {
            printf("  0x%03X\t| 0x55   -> 0x%02X\t| 0xAA   -> 0x%02X\t| ✅ REGISTRO L/E\n", puerto, leido1, leido2);
            encontrados++;
        }
    }

    printf("--------------------------------------------------------\n");
    printf("📋 ESCÁNER TERMINADO: Se encontraron %d registros activos.\n", encontrados);
    printf("========================================================\n");
}

int main() {
    // Solicitar permisos de acceso a hardware de I/O en Linux
    if (iopl(3) < 0) {
        perror("❌ Error grave: Requiere ejecutar con 'sudo' o privilegios root");
        return 1;
    }

    // Ejecutar el escáner automático directamente
    escanear_todo_el_bus_io();

    return 0;
}
