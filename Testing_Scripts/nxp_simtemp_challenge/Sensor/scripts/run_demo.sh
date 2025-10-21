#!/bin/bash
set -e  # Exit on error

echo "=== NXP SimTemp Demo ==="

# Limpiar módulo previo si existe
echo "Cleaning previous module..."
sudo rmmod nxp_simtemp 2>/dev/null || true

# Cargar módulo
echo "Loading kernel module..."
cd driver
sudo insmod nxp_simtemp.ko
echo "✅ Module loaded"

# Esperar inicialización
sleep 2

# Verificar que se creó el device
if [ ! -c "/dev/simtemp" ]; then
    echo "❌ ERROR: /dev/simtemp not created!"
    exit 1
fi

echo "✅ Device created: /dev/simtemp"

# Verificar sysfs
if [ ! -d "/sys/class/nxp_simtemp/simtemp" ]; then
    echo "❌ ERROR: sysfs interface not created!"
    exit 1
fi

echo "✅ Sysfs interface created"

# Ejecutar TEST MODE con Python
echo "=== Running Test Mode ==="
cd ../app

# Mostrar configuración actual
echo "Current configuration:"
cat /sys/class/nxp_simtemp/simtemp/temperature
cat /sys/class/nxp_simtemp/simtemp/threshold_high
cat /sys/class/nxp_simtemp/simtemp/amplitude

# Ejecutar test mode
echo "Starting test mode..."
if python3 nxp_cli.py test; then
    echo "✅ TEST PASSED"
else
    echo "❌ TEST FAILED"
    # No exit aquí para permitir limpieza
fi

# Descargar módulo
echo "Cleaning up..."
cd ../driver
sudo rmmod nxp_simtemp || true

# Verificar limpieza
if [ -c "/dev/simtemp" ]; then
    echo "⚠️  Warning: /dev/simtemp still exists after rmmod"
else
    echo "✅ Module unloaded cleanly"
fi

echo "✅ Demo completed"