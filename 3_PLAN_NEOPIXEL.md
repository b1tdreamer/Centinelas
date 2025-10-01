# Plan Técnico: Test Secuencial NeoPixel - ESP32 AudioKit 2.2

## Descripción
Crear un programa de prueba simple (`.ino`) para controlar un aro LED NeoPixel de 8 píxeles conectado al ESP32 AudioKit 2.2 en el pin IO23 (GPIO23). El comportamiento será: cada LED se enciende secuencialmente en color verde durante 3 segundos, con un intervalo de 700ms entre el inicio de cada LED consecutivo. Esto crea un efecto de "ola" con superposición donde múltiples LEDs pueden estar encendidos simultáneamente. El ciclo se repite indefinidamente.

## Archivos a Crear

### Nuevo Test: `tests/neopixel_simple/neopixel_simple.ino`
- **Función**: Programa de prueba secuencial para verificar funcionamiento del aro NeoPixel
- **Responsabilidades**: 
  - Inicializar librería Adafruit_NeoPixel
  - Encender cada LED en verde de forma secuencial con superposición
  - Gestionar timing independiente para cada LED (encendido 3s, intervalo 700ms)
  - Ejecutar en loop infinito con variables configurables

### Archivo de Configuración PlatformIO (opcional): `tests/neopixel_simple/platformio.ini`
- **Función**: Definir dependencias específicas para compilar con PlatformIO
- **Dependencias**:
  - `adafruit/Adafruit NeoPixel@^1.12.0`
  
### Documentación: `tests/neopixel_simple/README.md`
- **Función**: Instrucciones de uso y conexión del hardware
- **Contenido**: Especificaciones del aro LED, pin de conexión, voltaje

## Configuración de Hardware

### Aro LED NeoPixel
- **Modelo**: NeoPixel WS2812B (compatible)
- **Cantidad de LEDs**: 8 píxeles
- **Pin de datos**: GPIO23 (IO23) del ESP32 AudioKit 2.2
- **Voltaje**: 5V (alimentación externa recomendada)
- **Corriente máxima**: ~480mA (60mA x 8 LEDs al 100% brillo)

### Conexiones
```
NeoPixel Ring ─── ESP32 AudioKit 2.2
    VCC (5V)  ─── 5V o VIN
    GND       ─── GND
    DIN       ─── GPIO23
```

## Algoritmo de Implementación

### Variables Configurables (al inicio del programa)
```cpp
// Parámetros de temporización
const unsigned long LED_DURATION = 3000;      // Tiempo que cada LED permanece encendido (ms)
const unsigned long LED_INTERVAL = 1000;       // Intervalo entre inicio de LEDs consecutivos (ms)

// Parámetros de apariencia
const uint8_t BRIGHTNESS = 76;                // Brillo máximo 30% de 255 = 76
const uint8_t COLOR_R = 0;                    // Rojo (para verde = 0)
const uint8_t COLOR_G = 255;                  // Verde (para verde = 255)
const uint8_t COLOR_B = 0;                    // Azul (para verde = 0)
```

### Paso 1: Configuración Inicial (`setup()`)
1. **Inicializar Serial**: Baudrate 115200 para depuración
2. **Crear objeto NeoPixel**:
   - Pin: GPIO23
   - Cantidad: 8 LEDs
   - Tipo: `NEO_GRB + NEO_KHZ800` (estándar WS2812B)
3. **Llamar `pixels.begin()`**: Inicializar librería
4. **Configurar brillo**: `pixels.setBrightness(BRIGHTNESS)` (30% del máximo)
5. **Limpiar LEDs**: `pixels.clear()` y `pixels.show()` para estado inicial apagado
6. **Inicializar array de tiempos**: Array de 8 elementos para tracking del tiempo de encendido de cada LED

### Paso 2: Loop Principal (`loop()`)

#### Gestión de Timing No Bloqueante:
```
1. Obtener tiempo actual: millis()
2. Para cada LED (índice 0-7):
   a. Calcular tiempo de inicio de este LED basado en ciclo actual
   b. Calcular tiempo de apagado (inicio + LED_DURATION)
   c. Si tiempo actual está entre inicio y apagado:
      - Encender LED con color verde
   d. Si no:
      - Apagar LED
3. Aplicar cambios: pixels.show()
4. No usar delay() - permite ejecución fluida
```

#### Cálculo del Ciclo:
```
- Tiempo total del ciclo = LED_COUNT * LED_INTERVAL
- Posición en el ciclo = millis() % tiempoCicloTotal
- Tiempo de inicio LED[i] = i * LED_INTERVAL
```

## Especificaciones Técnicas

### Parámetros de NeoPixel
- **Protocolo**: WS2812B (800 KHz)
- **Formato de color**: GRB (Green-Red-Blue)
- **Resolución de color**: 8 bits por canal (0-255)
- **Timing crítico**: Requiere pulsos precisos (librería maneja automáticamente)

### Color Verde RGB
```cpp
R = 0    // Rojo apagado
G = 255  // Verde al máximo
B = 0    // Azul apagado
```

### Brillo y Consumo
- **Brillo configurado**: 76 (de 255 máximo = 30%)
- **Consumo aproximado**: ~144mA con 8 LEDs en verde al 30%
- **Consumo pico**: ~576mA si todos los LEDs están encendidos simultáneamente
- **Recomendación**: Usar alimentación USB adecuada (>600mA) o fuente externa

## Código Estructura Básica

### Definiciones Hardware
```cpp
#define LED_PIN       23      // GPIO23 - Pin de datos NeoPixel
#define LED_COUNT     8       // 8 LEDs en el aro
```

### Variables Configurables (FÁCIL MODIFICACIÓN)
```cpp
// ============================================
// CONFIGURACIÓN - Modificar aquí los valores
// ============================================

// Temporización
const unsigned long LED_DURATION = 3000;      // Duración encendido (ms)
const unsigned long LED_INTERVAL = 700;       // Intervalo entre LEDs (ms)

// Apariencia
const uint8_t BRIGHTNESS = 76;                // Brillo 0-255 (76 = 30%)
const uint8_t COLOR_R = 0;                    // Componente rojo
const uint8_t COLOR_G = 255;                  // Componente verde
const uint8_t COLOR_B = 0;                    // Componente azul

// ============================================
```

### Inicialización del objeto
```cpp
Adafruit_NeoPixel pixels(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
```

### Función setup()
```cpp
void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando NeoPixel Ring - Secuencia Verde");
  
  pixels.begin();
  pixels.setBrightness(BRIGHTNESS);
  pixels.clear();
  pixels.show();
  
  Serial.print("Duración LED: "); Serial.print(LED_DURATION); Serial.println(" ms");
  Serial.print("Intervalo: "); Serial.print(LED_INTERVAL); Serial.println(" ms");
  Serial.print("Ciclo total: "); 
  Serial.print(LED_COUNT * LED_INTERVAL); Serial.println(" ms");
}
```

### Función loop()
```cpp
void loop() {
  unsigned long currentTime = millis();
  unsigned long cycleTime = LED_COUNT * LED_INTERVAL;
  unsigned long positionInCycle = currentTime % cycleTime;
  
  // Actualizar estado de cada LED
  for(int i = 0; i < LED_COUNT; i++) {
    unsigned long ledStartTime = i * LED_INTERVAL;
    unsigned long ledEndTime = ledStartTime + LED_DURATION;
    
    // Verificar si este LED debe estar encendido
    bool shouldBeOn = false;
    
    if (LED_DURATION > cycleTime) {
      // Si la duración > ciclo, manejar superposición de vueltas
      shouldBeOn = (positionInCycle >= ledStartTime) || 
                   (positionInCycle < (ledEndTime % cycleTime));
    } else {
      // Caso normal: duración < ciclo
      shouldBeOn = (positionInCycle >= ledStartTime && 
                    positionInCycle < ledEndTime);
    }
    
    if (shouldBeOn) {
      pixels.setPixelColor(i, pixels.Color(COLOR_R, COLOR_G, COLOR_B));
    } else {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    }
  }
  
  pixels.show();
  delay(10);  // Pequeño delay para estabilidad (no afecta timing)
}
```

## Dependencias

### Librería Principal
- **Nombre**: Adafruit NeoPixel
- **Versión**: 1.12.0 o superior
- **Repositorio**: https://github.com/adafruit/Adafruit_NeoPixel
- **Instalación Arduino IDE**: 
  - Tools → Manage Libraries
  - Buscar "Adafruit NeoPixel"
  - Instalar versión más reciente

### Sin Dependencias Adicionales
- No requiere WiFi
- No requiere tarjeta SD
- No requiere librerías de audio
- Compatible con Arduino IDE y PlatformIO

## Verificación de Funcionamiento

### Test Exitoso:
1. ✅ Monitor serial muestra mensajes de inicio y parámetros de configuración
2. ✅ Los LEDs se encienden secuencialmente en color verde
3. ✅ Cada LED permanece encendido durante 3 segundos
4. ✅ Un nuevo LED comienza a encenderse cada 700ms
5. ✅ Múltiples LEDs (4-5 aprox.) están encendidos simultáneamente creando efecto "ola"
6. ✅ El patrón vuelve al LED 0 después de completar el aro (cada 5.6 segundos)
7. ✅ El ciclo se repite indefinidamente de forma suave

### Comportamiento Esperado:
- **Tiempo del ciclo completo**: 8 LEDs × 700ms = 5600ms (5.6 segundos)
- **LEDs simultáneos**: Con 3000ms de duración y 700ms de intervalo, aproximadamente 4-5 LEDs estarán encendidos al mismo tiempo
- **Patrón visual**: Efecto de "ola verde" girando alrededor del aro

### Solución de Problemas:
- **LEDs no encienden**: Verificar alimentación 5V y conexión GND
- **LEDs parpadean erráticamente**: Agregar resistencia 470Ω en línea de datos
- **Colores incorrectos**: Cambiar `NEO_GRB` por `NEO_RGB` en definición
- **Brillo muy bajo**: Aumentar valor de `BRIGHTNESS` (máx 255)
- **Timing incorrecto**: Ajustar `LED_DURATION` o `LED_INTERVAL` en variables configurables
- **Efecto no fluido**: Verificar que no haya otros `delay()` largos en el código

## Ventajas de Esta Implementación
1. **Variables configurables**: Fácil ajuste de timing, color y brillo sin tocar la lógica
2. **Timing no bloqueante**: Usa `millis()` en lugar de `delay()` para mayor flexibilidad
3. **Efecto visual atractivo**: Patrón de "ola" con múltiples LEDs encendidos simultáneamente
4. **Test de hardware completo**: Verifica comunicación, timing preciso y alimentación NeoPixel
5. **Sin configuración compleja**: No requiere WiFi, SD ni librerías adicionales
6. **Escalable**: Fácil modificar número de LEDs, duración o intervalos
7. **Depuración visual**: Efecto claramente visible para validar funcionamiento
8. **Código limpio**: Estructura modular con comentarios explicativos

## Personalización Rápida

### Ejemplos de Ajustes Comunes:

**Efecto más rápido (ola rápida):**
```cpp
const unsigned long LED_DURATION = 1500;  // 1.5 segundos
const unsigned long LED_INTERVAL = 400;   // 400ms entre LEDs
```

**Efecto más lento (ola lenta):**
```cpp
const unsigned long LED_DURATION = 5000;  // 5 segundos
const unsigned long LED_INTERVAL = 1000;  // 1 segundo entre LEDs
```

**Color diferente (azul en lugar de verde):**
```cpp
const uint8_t COLOR_R = 0;
const uint8_t COLOR_G = 0;
const uint8_t COLOR_B = 255;  // Azul
```

**Mayor/menor brillo:**
```cpp
const uint8_t BRIGHTNESS = 128;  // 50% de brillo
```

Este plan proporciona un test secuencial efectivo y visualmente atractivo para verificar que el aro NeoPixel de 8 LEDs está correctamente conectado al pin GPIO23 del ESP32 AudioKit 2.2 y permite una personalización rápida y sencilla de todos los parámetros.


