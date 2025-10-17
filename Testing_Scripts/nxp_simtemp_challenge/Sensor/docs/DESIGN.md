🏗️ Arquitectura del Sistema
Diagrama de Bloques
text
┌─────────────────┐    ┌──────────────────┐    ┌──────────────────┐
│   Espacio de    │    │   Espacio del    │    │  Configuración   │
│    Usuario      │    │     Kernel       │    │                  │
│                 │    │                  │    │                  │
│  CLI Python     │◄───┤  Dispositivo de  │◄───┤  Driver de       │
│    (CLI)        │    │   Carácter       │    │  Plataforma      │
│                 │    │   /dev/simtemp   │    │                  │
│                 │    │                  │    │                  │
│  Config Sysfs   │───►│  Interfaz Sysfs  │    │  Device Tree     │
│   (Config)      │    │ /sys/class/.../  │    │   (Opcional)     │
└─────────────────┘    └──────────────────┘    └──────────────────┘
                              │
                              ▼
                      ┌──────────────────┐
                      │  Motor de        │
                      │  Simulación de   │
                      │  Temperatura     │
                      └──────────────────┘
🔧 Diseño del Driver del Kernel
Componentes Principales
1. Infraestructura Platform Driver
c
static struct platform_driver nxp_simtemp_driver = {
    .probe = nxp_simtemp_probe,
    .remove = nxp_simtemp_remove,
    .driver = {
        .name = "nxp-simtemp",
        .of_match_table = nxp_simtemp_of_match,
    },
};
Binding Device Tree:

dts
nxp_simtemp: nxp-simtemp@0 {
    compatible = "nxp,simtemp";
    temp-base = <25000>;        /* 25.0°C base */
    amplitude = <10000>;        /* ±10°C variación */
    frequency = <50>;           /* 50 Hz */
    alarm-high = <30000>;       /* 30.0°C umbral alto */
    alarm-low = <20000>;        /* 20.0°C umbral bajo */
    update-interval = <1000>;   /* 1 segundo sampling */
};
2. Operaciones de Dispositivo de Carácter
c
static struct file_operations nxp_simtemp_fops = {
    .owner = THIS_MODULE,
    .open = nxp_simtemp_open,
    .release = nxp_simtemp_release,
    .read = nxp_simtemp_read,
    .poll = nxp_simtemp_poll,
};
Estructura de Datos:

c
struct simtemp_sample {
    __u64 timestamp_ns;   /* timestamp monotónico */
    __s32 temp_mC;        /* temperatura en mili-grados */
    __u32 flags;          /* NEW_SAMPLE | THRESHOLD_CROSS */
} __attribute__((packed));
3. Motor de Simulación de Temperatura
c
static int simulate_temperature(struct nxp_simtemp_data *data)
{
    /* Generación de onda triangular */
    u64 elapsed_us = div_u64(now_ns - data->wave_start_ns, 1000);
    u64 position = elapsed_us % data->wave_period_us;
    
    int half_period = data->wave_period_us / 2;
    int variation = data->amplitude_mC * ((int)position - half_period) / half_period;
    
    return data->base_temp + variation;
}
🔒 Modelo de Concurrencia y Bloqueos
Protección con Mutex
data->lock: Protege todo el estado compartido en nxp_simtemp_data

Uso: Callbacks de timer, operaciones sysfs, operaciones de lectura

Jerarquía de Bloqueos
c
mutex_lock(&data->lock);
/* Sección crítica:
   - cálculo de temperatura
   - verificación de umbrales  
   - actualización de estado de alarma
   - generación de muestras
*/
mutex_unlock(&data->lock);
📡 API Usuario-Kernel
1. API de Dispositivo de Carácter (/dev/simtemp)
Lectura bloqueante retorna muestras binarias de 16 bytes con:

Timestamp monotónico de 64 bits

Temperatura de 32 bits en mili-grados Celsius

Flags de 32 bits indicando tipo de muestra y alertas

Soporte poll/select para:

POLLIN: Nueva muestra disponible

POLLPRI: Evento de cruce de umbral

2. Interfaz de Control Sysfs
text
/sys/class/nxp_simtemp/simtemp/
├── temperature (ro)     - Temperatura actual
├── threshold_high (rw)  - Umbral de alarma alto
├── threshold_low (rw)   - Umbral de alarma bajo
├── sampling_ms (rw)     - Intervalo de actualización
├── amplitude (rw)       - Amplitud de variación
└── frequency (rw)       - Frecuencia de onda
⚡ Manejo de Eventos
Mecanismo de Alertas
c
/* Callback del timer */
data->alarm_active = (temp >= data->alarm_high) || (temp <= data->alarm_low);

if (data->alarm_active != old_alarm_state) {
    wake_up_interruptible(&data->wait_queue);  /* Despertar procesos en poll */
}
Implementación de Poll
c
mask |= POLLIN | POLLRDNORM;          /* Siempre readable */
if (data->alarm_active)
    mask |= POLLPRI;                   /* Alerta pendiente */
🎯 Decisiones de Diseño
1. Platform Driver vs Módulo Simple
Decisión: Platform driver con soporte DT
Razón:

Sigue estándares del modelo de drivers Linux

Listo para despliegue en plataformas embebidas NXP

Configuración via Device Tree como hardware real

2. Onda Triangular vs Onda Senoidal
Decisión: Generación de onda triangular
Razón:

Solo aritmética entera (sin FPU en kernel)

Comportamiento predecible y lineal

Fácil de entender y debuggear

3. Sysfs vs IOCTL para Configuración
Decisión: Sysfs por simplicidad
Razón:

Legible/editable por humanos

Estándar para ajuste simple de parámetros

Fácil integración con scripts de shell

🚀 Consideraciones de Rendimiento
Uso de Memoria
Estructura de datos de tamaño fijo (sin allocación dinámica beyond probe)

Instancia única de timer del kernel

Uso mínimo de stack en callbacks

Uso de CPU
Actualizaciones manejadas por timer (no por interrupciones)

Aritmética entera eficiente para simulación

Mutex con secciones críticas cortas

Escalabilidad
Límites actuales:

Instancia única de dispositivo

Período mínimo de sampling de 1ms

Adecuado para tasas típicas de sensores (1-100Hz)

Cuellos de botella a 10kHz:

Limitaciones de resolución del timer

Contención de mutex

Overhead de copia usuario-kernel

🔍 Estrategia de Testing
Testing Unitario
Ciclos manuales de carga/descarga

Validación de parámetros sysfs

Verificación de comportamiento poll/select

Testing de Integración
Script demo end-to-end

Detección de cruce de umbrales

Patrones de acceso concurrente

Testing de Estrés
Tasas de sampling máximas

Cambios rápidos de configuración

Operación de larga duración
