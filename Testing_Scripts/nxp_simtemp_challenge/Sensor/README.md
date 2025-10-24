# NXP Simulated Temperature Sensor Driver

## 📋 Descripción
Driver de kernel Linux que simula un sensor de temperatura virtual con capacidades de alerta y modificación de comportamiento, para esto hay funciones para modificar:
- **Temperatura base.**: Para poder partir de cualquier punto deseado
- **Frecuencia.**: Valores típicos 1Hz-1KHz. Límite superior depende de la resolución del timer del kernel.
- **Amplitud.**: Para una plasticidad de condiciones, con 0 no varía, lo cual es como modo normal, se podría modular la señal para una variación con ruido, el ruido podría hacerse en función o por datos aleatorios, pero la idea es la misma, así que por tiempos se dejo así.
- **Alarma alta.**: 
- **Alarma baja.**: 

 Desarrollado como parte del challenge para Systems Software Engineer en NXP.


## 🏗️ Arquitectura
- **Kernel Module**: Driver de plataforma con binding Device Tree
- **Character Device**: `/dev/simtemp` para lectura de muestras
- **Sysfs Interface**: Configuración en `/sys/class/nxp_simtemp/simtemp/`
- **User CLI**: Aplicación Python para monitoreo y testing

## 🚀 Quick Start

### Prerrequisitos

```cli
sudo apt-get update
sudo apt-get install linux-headers-$(uname -r) build-essential python3

```
Build

```bash
./scripts/build.sh
```
Demo Completo

```bash
./scripts/run_demo.sh
```
Uso Manual

```bash
# Cargar driver
cd driver
sudo insmod nxp_simtemp.ko

# Ejecutar test
cd ../user/app
python3 nxp_cli.py test

# Descargar driver
cd ../driver
sudo rmmod nxp_simtemp
```

## 📁 Estructura del Proyecto
```ascii
Sensor/
├── driver/                # Kernel module
│   ├── nxp_simtemp.c      # Main driver source
│   ├── nxp_simtemp.h      # Header file
│   ├── Makefile
│   └── dts/               # Device Tree sources
├── /app/                  # User space application
│   └── nxp_cli.py         # Python CLI tool
├── scripts/               # Build and demo scripts
│   ├── build.sh
│   └── run_demo.sh
└── docs/                  # Documentation
    ├── DESIGN.md
    └── AI_NOTES.md
└── dts/                  # DTS
    ├── nxp-simtemp.dts
    └── nxp-simtemp.dtbo
```
🎯 Características Implementadas

Kernel Driver

✅ Platform driver con Device Tree binding

✅ Character device con operaciones read/poll

✅ Sysfs attributes configurables

✅ Timer-based temperature simulation

✅ Threshold alerts con wait queues

User Space
✅ CLI con test mode automatizado

✅ Lectura continua con timestamps

✅ Configuración runtime via sysfs

✅ Detección de alertas HIGH/LOW

Scripts
✅ Build automático

✅ Demo end-to-end

✅ Manejo de errores

🔧 Configuración
Parámetros del Sensor (vía Sysfs)
bash
# Sampling interval (ms)
echo 500 | sudo tee /sys/class/nxp_simtemp/simtemp/sampling_ms

# Temperature thresholds (°C)
echo 30000 | sudo tee /sys/class/nxp_simtemp/simtemp/threshold_high
echo 20000 | sudo tee /sys/class/nxp_simtemp/simtemp/threshold_low

# Wave parameters
echo 10000 | sudo tee /sys/class/nxp_simtemp/simtemp/amplitude
echo 50 | sudo tee /sys/class/nxp_simtemp/simtemp/frequency
Comandos CLI
bash
# Test mode (30 segundos)
python3 nxp_cli.py test

# Lectura continua
python3 nxp_cli.py read 10

# Configuración
python3 nxp_cli.py config amplitude 5.0
python3 nxp_cli.py config threshold_high 28.0

# Información
python3 nxp_cli.py info
📊 Formato de Datos
Binary Sample (16 bytes)
c
struct simtemp_sample {
    __u64 timestamp_ns;    // Timestamp monotónico
    __s32 temp_mC;         // Temperatura en milli-grados Celsius
    __u32 flags;           // Flags (0x1=NEW_SAMPLE, 0x2=THRESHOLD_CROSS)
};
🧪 Testing
El test mode automatizado:

Configura thresholds en 20°C/30°C

Establece amplitud de ±10°C

Ejecuta lectura por 30 segundos

Verifica detección de alertas HIGH/LOW

Reporta PASS/FAIL

## 📞 Enlaces

- [**🔗 Ver repositorio completo**] (https://github.com/mikazki95/Embedmented-Projects/tree/main/Testing_Scripts/nxp_simtemp_challenge/Sensor)

- [**🎥 Ver video demostración**] (https://drive.google.com/file/d/12H0Wb_Q9idkFtfszM4WbHKkk_xFJCnjt/view?usp=drive_link)



👤 Autor
Tonatiuh Velazquez

📄 Licencia
GPL v2

















