#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/io.h>
#include <time.h>
#include <ctype.h> // Para usar tolower()



// Usaremos la dirección clásica de COM3 para la UART1
#define UART5_BASE       0X200  

// Usaremos la dirección clásica de COM4 para la UART2
#define UART6_BASE       0X208

//USART ENCONTRADAS 0X200/0X208/0X2F8/0X3F8

// 1. Funciones para el modo de configuración del Super I/O
// --------------------------------------------------------------


// --------------------------------------------------------------
// FUNCIÓN: BARRIDO AUTOMÁTICO SEGURO PARA ST16C2550
// --------------------------------------------------------------
void barrido_automatico() {
    printf("\n=== INICIANDO BARRIDO AUTOMÁTICO PARA ST16C2550 ===\n");
    printf("Buscando en el rango 0x200 - 0x3FF...\n");
    
    unsigned short base_inicio = 0x200;
    unsigned short base_fin    = 0x3FF;
    unsigned char valor_prueba = 0x80; // Valor para el LCR (DLAB=1)
    unsigned char valor_leido  = 0;
    int encontrado_a = 0;
    int encontrado_b = 0;

    for (unsigned short base = base_inicio; base <= base_fin; base += 8) {
        unsigned short lcr_addr = base + 3;

        // Escribir el valor de prueba en el LCR
        outb(valor_prueba, lcr_addr);
        
        // Leer el LCR
        valor_leido = inb(lcr_addr);
        
        // Si el valor leído coincide, hemos encontrado un UART
        if (valor_leido == valor_prueba) {
            printf("✅ ¡ÉXITO! UART encontrado en dirección base 0x%03X (LCR en 0x%04X)\n", base, lcr_addr);
            encontrado_a = 1;
            
            // Restaurar el LCR a un valor por defecto (8N1, DLAB=0)
            outb(0x03, lcr_addr);
            
            // Aquí podrías intentar acceder al Canal B si tiene una base diferente
            // o simplemente salir del bucle si solo te interesa un canal.
            // break; // Descomentar para detener el barrido tras el primer hallazgo
        }
    }

    if (!encontrado_a) {
        printf("\n❌ No se encontró ST16C2550 en el rango 0x%03X - 0x%03X.\n", base_inicio, base_fin);
        printf("   Prueba con un rango más amplio o verifica las conexiones de CSA#/CSB#.\n");
    }
}


// ... (Aquí van todas tus funciones de configuración del FDC37C669 y del ST16C2550)

// --------------------------------------------------------------
// FUNCIÓN: ESCRIBIR UN VALOR EN EL LCR DE UNA DIRECCIÓN BASE
// --------------------------------------------------------------
void probar_direccion(unsigned short base_addr) {
    unsigned short lcr_addr = base_addr + 3;
    unsigned char valor_prueba = 0x80; // Un valor fácil de ver en el osciloscopio (DLAB=1)
    
    printf("\n--- Probando dirección base 0x%03X ---\n", base_addr);
    printf("Escribiendo 0x%02X en LCR (0x%04X).\n", valor_prueba, lcr_addr);
    
    // Escribir el valor en el LCR
    outb(valor_prueba, lcr_addr);
    
    // Leer el LCR para ver si el chip responde (opcional, pero útil)
    unsigned char leido = inb(lcr_addr);
    if (leido == valor_prueba) {
        printf("✅ ¡RESPUESTA! El LCR devolvió 0x%02X. Parece que el chip está en 0x%03X.\n", leido, base_addr);
    } else {
        printf("❌ No hay respuesta en 0x%03X (LCR leyó 0x%02X).\n", base_addr, leido);
    }
}

// --------------------------------------------------------------
// FUNCIÓN: BARRIDO INTERACTIVO CON OSCILOSCOPIO
// --------------------------------------------------------------
void barrido_interactivo() {
    unsigned short base_inicio = 0x200;
    unsigned short base_fin    = 0x3FF;
    unsigned short base_actual = base_inicio;
    char comando = ' ';
    
    printf("\n=== BARRIDO INTERACTIVO PARA ST16C2550 ===\n");
    printf("Usa los comandos:\n");
    printf("  [n] -> Siguiente dirección (base + 8)\n");
    printf("  [p] -> Dirección anterior (base - 8)\n");
    printf("  [s] -> Salir del barrido\n");
    printf("  [r] -> Reiniciar barrido desde 0x200\n");
    printf("  [c] -> Cambiar la dirección base manualmente\n");
    printf("  [a] -> Barrido automático con pausa (presiona cualquier tecla para avanzar)\n\n");
    
    while (comando != 's') {
        probar_direccion(base_actual);
        
        printf("\nBase actual: 0x%03X. Comando: ", base_actual);
        scanf(" %c", &comando);
        comando = tolower(comando);
        
        switch(comando) {
            case 'n':
                base_actual += 8;
                if (base_actual > base_fin) base_actual = base_fin;
                break;
            case 'p':
                base_actual -= 8;
                if (base_actual < base_inicio) base_actual = base_inicio;
                break;
            case 'r':
                base_actual = base_inicio;
                break;
            case 'c':
                printf("Nueva base (hex, ej. 0x3E8): ");
                scanf("%hx", &base_actual);
                break;
            case 'a':
                printf("Iniciando barrido automático... Presiona cualquier tecla para detener.\n");
                while (base_actual <= base_fin) {
                    probar_direccion(base_actual);
                    printf("Base: 0x%03X. Presiona una tecla para avanzar...\n", base_actual);
                    getchar(); // Esperar una tecla (limpia el buffer)
                    getchar(); // Esperar otra tecla para confirmar
                    base_actual += 8;
                }
                printf("Barrido automático finalizado.\n");
                base_actual = base_inicio; // Reiniciar
                break;
            case 's':
                printf("Saliendo del barrido.\n");
                break;
            default:
                printf("Comando no reconocido.\n");
                break;
        }
    }
}

// --------------------------------------------------------------
// MAIN MODIFICADO
// --------------------------------------------------------------
int main() {
    if (iopl(3) < 0) {
        perror("Error grave: Requiere ejecutar con 'sudo' o privilegios root");
        return 1;
    }

    printf("========================================================\n");
    printf("BARRIDO INTERACTIVO ST16C2550 (DUART) 📊\n");
    printf("========================================================\n");
    printf("Conecta el osciloscopio al pin CSA# (o CSB#) del ST16C2550.\n");
    printf("Observa cuándo la señal baja a 0V (se activa) al escribir en el LCR.\n\n");

    //barrido_interactivo();
    barrido_automatico();
    return 0;
}