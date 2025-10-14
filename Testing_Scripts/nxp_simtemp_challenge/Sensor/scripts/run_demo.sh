#!/bin/bash
echo "=== NXP SimTemp Demo ==="
cd ../driver
sudo rmmod nxp_simtemp 2>/dev/null || true
sudo insmod nxp_simtemp.ko
echo "✅ Module loaded"
sleep 1
cd ../app
echo "=== Reading temperature ==="
sudo ./simtemp_cli
cd ../driver
sudo rmmod nxp_simtemp
echo "✅ Module unloaded"