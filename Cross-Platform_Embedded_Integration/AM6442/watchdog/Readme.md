
### Para **watchdog**:
```markdown
# Sistema de Recuperación de Fallos
> **Robustez del sistema** · Prevención de bloqueos · Recuperación automática

**Objetivo de aprendizaje:**  
Implementar mecanismos de seguridad que prevengan bloqueos catastróficos.

**Habilidad principal desarrollada:**  
- Configuración de watchdog  
- Estrategias de alimentación periódica  
- Recuperación de estado tras fallo  

**Fragmento clave:**
```c
void watchdog_feed(void) {
    /* Alimentación periódica del watchdog */
    static uint32_t counter = 0;
    if(++counter >= FEED_INTERVAL) {
        WATCHDOG->FEED = 0xAAAA;
        counter = 0;
    }
}
