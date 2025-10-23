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
##Valores por Defecto

##🖥️ Character Device API

##Estructura de Datos

##Flags

##Operaciones Soportadas

##Abre el dispositivo en modo solo lectura.

read()

##Lectura bloqueante, retorna estructura de 16 bytes.

poll() / select()

POLLIN: Nueva muestra disponible

POLLPRI: Alerta de umbral activa

##close()🛠️ Sysfs Interface

##Path Base

##Atributos

##Ejemplo:

##🐚 Ejemplos de Uso

##Shell

##Python

##C


