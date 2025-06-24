
### Para **UART_example**:
```markdown
# Comunicación Serial Básica
> **Protocolo esencial** · Configuración de periféricos · Comunicación punto a punto

**Objetivo de aprendizaje:**  
Establecer comunicación básica mediante UART para diagnóstico y control.

**Habilidad principal desarrollada:**  
- Configuración de baud rate  
- Transmisión/recepción bloqueante  
- Formateo básico de mensajes  

**Fragmento clave:**
```c
void uart_send(const char *msg) {
    while(*msg) {
        UART->TX = *msg++;
        while(!(UART->STATUS & TX_READY));
    }
}
