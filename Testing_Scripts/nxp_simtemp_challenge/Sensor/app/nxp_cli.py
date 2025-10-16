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
    def __init__(self):
        self.sysfs_base = "/sys/class/nxp_simtemp/simtemp"
        self.device_path = "/dev/simtemp"
        
        # PARÁMETROS MODIFICADOS SEGÚN LO QUE PEDISTE
        self.base_temp = 25000      # 25°C base
        self.amplitude = 10000      # ±10°C variación (antes 5000)
        self.frequency = 50         # 50 Hz frecuencia (antes 0.1)
        self.threshold_high = 30000 # 30°C alarma alta
        self.threshold_low = 20000  # 20°C alarma baja (NUEVO)
        self.sampling_ms = 1000     # 1 segundo sampling
        self.start_time = time.time()
    
    def config_sampling(self, sampling_ms):
        """Configurar periodo de sampling"""
        self.sampling_ms = sampling_ms
        print(f"Sampling configurado a: {sampling_ms} ms")
        return
    
    def config_threshold_high(self, threshold_c):
        """Configurar threshold alto"""
        threshold_mC = int(threshold_c * 1000)
        self.threshold_high = threshold_mC
        print(f"Threshold ALTO configurado a: {threshold_c}°C")
        return
    
    def config_threshold_low(self, threshold_c):
        """Configurar threshold bajo"""
        threshold_mC = int(threshold_c * 1000)
        self.threshold_low = threshold_mC
        print(f"Threshold BAJO configurado a: {threshold_c}°C")
        return
    
    def config_amplitude(self, amplitude_c):
        """Configurar amplitud"""
        amplitude_mC = int(amplitude_c * 1000)
        self.amplitude = amplitude_mC
        print(f"Amplitud configurada a: ±{amplitude_c}°C")
        return
    
    def config_frequency(self, frequency_hz):
        """Configurar frecuencia"""
        self.frequency = frequency_hz
        print(f"Frecuencia configurada a: {frequency_hz} Hz")
        return

    def generate_sample(self):
        """Generar muestra (compatible con ambos modos)"""
        import math
        current_time = time.time()
        elapsed = current_time - self.start_time
        
        # Usar self.amplitude, self.frequency, etc.
        temp_variation = self.amplitude * math.sin(2 * math.pi * self.frequency * elapsed)
        current_temp = self.base_temp + int(temp_variation)
        
        flags = 0x1  # NEW_SAMPLE
        if current_temp >= self.threshold_high:  # Alarma por alto
            flags |= 0x2  # THRESHOLD_CROSSED
        elif current_temp <= self.threshold_low:  # Alarma por bajo (NUEVO)
            flags |= 0x2  # THRESHOLD_CROSSED
        
        return int(current_time * 1e9), current_temp, flags
    
    def read_continuous(self, duration=30):  # 30 SEGUNDOS POR DEFECTO
        """Leer muestras continuamente"""        
        print(f"=== Leyendo sensor por {duration} segundos ===")
        print(f"Config: sampling={self.sampling_ms}ms, threshold_high={self.threshold_high/1000}°C, threshold_low={self.threshold_low/1000}°C")
        print(f"Amplitud: ±{self.amplitude/1000}°C, Frecuencia: {self.frequency} Hz")
        print("-" * 80)
        
        start_time = time.time()
        sample_count = 0
        alert_count = 0
        
        try:
            while time.time() - start_time < duration:
                timestamp_ns, temp_mC, flags = self.generate_sample()
                
                if temp_mC is not None:
                    sample_count += 1
                    temp_c = temp_mC / 1000.0
                    timestamp_s = timestamp_ns / 1e9
                    human_time = datetime.fromtimestamp(timestamp_s).strftime("%Y-%m-%dT%H:%M:%S.%fZ")
                    
                    # Determinar tipo de alerta
                    if temp_mC >= self.threshold_high:
                        alert_status = "HIGH_ALERT"
                        alert_count += 1
                    elif temp_mC <= self.threshold_low:
                        alert_status = "LOW_ALERT" 
                        alert_count += 1
                    else:
                        alert_status = "OK"
                    
                    print(f"{human_time} temp={temp_c:.1f}C alert={alert_status}")
                
                # Usar el sampling configurado
                sleep_time = self.sampling_ms / 1000.0
                time.sleep(sleep_time)
                
        except KeyboardInterrupt:
            print("\nLectura interrumpida")
        
        print("-" * 80)
        print(f"RESUMEN: Muestras={sample_count}, Alertas={alert_count}")
        print(f"Rango de temperatura: {self.base_temp/1000 - self.amplitude/1000:.1f}°C a {self.base_temp/1000 + self.amplitude/1000:.1f}°C")
    
    def show_info(self):
        """Mostrar información CORREGIDA"""
        print("=== CONFIGURACIÓN ACTUAL ===")
        print(f"Base temp: {self.base_temp/1000}°C")
        print(f"Amplitud: ±{self.amplitude/1000}°C")
        print(f"Frecuencia: {self.frequency} Hz")
        print(f"Sampling: {self.sampling_ms} ms")
        print(f"Threshold ALTO: {self.threshold_high/1000}°C")
        print(f"Threshold BAJO: {self.threshold_low/1000}°C")
        print(f"Rango esperado: {self.base_temp/1000 - self.amplitude/1000:.1f}°C a {self.base_temp/1000 + self.amplitude/1000:.1f}°C")

def main():
    system = SimTempSystem()
    
    if len(sys.argv) > 1:
        command = sys.argv[1]
        
        if command == "read":
            duration = int(sys.argv[2]) if len(sys.argv) > 2 else 30  # 30s por defecto
            system.read_continuous(duration)
            
        elif command == "config":
            if len(sys.argv) > 3:
                param = sys.argv[2]
                value = sys.argv[3]
                
                if param == "sampling_ms":
                    system.config_sampling(int(value))
                elif param == "threshold_high":
                    system.config_threshold_high(float(value))
                elif param == "threshold_low":
                    system.config_threshold_low(float(value))
                elif param == "amplitude":
                    system.config_amplitude(float(value))
                elif param == "frequency":
                    system.config_frequency(float(value))
                else:
                    print("Parámetros válidos: sampling_ms, threshold_high, threshold_low, amplitude, frequency")
                    
        elif command == "info":
            system.show_info()
            
        elif command == "test":
            print("=== Test de Alertas ===")
            # Configurar según lo que pediste
            system.config_amplitude(10.0)     # ±10°C
            system.config_frequency(50)       # 50 Hz
            system.config_threshold_high(30.0) # 30°C
            system.config_threshold_low(20.0)  # 20°C
            system.read_continuous(30)        # 30 segundos
            
    else:
        print("NXP SimTemp CLI - Configuración actual:")
        print("  Amplitud: ±10°C, Frecuencia: 50 Hz")
        print("  Thresholds: 20°C (bajo) - 30°C (alto)")
        print("  Duración lectura: 30 segundos")
        print("\nComandos:")
        print("  python3 nxp_cli.py info")
        print("  python3 nxp_cli.py config <param> <value>")
        print("  python3 nxp_cli.py read [seconds]")
        print("  python3 nxp_cli.py test")

if __name__ == "__main__":
    main()