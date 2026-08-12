#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/io.h>
#include <time.h>
#include <ctype.h> 

// Puertos de configuración del Super I/O (Validados por tu hardware)
#define SIO_CONFIG_PORT 0x370
#define SIO_DATA_PORT   0x371

// Registros del SJA1000T en Modo BasicCAN
#define SJA_CR   0  // Control Register
#define SJA_ACR  4  // Acceptance Code Register (Espejo de datos)

// --------------------------------------------------------------
// FUNCIÓN: MODO CONFIGURACIÓN SUPER I/O
// --------------------------------------------------------------
void habilitar_configuracion() {
    outb(0x55, SIO_CONFIG_PORT); 
}

void deshabilitar_configuracion() {
    outb(0xAA, SIO_CONFIG_PORT);
}

// --------------------------------------------------------------
// OPICÓN 1: BARRIDO AUTOMÁTICO SEGURO (Mapeo Lineal)
// --------------------------------------------------------------
void barrido_automatico(unsigned short addr_init, unsigned short addr_end) {
    printf("\n=== INICIANDO BARRIDO AUTOMÁTICO SJA1000T ===\n");
    printf("Buscando en el rango 0x%03X - 0x%03X en pasos de 32 bytes...\n", addr_init, addr_end);
    
    unsigned char valor_prueba1 = 0x55; 
    unsigned char valor_prueba2 = 0xAA;
    int encontrado = 0;

    for (unsigned short base = addr_init; base <= addr_end; base += 32) {
        unsigned short cr_addr  = base + SJA_CR;   
        unsigned short acr_addr = base + SJA_ACR;  

        // Forzar al chip a entrar en Modo Reset para desbloquear escritura
        outb(0x01, cr_addr); 
        usleep(5); // Retardo de estabilización

        // Prueba de espejo doble
        outb(valor_prueba1, acr_addr);
        usleep(5);
        unsigned char leido1 = inb(acr_addr);
        
        outb(valor_prueba2, acr_addr);
        usleep(5);
        unsigned char leido2 = inb(acr_addr);
        
        if (leido1 == valor_prueba1 && leido2 == valor_prueba2) {
            printf("✅ ¡ÉXITO! SJA1000T encontrado en dirección base 0x%03X\n", base);
            printf("   -> Registro ACR (0x%04X) validado con espejo de datos.\n", acr_addr);
            encontrado = 1;
            
            // Sacar del modo Reset antes de terminar
            outb(0x00, cr_addr); 
            break; 
        }
    }

    if (!encontrado) {
        printf("\n❌ No se encontró el SJA1000T en el rango especificado de forma lineal.\n");
    }
}

// --------------------------------------------------------------
// OPCIÓN 2: BUCLE CONTINUO FIJO PARA OSCILOSCOPIO
// --------------------------------------------------------------
void bucle_osciloscopio(unsigned short base_addr) {
    unsigned short acr_addr = base_addr + SJA_ACR;
    
    printf("\n📊 [MODO OSCILOSCOPIO] Generando ráfagas continuas en Base 0x%03X...\n", base_addr);
    printf("Conecta tu sonda al pin CS# (Pin 11) del U41.\n");
    printf("Presiona Ctrl+C para finalizar este bucle.\n\n");
    
    while(1) {
        outb(0x01, base_addr);    // Intentar forzar reset (CR)
        usleep(2);
        outb(0x55, acr_addr);     // Escritura ACR
        usleep(2);
        inb(acr_addr);            // Lectura ACR
        usleep(10);               // Pausa breve entre ráfagas
    }
}

// --------------------------------------------------------------
// OPCIÓN 3: DIAGNÓSTICO PROFUNDO MULTIPLEXADO (0x340)
// --------------------------------------------------------------
void diagnostico_profundo_sja() {
    printf("\n=== CORRIENDO PRUEBA EN 0x340 MEDIANTE BUS MULTIPLEXADO ===\n");
    
    // CONFIGURACIÓN A: Registro en 0x340, Datos en 0x341 (Estándar)
    outb(0x00, 0x340); // Apuntar a Control Register
    usleep(5);
    outb(0x01, 0x341); // Enviar comando Reset Request
    usleep(5);
    
    outb(0x04, 0x340); // Apuntar a ACR
    usleep(5);
    outb(0x55, 0x341); // Escribir 0x55
    usleep(5);
    unsigned char intentoA_1 = inb(0x341);
    
    outb(0x04, 0x340); // Volver a apuntar a ACR
    usleep(5);
    outb(0xAA, 0x341); // Escribir 0xAA
    usleep(5);
    unsigned char intentoA_2 = inb(0x341);

    // CONFIGURACIÓN B: Registro en 0x341, Datos en 0x340 (Invertido por la GAL)
    outb(0x00, 0x341); // Apuntar a Control Register
    usleep(5);
    outb(0x01, 0x340); // Enviar comando Reset Request
    usleep(5);
    
    outb(0x04, 0x341); // Apuntar a ACR
    usleep(5);
    outb(0x55, 0x340); // Escribir 0x55
    usleep(5);
    unsigned char intentoB_1 = inb(0x340);
    
    outb(0x04, 0x341); // Volver a apuntar a ACR
    usleep(5);
    outb(0xAA, 0x340); // Escribir 0xAA
    usleep(5);
    unsigned char intentoB_2 = inb(0x340);

    // Despliegue de resultados en pantalla
    printf("\n[Config A] Reg=340, Dat=341 -> Leyó: 0x%02X / 0x%02X (Esperado: 0x55 / 0xAA)\n", intentoA_1, intentoA_2);
    printf("[Config B] Reg=341, Dat=340 -> Leyó: 0x%02X / 0x%02X (Esperado: 0x55 / 0xAA)\n", intentoB_1, intentoB_2);

    if ((intentoA_1 == 0x55 && intentoA_2 == 0xAA)) {
        printf("✅ ¡ÉXITO ABSOLUTO! SJA1000T detectado en Configuración A (Normal).\n");
        outb(0x00, 0x340); outb(0x00, 0x341); // Sacar de reset de forma segura
    } 
    else if ((intentoB_1 == 0x55 && intentoB_2 == 0xAA)) {
        printf("✅ ¡ÉXITO ABSOLUTO! SJA1000T detectado en Configuración B (Invertida).\n");
        outb(0x00, 0x341); outb(0x00, 0x340); // Sacar de reset de forma segura
    } 
    else {
        printf("❌ Tampoco respondió de forma exacta con retardos de estabilidad.\n");
        printf("   Nota de Hardware: Si estás con el osciloscopio en el pin 11 (CS#) del U41,\n");
        printf("   verifica si la señal cae limpia a 0V o si se queda flotando.\n");
    }
}

void prueba_lineal_real_sja() {
    printf("\n=== CORRIENDO PRUEBA LINEAL DIRECTA EN PERIFÉRICO (U41) ===\n");
    printf("Escribiendo directo mediante bus de datos SN74HC245 y direcciones 74ACQ573...\n");

    // 1. Forzar Reset escribiendo 0x01 directamente en la dirección base 0x340 (Registro 0)
    outb(0x01, 0x340); 
    usleep(10); // Tiempo para que el latch 573 y el SJA estabilicen las líneas

    // 2. Intentar escribir 0x55 en el Registro 4 (0x344)
    outb(0x55, 0x344);
    usleep(10);
    unsigned char r1 = inb(0x344);

    // 3. Intentar escribir 0xAA en el Registro 4 (0x344)
    outb(0xAA, 0x344);
    usleep(10);
    unsigned char r2 = inb(0x344);

    printf("\nResultado de Lectura Directa en 0x344:\n");
    printf("-> Intentó 0x55 -> Leyó: 0x%02X\n", r1);
    printf("-> Intentó 0xAA -> Leyó: 0x%02X\n", r2);

    if (r1 == 0x55 && r2 == 0xAA) {
        printf("✅ ¡ÉXITO! Comunicación lineal directa validada a la perfección.\n");
        outb(0x00, 0x340); // Sacar de reset de forma segura
    } else {
        printf("❌ Los datos siguen sin coincidir exactamente.\n");
        printf("\n📊 DIAGNÓSTICO CON OSCILOSCOPIO OBLIGATORIO EN LOS LATCHES:\n");
        printf("Deja esta prueba corriendo en bucle y mide lo siguiente:\n");
        printf("1. Pin 11 (LE) del 74ACQ573: ¿Da pulsos cuando ejecutas el programa?\n");
        printf("2. Pines de salida del 74ACQ573 (Pines 12 al 16): ¿Cambian de voltaje (0V/5V)\n");
        printf("   cuando cambias de la dirección 0x340 a la 0x344?\n");
    }
}

void prueba_estricta_registro_0() {
    printf("\n=== PRUEBA DE ESCRITURA EN REGISTRO 0 (0x340) ===\n");
    printf("Validando acceso directo según la tabla de la hoja de datos...\n");

    // Intentamos escribir un valor de prueba directo en el registro 0 (Control)
    // El bit 0 en '1' (0x01) activa el Reset Mode.
    outb(0x01, 0x340);
    usleep(5);
    unsigned char r_reset = inb(0x340);

    printf("\nResultados en el Registro 0 (0x340):\n");
    printf("-> Se escribió 0x01 (Reset Request). El chip leyó: 0x%02X\n", r_reset);

    // Verificación binaria estricta
    // Al escribir 0x01, el bit 0 DEBE quedarse en 1. Los bits de interrupción pueden variar.
    if (r_reset & 0x01) {
        printf("✅ ¡ÉXITO! El bit de Reset se activó correctamente. El chip está en RESET MODE.\n");
        printf("   Ahora sí es seguro proceder a leer/escribir en el Registro 4 (0x344).\n");
        
        // Ahora probamos el Registro 4 (Acceptance Code) que ya debe estar desbloqueado
        outb(0x55, 0x344);
        usleep(5);
        unsigned char r_acr = inb(0x344);
        printf("-> Probando Registro 4 (0x344) con 0x55 en RESET MODE. Leyó: 0x%02X\n", r_acr);
    } else {
        printf("❌ ERROR: El chip rechazó el modo Reset (El Bit 0 se quedó en 0 o el bus flotó).\n");
        printf("   El hardware sigue atrapado en OPERATING MODE o el latch 74ACQ573 está bloqueado.\n");
    }
}

void corrimiento_datos_registro_4() {
    printf("\n========================================================\n");
    printf("📊 INICIANDO CORRIMIENTO DE DATOS EN REGISTRO 4 (0x344)\n");
    printf("========================================================\n");
    printf("Escribiendo valores de 0x00 a 0xFF en RESET MODE...\n\n");
    printf("ESC_HEX\tESC_BIN\t\t| LEY_HEX\tLEY_BIN\t\t| ESTADO\n");
    printf("--------------------------------------------------------\n");

    int fallas = 0;

    for (int valor = 0x00; valor <= 0xFF; valor++) {
        unsigned char esc = (unsigned char)valor;

        // 1. Asegurar que el Latch y el SJA1000T están en RESET MODE (Reg 0 = 0x01)
        outb(0x01, 0x340);
        usleep(2); // Retardo corto para estabilizar el Latch 74ACQ573

        // 2. Escribir el valor de prueba en el Registro 4 (0x344)
        outb(esc, 0x344);
        usleep(2); // Retardo corto para estabilizar el Transceptor 74HC245

        // 3. Leer el valor de regreso
        unsigned char ley = inb(0x344);

        // 4. Mostrar en pantalla si hay discrepancia (o mostrar todos para análisis)
        // Imprimimos en binario para ver visualmente qué pines fallan
        if (esc != ley) {
            fallas++;
            printf("0x%02X\t", esc);
            for(int i=7; i>=0; i--) printf("%d", (esc >> i) & 1);
            printf("\t| 0x%02X\t", ley);
            for(int i=7; i>=0; i--) printf("%d", (ley >> i) & 1);
            printf("\t| ❌ FALLA\n");
        } else {
            // Descomenta la línea de abajo si quieres ver también los que pasan con éxito
            // printf("0x%02X\t-------\t\t| 0x%02X\t-------\t\t| ✅ OK\n", esc, ley);
        }
    }

    printf("--------------------------------------------------------\n");
    if (fallas == 0) {
        printf("✅ ¡Felicidades! Todo el bus de datos respondió perfecto de 0x00 a 0xFF.\n");
    } else {
        printf("⚠️ Se encontraron %d valores con errores de un total de 256.\n", fallas);
        printf("Analiza qué columnas de bits (del 7 al 0) se quedan fijas en 0 o 1.\n");
    }
}

void bucle_estres_0xDF() {
    printf("\n========================================================\n");
    printf("📊 EXCITANDO EL BUS CON 0xEF (1101 1111) CONTINUO\n");
    printf("========================================================\n");
    printf("Escribiendo ráfagas en Registro 0 (0x340) y Registro 4 (0x344)...\n");
    printf("Presiona Ctrl+C para detener y apagar el programa.\n\n");
    
    // Forzamos un bucle infinito ultrarrápido para que el osciloscopio 
    // capture un tren de pulsos continuo y estable.
    while(1) {
        // 1. Asegurar Latch de Direcciones y Modo Reset
        outb(0x01, 0x340); 
        
        // 2. Inyectar el valor de prueba con los bits clave (Bit 7 = 1, Bit 5 = 0)
        outb(0xDF, 0x344); 
        
        // Pequeño retardo para no congelar el kernel de Linux, 
        // pero lo bastante rápido para ver la señal en el osciloscopio.
        usleep(5); 
    }
}

// --------------------------------------------------------------
// MAIN INTERACTIVO
// --------------------------------------------------------------
// --------------------------------------------------------------
// MAIN INTERACTIVO COMPLETO Y CORREGIDO
// --------------------------------------------------------------
int main() {
    if (iopl(3) < 0) {
        perror("Error grave: Requiere ejecutar con 'sudo' o privilegios root");
        return 1;
    }

    unsigned short addr_init, addr_end, addr_fija;
    int opcion;

    while (1) {
        printf("\n========================================================\n");
        printf("       SUITE DE INGENIERÍA INVERSA SJA1000T (CAF)       \n");
        printf("========================================================\n");
        printf("1. Barrido automático en un rango amplio\n");
        printf("2. Bucle continuo fijo (Para congelar CS# / ALE)\n");
        printf("3. Diagnóstico Multiplexado por software (0x340/0x341)\n");
        printf("4. Prueba lineal directa de hardware (0x340 y 0x344)\n");
        printf("5. Validar estrictamente Registro 0 (Control)\n");
        printf("6. Corrimiento binario de datos (0x00 a 0xFF en Reg 4)\n");
        printf("7. Bucle fijo con 0xEF (Bit 7=1, Bit 5=0)\n");
        printf("8. Bucle fijo con 0xDF (Bit 7=0, Bit 5=1)\n");
        printf("9. Salir del programa\n");
        printf("Selecciona una opción: ");
        
        if (scanf("%d", &opcion) != 1) break;
        if (opcion == 9) { printf("Saliendo de la suite.\n"); break; }
        
        switch (opcion) {
            case 1:
                printf("Introduce dirección de inicio (hex, ej. 0x200): ");
                scanf("%hx", &addr_init);
                printf("Introduce dirección de fin (hex, ej. 0x3FF): ");
                scanf("%hx", &addr_end);
                if (addr_init % 32 != 0) addr_init = (addr_init / 32) * 32;
                if (addr_end % 32 != 0) addr_end = (addr_end / 32) * 32;
                barrido_automatico(addr_init, addr_end);
                break;
                
            case 2:
                printf("Introduce la dirección base a excitar (hex, ej. 0x340): ");
                scanf("%hx", &addr_fija);
                if (addr_fija % 32 != 0) addr_fija = (addr_fija / 32) * 32;
                bucle_osciloscopio(addr_fija);
                break;
                
            case 3:
                diagnostico_profundo_sja();
                break;
                
            case 4:
                prueba_lineal_real_sja();
                break;
                
            case 5:
                prueba_estricta_registro_0();
                break;
                
            case 6:
                corrimiento_datos_registro_4();
                break;
                
            case 7:
                //bucle_estres_0xEF();
                printf("❌ Opción inválida. Intenta de nuevo.\n");
                break;
                
            case 8:
                bucle_estres_0xDF();
                break;
                
            default:
                printf("❌ Opción inválida. Intenta de nuevo.\n");
                break;
        }
        
        while (getchar() != '\n'); // Limpiar buffer de entrada
    }
    return 0;
}
