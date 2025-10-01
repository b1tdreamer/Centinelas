# Plan Técnico: Test Simple NeoPixel - ESP32 AudioKit 2.2

## Descripción
Crear un programa de prueba simple (`.ino`) para controlar un aro LED NeoPixel de 8 píxeles conectado al ESP32 AudioKit 2.2 en el pin IO23 (GPIO23). El comportamiento será: todos los LEDs se encienden en color rojo durante 1 segundo, luego se apagan durante 1 segundo, repitiéndose indefinidamente en el loop.

## Archivos a Crear

### Nuevo Test: `tests/neopixel_simple/neopixel_simple.ino`
- **Función**: Programa de prueba minimalista para verificar funcionamiento del aro NeoPixel
- **Responsabilidades**: 
  - Inicializar librería Adafruit_NeoPixel
  - Encender/apagar todos los LEDs en rojo con pausa de 1 segundo
  - Ejecutar en loop infinito

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

### Paso 1: Configuración Inicial (`setup()`)
1. **Inicializar Serial**: Baudrate 115200 para depuración
2. **Crear objeto NeoPixel**:
   - Pin: GPIO23
   - Cantidad: 8 LEDs
   - Tipo: `NEO_GRB + NEO_KHZ800` (estándar WS2812B)
3. **Llamar `pixels.begin()`**: Inicializar librería
4. **Configurar brillo**: `pixels.setBrightness(50)` (20% del máximo para pruebas)
5. **Limpiar LEDs**: `pixels.clear()` y `pixels.show()` para estado inicial apagado

### Paso 2: Loop Principal (`loop()`)

#### Secuencia de Encendido:
```
1. Iterar por todos los 8 LEDs (índice 0-7)
2. Configurar cada LED con color rojo: pixels.setPixelColor(i, 255, 0, 0)
3. Aplicar cambios: pixels.show()
4. Esperar 1 segundo: delay(1000)
```

#### Secuencia de Apagado:
```
5. Limpiar todos los LEDs: pixels.clear()
6. Aplicar cambios: pixels.show()
7. Esperar 1 segundo: delay(1000)
8. Repetir desde el paso 1
```

## Especificaciones Técnicas

### Parámetros de NeoPixel
- **Protocolo**: WS2812B (800 KHz)
- **Formato de color**: GRB (Green-Red-Blue)
- **Resolución de color**: 8 bits por canal (0-255)
- **Timing crítico**: Requiere pulsos precisos (librería maneja automáticamente)

### Color Rojo RGB
```cpp
R = 255  // Rojo al máximo
G = 0    // Verde apagado
B = 0    // Azul apagado
```

### Brillo y Consumo
- **Brillo configurado**: 50 (de 255 máximo = ~20%)
- **Consumo aproximado**: ~100mA con 8 LEDs en rojo al 20%
- **Recomendación**: Usar alimentación USB adecuada (>500mA)

## Código Estructura Básica

### Definiciones
```cpp
#define LED_PIN       23      // GPIO23 - Pin de datos NeoPixel
#define LED_COUNT     8       // 8 LEDs en el aro
```

### Inicialización del objeto
```cpp
Adafruit_NeoPixel pixels(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
```

### Función setup()
```cpp
void setup() {
  Serial.begin(115200);
  pixels.begin();
  pixels.setBrightness(50);  // Brillo 20%
  pixels.clear();
  pixels.show();
}
```

### Función loop()
```cpp
void loop() {
  // Encender todos en rojo
  for(int i=0; i<LED_COUNT; i++) {
    pixels.setPixelColor(i, pixels.Color(255, 0, 0));
  }
  pixels.show();
  delay(1000);
  
  // Apagar todos
  pixels.clear();
  pixels.show();
  delay(1000);
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
1. ✅ Monitor serial muestra mensajes de inicio
2. ✅ Todos los 8 LEDs se encienden en rojo simultáneamente
3. ✅ LEDs permanecen encendidos exactamente 1 segundo
4. ✅ Todos los LEDs se apagan simultáneamente
5. ✅ LEDs permanecen apagados exactamente 1 segundo
6. ✅ El ciclo se repite indefinidamente

### Solución de Problemas:
- **LEDs no encienden**: Verificar alimentación 5V y conexión GND
- **LEDs parpadean erráticamente**: Agregar resistencia 470Ω en línea de datos
- **Colores incorrectos**: Cambiar `NEO_GRB` por `NEO_RGB` en definición
- **Brillo muy bajo**: Aumentar `setBrightness()` a valores mayores (máx 255)

## Ventajas de Esta Implementación
1. **Simplicidad extrema**: ~30 líneas de código funcional
2. **Test de hardware completo**: Verifica comunicación y alimentación NeoPixel
3. **Sin configuración compleja**: No requiere WiFi ni SD
4. **Inmediato**: Funciona al cargar el sketch
5. **Escalable**: Fácil modificar para más LEDs o patrones complejos
6. **Depuración visual**: Ver claramente si funciona sin monitor serial

Este plan proporciona el test más básico y efectivo para verificar que el aro NeoPixel de 8 LEDs está correctamente conectado al pin GPIO23 del ESP32 AudioKit 2.2 y responde a comandos.


