---

## Propiedades Requeridas
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
```
## Valores por Defecto

```c
#define DEFAULT_BASE_TEMP       25000   // 25.0°C
#define DEFAULT_AMPLITUDE       0       // Sin variación
#define DEFAULT_FREQUENCY       100     // 0.1 Hz
#define DEFAULT_ALARM_HIGH      30000   // 30.0°C
#define DEFAULT_ALARM_LOW       20000   // 20.0°C  
#define DEFAULT_UPDATE_MS       1000    // 1 segundo
```

## 🖥️ Character Device API

Dispositivo

```dts
/dev/simtemp
```

## Estructura de Datos

```c
struct simtemp_sample {
    __u64 timestamp_ns;   // Timestamp nanosegundos (monotónico)
    __s32 temp_mC;        // Temperatura en mili-grados Celsius
    __u32 flags;          // Flags (ver abajo)
} __attribute__((packed));
```

## Flags

```c
#define SIMTEMP_FLAG_NEW_SAMPLE       0x1  // Nueva muestra disponible
#define SIMTEMP_FLAG_THRESHOLD_CROSS  0x2  // Umbral cruzado
```

## Operaciones Soportadas

open()

```c
int fd = open("/dev/simtemp", O_RDONLY);
```

Abre el dispositivo en modo solo lectura.

read()

```c
struct simtemp_sample sample;
read(fd, &sample, sizeof(sample));
```

Lectura bloqueante, retorna estructura de 16 bytes.

poll() / select()

```c
struct pollfd pfd = {fd, POLLIN | POLLPRI, 0};
poll(&pfd, 1, timeout);
```

- POLLIN: Nueva muestra disponible

- POLLPRI: Alerta de umbral activa

close()

```c
close(fd);
```

##🛠️ Sysfs Interface

Path Base

```c
/sys/class/nxp_simtemp/simtemp/
```

## Atributos

```c
├── temperature (ro)       - Temperatura actual
├── threshold_high (rw)    - Umbral de alarma alto
├── threshold_low (rw)     - Umbral de alarma bajo
├── sampling_ms (rw)       - Intervalo de actualización
├── amplitude (rw)         - Amplitud de variación
└── frequency (rw)         - Frecuencia de onda
```

## Ejemplo:

```c

```

## 🐚 Ejemplos de Uso

```c
# Leer temperatura
cat /sys/class/nxp_simtemp/simtemp/temperature

# Configurar umbral alto
echo 28000 sudo | tee /sys/class/nxp_simtemp/simtemp/threshold_high
```

## Shell

```c
sudo insmod nxp_simtemp.ko
cat /sys/class/nxp_simtemp/simtemp/temperature
sudo cat /dev/simtemp | hexdump -C
echo 500 sudo | tee /sys/class/nxp_simtemp/simtemp/sampling_ms
sudo rmmod nxp_simtemp
```

## Python

```c
import struct

with open('/dev/simtemp', 'rb') as f:
    data = f.read(16)
    timestamp_ns, temp_mC, flags = struct.unpack('<QII', data)
    temp_c = temp_mC / 1000.0
    print(f"Temp: {temp_c:.1f}°C, Flags: 0x{flags:x}")
```

## C

```c
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
```



