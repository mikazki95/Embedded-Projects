#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/io.h>

// ================================================================
// 1. DEFINICIONES
// ================================================================

#define SIO_CONFIG_PORT 0x370
#define SIO_DATA_PORT   0x371
#define UART_BASE_1     0x200
#define UART_BASE_2     0x208
#define SJA_BASE        0x340

// ================================================================
// 2. FUNCIONES DE PRUEBA
// ================================================================

// ---------- FDC37C669 ----------
int test_fdc37c669() {
    unsigned char id = 0;

    outb(0x55, SIO_CONFIG_PORT);
    outb(0x0D, SIO_CONFIG_PORT);
    id = inb(SIO_DATA_PORT);
    outb(0xAA, SIO_CONFIG_PORT);

    return (id == 0x03);
}

// ---------- ST16C2550 ----------
int test_st16c2550(unsigned short base) {
    unsigned short lcr_addr = base + 3;
    unsigned char test_val = 0x80;

    outb(test_val, lcr_addr);
    unsigned char read_val = inb(lcr_addr);
    outb(0x03, lcr_addr);

    return (read_val == test_val);
}

// ---------- SJA1000T ----------
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

// ---------- SAA7130HL (SIN dirent.h) ----------
int test_saa7130hl() {
    FILE *fp;
    char buffer[256];
    
    // Ejecutar el mismo comando que funcionó manualmente
    fp = popen("cat /sys/bus/pci/devices/*/vendor 2>/dev/null | grep 1131", "r");
    if (fp == NULL) return 0;
    
    int found = (fgets(buffer, sizeof(buffer), fp) != NULL);
    pclose(fp);
    return found;
}

// ================================================================
// 3. MAIN
// ================================================================
int main() {
    if (iopl(3) < 0) {
        perror("Error: Ejecutar con 'sudo'");
        return 1;
    }

    printf("\n========================================================\n");
    printf("   TEST DE HARDWARE - TARJETA PANTALLA PIXY\n");
    printf("========================================================\n\n");

    // 1. FDC37C669
    printf("[1] FDC37C669 (U75) en 0x%03X... ", SIO_CONFIG_PORT);
    fflush(stdout);
    printf(test_fdc37c669() ? "✅ DETECTADO\n" : "❌ NO DETECTADO\n");

    // 2. ST16C2550 - Canal A
    printf("[2a] ST16C2550 (U59) en 0x%03X... ", UART_BASE_1);
    fflush(stdout);
    printf(test_st16c2550(UART_BASE_1) ? "✅ DETECTADO\n" : "❌ NO DETECTADO\n");

    // 3. ST16C2550 - Canal B
    printf("[2b] ST16C2550 (U59) en 0x%03X... ", UART_BASE_2);
    fflush(stdout);
    printf(test_st16c2550(UART_BASE_2) ? "✅ DETECTADO\n" : "❌ NO DETECTADO\n");

    // 4. SJA1000T
    printf("[3] SJA1000T (U41) en 0x%03X... ", SJA_BASE);
    fflush(stdout);
    printf(test_sja1000t(SJA_BASE) ? "✅ DETECTADO\n" : "❌ NO DETECTADO\n");

    // 5. SAA7130HL
    printf("[4] SAA7130HL (U29) en PCI... ");
    fflush(stdout);
    printf(test_saa7130hl() ? "✅ DETECTADO\n" : "❌ NO DETECTADO\n");

    printf("\n========================================================\n");
    printf("   PRUEBAS COMPLETADAS\n");
    printf("========================================================\n");

    return 0;
}