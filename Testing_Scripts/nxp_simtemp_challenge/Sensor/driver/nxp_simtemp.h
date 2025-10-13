#ifndef NXP_SIMTEMP_H
#define NXP_SIMTEMP_H

#include <linux/types.h>
#include <linux/mutex.h>
#include <linux/timer.h>
#include <linux/wait.h>
#include <linux/cdev.h>

/* Constantes */
#define DEFAULT_BASE_TEMP       25000   // 25.0°C
#define DEFAULT_AMPLITUDE       0    // ±5°C
#define DEFAULT_FREQUENCY       100     // 0.1 Hz (10 segundos por ciclo)
#define DEFAULT_ALARM_HIGH      30000   // 30.0°C
#define DEFAULT_ALARM_LOW       20000   // 20.0°C  
#define DEFAULT_UPDATE_MS         1000    // 1 segundo

/* Estructura de datos del dispositivo */
struct nxp_simtemp_data {
    /* Configuración */
    int base_temp;
    int amplitude_mC;
    int frequency_hz;
    int alarm_high;
    int alarm_low;
    unsigned int update_interval_ms;
    int wave_counter;        // Contador interno
    u64 wave_start_ns;        
    u32 wave_period_us;       
    
    /* Estado actual */
    int current_temp;
    unsigned long last_update;
    bool alarm_active;
    
    /* Mecanismos de kernel */
    struct mutex lock;
    struct timer_list timer;
    wait_queue_head_t wait_queue;
    struct cdev cdev;
    struct device *device;
};

/* Estructura para muestras (user-kernel API) */
struct simtemp_sample {
    __u64 timestamp_ns;
    __s32 temp_mC;
    __u32 flags;
} __attribute__((packed));

/* Definiciones de flags */
#define SIMTEMP_FLAG_NEW_SAMPLE       0x1
#define SIMTEMP_FLAG_THRESHOLD_CROSS  0x2

#endif /* NXP_SIMTEMP_H */