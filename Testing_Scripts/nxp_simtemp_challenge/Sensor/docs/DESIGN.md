# 🏗️ Arquitectura del Sistema NXP SimTemp

Este documento describe la arquitectura y diseño del driver de simulación de temperatura para Linux.

---

## 📐 Diagrama de Bloques

nxp_simtemp_architecture_diagram.png

---

## 🔧 Diseño del Driver del Kernel

### 1. Infraestructura Platform Driver
```c
static struct platform_driver nxp_simtemp_driver = {
    .probe = nxp_simtemp_probe,
    .remove = nxp_simtemp_remove,
    .driver = {
        .name = "nxp-simtemp",
        .of_match_table = nxp_simtemp_of_match,
    },
};
```
``
### 2. Binding Device Tree
```c
nxp_simtemp: nxp-simtemp@0 {
    compatible = "nxp,simtemp";
    temp-base = <25000>;       /* 25.0°C base */
    amplitude = <10000>;       /* ±10°C variación */
    frequency = <50>;          /* 50 Hz */
    alarm-high = <30000>;      /* 30.0°C umbral alto */
    alarm-low = <20000>;       /* 20.0°C umbral bajo */
    update-interval = <1000>;  /* 1 segundo sampling */
};
```
### 3. Operaciones de Dispositivo de Carácter

```c
static struct file_operations nxp_simtemp_fops = {
    .owner = THIS_MODULE,
    .open = nxp_simtemp_open,
    .release = nxp_simtemp_release,
    .read = nxp_simtemp_read,
    .poll = nxp_simtemp_poll,
};
```
### 4. Estructura de Datos

```c
struct simtemp_sample {
    __u64 timestamp_ns; /* timestamp monotónico */
    __s32 temp_mC;      /* temperatura en mili-grados */
    __u32 flags;        /* NEW_SAMPLE | THRESHOLD_CROSS */
} __attribute__((packed));
```
### 5. Motor de Simulación

```c

static int simulate_temperature(struct nxp_simtemp_data data) {
    /* Generación de onda triangular */
    u64 elapsed_us = div_u64(now_ns - data->wave_start_ns, 1000);
    u64 position = elapsed_us % data->wave_period_us;

    int half_period = data->wave_period_us / 2;
    int variation = data->amplitude_mC * ((int)position - half_period) / half_period;

    return data->base_temp + variation;
}
```

🔒 Modelo de Concurrencia
Protección con mutex:

```c
mutex_lock(&data->lock);
/* Sección crítica:
   - cálculo de temperatura
   - verificación de umbrales
   - actualización de estado de alarma
   - generación de muestras */
mutex_unlock(&data->lock);

```

## 📡 API Usuario-Kernel

### Dispositivo de carácter: `/dev/simtemp`
- **Lectura bloqueante**: retorna muestras binarias (16 bytes) con:
  - Timestamp monotónico (64 bits)
  - Temperatura en miligrados Celsius (32 bits)
  - Flags (32 bits) indicando tipo de muestra y alertas
- **Soporte poll/select**:
  - `POLLIN`: Nueva muestra disponible
  - `POLLPRI`: Evento de cruce de umbral

### Interfaz Sysfs: `/sys/class/nxp_simtemp/simtemp/`

```
├── temperature (ro)       - Temperatura actual
├── threshold_high (rw)    - Umbral de alarma alto
├── threshold_low (rw)     - Umbral de alarma bajo
├── sampling_ms (rw)       - Intervalo de actualización
├── amplitude (rw)         - Amplitud de variación
└── frequency (rw)         - Frecuencia de onda
```


⚡ Manejo de Eventos


```c
/* Callback del timer */
data->alarm_active = (temp >= data->alarm_high) || (temp <= data->alarm_low);
if (data->alarm_active != old_alarm_state) {
    wake_up_interruptible(&data->wait_queue); /* Despertar procesos en poll */
}


```
## 🎯 Decisiones de Diseño

- **Platform Driver vs Módulo Simple**: Platform driver con soporte DT  
- **Onda Triangular vs Senoidal**: Triangular (sin FPU, fácil debug)  
- **Sysfs vs IOCTL**: Sysfs (legible, estándar)  

---

## 🚀 Consideraciones de Rendimiento

- Instancia única  
- Timer eficiente  
- Mutex con secciones críticas cortas  
- Sampling mínimo: 1ms  

---

## 🔍 Estrategia de Testing

- **Unitario**: carga/descarga, sysfs, poll/select  
- **Integración**: script end-to-end  
- **Estrés**: sampling máximo, cambios rápidos  

