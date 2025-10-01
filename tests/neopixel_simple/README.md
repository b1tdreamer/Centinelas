# Test Simple NeoPixel - ESP32 AudioKit 2.2

## Descripción
Programa de prueba básico para verificar el correcto funcionamiento de un aro LED NeoPixel de 8 píxeles conectado al ESP32 AudioKit 2.2.

## Comportamiento
- ✅ Todos los LEDs se encienden en color **ROJO**
- ⏱️ Permanecen encendidos **1 segundo**
- ✅ Todos los LEDs se **apagan**
- ⏱️ Permanecen apagados **1 segundo**
- 🔁 El ciclo se repite **indefinidamente**

## Hardware Requerido

### Componentes
- **ESP32 AudioKit v2.2**
- **Aro NeoPixel** WS2812B de 8 LEDs (o compatible)
- **Cables jumper** para conexiones

### Conexiones

```
┌─────────────────┐         ┌──────────────────┐
│  NeoPixel Ring  │         │ ESP32 AudioKit   │
├─────────────────┤         ├──────────────────┤
│   VCC (5V)      │────────→│  5V o VIN        │
│   GND           │────────→│  GND             │
│   DIN (Data)    │────────→│  GPIO23 (IO23)   │
└─────────────────┘         └──────────────────┘
```

### Especificaciones
- **Pin de datos**: GPIO23
- **Cantidad de LEDs**: 8 píxeles
- **Voltaje**: 5V
- **Protocolo**: WS2812B (NEO_GRB + NEO_KHZ800)
- **Consumo aproximado**: ~100mA a 20% brillo

## Instalación

### 1. Instalar Librería Adafruit NeoPixel

#### Método A: Arduino IDE Library Manager
1. Abrir Arduino IDE
2. Ir a: `Sketch` → `Include Library` → `Manage Libraries...`
3. Buscar: **"Adafruit NeoPixel"**
4. Instalar la versión más reciente (1.12.0 o superior)

#### Método B: Manual
```bash
git clone https://github.com/adafruit/Adafruit_NeoPixel.git
# Copiar a la carpeta libraries de Arduino
```

### 2. Configurar Arduino IDE
- **Board**: ESP32 Dev Module (o ESP32 Audio Kit si está disponible)
- **Upload Speed**: 921600
- **CPU Frequency**: 240MHz
- **Flash Frequency**: 80MHz
- **Flash Mode**: QIO
- **Flash Size**: 4MB (32Mb)
- **Partition Scheme**: Default 4MB with spiffs
- **Port**: Seleccionar puerto COM correspondiente

### 3. Cargar el Programa
1. Abrir `neopixel_simple.ino`
2. Conectar ESP32 AudioKit por USB
3. Verificar/compilar (✓)
4. Cargar al ESP32 (→)

## Uso

### Ejecución
1. Conectar el aro NeoPixel según el diagrama
2. Alimentar el ESP32 AudioKit por USB
3. El programa iniciará automáticamente
4. Observar el patrón de encendido/apagado rojo

### Monitor Serial
Abrir el Monitor Serial (115200 baud) para ver mensajes de debug:
```
======================================
Test NeoPixel Simple - ESP32 AudioKit
======================================
Pin de datos: GPIO23
Cantidad de LEDs: 8
Brillo configurado: 50 (~20%)
======================================

✓ NeoPixel inicializado correctamente
✓ Iniciando secuencia de prueba...

► Encendiendo LEDs en ROJO...
► Apagando LEDs...

► Encendiendo LEDs en ROJO...
► Apagando LEDs...
...
```

## Solución de Problemas

### ❌ LEDs no encienden
- Verificar que el pin **VCC** esté conectado a **5V**
- Verificar que **GND** esté conectado correctamente
- Comprobar que el cable de datos esté en **GPIO23**
- Verificar alimentación USB adecuada (>500mA)

### ❌ LEDs parpadean erráticamente
- Agregar resistencia de **470Ω** en serie en la línea de datos
- Usar cable de datos más corto (<30cm)
- Agregar capacitor de **1000µF** entre VCC y GND del NeoPixel

### ❌ Colores incorrectos (no aparece rojo)
- Cambiar en el código `NEO_GRB` por `NEO_RGB`
- Algunos NeoPixels usan orden RGB en lugar de GRB

### ❌ Brillo muy bajo
- Aumentar el valor de `BRIGHTNESS` en el código (máx 255)
- Nota: Mayor brillo = mayor consumo de corriente

### ❌ Error de compilación "Adafruit_NeoPixel.h: No such file"
- Instalar la librería Adafruit NeoPixel desde Library Manager
- Reiniciar Arduino IDE después de instalar

## Personalización

### Cambiar el Color
Modificar la línea en `loop()`:
```cpp
// Rojo (actual)
pixels.setPixelColor(i, pixels.Color(255, 0, 0));

// Verde
pixels.setPixelColor(i, pixels.Color(0, 255, 0));

// Azul
pixels.setPixelColor(i, pixels.Color(0, 0, 255));

// Blanco
pixels.setPixelColor(i, pixels.Color(255, 255, 255));

// Amarillo
pixels.setPixelColor(i, pixels.Color(255, 255, 0));
```

### Cambiar la Velocidad
Modificar los valores de `delay()`:
```cpp
delay(1000);  // 1 segundo (actual)
delay(500);   // 0.5 segundos (más rápido)
delay(2000);  // 2 segundos (más lento)
```

### Cambiar el Brillo
Modificar la constante al inicio:
```cpp
#define BRIGHTNESS    50    // Actual (20%)
#define BRIGHTNESS    128   // Medio (50%)
#define BRIGHTNESS    255   // Máximo (100%)
```

## Siguiente Paso
Una vez verificado el funcionamiento, puedes extender este test para:
- ✨ Crear patrones de animación
- 🌈 Agregar efectos de arcoíris
- 🎨 Cambiar colores basados en sensores
- 🎵 Sincronizar con audio del AudioKit

## Licencia
Proyecto de código abierto - Centinelas del Bosque

## Autor
Proyecto Sinfonía Biótica XR


