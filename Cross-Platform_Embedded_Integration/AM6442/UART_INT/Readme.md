
### Para **UART_INT**:
```markdown
# Comunicación por Interrupciones
> **Eficiencia en E/S** · Buffering asíncrono · Manejo de eventos

**Objetivo de aprendizaje:**  
Optimizar la comunicación serial mediante el uso de interrupciones y buffers circulares.

**Habilidad principal desarrollada:**  
- Configuración de NVIC para UART  
- Implementación de ring buffers  
- Manejo de colas de transmisión  

**Fragmento clave:**
```c
void UART_IRQHandler(void) {
    if(UART->STATUS & RX_DATA_READY) {
        ring_buffer_put(&rx_buf, UART->RX_DATA);
    }
}
