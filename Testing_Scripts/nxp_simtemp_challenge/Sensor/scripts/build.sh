#!/bin/bash
echo "=== Building NXP SimTemp Driver ==="
cd ../driver
make clean
make
echo "=== Building User CLI ==="  
cd ../app
gcc -o simtemp_cli simtemp_cli.c
echo "✅ Build completed"