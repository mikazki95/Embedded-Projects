#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/io.h>

// Estructura para definir un rango de exclusión
typedef struct {
    unsigned short inicio;
    unsigned short fin;
} rango_exclusion_t;

// Lista de rangos a excluir (todos los conocidos)
rango_exclusion_t rangos_excluidos[] = {
    {0x000, 0x00F},  // DMA Controller
    {0x020, 0x021},  // PIC
    {0x040, 0x043},  // PIT
    {0x060, 0x064},  // Keyboard
    {0x070, 0x071},  // CMOS/RTC
    {0x080, 0x09F},  // DMA Page Registers
    {0x0A0, 0x0A1},  // PIC (second)
    {0x0C0, 0x0DF},  // DMA Controller
    {0x0F0, 0x0FF},  // Math Coprocessor
    {0x1F0, 0x1F7},  // IDE Primary
    {0x200, 0x20F},  // ST16C2550
    {0x2F8, 0x2FF},  // COM2 (FDC37C669)
    {0x340, 0x35F},  // SJA1000T
    {0x370, 0x377},  // FDC37C669 (configuración)
    {0x378, 0x37F},  // Parallel Port (FDC37C669)
    {0x3F0, 0x3F7},  // Floppy + Configuración (FDC37C669)
    {0x3F8, 0x3FF},  // COM1 (FDC37C669)
    {0x3C0, 0x3DF},  // VGA (posible)
};

int esta_excluido(unsigned short direccion) {
    int num_rangos = sizeof(rangos_excluidos) / sizeof(rangos_excluidos[0]);
    for (int i = 0; i < num_rangos; i++) {
        if (direccion >= rangos_excluidos[i].inicio && direccion <= rangos_excluidos[i].fin) {
            return 1;
        }
    }
    return 0;
}

void barrido_con_exclusion() {
    unsigned short base_inicio = 0x0;//0x200;
    unsigned short base_fin = 0xFFFF;//0x3FF;
    unsigned char test_vals[] = {0xAA, 0x55};

    printf("=== BARRIDO CON PRUEBA DE AA Y 55 ===\n");
    printf("Buscando en el rango 0x%04X - 0x%04X...\n", base_inicio, base_fin);
    printf("Excluyendo rangos conocidos (FDC37C669, ST16C2550, SJA1000T, VGA, sistema, etc.)\n\n");

    for (unsigned short base = base_inicio; base <= base_fin; base += 8) {
        if (esta_excluido(base)) continue;

        int match = 1;
        for (int i = 0; i < 2; i++) {
            outb(test_vals[i], base);
            usleep(10);
            unsigned char read_val = inb(base);
            if (read_val != test_vals[i]) {
                match = 0;
                break;
            }
        }

        if (match) {
            printf("✅ Chip en 0x%04X (responde a AA y 55)\n", base);
        }
    }
}

int main() {
    if (iopl(3) < 0) {
        perror("Error: Ejecutar con 'sudo'");
        return 1;
    }

    barrido_con_exclusion();
    return 0;
}