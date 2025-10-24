# 📋 NXP Simulated Temperature Sensor

Compatible

"nxp,simtemp"

Descripción

Este binding describe un sensor de temperatura simulado para sistemas NXP. El driver genera lecturas de temperatura periódicas con variación de onda configurable y soporte para alertas por umbral.

Nodo Requerido

Un nodo hijo con el compatible "nxp,simtemp"

Propiedades

Propiedades Requeridas

compatible: debe ser "nxp,simtemp"

Propiedades Opcionales

temp-base: Temperatura base en mili-grados Celsius

Valor por defecto: 25000 (25.0°C)

Rango: -40000 a 125000 (-40°C a 125°C)

amplitude: Amplitud de variación en mili-grados Celsius

Valor por defecto: 0 (sin variación)

Rango: 0 a 50000 (0°C a 50°C)

frequency: Frecuencia de variación en Hertz

Valor por defecto: 100 (0.1 Hz)

Rango: 1 a 1000 (1 mHz a 1 kHz)

alarm-high: Umbral alto de alarma en mili-grados Celsius

Valor por defecto: 30000 (30.0°C)

Rango: -40000 a 125000

alarm-low: Umbral bajo de alarma en mili-grados Celsius

Valor por defecto: 20000 (20.0°C)

Rango: -40000 a 125000

update-interval: Intervalo de actualización en milisegundos

Valor por defecto: 1000 (1 segundo)

Rango: 10 a 10000 (10ms a 10s)

status: Estado del dispositivo

Valores: "okay", "disabled"

Valor por defecto: "okay"


Ejemplos

Ejemplo 1: Configuración Básica

```dts
&{/} {
    nxp_simtemp: nxp-simtemp@0 {
        compatible = "nxp,simtemp";
        status = "okay";
    };
};
```

Ejemplo 2: Configuración Personalizada

```dts
&{/} {
    nxp_simtemp: nxp-simtemp@0 {
        compatible = "nxp,simtemp";
        temp-base = <25000>;        /* 25.0°C base */
        amplitude = <10000>;        /* ±10.0°C variación */
        frequency = <50>;           /* 50 Hz */
        alarm-high = <30000>;       /* 30.0°C alarma alta */
        alarm-low = <20000>;        /* 20.0°C alarma baja */
        update-interval = <1000>;   /* 1 segundo sampling */
        status = "okay";
    };
};
```
Ejemplo 3: Sensor de Alta Precisión

```dts
&{/} {
    nxp_simtemp: nxp-simtemp@0 {
        compatible = "nxp,simtemp";
        temp-base = <20000>;        /* 20.0°C base */
        amplitude = <5000>;         /* ±5.0°C variación */
        frequency = <10>;           /* 10 Hz */
        alarm-high = <35000>;       /* 35.0°C alarma alta */
        alarm-low = <15000>;        /* 15.0°C alarma baja */
        update-interval = <100>;    /* 100ms sampling rápido */
        status = "okay";
    };
};
```

Notas de Implementación

Driver Kernel

El driver implementa en nxp_simtemp_probe():

```c
/* Parsing de propiedades DT */
of_property_read_u32(np, "temp-base", &data->base_temp);
of_property_read_u32(np, "amplitude", &data->amplitude_mC);
of_property_read_u32(np, "frequency", &data->frequency_hz);
of_property_read_u32(np, "alarm-high", &data->alarm_high);
of_property_read_u32(np, "alarm-low", &data->alarm_low);
of_property_read_u32(np, "update-interval", &data->update_interval_ms);
```
Valores por Defecto

Si alguna propiedad no está presente en el DT, el driver usa:

```c
#define DEFAULT_BASE_TEMP       25000
#define DEFAULT_AMPLITUDE       0
#define DEFAULT_FREQUENCY       100
#define DEFAULT_ALARM_HIGH      30000
#define DEFAULT_ALARM_LOW       20000
#define DEFAULT_UPDATE_MS       1000
```


Validación

El binding sigue el schema estándar de Linux kernel y puede ser validado con:


## Validar DTS

```dtc
 -I fs /sys/firmware/devicetree/base
```
## Verificar propiedades

```dtc
cat /proc/device-tree/nxp-simtemp@0/temp-base
```


