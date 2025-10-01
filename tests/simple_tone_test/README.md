# Test Simple de Sonido - ESP32-A1S AudioKit 2.2

## Descripción
Test minimalista para verificar la funcionalidad de audio del ESP32-A1S AudioKit 2.2. Genera un tono puro de 440 Hz (nota La4) por la salida LOUT (canal izquierdo) del jack de auriculares.

## Características
- ✅ **Sin dependencias externas**: Solo usa I2S nativo del ESP32
- ✅ **Sin tarjeta SD**: No requiere archivos de audio
- ✅ **Test inmediato**: Sonido al encender el ESP32
- ✅ **Diagnóstico visual**: LED parpadeante + monitor serial
- ✅ **Verificación completa**: Prueba toda la cadena de audio I2S

## Hardware Requerido
- ESP32-A1S AudioKit 2.2
- Auriculares o altavoz con jack de 3.5mm
- Cable USB para programación

## Instalación y Uso

### 1. Preparación
```bash
# Abrir Arduino IDE
# Seleccionar placa: "ESP32 Dev Module" o "ESP32-A1S AudioKit"
# Puerto COM correcto
```

### 2. Carga del Código
1. Abrir `simple_tone_test.ino`
2. Verificar configuración de placa
3. Compilar y subir al ESP32

### 3. Prueba de Audio
1. Conectar auriculares al jack de 3.5mm del AudioKit
2. El ESP32 comenzará a generar el tono automáticamente
3. Verificar indicadores de funcionamiento

## Verificación de Funcionamiento

### ✅ Prueba Exitosa
- **LED GPIO2**: Parpadea cada segundo
- **Monitor Serial**: Mensajes de confirmación a 115200 baud
- **Audio**: Tono continuo de 440 Hz en auricular **IZQUIERDO**
- **Canal derecho**: Silencio total

### 📊 Parámetros Técnicos
- **Frecuencia del tono**: 440 Hz (La4)
- **Frecuencia de muestreo**: 16000 Hz
- **Resolución**: 16 bits
- **Formato**: Estéreo (solo canal izquierdo activo)
- **Amplitud**: 50% del máximo (16000/32768)

### 🔧 Pines Utilizados
```cpp
I2S_BCLK = 27    // Bit Clock
I2S_LRC = 26     // Left/Right Clock
I2S_DOUT = 25    // Data Output -> LOUT
LED_SYSTEM = 2   // LED indicador
```

## Solución de Problemas

### ❌ No se escucha nada
**Causas posibles:**
- Auriculares no conectados al jack del AudioKit
- Volumen de auriculares muy bajo
- Problema con el codec AC101

**Soluciones:**
1. Verificar conexión física del jack
2. Subir volumen de auriculares
3. Probar con diferentes auriculares

### ❌ Tono distorsionado o muy fuerte
**Causa:** Amplitud muy alta
**Solución:** Cambiar `AMPLITUDE` de 16000 a 8000 en el código

### ❌ LED no parpadea
**Causa:** Problema con el ESP32, no con audio
**Solución:** Verificar alimentación y programación

### ❌ Errores de compilación
**Causas posibles:**
- Librerías I2S no disponibles
- Configuración incorrecta de placa

**Soluciones:**
1. Usar ESP32 Arduino Core 2.0.0 o superior
2. Seleccionar placa "ESP32 Dev Module"

## Estructura del Código

### Configuración I2S
- Configuración mínima para transmisión
- Pines específicos del AudioKit 2.2
- Buffer DMA optimizado

### Generación de Onda
- Tabla pre-calculada de 64 muestras
- Onda senoidal pura de 440 Hz
- Formato int16_t para I2S

### Loop Principal
- Envío continuo al buffer I2S
- Control de LED indicador
- Sin bloqueos del sistema

## Ventajas de Esta Implementación

1. **Simplicidad**: ~150 líneas de código total
2. **Confiabilidad**: Usa hardware I2S nativo
3. **Diagnóstico**: Múltiples indicadores de estado
4. **Portabilidad**: Funciona en cualquier ESP32-A1S
5. **Base sólida**: Fácil de extender para audio más complejo

## Próximos Pasos

Una vez verificado que este test funciona correctamente, se puede proceder a:
- Implementar reproducción de archivos WAV
- Agregar controles de volumen
- Implementar múltiples tonos
- Crear sintetizador básico

---

**Nota**: Este test verifica específicamente la salida LOUT del ESP32-A1S AudioKit 2.2. Si funciona correctamente, confirma que toda la cadena de audio I2S → Codec AC101 → Jack de auriculares está operativa.
