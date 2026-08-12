#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/io.h>

#define SIO_CONFIG_PORT 0x2E
#define SIO_DATA_PORT   0x2F

// Leer un registro del Super I/O
unsigned char leer_sio(unsigned char registro) {
    outb(registro, SIO_CONFIG_PORT);
    return inb(SIO_DATA_PORT);
}

// Habilitar modo de configuración
void habilitar_configuracion() {
    outb(0x87, SIO_CONFIG_PORT);
    outb(0x01, SIO_CONFIG_PORT);
    outb(0x55, SIO_CONFIG_PORT);
    outb(0x55, SIO_CONFIG_PORT);
}

// Deshabilitar modo de configuración
void deshabilitar_configuracion() {
    outb(0xAA, SIO_CONFIG_PORT);
}

int main() {
    if (iopl(3) < 0) {
        perror("iopl (necesitas permisos de root)");
        return 1;
    }

    printf("=== Verificando comunicación con FDC37C669 ===\n\n");

    // 1. Habilitar configuración
    printf("1. Habilitando modo de configuración...\n");
    habilitar_configuracion();

    // 2. Leer registros de identificación
    printf("\n2. Leyendo registros de identificación:\n");
    
    // Registro 0x20: Device ID (debería ser 0x01 o 0x03)
    unsigned char device_id = leer_sio(0x20);
    printf("   Device ID (0x20): 0x%02X\n", device_id);
    
    // Registro 0x21: Device Revision (debería ser 0x00 o 0x01)
    unsigned char revision = leer_sio(0x21);
    printf("   Device Revision (0x21): 0x%02X\n", revision);
    
    // Registro 0x22: Vendor ID (debería ser 0x07 para SMSC/FDC)
    unsigned char vendor_id = leer_sio(0x22);
    printf("   Vendor ID (0x22): 0x%02X\n", vendor_id);

    // 3. Verificar si los valores son los esperados
    printf("\n3. Análisis:\n");
    if (device_id == 0x01 || device_id == 0x03) {
        printf("   ✅ Device ID correcto (0x%02X). Chip FDC37C669 presente.\n", device_id);
    } else {
        printf("   ❌ Device ID inesperado (0x%02X). Verifica:\n", device_id);
        printf("      - ¿El chip es realmente un FDC37C669?\n");
        printf("      - ¿Está en la dirección 0x2E/0x2F?\n");
        printf("      - ¿El bus ISA está funcionando correctamente?\n");
    }

    if (vendor_id == 0x07) {
        printf("   ✅ Vendor ID correcto (0x07). Fabricante SMSC/FDC.\n");
    } else {
        printf("   ❌ Vendor ID inesperado (0x%02X).\n", vendor_id);
    }

    // 4. Deshabilitar configuración
    printf("\n4. Deshabilitando modo de configuración...\n");
    deshabilitar_configuracion();

    printf("\n=== Prueba completada ===\n");
    return 0;
}