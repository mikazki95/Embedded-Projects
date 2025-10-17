# NXP SimTemp - Documentación de API

## 📡 Device Tree Binding

### Compatible String
"nxp,simtemp"

text

### Propiedades Requeridas
```dts
nxp_simtemp: nxp-simtemp@0 {
    compatible = "nxp,simtemp";
    temp-base = <25000>;        /* Temperatura base en mili-grados */
    amplitude = <10000>;        /* Amplitud de variación */
    frequency = <50>;           /* Frecuencia en Hz */
    alarm-high = <30000>;       /* Umbral alto de alarma */
    alarm-low = <20000>;        /* Umbral bajo de alarma */
    update-interval = <1000>;   /* Intervalo de actualización en ms */
    status = "okay";
};
Valores por Defecto
Si no se especifica Device Tree, el driver usa:

c
#define DEFAULT_BASE_TEMP       25000   // 25.0°C
#define DEFAULT_AMPLITUDE       0       // Sin variación
#define DEFAULT_FREQUENCY       100     // 0.1 Hz
#define DEFAULT_ALARM_HIGH      30000   // 30.0°C
#define DEFAULT_ALARM_LOW       20000   // 20.0°C  
#define DEFAULT_UPDATE_MS       1000    // 1 segundo
🖥️ Character Device API
Dispositivo
text
/dev/simtemp
Estructura de Datos
c
struct simtemp_sample {
    __u64 timestamp_ns;   // Timestamp nanosegundos (monotónico)
    __s32 temp_mC;        // Temperatura en mili-grados Celsius
    __u32 flags;          // Flags (ver abajo)
} __attribute__((packed));
Flags
c
#define SIMTEMP_FLAG_NEW_SAMPLE       0x1  // Nueva muestra disponible
#define SIMTEMP_FLAG_THRESHOLD_CROSS  0x2  // Umbral cruzado
Operaciones Soportadas
open()
c
int fd = open("/dev/simtemp", O_RDONLY);
Abre el dispositivo en modo solo lectura

read()
c
struct simtemp_sample sample;
read(fd, &sample, sizeof(sample));
Lectura bloqueante

Retorna estructura de 16 bytes

Siempre retorna muestra completa

poll() / select()
c
struct pollfd pfd = {fd, POLLIN | POLLPRI, 0};
poll(&pfd, 1, timeout);
POLLIN: Nueva muestra disponible

POLLPRI: Alerta de umbral activa

close()
c
close(fd);
Cierra el dispositivo

🛠️ Sysfs Interface
Path Base
text
/sys/class/nxp_simtemp/simtemp/
Atributos
temperature (read-only)
text
cat /sys/class/nxp_simtemp/simtemp/temperature
25000
Temperatura actual en mili-grados Celsius

threshold_high (read-write)
text
# Leer
cat /sys/class/nxp_simtemp/simtemp/threshold_high
30000

# Escribir  
echo 28000 > /sys/class/nxp_simtemp/simtemp/threshold_high
Umbral alto de alarma en mili-grados

threshold_low (read-write)
text
# Leer
cat /sys/class/nxp_simtemp/simtemp/threshold_low
20000

# Escribir
echo 22000 > /sys/class/nxp_simtemp/simtemp/threshold_low
Umbral bajo de alarma en mili-grados

sampling_ms (read-write)
text
# Leer
cat /sys/class/nxp_simtemp/simtemp/sampling_ms
1000

# Escribir
echo 500 > /sys/class/nxp_simtemp/simtemp/sampling_ms
Intervalo de muestreo en milisegundos

Rango válido: 10-10000 ms

amplitude (read-write)
text
# Leer
cat /sys/class/nxp_simtemp/simtemp/amplitude
10000

# Escribir
echo 5000 > /sys/class/nxp_simtemp/simtemp/amplitude
Amplitud de variación en mili-grados

Debe ser ≥ 0

frequency (read-write)
text
# Leer
cat /sys/class/nxp_simtemp/simtemp/frequency
50

# Escribir
echo 100 > /sys/class/nxp_simtemp/simtemp/frequency
Frecuencia de onda en Hz

Debe ser > 0

🐚 Ejemplos de Uso
Lectura Básica desde Shell
bash
# Cargar driver
sudo insmod nxp_simtemp.ko

# Leer temperatura actual
cat /sys/class/nxp_simtemp/simtemp/temperature

# Leer muestra binaria
sudo cat /dev/simtemp | hexdump -C

# Configurar parámetros
echo 500 > /sys/class/nxp_simtemp/simtemp/sampling_ms
echo 26000 > /sys/class/nxp_simtemp/simtemp/threshold_high

# Descargar driver
sudo rmmod nxp_simtemp
Uso con Python
python
import struct

with open('/dev/simtemp', 'rb') as f:
    data = f.read(16)
    timestamp_ns, temp_mC, flags = struct.unpack('<QII', data)
    temp_c = temp_mC / 1000.0
    print(f"Temp: {temp_c:.1f}°C, Flags: 0x{flags:x}")
Uso con C
c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

struct simtemp_sample {
    unsigned long long timestamp_ns;
    int temp_mC;
    int flags;
};

int main() {
    int fd = open("/dev/simtemp", O_RDONLY);
    struct simtemp_sample sample;
    
    read(fd, &sample, sizeof(sample));
    printf("Temp: %.1f C | Flags: 0x%x\n", 
           sample.temp_mC / 1000.0, sample.flags);
    
    close(fd);
    return 0;
}
⚠️ Códigos de Error
Character Device
-ENODEV: Dispositivo no encontrado

-EINVAL: Parámetros inválidos

-EFAULT: Error copiando a espacio de usuario

Sysfs Interface
-EINVAL: Valor fuera de rango o inválido

-EACCES: Permiso denegado (escritura sin root)

🔄 Comportamiento de Alarms
Activación de Alarmas
La alarma se activa cuando temp ≥ threshold_high O temp ≤ threshold_low

El flag THRESHOLD_CROSS se establece en la muestra

Los procesos en poll() son despertados con POLLPRI

Notificaciones
Solo se notifica cuando el estado de alarma cambia

Transición: OK → ALARM o ALARM → OK

Múltiples lecturas en estado de alarma no generan notificaciones adicionales
