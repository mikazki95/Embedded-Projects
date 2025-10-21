#!/bin/bash
set -e  # Exit on error

echo "=== Building NXP SimTemp Driver ==="

# Compilar kernel module
cd driver
make clean
make

# (Opcional) Compilar Device Tree Overlay
echo "=== Building Device Tree Overlay ==="
if [ -f "dts/nxp-simtemp.dts" ] && command -v dtc >/dev/null; then
    dtc -I dts -O dtb -o dts/nxp-simtemp.dtbo dts/nxp-simtemp.dts
    echo "✅ DTS overlay built"
else
    echo "⚠️  Skipping DTS (file missing or dtc not installed)"
fi

# Verificar que el CLI Python existe
echo "=== Setting up User CLI ==="
cd ../app
if [ -f "nxp_cli.py" ]; then
    # Hacer ejecutable el script Python
    chmod +x nxp_cli.py
    echo "✅ Python CLI app ready: nxp_cli.py"
else
    echo "❌ nxp_cli.py not found!"
    exit 1
fi

echo "✅ Build completed successfully!"