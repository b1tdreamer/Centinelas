# Test Secuencial NeoPixel - ESP32 AudioKit 2.2

## Descripción
Programa de prueba para controlar un aro LED NeoPixel de 8 píxeles con efecto de "ola verde" secuencial.

## Hardware Requerido

### Componentes
- **ESP32 AudioKit 2.2**
- **Aro NeoPixel WS2812B** de 8 LEDs
- **Cables de conexión**
- **Fuente de alimentación** (USB >500mA recomendado)

### Especificaciones del NeoPixel
- Modelo: WS2812B compatible
- Cantidad: 8 píxeles
- Voltaje: 5V
- Corriente máxima: ~480mA (60mA × 8 LEDs al 100%)
- Protocolo: 800 KHz, formato GRB

## Conexiones

```
NeoPixel Ring ─── ESP32 AudioKit 2.2
    VCC (5V)  ─── 5V o VIN
    GND       ─── GND
    DIN       ─── GPIO23 (IO23)
```

### Notas de Conexión
- **Alimentación**: Se recomienda fuente USB de al menos 600mA para operación segura
- **Resistencia opcional**: Se puede agregar 470Ω entre GPIO23 y DIN para mayor estabilidad
- **Capacitor opcional**: 1000µF entre VCC y GND ayuda a estabilizar alimentación

## Instalación

### Requisitos
1. **Arduino IDE** o **PlatformIO**
2. **Librería**: Adafruit NeoPixel v1.12.0 o superior

### Instalación de Librería (Arduino IDE)
1. Abrir Arduino IDE
2. Ir a: `Tools → Manage Libraries...`
3. Buscar: "Adafruit NeoPixel"
4. Instalar la versión más reciente

### Instalación con PlatformIO
El archivo `platformio.ini` incluido ya tiene las dependencias configuradas.

## Uso

### Carga del Programa
1. Conectar ESP32 AudioKit 2.2 al computador vía USB
2. Seleccionar placa: ESP32 Dev Module
3. Seleccionar puerto COM correspondiente
4. Cargar el sketch `neopixel_simple.ino`

### Monitor Serial
- **Baudrate**: 115200
- Muestra configuración actual y parámetros del ciclo

## Configuración

### Variables Modificables

Todas las variables están al inicio del código para fácil personalización:

```cpp
// Temporización
const unsigned long LED_DURATION = 3000;   // Duración encendido (ms)
const unsigned long LED_INTERVAL = 1000;   // Intervalo entre LEDs (ms)

// Apariencia
const uint8_t BRIGHTNESS = 76;             // Brillo 0-255 (30%)
const uint8_t COLOR_R = 0;                 // Rojo
const uint8_t COLOR_G = 255;               // Verde
const uint8_t COLOR_B = 0;                 // Azul
```

### Ejemplos de Personalización

**Efecto más rápido:**
```cpp
const unsigned long LED_DURATION = 1500;
const unsigned long LED_INTERVAL = 400;
```

**Efecto más lento:**
```cpp
const unsigned long LED_DURATION = 5000;
const unsigned long LED_INTERVAL = 1500;
```

**Cambiar a azul:**
```cpp
const uint8_t COLOR_R = 0;
const uint8_t COLOR_G = 0;
const uint8_t COLOR_B = 255;
```

**Mayor brillo (50%):**
```cpp
const uint8_t BRIGHTNESS = 128;
```

## Comportamiento Esperado

### Con Configuración por Defecto
- **Ciclo completo**: 8 segundos (8 LEDs × 1000ms)
- **LEDs simultáneos**: 3-4 LEDs encendidos al mismo tiempo
- **Efecto visual**: Ola verde rotando alrededor del aro
- **Color**: Verde brillante
- **Brillo**: 30% del máximo

### Verificación de Funcionamiento

✅ **Test Exitoso:**
1. Monitor serial muestra mensajes de inicio y configuración
2. Los LEDs se encienden secuencialmente en color verde
3. Cada LED permanece encendido durante 3 segundos
4. Un nuevo LED comienza a encenderse cada 1 segundo
5. Múltiples LEDs están encendidos simultáneamente (efecto ola)
6. El patrón vuelve al LED 0 después de completar el aro
7. El ciclo se repite indefinidamente de forma suave

## Solución de Problemas

### LEDs no encienden
- ✅ Verificar alimentación 5V conectada
- ✅ Verificar conexión GND común
- ✅ Verificar cable de datos en GPIO23
- ✅ Revisar continuidad de cables

### LEDs parpadean erráticamente
- ✅ Agregar resistencia 470Ω en línea de datos (GPIO23 → DIN)
- ✅ Verificar alimentación adecuada (>500mA)
- ✅ Acortar cables de conexión
- ✅ Agregar capacitor 1000µF en alimentación

### Colores incorrectos
- ✅ Cambiar `NEO_GRB` por `NEO_RGB` en línea 48 del código
- ✅ Verificar valores de COLOR_R, COLOR_G, COLOR_B

### Brillo muy bajo
- ✅ Aumentar valor de `BRIGHTNESS` (máximo 255)
- ✅ Verificar voltaje de alimentación (debe ser 5V)

### Timing incorrecto
- ✅ Ajustar `LED_DURATION` para duración de encendido
- ✅ Ajustar `LED_INTERVAL` para velocidad de propagación
- ✅ Verificar que no haya otros delays en el código

### Efecto no fluido
- ✅ Verificar que no haya otros `delay()` largos
- ✅ El código usa timing no bloqueante con `millis()`

## Características Técnicas

### Consumo de Corriente
- **Configuración actual**: ~144mA (8 LEDs verde 30%)
- **Máximo teórico**: ~576mA (8 LEDs 100% brillo, color blanco)
- **Recomendación**: Fuente de al menos 600mA

### Rendimiento
- **Actualización**: ~100 Hz (cada 10ms)
- **Precisión timing**: ±10ms
- **Método**: Timing no bloqueante con `millis()`

## Ventajas de Esta Implementación

1. ✅ **Variables configurables** al inicio del código
2. ✅ **Timing no bloqueante** (usa millis, no delay)
3. ✅ **Efecto visual atractivo** (ola con superposición)
4. ✅ **Sin dependencias complejas** (solo Adafruit NeoPixel)
5. ✅ **Fácil personalización** sin tocar la lógica
6. ✅ **Escalable** para más LEDs o patrones complejos
7. ✅ **Depuración visual** inmediata
8. ✅ **Código limpio** con comentarios explicativos

## Próximos Pasos

Este código sirve como base para:
- Integración con sensores
- Patrones más complejos
- Sincronización con audio
- Control remoto vía WiFi/Bluetooth
- Múltiples efectos intercambiables

## Autor
Proyecto: Centinelas del Bosque - Sinfonía Biótica XR

## Licencia
Ver archivo LICENSE en la raíz del proyecto
