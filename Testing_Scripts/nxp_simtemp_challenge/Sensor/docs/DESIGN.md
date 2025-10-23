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
aca no va codigo 

