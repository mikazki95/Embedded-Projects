
### Para **DDR_test**:
```markdown
# Pruebas de Memoria DDR
> **Validación de hardware** · Acceso a memoria dinámica · Pruebas de estrés

**Objetivo de aprendizaje:**  
Verificar la integridad del subsistema de memoria mediante patrones de prueba avanzados.

**Habilidad principal desarrollada:**  
- Patrones de prueba de memoria (March C-)  
- Acceso DMA a memoria  
- Detección de errores de hardware  

**Fragmento clave:**
```c
/* Patrón de prueba de memoria */
for(uint32_t *ptr = DDR_BASE; ptr < DDR_LIMIT; ptr++) {
    *ptr = 0xAAAAAAAA;  // Patrón alternante
    if(*ptr != 0xAAAAAAAA) {
        memory_fault_handler();
    }
}
