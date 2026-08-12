#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/io.h>


int main() {
    if (iopl(3) < 0) {
        perror("iopl falló. Corre con 'sudo'");
        return 1;
    }

    printf("=== ESCANEO DE IDENTIDAD DEL SUPER I/O SMSC ===\n\n");

    unsigned char id_leido = 0;

    // --- INTENTO 1: Escanear en 0x3F0 ---
    printf("[1] Despertando chip en 0x3F0...\n");
    outb(0x55, 0x3F0); // Comando SMSC para entrar a configuración
    
    // El registro 0x0D en el FDC37C669 contiene el Device ID de fábrica
    outb(0x0D, 0x3F0); 
    id_leido = inb(0x3F1);
    
    printf("    -> Registro 0x0D (Device ID) en 0x3F0 devolvió: 0x%02X\n", id_leido);
    outb(0xAA, 0x3F0); // Cerrar modo configuración

    if (id_leido != 0xFF && id_leido != 0x00) {
        printf("    ✅ ¡El chip respondió en 0x3F0! ID: 0x%02X\n\n", id_leido);
    }

    // --- INTENTO 2: Escanear en 0x370 ---
    printf("[2] Despertando chip en 0x370...\n");
    outb(0x55, 0x370); // Comando SMSC para entrar a configuración
    
    outb(0x0D, 0x370); 
    id_leido = inb(0x371);
    
    printf("    -> Registro 0x0D (Device ID) en 0x370 devolvió: 0x%02X\n", id_leido);
    outb(0xAA, 0x370); // Cerrar modo configuración

    if (id_leido != 0xFF && id_leido != 0x00) {
        printf("    ✅ ¡El chip respondió en 0x370! ID: 0x%02X\n\n", id_leido);
    }

    // --- CONCLUSIÓN ---
    if (inb(0x3F1) == 0xFF && inb(0x371) == 0xFF) {
        printf("\n❌ CONCLUSIÓN DE HARDWARE:\n");
        printf("El chip devuelve 0xFF en sus propios registros de configuración.\n");
        printf("Esto significa que el pin nCS (Pin 27) está desactivado por la placa,\n");
        printf("o el BIOS de tu PC tiene bloqueados los ciclos de E/S ISA/LPC.\n");
        printf("Por favor, reinicia la PC, entra al BIOS y activa los puertos periféricos.\n");
    }

    return 0;
}
