#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/io.h>
#include <ctype.h> // Para usar tolower()
#include <fcntl.h>
#include <sys/mman.h>

// ================================================================
// 1. DEFINICIONES GLOBALES
// ================================================================


#define SJA_BASE        0x340
// Puertos de configuración del Super I/O validados por tu hardware
#define SIO_CONFIG_PORT 0x370
#define SIO_DATA_PORT   0x371
// Usaremos la dirección clásica de COM3 para la UART1
#define UART3_BASE       0x3E8  
// Usaremos la dirección clásica de COM4 para la UART2
#define UART4_BASE       0x2E8 
// Usaremos la dirección clásica de COM5 para la UART1
#define UART5_BASE       0X200  
// Usaremos la dirección clásica de COM6 para la UART2
#define UART6_BASE       0X208

// Constantes para los resultados de las pruebas
#define EXITO            1
#define SIN_COMUNICACION 0
#define BITS_CORRUPTOS  -1

// Definición de tu mapa lineal directo (Sin offsets en la FPGA)
#define MVBC_BASE   0x0F80
#define REG_SCR     (MVBC_BASE + 0x00) // Registro 0x0F80 (Palabra par)
#define REG_MCR     (MVBC_BASE + 0x04) // Registro 0x0F84 (Saltos de 4 en 4 por A0)

unsigned  conf_SCR= 0xE21D;
unsigned  full_SCR= 0xE21F;
unsigned  conf_MCR= 0xB002;
// Tu zona real de Traffic Memory Libre (SRAM Samsung)
#define TM_BASE     0x8000  // Justo donde termina la zona SCR
#define TM_SIZE     0x100 //0x1000  // Barremos 4096 bytes (2048 palabras) para una prueba sólida
unsigned  TM_I= TM_BASE;
#define BASE_MEMORIA_RANGO3  0xD0000//0xF00000
#define TAMANO_BLOQUE        0x1000  // 4 KB (tamaño de página)
// ================================================================
// 1.1. FUNCIONES especiales 
// ================================================================
// Función para leer un word (16 bits) de una dirección
unsigned short leer_word(unsigned short direccion) {
    unsigned short valor = inw(direccion);
    //printf("  📖 Leído 0x%04X de 0x%04X\n", valor, direccion);
    //usleep(1000);
    return valor;
}

// Función para escribir un word (16 bits) en una dirección
void escribir_word(unsigned short direccion, unsigned short dato) {
    outw(dato, direccion);
    //usleep(1000);
    //printf("  ✅ Escrito 0x%04X en 0x%04X\n", dato, direccion);
}

void bucle_diagnostico_uart(unsigned short base, int segundos) {
    printf("   Activando modo de diagnóstico para 0x%04X (%d segundos)...\n", base, segundos);
    printf("   Conecta tu osciloscopio al pin correspondiente.\n");
    printf("   Presiona cualquier tecla para iniciar...\n");
    getchar();
    
    for (int i = 0; i < segundos; i++) {
        // Escribir y leer en el LCR para generar actividad
        outb(0x80, base + 3);
        usleep(5);
        inb(base + 3);
        outb(0x03, base + 3);
        usleep(5);
        inb(base + 3);
        
        printf(".");
        fflush(stdout);
        usleep(900000);  // 0.9 segundos
    }
    printf("\n   Bucle de diagnóstico completado.\n");
}

int probar_direccion(unsigned short base_addr, unsigned debug) {
    unsigned short lcr_addr = base_addr + 3;
    unsigned char valor_prueba = 0x80; // Un valor fácil de ver en el osciloscopio (DLAB=1)
    
    if (!debug)
    {
        printf("\n--- Probando dirección base 0x%03X ---\n", base_addr);
        printf("Escribiendo 0x%02X en LCR (0x%04X).\n", valor_prueba, lcr_addr);
    }
    
    // Escribir el valor en el LCR
    outb(valor_prueba, lcr_addr);
    
    // Leer el LCR para ver si el chip responde (opcional, pero útil)
    unsigned char leido = inb(lcr_addr);
    if (leido == valor_prueba) {
        if (!debug)
        {
            printf("✅ ¡RESPUESTA! El LCR devolvió 0x%02X. Parece que el chip está en 0x%03X.\n", leido, base_addr);
            return 0;
        }
    } else {
        if (!debug)
        {
            printf("❌ No hay respuesta en 0x%03X (LCR leyó 0x%02X).\n", base_addr, leido);
            return 1;
        }
    }
}
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
void corrimiento_binario_analisis(unsigned short base) {
    unsigned char resultados[256];
    int fallos = 0;

    printf("\n=== CORRIMIENTO BINARIO CON ANÁLISIS DE BITS ===\n");
    printf("Recolectando datos...\n");

    // 1. Guardar todos los resultados en memoria
    for (int valor = 0x00; valor <= 0xFF; valor++) {
        unsigned char esc = (unsigned char)valor;
        outb(esc, base + 4);
        usleep(2);
        unsigned char ley = inb(base + 4);
        resultados[valor] = ley;
        if (esc != ley) fallos++;
    }

    // 2. Analizar cada bit (0 a 7)
    printf("\n--- ANÁLISIS DE BITS ---\n");
    int bits_fijos = 0;
    
    for (int bit = 0; bit < 8; bit++) {
        int siempre_0 = 1;
        int siempre_1 = 1;
        int es_fijo = 0;

        // Verificar si el bit es siempre 0 o siempre 1
        for (int valor = 0x00; valor <= 0xFF; valor++) {
            int bit_actual = (resultados[valor] >> bit) & 1;
            if (bit_actual == 1) siempre_0 = 0;
            if (bit_actual == 0) siempre_1 = 0;
        }

        if (siempre_0) {
            printf("   Bit %d: SIEMPRE 0 (fijo en 0)\n", bit);
            bits_fijos++;
            es_fijo = 1;
        } else if (siempre_1) {
            printf("   Bit %d: SIEMPRE 1 (fijo en 1)\n", bit);
            bits_fijos++;
            es_fijo = 1;
        } else {
            printf("   Bit %d: VARIABLE (funciona correctamente)\n", bit);
        }
    }

    // 3. Resumen de resultados
    printf("\n--- RESUMEN ---\n");
    if (bits_fijos == 0) {
        printf("✅ No se detectaron bits fijos. El bus de datos funciona correctamente.\n");
        if (fallos > 0) {
            printf("   Aunque hay %d fallos, ningún bit está fijo.\n", fallos);
            printf("   Mostrando todos los fallos...\n\n");
            // Mostrar todos los fallos (solo si no hay bits fijos)
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
        printf("   Revisa los bits fijos indicados arriba.\n");
    }
}

// Entrar en modo configuración (SMSC FDC37C669 requiere SOLO UNA VEZ el byte 0x55)
void habilitar_configuracion() {
    outb(0x55, SIO_CONFIG_PORT); 
}

// Salir del modo configuración (escribir 0xAA)
void deshabilitar_configuracion() {
    outb(0xAA, SIO_CONFIG_PORT);
}

// Escribir un valor en un registro de configuración del Super I/O
void escribir_registro_sio(unsigned char registro, unsigned char valor) {
    outb(registro, SIO_CONFIG_PORT);  // Seleccionar el registro
    outb(valor, SIO_DATA_PORT);       // Escribir el valor
}

// Leer un valor de un registro de configuración del Super I/O
unsigned char leer_registro_sio(unsigned char registro) {
    outb(registro, SIO_CONFIG_PORT);  // Seleccionar el registro
    return inb(SIO_DATA_PORT);        // Leer el valor
}

// --------------------------------------------------------------
// 3. Función para escribir en el LCR del UART
// --------------------------------------------------------------
void escribir_lcr(unsigned short base_addr, unsigned char valor, uint debug) {
    unsigned short lcr_addr = base_addr + 3;  // El LCR siempre está en base+3
    if (!debug)
    {
        printf("Escribiendo 0x%02X en el LCR (dirección 0x%04X)...\n", valor, lcr_addr);
    }
    outb(valor, lcr_addr);
}

// --------------------------------------------------------------
// 4. Función para leer el LCR del UART (y verificar)
// --------------------------------------------------------------
unsigned char leer_lcr(unsigned short base_addr, uint debug) {
    unsigned short lcr_addr = base_addr + 3;
    unsigned char valor = inb(lcr_addr);
    if (!debug)
    {
        printf("Valor leído del LCR (0x%04X): 0x%02X\n", lcr_addr, valor);
    }
    return valor;
}

// --------------------------------------------------------------
// 2. Función para configurar el UART1 en una dirección base
// --------------------------------------------------------------
int configurar_uart1_en(uint base_addr,uint debug) {
    habilitar_configuracion();
    if (!debug)
    {
        printf("Configurando UART1 en dirección base 0x%02X...\n", base_addr);
    }
    

    // --- Paso 2: Seleccionar el dispositivo lógico UART1 (0x24) ---
    escribir_registro_sio(0x24, base_addr);
    
    // --- PAUSA DE DEBUEGO (0.1 SEGUNDOS) ---
    usleep(100000); 

    // --- LECTURA DE VERIFICACIÓN ---
    if (!debug)
    {
        printf("[DEBUG] Verificando registro UART1...\n");
    }
    
    unsigned char reg_24_validar = leer_registro_sio(0x24);
    
    if ((reg_24_validar == base_addr)&(!debug)) { 
        printf("[OK] Confirmado: CR24 retiene el valor 0x%02X con éxito.\n", reg_24_validar);
        //return 1; // Retorna Éxito
    } else if (!debug) {
        printf("[ER] Error de bus: CR24 leyó 0x%02X (Se esperaba 0x%02X).\n", reg_24_validar, base_addr);
        return 0; // Retorna Falla
    }
    deshabilitar_configuracion();
    if (!debug)
    {
        printf("[UART-1] Activando bit DLAB en el LCR...\n");
    }
    escribir_lcr(UART3_BASE, 0x83, debug); 
    
    // Pausa de 0.1 seg para debugeo
    usleep(100000); 

    // Verificamos la lectura
    unsigned char lcr_validar = leer_lcr(UART3_BASE, debug );
    if ((lcr_validar == 0x83)&(!debug)) {
        printf("[UART-OK] LCR retiene el valor 0x83 con éxito. Candado de velocidad abierto.\n");
        //return 1; // Retorna Éxito
    } else if (!debug){
        printf("[UART-ER] Error: LCR devolvió 0x%02X. Abortando.\n", lcr_validar);
        return 0;
    }
    // ===================================================================
    // PASOS COMPLEMENTARIOS OBLIGATORIOS PARA REAVIVAR EL PIN FÍSICO
    // ===================================================================
    
    // PASO A: Cargar Divisor LSB para 4800 baudios (Valor = 24 -> 0x18)
    if (!debug)
    {
        printf("[UART-1] Cargando Divisor LSB (0x18) en la dirección 0x%04X...\n", UART3_BASE);
    }
    
    outb(0x18, UART3_BASE + 0); // Modifica el registro DLL
    
    // PASO B: Cargar Divisor MSB para 4800 baudios (Valor = 0 -> 0x00)
    if (!debug)
    {
        printf("[UART-1] Cargando Divisor MSB (0x00) en la dirección 0x%04X...\n", UART3_BASE + 1);
    }
    
    outb(0x00, UART3_BASE + 1); // Modifica el registro DLM
    
    // PASO C: Fijar trama 8N1 y APAGAR el bit DLAB (Valor = 0x03)
    if (!debug)
    {
        printf("[UART-1] Fijando trama final 8N1 y cerrando DLAB...\n");
    }
    
    escribir_lcr(UART3_BASE, 0x03, debug); // El bit 7 se pone en 0. Libera el THR para datos.
    
    // Verificación final del estado operativo de la USART
    unsigned char lcr_final = leer_lcr(UART3_BASE, debug);
    if (lcr_final == 0x03) {
        if (!debug)
        {
            printf("[UART-READY] UART1 configurada a 4800 8N1 de forma exitosa. Lista para transmitir.\n");
        }
        return 1; // Retorna Éxito real de transmisión
    } else {
        if (!debug)
        {
            printf("[UART-ER] Error: El LCR no regresó a modo normal (Leyó: 0x%02X).\n", lcr_final);
        }
        return 0;
    }
}

// --------------------------------------------------------------
// 2. Función para configurar el UART2 en una dirección base
// --------------------------------------------------------------
int configurar_uart2_en(uint base_addr, uint debug) {
    habilitar_configuracion();
    if (!debug)
    {
        printf("Configurando UART2 en dirección base 0x%02X...\n", base_addr);
    }

    // --- Paso 2: Seleccionar el dispositivo lógico UART1 (0x24) ---
    escribir_registro_sio(0x25, base_addr);
    
    // --- PAUSA DE DEBUEGO (0.1 SEGUNDOS) ---
    usleep(100000); 

    // --- LECTURA DE VERIFICACIÓN ---
    if (!debug)
    {
        printf("[DEBUG] Verificando registro UART2...\n");
    }
    unsigned char reg_25_validar = leer_registro_sio(0x25);
    
    if (reg_25_validar == base_addr) { 
        if (!debug)
        {
            printf("[OK] Confirmado: CR25 retiene el valor 0x%02X con éxito.\n", reg_25_validar);
        }
        //return 1; // Retorna Éxito
    } else {
        if (!debug)
        {
            printf("[ER] Error de bus: CR25 leyó 0x%02X (Se esperaba 0x%02X).\n", reg_25_validar, base_addr);
        }
        return 0; // Retorna Falla
    }
    deshabilitar_configuracion();

    if (!debug)
    {
        printf("[UART-1] Activando bit DLAB en el LCR...\n");
    }
    escribir_lcr(UART4_BASE, 0x83,debug); 
    
    // Pausa de 0.1 seg para debugeo
    usleep(100000); 

    // Verificamos la lectura
    unsigned char lcr_validar = leer_lcr(UART4_BASE, debug);
    if (lcr_validar == 0x83) {
        if (!debug)
        {
            printf("[UART-OK] LCR retiene el valor 0x83 con éxito. Candado de velocidad abierto.\n");
        }
        //return 1; // Retorna Éxito
    } else {
        if (!debug)
        {
            printf("[UART-ER] Error: LCR devolvió 0x%02X. Abortando.\n", lcr_validar);
        }
        return 0;
    }
    // ===================================================================
    // PASOS COMPLEMENTARIOS OBLIGATORIOS PARA REAVIVAR EL PIN FÍSICO
    // ===================================================================
    
    // PASO A: Cargar Divisor LSB para 4800 baudios (Valor = 24 -> 0x18)
    if (!debug)
    {
        printf("[UART-2] Cargando Divisor LSB (0x18) en la dirección 0x%04X...\n", UART4_BASE);
    }
    outb(0x18, UART4_BASE + 0); // Modifica el registro DLL
    
    // PASO B: Cargar Divisor MSB para 4800 baudios (Valor = 0 -> 0x00)
    if (!debug)
    {
        printf("[UART-1] Cargando Divisor MSB (0x00) en la dirección 0x%04X...\n", UART4_BASE + 1);
    }
    outb(0x00, UART4_BASE + 1); // Modifica el registro DLM
    
    // PASO C: Fijar trama 8N1 y APAGAR el bit DLAB (Valor = 0x03)
    if (!debug)
    {
        printf("[UART-1] Fijando trama final 8N1 y cerrando DLAB...\n");
    }
    escribir_lcr(UART4_BASE, 0x03,debug); // El bit 7 se pone en 0. Libera el THR para datos.
    
    // Verificación final del estado operativo de la USART
    unsigned char lcr_final = leer_lcr(UART4_BASE, debug);
    if (lcr_final == 0x03) {
        if (!debug)
        {
            printf("[UART-READY] UART2 configurada a 4800 8N1 de forma exitosa. Lista para transmitir.\n");
        }
        return 1; // Retorna Éxito real de transmisión
    } else {
        if (!debug)
        {
            printf("[UART-ER] Error: El LCR no regresó a modo normal (Leyó: 0x%02X).\n", lcr_final);
        }
        return 0;
    }
}

// --------------------------------------------------------------
// 5. Función para enviar un solo byte por la UART (Modo Polling)
// --------------------------------------------------------------
void transmitir_byte(unsigned short base_addr, unsigned char dato) {
    unsigned short lsr_addr = base_addr + 5; // El LSR siempre está en Base + 5
    
    // Monitorear el Bit 5 (0x20). Esperamos mientras el bit sea 0 (Buffer ocupado)
    while ((inb(lsr_addr) & 0x20) == 0);
    
    // El buffer está libre, enviamos el dato al THR (Base + 0)
    outb(dato, base_addr + 0);
}

// --------------------------------------------------------------
// 6. Función para enviar una cadena de texto por la UART
// --------------------------------------------------------------
void transmitir_cadena(unsigned short base_addr, const char *cadena) {
    while (*cadena) {               // Mientras no lleguemos al final de la cadena
        transmitir_byte(base_addr, (unsigned char)*cadena);  // Enviar cada carácter
        cadena++;                   // Avanzar al siguiente carácter
    }
}

// ================================================================
// 2. FUNCIONES DE PAUSA
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
// 3. PRUEBAS GENERALES (TEST_HW)
// ================================================================

int test_fdc37c669() {
    unsigned char id_leido = 0;
    uint UART3 = (UART3_BASE & 0xFF8) >> 2;
    uint UART4 = (UART4_BASE & 0xFF8) >> 2;
    
    // --- Paso 1: Entrar en modo configuración ---
    habilitar_configuracion();
    
       // El registro 0x0D en el FDC37C669 contiene el Device ID de fábrica
    id_leido = leer_registro_sio(0x0D);
    if (id_leido != 0x03)
    {
        deshabilitar_configuracion();
        return 0;
    }
    deshabilitar_configuracion();
    
    if ((configurar_uart1_en(UART3,1) != 1)& (configurar_uart2_en(UART4,1) != 1) )
    {
        deshabilitar_configuracion(); // Cerramos el chip antes de abortar
        return 0;
    } 

    return (1);
}

int test_st16c2550(unsigned short base) {
    

    if(probar_direccion(base, 1) )
    {
        return 1;
    }
    return 0;
    
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

    // 1. Verificar presencia en PCI
    fp = popen("cat /sys/bus/pci/devices/*/vendor 2>/dev/null | grep 1131", "r");
    if (fp == NULL) return 0;
    int found = (fgets(buffer, sizeof(buffer), fp) != NULL);
    pclose(fp);
    if (!found) return 0;

    // 2. Verificar que el driver está cargado
    fp = popen("lsmod | grep -q saa7134", "r");
    if (fp == NULL) return 0;
    int driver_loaded = (pclose(fp) == 0);
    if (!driver_loaded) return 0;

    // 3. Verificar que /dev/video0 existe
    fp = popen("test -e /dev/video0", "r");
    if (fp == NULL) return 0;
    int dev_exists = (pclose(fp) == 0);
    if (!dev_exists) return 0;

    // 4. Verificar que el driver no está en modo genérico (opcional)
    //    Si el driver está en modo genérico, la tarjeta no funciona correctamente
    fp = popen("v4l2-ctl --all | grep -q 'UNKNOWN/GENERIC'", "r");
    if (fp == NULL) return 0;
    int is_generic = (pclose(fp) == 0);
    if (is_generic) return 0; // Falla si está en modo genérico

    // 5. Verificar que hay señal de video (opcional)
    //    Intentar capturar un fotograma con dd (rápido)
    fp = popen("dd if=/dev/video0 of=/dev/null bs=1M count=1 2>/dev/null", "r");
    if (fp == NULL) return 0;
    int capture_ok = (pclose(fp) == 0);
    if (!capture_ok) return 0;

    return 1; // Todo está bien
}

int test_MVBC01(){
    escribir_word(REG_SCR, conf_SCR);
    escribir_word(REG_MCR, conf_MCR);
    unsigned  scr_leido = leer_word(REG_SCR);
    unsigned  mcr_leido = leer_word(REG_MCR);
    if ((mcr_leido & 0x0007) == 0x0002) {
        //printf("✅ MVBC01 acoplado correctamente en MODO 2 (64 KB).\n\n");
    } else {
        printf("❌ ERROR: El MCR no retiene el Modo 2. Revisa tu pin 129 IOCS16#.\n");
        return 0;
    }
    escribir_word(REG_SCR, full_SCR);
    sleep(1);
    unsigned  patron_55 = 0x5555;
    unsigned  patron_aa = 0xAAAA;
    
    int errores = 0;
    
    sleep(1);
    // ESCRIBIR Y VERIFICAR PATRÓN 1 (0x5555)
    
    for ( TM_I=TM_BASE; TM_I < TM_BASE + TM_SIZE; TM_I += 4) {
        escribir_word(TM_I, patron_55);
    }
    for ( TM_I=TM_BASE; TM_I < TM_BASE + TM_SIZE; TM_I += 4) {
        unsigned  leido = leer_word(TM_I);
        if (leido != patron_55) {
            errores++;
            if (errores > 10) return 0; // Freno de seguridad
        }
    }

    sleep(1); 
        // ESCRIBIR Y VERIFICAR PATRÓN 1 (0xAAAA)
    for ( TM_I=TM_BASE; TM_I < TM_BASE + TM_SIZE; TM_I += 4) {
        escribir_word(TM_I, patron_aa);
    }
    
    for ( TM_I=TM_BASE; TM_I < TM_BASE + TM_SIZE; TM_I += 4) {
        unsigned  leido = leer_word(TM_I);
        if (leido != patron_aa) {
            errores++;
            if (errores > 10) return 0; // Freno de seguridad
        }
    }

    // VERDICTO FINAL DEL RACK
    if (errores == 0) {
        return 1;
    } else {
        return 0;
    }
    
}

int test_MVBCS1(){

    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) {
        perror("❌ ERROR: No se pudo abrir /dev/mem (¿Usaste sudo?)");
        return 0;
    }
    // Mapear la página de 4 KB que contiene la dirección 0xF00000
    void *mem_ptr = mmap(NULL, TAMANO_BLOQUE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, BASE_MEMORIA_RANGO3);
    if (mem_ptr == MAP_FAILED) {
        perror("❌ ERROR: mmap falló (¿El kernel bloquea esta zona?)");
        close(fd);
        return 0;
    }

    // Ahora podemos acceder a la memoria directamente como un puntero
    unsigned short *mem_16 = (unsigned short *)mem_ptr;

    unsigned short dato_envio1 = 0x5555;
    unsigned short dato_envio2 = 0xAAAA;
    unsigned short dato_leido1 = 0;
    unsigned short dato_leido2 = 0;

    //printf("[PASO 1] Escribiendo 0x5555 en 0xF00000...\n");
    *mem_16 = dato_envio1;
    dato_leido1 = *mem_16;

    //printf("[PASO 2] Escribiendo 0xAAAA en 0xF00000...\n");
    *mem_16 = dato_envio2;
    dato_leido2 = *mem_16;

    

    if (dato_leido1 == 0x5555 && dato_leido2 == 0xAAAA) {
        munmap(mem_ptr, TAMANO_BLOQUE);
        close(fd);  
        return 1;
    } else {
        return 0;
    }

    
    return 0;
}
// ================================================================
// 4. FUNCIONES DE DIAGNÓSTICO (PLACEHOLDERS)
// ================================================================

int diagnosticar_fdc37c669() {
    printf("\n=== DIAGNÓSTICO DEL FDC37C669 (U75) ===\n");
    printf("Función en desarrollo...\n");
    unsigned char id_leido = 0;
    uint UART3 = (UART3_BASE & 0xFF8) >> 2;
    uint UART4 = (UART4_BASE & 0xFF8) >> 2;
    printf("========================================================\n");
    printf("Prueba de UART3 y USART4 (FDC37C669) 📊\n");
    printf("========================================================\n\n");
    printf("[1] Despertando chip en 0x3F0...\n");
    // --- Paso 1: Entrar en modo configuración ---
    habilitar_configuracion();
    
       // El registro 0x0D en el FDC37C669 contiene el Device ID de fábrica
    id_leido = leer_registro_sio(0x0D);
    if (id_leido == 0x03)
    {
        printf("[2] ID correcto...\n");
    }
    else
    {
        printf("[ER] Error al leer ID termina prueba ...\n");
        deshabilitar_configuracion();
        return 1;
    }
    deshabilitar_configuracion();
    printf("[3] Preparar el Usart3...\n");
    if ((configurar_uart1_en(UART3,0) == 1) & (configurar_uart2_en(UART4,0) == 1))
    {
        printf("[4] Configuración de UART1 exitosa en hardware.\n");       
        // Aquí ya puedes continuar de frente con:
        // escribir_lcr(UART3_BASE, 0x83); -> Configurar Baudios
    } 
    else 
    {
        printf("[ER] Falla catastrófica al configurar la dirección de UART1. Terminando.\n");
        deshabilitar_configuracion(); // Cerramos el chip antes de abortar
        return 1;
    }
    
    printf("[5] Configuracion exitosa...\n");
//////////////////////////fin de configuracion//////////////////////////////
    printf("\n🚀 EMPEZANDO TRANSMISIÓN CONTINUA PARA OSCILOSCOPIO... 🚀\n");
    printf("Conecta la sonda en el Pin 79 (UART1) o Pin 72 (UART2).\n");
    printf("Presiona Ctrl+C para detener la prueba.\n\n");
    transmitir_cadena(UART3_BASE, "Hola mundo desde UART1 (0x3E8)\r\n");
/*
    unsigned long contador = 0;
    unsigned short lsr_addr = UART3_BASE + 5; // Dirección del LSR
    unsigned short rbr_addr = UART3_BASE + 0; // Dirección del RBR (Receiver Buffer Register)
    unsigned char dato_recibido;
    // Aquí irán las pruebas dedicadas para el FDC37C669
    */
    pausa();
}

void diagnosticar_st16c2550() {
    printf("\n=== DIAGNÓSTICO DEL ST16C2550 (U59) ===\n");
    
    // Canal A
    printf("Probando Canal A en 0x%04X... ", UART5_BASE);
    if (probar_direccion(UART5_BASE, 0)) {
        printf("✅ OK\n");
    } else {
        printf("❌ FALLO\n");
        bucle_diagnostico_uart(UART5_BASE, 30);
    }
    
    pausa();
    
    // Canal B
    printf("Probando Canal B en 0x%04X... ", UART6_BASE);
    if (probar_direccion(UART6_BASE, 0)) {
        printf("✅ OK\n");
    } else {
        printf("❌ FALLO\n");
        bucle_diagnostico_uart(UART6_BASE, 30);
    }
    
    pausa_con_mensaje("Diagnóstico del ST16C2550 completado.");
}

void diagnosticar_sja1000t() {
    printf("\n=== DIAGNÓSTICO DEL SJA1000T (U41) ===\n");
    printf("Función en desarrollo...\n");
    // Aquí irán las pruebas dedicadas para el SJA1000T
    unsigned short base = SJA_BASE;
    unsigned char test_val_1 = 0x55;
    unsigned char test_val_2 = 0xAA;
    unsigned char read_val_1, read_val_2;

    printf("\n=== DIAGNÓSTICO DEL SJA1000T (U41) ===\n");
    printf("Presiona cualquier tecla para continuar con las pruebas dedicadas...\n");
    //getchar();

    // --- 1. Prueba lineal directa (Opción 4) ---
    printf("\n[1] Prueba lineal directa en 0x%03X...\n", base);
    printf("Presiona cualquier tecla para ejecutar...\n");
    getchar();

    // Forzar modo reset
    outb(0x01, base + 0);
    usleep(5);

    // Escribir 0x55 en ACR (Registro 4)
    outb(test_val_1, base + 4);
    usleep(5);
    read_val_1 = inb(base + 4);

    // Escribir 0xAA en ACR (Registro 4)
    outb(test_val_2, base + 4);
    usleep(5);
    read_val_2 = inb(base + 4);

    // Salir del modo reset
    outb(0x00, base + 0);

    printf("\nResultado de Lectura Directa en 0x%04X:\n", base + 4);
    printf("-> Intentó 0x%02X -> Leyó: 0x%02X\n", test_val_1, read_val_1);
    printf("-> Intentó 0x%02X -> Leyó: 0x%02X\n", test_val_2, read_val_2);

    // --- 2. Análisis del resultado ---
    if (read_val_1 == test_val_1 && read_val_2 == test_val_2) {
        printf("\n✅ ¡ÉXITO! El chip responde correctamente.\n");
        pausa();
        return;
    }
    else if ((read_val_1 == 0xFF && read_val_2 == 0xFF) || 
             (read_val_1 == 0x00 && read_val_2 == 0x00)) {
        printf("\n❌ SIN COMUNICACIÓN: El chip no responde.\n");
        printf("   Posible causa: GAL U36, soldadura o chip dañado.\n");
        pausa_con_mensaje("Pasando a prueba de osciloscopio...");
        
        // --- 3. Prueba de osciloscopio (Opción 2) ---
        printf("\n[2] Prueba de osciloscopio (CS#)\n");
        printf("Conecta tu sonda al pin CS# (Pin 11) del U41.\n");
        printf("Presiona cualquier tecla para iniciar el bucle de 30 segundos...\n");
        getchar();

        // Bucle de 30 segundos con actividad
        for (int i = 0; i < 30; i++) {
            outb(0x01, base + 0);  // Reset
            outb(0x55, base + 4);  // Escribir ACR
            inb(base + 4);         // Leer ACR
            usleep(100000);        // 0.1 segundos
            printf(".");
            fflush(stdout);
        }
        printf("\nBucle de osciloscopio completado.\n");
        pausa();
    }
    else {
        printf("\n⚠️ BITS CORRUPTOS detectados.\n");
        printf("   Posible causa: Pistas rotas, soldaduras frías o buffer dañado.\n");
        pausa_con_mensaje("Pasando a corrimiento binario...");
    }

    // --- 4. Corrimiento binario (Opción 6) ---
    printf("\n[3] Corrimiento binario de datos (0x00 a 0xFF) en Registro 4...\n");
    printf("Presiona cualquier tecla para ejecutar...\n");
    getchar();

    // Forzar modo reset
    outb(0x01, base + 0);
    usleep(5);

    corrimiento_binario_analisis(base);

    // Salir del modo reset
    outb(0x00, base + 0);

    
    pausa_con_mensaje("Diagnóstico del SJA1000T completado.");

    pausa();
}

void diagnosticar_saa7130hl() {
    printf("\n=== DIAGNÓSTICO DEL SAA7130HL (U29) ===\n");
    printf("Función en desarrollo...\n");
    // Aquí irán las pruebas dedicadas para el SAA7130HL
    printf("\n=== DIAGNÓSTICO DEL SAA7130HL (U29) ===\n");
    printf("Este diagnóstico se ejecutará en pasos. Presiona una tecla para avanzar.\n");
    pausa();

    // 1. Verificar presencia en PCI
    printf("\n[1] Verificando dispositivo en PCI...\n");
    system("lspci -n | grep 1131:7130 | sed 's/^/   /'");
    pausa();

    // 2. Verificar driver
    printf("\n[2] Verificando driver cargado...\n");
    system("lsmod | grep saa7134 | sed 's/^/   /'");
    pausa();

    // 3. Estado del dispositivo
    printf("\n[3] Estado del dispositivo (v4l2-ctl --all):\n");
    system("v4l2-ctl --all 2>/dev/null | sed 's/^/   /'");
    pausa();

    // 4. Entradas de video
    printf("\n[4] Entradas de video disponibles:\n");
    system("v4l2-ctl --list-inputs 2>/dev/null | sed 's/^/   /'");
    pausa();

    // 5. Probar card=2
    printf("\n[5] Probando configuración con card=2...\n");
    printf("   Descargando módulo...\n");
    system("sudo modprobe -r saa7134 2>/dev/null");
    printf("   Cargando módulo con card=2...\n");
    system("sudo modprobe saa7134 card=2 2>/dev/null");
    printf("   Estado actual:\n");
    system("v4l2-ctl --all 2>/dev/null | grep -E 'Card type|Hardware revision' | sed 's/^/   /'");
    pausa();

    pausa_con_mensaje("Diagnóstico del SAA7130HL completado.");
    pausa();
}

void diagnosticar_MVBC01(){
    
    printf("==================================================\n");
    printf("🚀 INITIALIZATION & TRAFFIC MEMORY TEST (16-BIT)\n");
    printf("==================================================\n");

    // 1. Inicialización del SCR con tu valor binario auditado (Intel Mode + IL0)
    printf("[1/4] Cargando SCR (0x%04X) con tu valor maestro 0xE21D...\n", REG_SCR);
    //outw(0xE21D, REG_SCR);
    //usleep(1000);
    escribir_word(REG_SCR, conf_SCR);
    sleep(1);
    // 2. Inicialización del MCR en Modo 2 (64 KB) respetando los bits RO altos (0xB031)
    printf("[2/4] Cargando MCR (0x%04X) en Modo 2 (0x0002)...\n", REG_MCR);
    escribir_word(REG_MCR, conf_MCR);
    sleep(1);
    // 3. Verificación de retención de registros
    unsigned  scr_leido = leer_word(REG_SCR);
    sleep(1);
    unsigned  mcr_leido = leer_word(REG_MCR);
    sleep(1);

    printf("\n--- VERIFICACIÓN DE CONTROL DEL SILICIO ---\n");
    printf("  SCR Leído: 0x%04X (Esperado: 0xE21D)\n", scr_leido);
    printf("  MCR Leído: 0x%04X (Esperado: Modo 2 en bits bajos)\n", mcr_leido);

    // Filtramos los bits bajos del MCR (MCM) para validar tu '2'
    if ((mcr_leido & 0x0007) == 0x0002) {
        printf("✅ MVBC01 acoplado correctamente en MODO 2 (64 KB).\n\n");
    } else {
        printf("❌ ERROR: El MCR no retiene el Modo 2. Revisa tu pin 129 IOCS16#.\n");
        
    }
    // 4. corriendo full 
    printf("[4/5] Corriendo full MVBC01 (0x%04X) en Modo 2 (0x0002)...\n", REG_SCR);
    escribir_word(REG_SCR, full_SCR);
    sleep(1);
    // 5. BARRIDO DE LA VICTORIA EN LA SRAM SAMSUNG (TRAFFIC MEMORY)
    printf("[5/5] Iniciando barrido síncrono en la SRAM libre (0x%04X - 0x%04X)...\n", 
           TM_BASE, TM_BASE + TM_SIZE - 2);

    unsigned  short patron_55 = 0x5555;
    unsigned  short patron_aa = 0xAAAA;
    
    int errores = 0;
    
    sleep(1);
    // ESCRIBIR Y VERIFICAR PATRÓN 1 (0x5555)
    printf("  ✍️  Escribiendo ráfaga 0x5555...\n");
    for (unsigned dir = TM_BASE; dir <= (TM_BASE+TM_SIZE); dir += 4) { // Saltos de 4
        escribir_word(dir, patron_55);
    }
    /*
    for ( TM_I=TM_BASE; TM_I < TM_BASE + TM_SIZE; TM_I += 4) {
        escribir_word(TM_I, patron_55);
    }
        */
    /*
    printf("\n--- LECTURA DE RANGO 0x%04X - 0x%04X ---\n", TM_BASE, (TM_BASE+TM_SIZE));
    printf("Dirección | Valor (WORD)\n");
    printf("----------|-------------\n");
    for (unsigned short dir = TM_BASE; dir <= (TM_BASE+TM_SIZE); dir += 4) { // Saltos de 4
        unsigned short valor = leer_word(dir);
        printf("0x%04X   | 0x%04X\n", dir, valor);
    }
    */
    printf("  📖 Verificando ráfaga 0x5555...\n");
    for ( TM_I=TM_BASE; TM_I < TM_BASE + TM_SIZE; TM_I += 4) {
        unsigned  leido = leer_word(TM_I);
        if (leido != patron_55) {
            printf("    ❌ Fallo en 0x%04X: Enviado 0x5555 -> Leyó: 0x%04X\n", TM_I, leido);
            errores++;
            if (errores > 100) break; // Freno de seguridad
        }
    }
    
    sleep(1); 
        // ESCRIBIR Y VERIFICAR PATRÓN 1 (0xAAAA)
    printf("  ✍️  Escribiendo ráfaga 0xAAAA...\n");
    for ( TM_I=TM_BASE; TM_I < TM_BASE + TM_SIZE; TM_I += 4) {
        escribir_word(TM_I, patron_aa);
    }
    
    

    printf("  📖 Verificando ráfaga 0xAAAA...\n");
    for ( TM_I=TM_BASE; TM_I < TM_BASE + TM_SIZE; TM_I += 4) {
        unsigned  leido = leer_word(TM_I);
        if (leido != patron_aa) {
            printf("    ❌ Fallo en 0x%04X: Enviado 0xAAAA -> Leyó: 0x%04X\n", TM_I, leido);
            errores++;
            if (errores > 200) break; // Freno de seguridad
        }
    }

    // VERDICTO FINAL DEL RACK
    printf("\n==================================================\n");
    if (errores == 0) {
        printf("✅ ¡VICTORIA ABSOLUTA! 0 errores en la Traffic Memory.\n");
        printf("   El MVBC01 abrió el puente y la SRAM Samsung retiene los 16 bits.\n");
    } else {
        printf("❌ TEST FALLIDO: Se detectaron errores de acoplamiento.\n");
    }
    printf("==================================================\n");

}

void diagnosticar_MVBCS1(){
    printf("\n==================================================\n");
    printf("     TEST MVBCS1 VIA MMAP: ASIC MVBCS1\n");
    printf("==================================================\n");
    printf("[INFO] Abriendo /dev/mem...\n");

    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) {
        perror("❌ ERROR: No se pudo abrir /dev/mem (¿Usaste sudo?)");
        return;
    }

    // Mapear la página de 4 KB que contiene la dirección 0xF00000
    printf("[INFO] Mapeando memoria física en 0xF00000...\n");
    void *mem_ptr = mmap(NULL, TAMANO_BLOQUE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, BASE_MEMORIA_RANGO3);
    if (mem_ptr == MAP_FAILED) {
        perror("❌ ERROR: mmap falló (¿El kernel bloquea esta zona?)");
        close(fd);
        return;
    }

    // Ahora podemos acceder a la memoria directamente como un puntero
    unsigned short *mem_16 = (unsigned short *)mem_ptr;

    unsigned short dato_envio1 = 0x5555;
    unsigned short dato_envio2 = 0xAAAA;
    unsigned short dato_leido1 = 0;
    unsigned short dato_leido2 = 0;

    printf("[PASO 1] Escribiendo 0x5555 en 0xF00000...\n");
    *mem_16 = dato_envio1;
    dato_leido1 = *mem_16;

    printf("[PASO 2] Escribiendo 0xAAAA en 0xF00000...\n");
    *mem_16 = dato_envio2;
    dato_leido2 = *mem_16;

    // Reportar resultados
    printf("\n==================================================\n");
    printf("📊 RESULTADO DEL ESCANEO DE MEMORIA ALTA\n");
    printf("==================================================\n");
    printf("-> Patrón 1 | Enviado: 0x5555 | Recuperado: 0x%04X\n", dato_leido1);
    printf("-> Patrón 2 | Enviado: 0xAAAA | Recuperado: 0x%04X\n", dato_leido2);

    if (dato_leido1 == 0x5555 && dato_leido2 == 0xAAAA) {
        printf("\n✅ ¡VICTORIA! El bus de 16 bits de la tarjeta Siemens respondió impecable.\n");
    } else {
        printf("\n❌ FALLA: Los datos no coinciden. El bus devolvió basura o está flotando.\n");
    }
    printf("==================================================\n");

    munmap(mem_ptr, TAMANO_BLOQUE);
    close(fd);
}
// ================================================================
// 5. MOSTRAR RESULTADOS GENERALES
// ================================================================

void mostrar_resultados_generales() {
    printf("\n========================================================\n");
    printf("   TEST DE HARDWARE - TARJETA PANTALLA PIXY\n");
    printf("========================================================\n\n");

    printf("[1] FDC37C669 (U75) en 0x%03X... ", SIO_CONFIG_PORT);
    fflush(stdout);
    printf(test_fdc37c669() ? "✅ DETECTADO\n" : "❌ NO DETECTADO\n");

    printf("[2a] ST16C2550 (U59) en 0x%03X... ", UART5_BASE);
    fflush(stdout);
    printf(test_st16c2550(UART5_BASE) ? "✅ DETECTADO\n" : "❌ NO DETECTADO\n");

    printf("[2b] ST16C2550 (U59) en 0x%03X... ", UART6_BASE);
    fflush(stdout);
    printf(test_st16c2550(UART6_BASE) ? "✅ DETECTADO\n" : "❌ NO DETECTADO\n");

    printf("[3] SJA1000T (U41) en 0x%03X... ", SJA_BASE);
    fflush(stdout);
    printf(test_sja1000t(SJA_BASE) ? "✅ DETECTADO\n" : "❌ NO DETECTADO\n");

    // ✅ Este ahora usa la nueva versión de test_saa7130hl()
    printf("[4] SAA7130HL (U29) en PCI... ");
    fflush(stdout);
    printf(test_saa7130hl() ? "✅ DETECTADO\n" : "❌ NO DETECTADO\n");

    printf("[5] MVBC01 (U49) en 0x%03X... ", MVBC_BASE);
    fflush(stdout);
    printf(test_MVBC01() ? "✅ DETECTADO\n" : "❌ NO DETECTADO\n");
/*
    printf("[6] MVBCS1 en 0x%03X... ", BASE_MEMORIA_RANGO3);
    fflush(stdout);
    printf(test_MVBCS1() ? "✅ DETECTADO\n" : "❌ NO DETECTADO\n");
*/
    printf("\n========================================================\n");
}


// ================================================================
// 6. MAIN
// ================================================================
// ================================================================
// 7. MENÚ PRINCIPAL
// ================================================================

void menu() {
    printf("\n========================================================\n");
    printf("   MENÚ DE DIAGNÓSTICO - MONITORES PIXY\n");
    printf("========================================================\n");
    printf("1. Prueba General (Todas)\n");
    printf("2. FDC37C669 (Super I/O)\n");
    printf("3. ST16C2550 (UART)\n");
    printf("4. SJA1000T (CAN)\n");
    printf("5. SAA7130HL (Video)\n");
    printf("6. MVBC01\n");
    //printf("7. MVBC01\n");
    printf("8. Salir\n");
    printf("========================================================\n");
    printf("Selecciona una opción: ");
}

int main() {
    if (iopl(3) < 0) {
        perror("Error: Ejecutar con 'sudo'");
        return 1;
    }

  

    int opcion;
    do {
        menu();
        scanf("%d", &opcion);
        while (getchar() != '\n'); // Limpiar buffer

        switch(opcion) {
            case 1:
                printf("\n--- EJECUTANDO PRUEBA GENERAL ---\n");
                mostrar_resultados_generales();
                pausa();
                int resultado_fdc = test_fdc37c669();
                int resultado_st16_a = test_st16c2550(UART5_BASE);
                int resultado_st16_b = test_st16c2550(UART6_BASE);
                int resultado_sja = test_sja1000t(SJA_BASE);
                int resultado_saa = test_saa7130hl();  // ✅ Usa la nueva versión
                int resultado_MVBC01 = test_MVBC01();
                int resultado_MVBCS1 = test_MVBCS1();
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
                    diagnosticar_saa7130hl();  // ✅ Este debe ser el diagnóstico detallado
                }

                if (!resultado_MVBC01) {
                    printf("\n❌ Fallo detectado en MVBC01 (U49).\n");
                    diagnosticar_MVBC01();  // ✅ Este debe ser el diagnóstico detallado
                }
/*
                if (!resultado_MVBCS1) {
                    printf("\n❌ Fallo detectado en MVBCS1 (Tarjeta PC-104).\n");
                    diagnosticar_MVBCS1();  // ✅ Este debe ser el diagnóstico detallado
                }
*/
                printf("\n========================================================\n");
                printf("   PRUEBAS COMPLETADAS\n");
                printf("========================================================\n");
                break;
            case 2:
                printf("\n--- PRUEBA FDC37C669 ---\n");
                test_fdc37c669();
                diagnosticar_fdc37c669();
                pausa();
                break;
            case 3:
                printf("\n--- PRUEBA ST16C2550 ---\n");
                test_st16c2550(UART5_BASE);
                test_st16c2550(UART6_BASE);
                diagnosticar_st16c2550();
                pausa();
                break;
            case 4:
                printf("\n--- PRUEBA SJA1000T ---\n");
                test_sja1000t(SJA_BASE);
                diagnosticar_sja1000t();
                pausa();
                break;
            case 5:
                printf("\n--- PRUEBA SAA7130HL ---\n");
                test_saa7130hl();
                diagnosticar_saa7130hl();
                pausa();
                break;
            case 6:
                printf("\n--- PRUEBA MVBC01 ---\n");
                test_MVBC01();
                diagnosticar_MVBC01();
                pausa();
                break;
/*
            case 7:
                printf("\n--- PRUEBA MVBCS1 ---\n");
                test_MVBCS1();
                diagnosticar_MVBCS1();
                pausa();
                break;
*/
            case 8:
                printf("Saliendo...\n");
                break;
            default:
                printf("Opción no válida.\n");
        }
    } while (opcion != 8);

    return 0;
}
