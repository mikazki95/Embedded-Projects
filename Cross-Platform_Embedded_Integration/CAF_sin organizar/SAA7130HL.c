#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

void verificar_pci_directo_saa7130() {
    printf("\n[PASO 4] Escaneando Bus PCI directo en /sys (Inmune a fallas de lspci)...\n");
    
    DIR *dir;
    struct dirent *ent;
    FILE *fp;
    char ruta_vendor[256];
    char ruta_device[256];
    char id_vendor[64];
    char id_device[64];
    int encontrado = 0;

    dir = opendir("/sys/bus/pci/devices");
    if (dir == NULL) {
        printf("         ❌ ERROR: No se puede acceder al subsistema PCI del kernel.\n");
        return;
    }

    while ((ent = readdir(dir)) != NULL) {
        // Ignorar carpetas ocultas relativas
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
            continue;
        }

        // Construir rutas de lectura directa en el sysfs de Linux
        snprintf(ruta_vendor, sizeof(ruta_vendor), "/sys/bus/pci/devices/%s/vendor", ent->d_name);
        snprintf(ruta_device, sizeof(ruta_device), "/sys/bus/pci/devices/%s/device", ent->d_name);

        // Inicializar buffers
        id_vendor[0] = '\0';
        id_device[0] = '\0';

        // Leer el Vendor ID (Philips es 0x1131)
        fp = fopen(ruta_vendor, "r");
        if (fp) {
            if (fscanf(fp, "%63s", id_vendor) != 1) { id_vendor[0] = '\0'; }
            fclose(fp);
        }

        // Leer el Device ID (SAA7130 es 0x7130)
        fp = fopen(ruta_device, "r");
        if (fp) {
            if (fscanf(fp, "%63s", id_device) != 1) { id_device[0] = '\0'; }
            fclose(fp);
        }

        // Validar si coincide con las firmas del chip de video de CAF/Philips
        if (strstr(id_vendor, "1131") && strstr(id_device, "7130")) {
            printf("         ✅ ¡ÉXITO PCI! SAA7130HL detectado en la ranura: %s\n", ent->d_name);
            printf("            -> Vendor ID: %s | Device ID: %s\n", id_vendor, id_device);
            encontrado = 1;
            break;
        }
    }
    closedir(dir);

    if (!encontrado) {
        printf("         ❌ FALLA PCI: El integrado SAA7130HL no fue encontrado.\n");
        printf("            El chip de video está invisible para el bus PCI de la tarjeta madre.\n");
    }
}
