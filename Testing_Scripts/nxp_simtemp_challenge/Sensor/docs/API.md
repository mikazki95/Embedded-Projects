# NXP Simulated Temperature Sensor Driver - API Document

Este documento describe la API del sistema de simulación de sensor de temperatura desarrollado como driver de kernel Linux, junto con su interfaz de usuario en Python. Se divide en tres secciones principales:


## 🧠 1. Kernel Driver API

### 📁 Dispositivo de carácter

- Ruta: /dev/simtemp

- Estructura de datos:
  
```c
struct simtemp_sample {
    __u64 timestamp_ns;   // Timestamp en nanosegundos (monotónico)
    __s32 temp_mC;        // Temperatura en mili-grados Celsius
    __u32 flags;          // Flags: 0x1 = NEW_SAMPLE, 0x2 = THRESHOLD_CROSS
} __attribute__((packed));

```


### 🛠️ Operaciones soportadas

- open()

  -  Abre el dispositivo en modo lectura.

    - Ejemplo: int fd = open("/dev/simtemp", O_RDONLY);

- read()

    - Lectura bloqueante de una muestra.

    - Ejemplo:

```c
struct simtemp_sample sample;

read(fd, &sample, sizeof(sample));

```
# 
  - poll() / select()
    
  - Soporta POLLIN (nueva muestra) y POLLPRI (alerta de umbral).
    
  - Ejemplo:

```c
    
struct pollfd pfd = {fd, POLLIN | POLLPRI, 0};
poll(&pfd, 1, timeout);

```

- close()

-  Cierra el descriptor de archivo.
Ejemplo: close(fd);


## ⚙️ 2. Sysfs Interface

### 📁 Ruta base

/sys/class/nxp_simtemp/simtemp/
🔧 Atributos disponibles

temperature (ro): Temperatura actual

threshold_high (rw): Umbral alto de alarma

threshold_low (rw): Umbral bajo de alarma

sampling_ms (rw): Intervalo de actualización en milisegundos

amplitude (rw): Amplitud de variación

frequency (rw): Frecuencia de onda

base_temp (rw): Temperatura base


## 🐚 Ejemplos de uso

### Leer temperatura

cat /sys/class/nxp_simtemp/simtemp/temperature

### Configurar umbral alto

echo 28000 | sudo tee /sys/class/nxp_simtemp/simtemp/threshold_high

### Configurar frecuencia

echo 50 | sudo tee /sys/class/nxp_simtemp/simtemp/frequency


## 🖥️ 3. CLI Python (Simulación en espacio de usuario)

### 📁 Archivo

app/nxp_cli.py

## 🧪 Comandos disponibles

### Modo test automatizado (30 segundos)

python3 nxp_cli.py test

### Lectura continua por N segundos

python3 nxp_cli.py read 10

### Configuración de parámetros

python3 nxp_cli.py config amplitude 5.0

python3 nxp_cli.py config threshold_high 28.0

### Mostrar configuración actual

python3 nxp_cli.py info


⚠️ Nota
La CLI Python simula el comportamiento del sensor. No interactúa directamente con /dev/simtemp, aunque puede adaptarse para hacerlo.


## 📌 Observaciones

El documento original mezclaba funciones del driver, funciones POSIX y simulaciones Python.
Esta versión separa claramente cada capa para evitar confusión.
Todos los ejemplos están basados en el código real del proyecto.


👤 Autor
Tonatiuh Velazquez Rojas
📄 Licencia
GPL v2





