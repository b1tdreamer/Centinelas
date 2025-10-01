# Plan Técnico: Test de Sonido Simple - ESP32-A1S AudioKit 2.2

## Descripción
Implementar el test más sencillo posible para generar un tono internamente en el ESP32-A1S AudioKit 2.2 y sacarlo por la salida LOUT (Left Output), sin necesidad de archivos WAV ni tarjeta SD. El objetivo es verificar que la conexión de audio está funcionando correctamente.

## Análisis de Opciones Técnicas

### Opción 1: I2S con generación de onda senoidal (RECOMENDADA)
**Pros:**
- Utiliza el hardware I2S nativo del AudioKit
- Salida directa por LOUT/ROUT (jack de auriculares)
- Calidad de audio decente
- Control completo de la forma de onda

**Contras:**
- Requiere cálculos de onda senoidal
- Configuración I2S necesaria (~50 líneas)

### Opción 2: DAC interno ESP32 (GPIO25/26)
**Pros:**
- Más simple que I2S
- Solo 2 líneas de código para generar tono
- Sin librerías externas

**Contras:**
- NO funciona con LOUT del AudioKit (requiere GPIO directo)
- Calidad de audio muy básica
- No aprovecha el codec AC101

### Opción 3: tone() en pin I2S_DOUT (NO RECOMENDADA)
**Pros:**
- Extremadamente simple (1 línea)

**Contras:**
- NO produce sonido en la salida de auriculares del AudioKit
- Solo PWM en el pin, no señal I2S válida
- No activaría el codec AC101

## Solución Elegida: Opción 1 - I2S con onda senoidal
**Justificación:** Es la única opción que realmente produce sonido audible por la salida LOUT del AudioKit. Aunque requiere más código que `tone()`, sigue siendo simple (~100 líneas totales) y verifica correctamente toda la cadena de audio.

## Archivos a Crear

### Archivo Principal
- **`tests/simple_tone_test/simple_tone_test.ino`** (CREAR) - Test minimalista de tono

## Algoritmo de Implementación

### Paso 1: Configuración de I2S
1. **Configurar bus I2S** con parámetros mínimos:
   ```cpp
   Frecuencia muestreo: 16000 Hz (baja para simplificar)
   Bits por muestra: 16 bits
   Canales: 2 (estéreo, solo usaremos izquierdo)
   Modo: TX (solo transmisión)
   ```

2. **Asignar pines I2S** para AudioKit 2.2:
   ```cpp
   I2S_BCLK = 27    // Bit Clock
   I2S_LRC = 26     // Left/Right Clock  
   I2S_DOUT = 25    // Data Output (hacia LOUT)
   ```

### Paso 2: Generación de Onda Senoidal
1. **Pre-calcular tabla de onda senoidal** (64 muestras) en `setup()`:
   - Frecuencia objetivo: 440 Hz (nota La, estándar)
   - Amplitud: 50% para evitar saturación
   - Formato: int16_t para I2S

2. **Crear buffer estéreo**:
   - Canal izquierdo (LOUT): datos de onda senoidal
   - Canal derecho (ROUT): silencio (0)

### Paso 3: Loop de Reproducción
1. **Escribir continuamente** muestras al buffer I2S
2. **Mantener frecuencia constante** de 440 Hz
3. **LED indicador** parpadeando para confirmar ejecución

## Estructura del Proyecto

```
tests/
└── simple_tone_test/
    ├── simple_tone_test.ino    # Programa principal (~100 líneas)
    └── README.md               # Instrucciones de uso
```

## Código Núcleo

### Configuración I2S Mínima
```cpp
i2s_config_t i2s_config = {
  .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
  .sample_rate = 16000,
  .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
  .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
  .communication_format = I2S_COMM_FORMAT_I2S,
  .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
  .dma_buf_count = 8,
  .dma_buf_len = 64,
  .use_apll = false,
  .tx_desc_auto_clear = true
};
```

### Pines I2S
```cpp
i2s_pin_config_t pin_config = {
  .bck_io_num = 27,           // BCLK
  .ws_io_num = 26,            // LRCK (Word Select)
  .data_out_num = 25,         // DOUT -> LOUT
  .data_in_num = I2S_PIN_NO_CHANGE
};
```

### Generación de Tono
```cpp
// Pre-calcular 64 muestras de onda senoidal (440 Hz)
int16_t sineWave[64];
for (int i = 0; i < 64; i++) {
  sineWave[i] = (int16_t)(sin(2.0 * PI * i / 64.0) * 16000);
}

// Loop: enviar al I2S
for (int i = 0; i < 64; i++) {
  int16_t samples[2];
  samples[0] = sineWave[i];  // Canal izquierdo (LOUT)
  samples[1] = 0;            // Canal derecho (ROUT) silencio
  
  i2s_write(I2S_NUM_0, samples, 4, &bytes_written, portMAX_DELAY);
}
```

## Especificaciones Técnicas

### Parámetros de Audio
- **Frecuencia**: 440 Hz (La4 - nota estándar)
- **Frecuencia de muestreo**: 16000 Hz
- **Resolución**: 16 bits
- **Salida**: Solo LOUT (canal izquierdo)
- **Amplitud**: 50% del máximo (16000 / 32768)

### Pines Utilizados
```cpp
#define I2S_BCLK      27    // Bit Clock
#define I2S_LRC       26    // Left/Right Clock
#define I2S_DOUT      25    // Data Output -> LOUT
#define LED_SYSTEM    2     // LED indicador de actividad
```

### Sin Dependencias Externas
- Solo librería `driver/i2s.h` (nativa ESP32)
- Sin tarjeta SD
- Sin archivos de audio
- Sin librería ESP8266Audio

## Control Mínimo
- **Sin botones**: Tono continuo al encender
- **LED GPIO2**: Parpadea para confirmar que el programa funciona
- **Monitor Serial**: Mensajes de confirmación (115200 baud)

## Verificación de Funcionamiento

### Prueba Exitosa:
1. ✅ LED del sistema parpadea cada segundo
2. ✅ Monitor serial muestra "Generando tono 440Hz..."
3. ✅ Se escucha un tono continuo por el auricular **IZQUIERDO** (LOUT)
4. ✅ Auricular derecho está en silencio

### Solución de Problemas:
- **No se escucha nada**: Verificar que auriculares estén conectados al jack del AudioKit
- **Tono distorsionado**: Reducir amplitud de onda (cambiar 16000 a 8000)
- **LED no parpadea**: Problema con el ESP32, no con audio

## Ventajas de Esta Implementación
1. **No requiere SD**: Sin archivos, sin formateo
2. **Test de hardware completo**: Verifica I2S y salida LOUT
3. **Simple**: ~100 líneas de código
4. **Inmediato**: Sonido al encender el ESP32
5. **Diagnóstico claro**: LED y serial confirman ejecución

Este plan proporciona el test más sencillo y efectivo para verificar que la salida LOUT del ESP32-A1S AudioKit 2.2 está funcionando correctamente, generando un tono puro de 440 Hz sin necesidad de archivos externos.