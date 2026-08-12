#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/io.h>

// ================================================================
// 1. DEFINICIONES GLOBALES
// ================================================================
#define SIO_CONFIG_PORT 0x370
#define SIO_DATA_PORT   0x371
#define UART_BASE_1     0x200
#define UART_BASE_2     0x208
#define SJA_BASE        0x340
#define UART3_BASE      0x3E8
#define UART4_BASE      0x2E8

// ================================================================
// 2. PROTOTIPOS DE FUNCIONES (para que el orden no importe)
// ================================================================
void corrimiento_binario_analisis(unsigned short base);
void pausa();
void pausa_con_mensaje(const char *mensaje);
void habilitar_configuracion();
void deshabilitar_configuracion();
void escribir_registro_sio(unsigned char registro, unsigned char valor);
unsigned char leer_registro_sio(unsigned char registro);
int configurar_uart1_en(uint base_addr, int modo_silencioso);
int configurar_uart2_en(uint base_addr, int modo_silencioso);
int test_fdc37c669();
int test_st16c2550(unsigned short base);
int test_sja1000t(unsigned short base);
int test_saa7130hl();
void diagnosticar_fdc37c669();
void diagnosticar_st16c2550();
void diagnosticar_sja1000t();
void diagnosticar_saa7130hl();
void mostrar_resultados_generales();

// ================================================================
// 3. FUNCIONES DE PAUSA
// ================================================================
void pausa() {
    printf("\nPresiona cualquier tecla para continuar...");
    getchar();
}

void pausa_con_mensaje(const char *mensaje) {
    printf("\n%s\n", mensaje);
    printf("Presiona cualquier tecla para continuar...");
    getchar();
}

// ================================================================
// 4. FUNCIONES DE CONFIGURACIÓN DEL SUPER I/O (FDC37C669)
// ================================================================
void habilitar_configuracion() {
    outb(0x55, SIO_CONFIG_PORT);
}

void deshabilitar_configuracion() {
    outb(0xAA, SIO_CONFIG_PORT);
}

void escribir_registro_sio(unsigned char registro, unsigned char valor) {
    outb(registro, SIO_CONFIG_PORT);
    outb(valor, SIO_DATA_PORT);
}

unsigned char leer_registro_sio(unsigned char registro) {
    outb(registro, SIO_CONFIG_PORT);
    return inb(SIO_DATA_PORT);
}

// ================================================================
// 5. CORRIMIENTO BINARIO PARA SJA1000T (DEFINIDO ANTES DE USARSE)
// ================================================================
void corrimiento_binario_analisis(unsigned short base) {
    unsigned char resultados[256];
    int fallos = 0;

    printf("\n=== CORRIMIENTO BINARIO CON ANÁLISIS DE BITS ===\n");
    printf("Recolectando datos...\n");

    for (int valor = 0x00; valor <= 0xFF; valor++) {
        unsigned char esc = (unsigned char)valor;
        outb(esc, base + 4);
        usleep(2);
        unsigned char ley = inb(base + 4);
        resultados[valor] = ley;
        if (esc != ley) fallos++;
    }

    printf("\n--- ANÁLISIS DE BITS ---\n");
    int bits_fijos = 0;
    for (int bit = 0; bit < 8; bit++) {
        int siempre_0 = 1;
        int siempre_1 = 1;
        for (int valor = 0x00; valor <= 0xFF; valor++) {
            int bit_actual = (resultados[valor] >> bit) & 1;
            if (bit_actual == 1) siempre_0 = 0;
            if (bit_actual == 0) siempre_1 = 0;
        }
        if (siempre_0) {
            printf("   Bit %d: SIEMPRE 0 (fijo en 0)\n", bit);
            bits_fijos++;
        } else if (siempre_1) {
            printf("   Bit %d: SIEMPRE 1 (fijo en 1)\n", bit);
            bits_fijos++;
        } else {
            printf("   Bit %d: VARIABLE (funciona correctamente)\n", bit);
        }
    }

    printf("\n--- RESUMEN ---\n");
    if (bits_fijos == 0) {
        printf("✅ No se detectaron bits fijos.\n");
        if (fallos > 0) {
            printf("   Aunque hay %d fallos, ningún bit está fijo.\n", fallos);
            printf("   Mostrando todos los fallos...\n\n");
            for (int valor = 0x00; valor <= 0xFF; valor++) {
                unsigned char esc = (unsigned char)valor;
                unsigned char ley = resultados[valor];
                if (esc != ley) {
                    printf("0x%02X\t", esc);
                    for (int i = 7; i >= 0; i--) printf("%d", (esc >> i) & 1);
                    printf("\t| 0x%02X\t", ley);
                    for (int i = 7; i >= 0; i--) printf("%d", (ley >> i) & 1);
                    printf("\t| ❌ FALLA\n");
                }
            }
        }
    } else {
        printf("⚠️ Se detectaron %d bits fijos.\n", bits_fijos);
        printf("   Posibles causas: pistas rotas, soldaduras frías, buffer dañado.\n");
    }
}

// ================================================================
// 6. FUNCIONES DE CONFIGURACIÓN DE UARTs (FDC37C669)
// ================================================================
int configurar_uart1_en(uint base_addr, int modo_silencioso) {
    unsigned char lcr;
    habilitar_configuracion();
    escribir_registro_sio(0x07, 0x04);
    escribir_registro_sio(0x60, (base_addr >> 8) & 0xFF);
    escribir_registro_sio(0x61, base_addr & 0xFF);
    escribir_registro_sio(0x70, 0x04);
    escribir_registro_sio(0x30, 0x01);
    deshabilitar_configuracion();

    outb(0x83, UART3_BASE + 3);
    usleep(5);
    lcr = inb(UART3_BASE + 3);
    if (lcr != 0x83) return 0;

    outb(0x18, UART3_BASE + 0);
    outb(0x00, UART3_BASE + 1);
    outb(0x03, UART3_BASE + 3);
    usleep(5);
    lcr = inb(UART3_BASE + 3);
    if (lcr != 0x03) return 0;

    return 1;
}

int configurar_uart2_en(uint base_addr, int modo_silencioso) {
    unsigned char lcr;
    habilitar_configuracion();
    escribir_registro_sio(0x07, 0x05);
    escribir_registro_sio(0x60, (base_addr >> 8) & 0xFF);
    escribir_registro_sio(0x61, base_addr & 0xFF);
    escribir_registro_sio(0x70, 0x03);
    escribir_registro_sio(0x30, 0x01);
    deshabilitar_configuracion();

    outb(0x83, UART4_BASE + 3);
    usleep(5);
    lcr = inb(UART4_BASE + 3);
    if (lcr != 0x83) return 0;

    outb(0x18, UART4_BASE + 0);
    outb(0x00, UART4_BASE + 1);
    outb(0x03, UART4_BASE + 3);
    usleep(5);
    lcr = inb(UART4_BASE + 3);
    if (lcr != 0x03) return 0;

    return 1;
}

// ================================================================
// 7. PRUEBAS SILENCIOSAS (SOLO RETORNAN 0 o 1)
// ================================================================

int test_fdc37c669() {
    unsigned char id = 0;
    outb(0x55, SIO_CONFIG_PORT);
    outb(0x0D, SIO_CONFIG_PORT);
    id = inb(SIO_DATA_PORT);
    outb(0xAA, SIO_CONFIG_PORT);
    if (id != 0x03) return 0;

    if (!configurar_uart1_en(UART3_BASE, 1)) return 0;
    if (!configurar_uart2_en(UART4_BASE, 1)) return 0;

    return 1;
}

int test_st16c2550(unsigned short base) {
    unsigned char lcr, ier, mcr, rec;

    outb(0x80, base + 3);
    lcr = inb(base + 3);
    if (lcr != 0x80) return 0;
    outb(0x03, base + 3);

    outb(0x0F, base + 1);
    ier = inb(base + 1);
    if (ier != 0x0F) return 0;
    outb(0x00, base + 1);

    outb(0x10, base + 4);
    mcr = inb(base + 4);
    if ((mcr & 0x10) != 0x10) return 0;

    outb('A', base + 0);
    rec = inb(base + 0);
    if (rec != 'A') return 0;

    outb(0x00, base + 4);
    return 1;
}

int test_sja1000t(unsigned short base) {
    unsigned short cr_addr = base + 0;
    unsigned short acr_addr = base + 4;
    unsigned char test_val = 0x55;

    outb(0x01, cr_addr);
    usleep(5);
    outb(test_val, acr_addr);
    usleep(5);
    unsigned char read_val = inb(acr_addr);
    outb(0x00, cr_addr);

    return (read_val == test_val);
}

int test_saa7130hl() {
    FILE *fp;
    char buffer[256];
    fp = popen("cat /sys/bus/pci/devices/*/vendor 2>/dev/null | grep 1131", "r");
    if (fp == NULL) return 0;
    int found = (fgets(buffer, sizeof(buffer), fp) != NULL);
    pclose(fp);
    return found;
}

// ================================================================
// 8. DIAGNÓSTICOS DETALLADOS
// ================================================================

void diagnosticar_fdc37c669() {
    printf("\n=== DIAGNÓSTICO DEL FDC37C669 (U75) ===\n");
    unsigned char id = 0;
    outb(0x55, SIO_CONFIG_PORT);
    outb(0x0D, SIO_CONFIG_PORT);
    id = inb(SIO_DATA_PORT);
    outb(0xAA, SIO_CONFIG_PORT);
    printf("ID leído: 0x%02X %s\n", id, (id == 0x03) ? "✅" : "❌ (esperado 0x03)");

    printf("Probando UART1 en 0x%04X... ", UART3_BASE);
    if (configurar_uart1_en(UART3_BASE, 0)) {
        printf("✅ OK\n");
    } else {
        printf("❌ FALLO\n");
    }

    printf("Probando UART2 en 0x%04X... ", UART4_BASE);
    if (configurar_uart2_en(UART4_BASE, 0)) {
        printf("✅ OK\n");
    } else {
        printf("❌ FALLO\n");
    }
    pausa();
}

void diagnosticar_st16c2550() {
    printf("\n=== DIAGNÓSTICO DEL ST16C2550 (U59) ===\n");
    printf("Probando Canal A en 0x%04X... ", UART_BASE_1);
    if (test_st16c2550(UART_BASE_1)) {
        printf("✅ OK\n");
    } else {
        printf("❌ FALLO\n");
    }
    printf("Probando Canal B en 0x%04X... ", UART_BASE_2);
    if (test_st16c2550(UART_BASE_2)) {
        printf("✅ OK\n");
    } else {
        printf("❌ FALLO\n");
    }
    pausa();
}

void diagnosticar_sja1000t() {
    printf("\n=== DIAGNÓSTICO DEL SJA1000T (U41) ===\n");
    unsigned short base = SJA_BASE;
    unsigned char test_val_1 = 0x55;
    unsigned char test_val_2 = 0xAA;
    unsigned char read_val_1, read_val_2;

    printf("Prueba lineal directa en 0x%04X...\n", base + 4);
    outb(0x01, base + 0);
    usleep(5);
    outb(test_val_1, base + 4);
    usleep(5);
    read_val_1 = inb(base + 4);
    outb(test_val_2, base + 4);
    usleep(5);
    read_val_2 = inb(base + 4);
    outb(0x00, base + 0);

    printf("  -> 0x55 -> 0x%02X %s\n", read_val_1, (read_val_1 == 0x55) ? "✅" : "❌");
    printf("  -> 0xAA -> 0x%02X %s\n", read_val_2, (read_val_2 == 0xAA) ? "✅" : "❌");

    if (read_val_1 == 0x55 && read_val_2 == 0xAA) {
        printf("✅ Chip responde correctamente.\n");
    } else {
        printf("⚠️ Se detectaron problemas. Ejecutando corrimiento binario...\n");
        corrimiento_binario_analisis(base);
    }
    pausa();
}

void diagnosticar_saa7130hl() {
    printf("\n=== DIAGNÓSTICO DEL SAA7130HL (U29) ===\n");
    printf("Buscando en PCI... ");
    if (test_saa7130hl()) {
        printf("✅ Encontrado\n");
    } else {
        printf("❌ No encontrado\n");
        printf("Verifica que el chip esté correctamente insertado y alimentado.\n");
    }
    pausa();
}

// ================================================================
// 9. MOSTRAR RESULTADOS GENERALES
// ================================================================
void mostrar_resultados_generales() {
    printf("\n========================================================\n");
    printf("   TEST DE HARDWARE - TARJETA PANTALLA PIXY\n");
    printf("========================================================\n\n");

    printf("[1] FDC37C669 (U75) en 0x%03X... ", SIO_CONFIG_PORT);
    fflush(stdout);
    printf(test_fdc37c669() ? "✅ DETECTADO\n" : "❌ NO DETECTADO\n");

    printf("[2a] ST16C2550 (U59) en 0x%03X... ", UART_BASE_1);
    fflush(stdout);
    printf(test_st16c2550(UART_BASE_1) ? "✅ DETECTADO\n" : "❌ NO DETECTADO\n");

    printf("[2b] ST16C2550 (U59) en 0x%03X... ", UART_BASE_2);
    fflush(stdout);
    printf(test_st16c2550(UART_BASE_2) ? "✅ DETECTADO\n" : "❌ NO DETECTADO\n");

    printf("[3] SJA1000T (U41) en 0x%03X... ", SJA_BASE);
    fflush(stdout);
    printf(test_sja1000t(SJA_BASE) ? "✅ DETECTADO\n" : "❌ NO DETECTADO\n");

    printf("[4] SAA7130HL (U29) en PCI... ");
    fflush(stdout);
    printf(test_saa7130hl() ? "✅ DETECTADO\n" : "❌ NO DETECTADO\n");

    printf("\n========================================================\n");
}

// ================================================================
// 10. MAIN
// ================================================================
int main() {
    if (iopl(3) < 0) {
        perror("Error: Ejecutar con 'sudo'");
        return 1;
    }

    mostrar_resultados_generales();
    pausa();

    int resultado_fdc = test_fdc37c669();
    int resultado_st16_a = test_st16c2550(UART_BASE_1);
    int resultado_st16_b = test_st16c2550(UART_BASE_2);
    int resultado_sja = test_sja1000t(SJA_BASE);
    int resultado_saa = test_saa7130hl();

    if (!resultado_fdc) {
        printf("\n❌ Fallo detectado en FDC37C669 (U75).\n");
        diagnosticar_fdc37c669();
    }

    if (!resultado_st16_a || !resultado_st16_b) {
        printf("\n❌ Fallo detectado en ST16C2550 (U59).\n");
        diagnosticar_st16c2550();
    }

    if (!resultado_sja) {
        printf("\n❌ Fallo detectado en SJA1000T (U41).\n");
        diagnosticar_sja1000t();
    }

    if (!resultado_saa) {
        printf("\n❌ Fallo detectado en SAA7130HL (U29).\n");
        diagnosticar_saa7130hl();
    }

    printf("\n========================================================\n");
    printf("   PRUEBAS COMPLETADAS\n");
    printf("========================================================\n");
    pausa();

    return 0;
}