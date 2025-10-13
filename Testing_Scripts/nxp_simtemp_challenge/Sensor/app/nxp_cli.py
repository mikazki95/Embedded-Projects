#!/usr/bin/env python3
"""
NXP SimTemp CLI - Versión CORREGIDA
"""

import time
import struct
import os
import sys
from datetime import datetime

class SimTempSystem:
    def __init__(self, simulation_mode=True):
        self.simulation_mode = simulation_mode
        self.sysfs_base = "/sys/class/nxp_simtemp/simtemp"
        self.device_path = "/dev/simtemp"
        
        # Parámetros de simulación (atributos de instancia)
        self.base_temp = 25000
        self.amplitude = 5000
        self.frequency = 0.1
        self.threshold_high = 30000  # ← threshold_high en mC
        self.sampling_ms = 1000      # ← sampling_ms en milisegundos
        self.start_time = time.time()
    
    def detect_mode(self):
        """Detectar si el driver real está disponible"""
        if os.path.exists(self.sysfs_base):
            self.simulation_mode = False
            print("✅ Modo REAL: Driver del kernel detectado")
        else:
            self.simulation_mode = True
            print("🔧 Modo SIMULACIÓN: Usando simulador interno")
        return self.simulation_mode
    
    def config_sampling(self, sampling_ms):
        """Configurar periodo de sampling"""
        if not self.simulation_mode:
            try:
                with open(f"{self.sysfs_base}/sampling_ms", 'w') as f:
                    f.write(str(sampling_ms))
                print(f"Sampling configurado a: {sampling_ms} ms (via sysfs)")
                return True
            except:
                print("Error escribiendo a sysfs, cambiando a modo simulación")
                self.simulation_mode = True
        
        # MODO SIMULACIÓN: Actualizar atributo de instancia
        self.sampling_ms = sampling_ms
        print(f"Sampling configurado a: {sampling_ms} ms (simulación)")
        return True
    
    def config_threshold(self, threshold_c):
        """Configurar threshold"""
        threshold_mC = int(threshold_c * 1000)
        
        if not self.simulation_mode:
            try:
                with open(f"{self.sysfs_base}/threshold_mC", 'w') as f:
                    f.write(str(threshold_mC))
                print(f"Threshold configurado a: {threshold_c}°C (via sysfs)")
                return True
            except:
                print("Error escribiendo a sysfs, cambiando a modo simulación")
                self.simulation_mode = True
        
        # MODO SIMULACIÓN: Actualizar atributo de instancia
        self.threshold_high = threshold_mC
        print(f"Threshold configurado a: {threshold_c}°C (simulación)")
        return True
    
    def generate_sample(self):
        """Generar muestra (compatible con ambos modos)"""
        if not self.simulation_mode:
            try:
                with open(self.device_path, 'rb') as f:
                    data = f.read(16)
                    if len(data) == 16:
                        timestamp_ns, temp_mC, flags = struct.unpack("<QII", data)
                        return timestamp_ns, temp_mC, flags
            except:
                print("Error leyendo dispositivo, cambiando a simulación")
                self.simulation_mode = True
        
        # MODO SIMULACIÓN: Generar muestra usando los atributos actuales
        import math
        current_time = time.time()
        elapsed = current_time - self.start_time
        
        # Usar self.amplitude, self.frequency, etc.
        temp_variation = self.amplitude * math.sin(2 * math.pi * self.frequency * elapsed)
        current_temp = self.base_temp + int(temp_variation)
        
        flags = 0x1  # NEW_SAMPLE
        if current_temp >= self.threshold_high:  # Usar self.threshold_high
            flags |= 0x2  # THRESHOLD_CROSSED
        
        return int(current_time * 1e9), current_temp, flags
    
    def read_continuous(self, duration=10):
        """Leer muestras continuamente"""
        self.detect_mode()
        
        print(f"=== Leyendo sensor por {duration} segundos ===")
        print(f"Modo: {'SIMULACIÓN' if self.simulation_mode else 'REAL'}")
        print(f"Config: sampling={self.sampling_ms}ms, threshold={self.threshold_high/1000}°C")
        print("-" * 60)
        
        start_time = time.time()
        sample_count = 0
        
        try:
            while time.time() - start_time < duration:
                timestamp_ns, temp_mC, flags = self.generate_sample()
                
                if temp_mC is not None:
                    sample_count += 1
                    temp_c = temp_mC / 1000.0
                    timestamp_s = timestamp_ns / 1e9
                    human_time = datetime.fromtimestamp(timestamp_s).strftime("%Y-%m-%dT%H:%M:%S.%fZ")
                    
                    alert_status = "ALERT" if (flags & 0x2) else "OK"
                    print(f"{human_time} temp={temp_c:.1f}C alert={alert_status}")
                
                # Usar el sampling configurado
                sleep_time = self.sampling_ms / 1000.0
                time.sleep(sleep_time)
                
        except KeyboardInterrupt:
            print("\nLectura interrumpida")
        
        print("-" * 60)
        print(f"Muestras leídas: {sample_count}")
    
    def show_info(self):
        """Mostrar información CORREGIDA"""
        self.detect_mode()
        print(f"Modo actual: {'SIMULACIÓN' if self.simulation_mode else 'REAL'}")
        if self.simulation_mode:
            print(f"Base temp: {self.base_temp/1000}°C")
            print(f"Amplitud: ±{self.amplitude/1000}°C")
            print(f"Frecuencia: {self.frequency} Hz")
            print(f"Sampling: {self.sampling_ms} ms")
            print(f"Threshold: {self.threshold_high/1000}°C")

def main():
    system = SimTempSystem()
    
    if len(sys.argv) > 1:
        command = sys.argv[1]
        
        if command == "read":
            duration = int(sys.argv[2]) if len(sys.argv) > 2 else 10
            system.read_continuous(duration)
            
        elif command == "config":
            if len(sys.argv) > 3:
                param = sys.argv[2]
                value = sys.argv[3]
                
                if param == "sampling_ms":
                    system.config_sampling(int(value))
                elif param == "threshold":
                    system.config_threshold(float(value))
                else:
                    print("Parámetro desconocido. Usa: sampling_ms, threshold")
                    
        elif command == "info":
            system.show_info()
            
        elif command == "test":
            print("=== Test de Alertas ===")
            system.config_threshold(26.0)  # Threshold bajo
            system.read_continuous(5)
            
    else:
        print("NXP SimTemp CLI - Modo automático (real/simulación)")
        print("Comandos:")
        print("  python3 nxp_cli.py info")
        print("  python3 nxp_cli.py config sampling_ms 500")
        print("  python3 nxp_cli.py config threshold 26.0")
        print("  python3 nxp_cli.py read [seconds]")
        print("  python3 nxp_cli.py test")

if __name__ == "__main__":
    main()