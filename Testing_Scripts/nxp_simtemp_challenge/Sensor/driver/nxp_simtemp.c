#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/ktime.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/poll.h>  
//#include <linux/math.h>
#include "nxp_simtemp.h"

static dev_t dev_number;
static struct class *simtemp_class;
static struct nxp_simtemp_data *device_data;

/* Función para simular temperatura con onda sinusoidal */
static int simulate_temperature(struct nxp_simtemp_data *data)
{
    unsigned long current_jiffies = jiffies;
    unsigned long elapsed_seconds = (current_jiffies - data->last_update) / HZ;
    
    /* Variación simple de temperatura: oscila entre base_temp ± amplitude */
    int cycle_position = elapsed_seconds % 20;  // Ciclo de 20 segundos
    int temp_variation;
    
    if (cycle_position < 10) {
        // Subiendo: 0 a amplitude
        temp_variation = (data->amplitude * cycle_position) / 10;
    } else {
        // Bajando: amplitude a 0  
        temp_variation = (data->amplitude * (20 - cycle_position)) / 10;
    }
    
    // Centrar la variación alrededor de base_temp
    return data->base_temp + temp_variation - (data->amplitude / 2);
}

/* Timer callback - actualiza temperatura y verifica alarmas */
static void update_temperature(struct timer_list *t)
{
    struct nxp_simtemp_data *data = from_timer(data, t, timer);
    bool old_alarm_state;
    
    mutex_lock(&data->lock);
    
    /* Actualizar temperatura */
    data->current_temp = simulate_temperature(data);
    data->last_update = jiffies;
    
    /* Verificar si cambió el estado de alarma */
    old_alarm_state = data->alarm_active;
    data->alarm_active = (data->current_temp >= data->alarm_high) || 
                         (data->current_temp <= data->alarm_low);
    
    /* Despertar wait_queue si cambió el estado de alarma */
    if (data->alarm_active != old_alarm_state) {
        wake_up_interruptible(&data->wait_queue);
        printk(KERN_INFO "NXP SimTemp: Alarma %s - Temp: %d mC\n",
               data->alarm_active ? "ACTIVADA" : "DESACTIVADA", 
               data->current_temp);
    }
    
    /* Reprogramar timer */
    mod_timer(&data->timer, jiffies + msecs_to_jiffies(data->update_interval_ms));
    
    mutex_unlock(&data->lock);
}

/* Operaciones del char device */
static int nxp_simtemp_open(struct inode *inode, struct file *filp)
{
    printk(KERN_DEBUG "NXP SimTemp: Dispositivo abierto\n");
    return 0;
}

static int nxp_simtemp_release(struct inode *inode, struct file *filp)
{
    printk(KERN_DEBUG "NXP SimTemp: Dispositivo cerrado\n");
    return 0;
}

static ssize_t nxp_simtemp_read(struct file *filp, char __user *buf, 
                               size_t count, loff_t *f_pos)
{
    struct nxp_simtemp_data *data = device_data;
    struct simtemp_sample sample;
    
    if (count < sizeof(sample)) 
        return -EINVAL;
    
    mutex_lock(&data->lock);
    
    /* Llenar estructura de muestra */
    sample.timestamp_ns = ktime_get_ns();
    sample.temp_mC = data->current_temp;
    sample.flags = SIMTEMP_FLAG_NEW_SAMPLE;
    
    if (data->alarm_active)
        sample.flags |= SIMTEMP_FLAG_THRESHOLD_CROSS;
    
    mutex_unlock(&data->lock);
    
    /* Copiar a espacio de usuario */
    if (copy_to_user(buf, &sample, sizeof(sample))) 
        return -EFAULT;
    
    return sizeof(sample);
}

static __poll_t nxp_simtemp_poll(struct file *filp, poll_table *wait)
{
    struct nxp_simtemp_data *data = device_data;
    __poll_t mask = 0;
    
    poll_wait(filp, &data->wait_queue, wait);
    
    mutex_lock(&data->lock);
    
    /* Siempre hay datos para leer (nueva muestra) */
    mask |= POLLIN | POLLRDNORM;
    
    /* Alertas de threshold */
    if (data->alarm_active)
        mask |= POLLPRI;
    
    mutex_unlock(&data->lock);
    
    return mask;
}

static struct file_operations nxp_simtemp_fops = {
    .owner = THIS_MODULE,
    .open = nxp_simtemp_open,
    .release = nxp_simtemp_release,
    .read = nxp_simtemp_read,
    .poll = nxp_simtemp_poll,
};

/* Sysfs attributes */
static ssize_t temperature_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    struct nxp_simtemp_data *data = device_data;
    return sprintf(buf, "%d\n", data->current_temp);
}
static DEVICE_ATTR_RO(temperature);

static ssize_t threshold_high_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    struct nxp_simtemp_data *data = device_data;
    return sprintf(buf, "%d\n", data->alarm_high);
}

static ssize_t threshold_high_store(struct device *dev, struct device_attribute *attr,
                                   const char *buf, size_t count)
{
    struct nxp_simtemp_data *data = device_data;
    int new_threshold;
    
    if (kstrtoint(buf, 10, &new_threshold))
        return -EINVAL;
    
    mutex_lock(&data->lock);
    data->alarm_high = new_threshold;
    mutex_unlock(&data->lock);
    
    return count;
}
static DEVICE_ATTR_RW(threshold_high);

static ssize_t sampling_ms_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    struct nxp_simtemp_data *data = device_data;
    return sprintf(buf, "%u\n", data->update_interval_ms);
}

static ssize_t sampling_ms_store(struct device *dev, struct device_attribute *attr,
                                const char *buf, size_t count)
{
    struct nxp_simtemp_data *data = device_data;
    unsigned int new_interval;
    
    if (kstrtouint(buf, 10, &new_interval) || new_interval < 10 || new_interval > 10000)
        return -EINVAL;
    
    mutex_lock(&data->lock);
    data->update_interval_ms = new_interval;
    
    /* Reprogramar timer con nuevo intervalo */
    mod_timer(&data->timer, jiffies + msecs_to_jiffies(new_interval));
    
    mutex_unlock(&data->lock);
    return count;
}
static DEVICE_ATTR_RW(sampling_ms);

static struct attribute *nxp_simtemp_attrs[] = {
    &dev_attr_temperature.attr,
    &dev_attr_threshold_high.attr,
    &dev_attr_sampling_ms.attr,
    NULL,
};

static struct attribute_group nxp_simtemp_attr_group = {
    .attrs = nxp_simtemp_attrs,
    .name = NULL,
};

/* Función de inicialización del módulo */
static int __init nxp_simtemp_init(void)
{
    int ret;
    
    printk(KERN_INFO "NXP SimTemp: Iniciando módulo...\n");
    
    /* Reservar número de dispositivo */
    ret = alloc_chrdev_region(&dev_number, 0, 1, "nxp_simtemp");
    if (ret < 0) {
        printk(KERN_ERR "NXP SimTemp: Error reservando dispositivo\n");
        return ret;
    }
    
    /* Crear clase de dispositivo */
    simtemp_class = class_create(THIS_MODULE, "nxp_simtemp");
    if (IS_ERR(simtemp_class)) {
        ret = PTR_ERR(simtemp_class);
        goto error_class;
    }
    
    /* Allocar estructura de datos */
    device_data = kzalloc(sizeof(struct nxp_simtemp_data), GFP_KERNEL);
    if (!device_data) {
        ret = -ENOMEM;
        goto error_alloc;
    }
    
    /* Inicializar con valores por defecto */
    device_data->base_temp = DEFAULT_BASE_TEMP;
    device_data->amplitude = DEFAULT_AMPLITUDE;
    device_data->frequency = DEFAULT_FREQUENCY;
    device_data->alarm_high = DEFAULT_ALARM_HIGH;
    device_data->alarm_low = DEFAULT_ALARM_LOW;
    device_data->update_interval_ms = DEFAULT_UPDATE_MS;
    device_data->current_temp = DEFAULT_BASE_TEMP;
    device_data->last_update = jiffies;
    
    /* Inicializar mutex y wait queue */
    mutex_init(&device_data->lock);
    init_waitqueue_head(&device_data->wait_queue);
    
    /* Configurar timer */
    timer_setup(&device_data->timer, update_temperature, 0);
    mod_timer(&device_data->timer, 
              jiffies + msecs_to_jiffies(device_data->update_interval_ms));
    
    /* Configurar char device */
    cdev_init(&device_data->cdev, &nxp_simtemp_fops);
    device_data->cdev.owner = THIS_MODULE;
    
    ret = cdev_add(&device_data->cdev, dev_number, 1);
    if (ret < 0) {
        printk(KERN_ERR "NXP SimTemp: Error agregando char device\n");
        goto error_cdev;
    }
    
    /* Crear dispositivo */
    device_data->device = device_create(simtemp_class, NULL, dev_number, 
                                       NULL, "simtemp");
    if (IS_ERR(device_data->device)) {
        ret = PTR_ERR(device_data->device);
        goto error_device;
    }
    
    /* Crear sysfs attributes */
    ret = sysfs_create_group(&device_data->device->kobj, &nxp_simtemp_attr_group);
    if (ret) {
        printk(KERN_ERR "NXP SimTemp: Error creando sysfs\n");
        goto error_sysfs;
    }
    
    printk(KERN_INFO "NXP SimTemp: Driver inicializado. Device: /dev/simtemp\n");
    printk(KERN_INFO "NXP SimTemp: Temp inicial: %d mC, Intervalo: %u ms\n",
           device_data->current_temp, device_data->update_interval_ms);
    
    return 0;

error_sysfs:
    device_destroy(simtemp_class, dev_number);
error_device:
    cdev_del(&device_data->cdev);
error_cdev:
    del_timer(&device_data->timer);
    kfree(device_data);
error_alloc:
    class_destroy(simtemp_class);
error_class:
    unregister_chrdev_region(dev_number, 1);
    return ret;
}

/* Función de limpieza del módulo */
static void __exit nxp_simtemp_exit(void)
{
    printk(KERN_INFO "NXP SimTemp: Descargando módulo...\n");
    
    if (device_data) {
        /* Limpieza en orden inverso */
        del_timer_sync(&device_data->timer);
        sysfs_remove_group(&device_data->device->kobj, &nxp_simtemp_attr_group);
        device_destroy(simtemp_class, dev_number);
        cdev_del(&device_data->cdev);
        kfree(device_data);
        device_data = NULL;
    }
    
    class_destroy(simtemp_class);
    unregister_chrdev_region(dev_number, 1);
    
    printk(KERN_INFO "NXP SimTemp: Driver descargado correctamente\n");
}

module_init(nxp_simtemp_init);
module_exit(nxp_simtemp_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tonatiuh Velazquez");
MODULE_DESCRIPTION("NXP Simulated Temperature Sensor Driver");
MODULE_VERSION("1.0");