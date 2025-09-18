# Plan Técnico: Reproductor WAV para ESP32 AudioKit v2.2

## Descripción

Desarrollar un programa `.ino` que permita reproducir archivos WAV a través de la salida de auriculares del ESP32 AudioKit v2.2. El sistema debe ser capaz de leer archivos WAV desde una tarjeta microSD y reproducirlos usando el codec de audio integrado AC101, con control por botones para funciones básicas de reproducción.

## Análisis de Enfoques Técnicos

### Opción Seleccionada: ESP32-audioI2S Library
**Razones:**
- Optimizada específicamente para ESP32 y codec AC101
- Manejo automático del protocolo I2S para audio de alta calidad
- Soporte nativo para archivos WAV desde tarjeta SD
- Mejor rendimiento y menor latencia que alternativas genéricas
- Configuración simplificada para AudioKit v2.2

### Alternativas Descartadas:
- **Audio Library genérica**: Requiere configuración manual completa del I2S
- **ESP32-audiokit oficial**: Dependencias pesadas y menos flexible

## Archivos y Funciones a Crear/Modificar

### Archivo Principal: `wav_player.ino` en la carpeta tests
- **Función**: Programa principal con configuración inicial y loop principal
- **Responsabilidades**: Inicialización del sistema, manejo del estado de reproducción

### Configuración PlatformIO: `platformio.ini`
- **Modificaciones**: Agregar dependencias para audio I2S y manejo de SD
- **Nuevas librerías**:
  - `schreibfaul1/ESP32-audioI2S@^3.0.8` - Librería principal de audio
  - `arduino-libraries/SD@^1.2.4` - Manejo de tarjeta SD

### Estructura de Directorios:
```
wav_player/
├── wav_player.ino          # Programa principal
├── platformio.ini          # Configuración de dependencias
├── audio_config.h          # Configuración de pines y parámetros de audio
├── button_handler.h        # Manejo de botones del AudioKit
└── data/                   # Archivos WAV de ejemplo
    └── example.wav
```

## Algoritmo de Reproducción WAV

### Paso 1: Inicialización del Sistema
1. **Configurar pines I2S** según especificaciones del AudioKit v2.2:
   - BCLK (Bit Clock): GPIO27
   - LRC (Word Select): GPIO26  
   - DOUT (Data Out): GPIO25
2. **Inicializar tarjeta SD** en pin CS GPIO5
3. **Configurar codec AC101** para salida de auriculares
4. **Inicializar objeto Audio** con configuración I2S

### Paso 2: Manejo de Archivos WAV
1. **Validar formato WAV**: Verificar header y formato compatible
2. **Cargar archivo desde SD**: Streaming para archivos grandes
3. **Configurar parámetros de audio**: Sample rate, bits por muestra, canales

### Paso 3: Control de Reproducción
1. **Sistema de estados**: STOPPED, PLAYING, PAUSED
2. **Buffer de audio**: Manejo de streaming continuo
3. **Control de volumen**: Ajuste por potenciómetro o botones

### Paso 4: Interfaz de Usuario
1. **Botones del AudioKit**: 
   - Botón 1 (GPIO36): Play/Pause
   - Botón 2 (GPIO13): Stop
   - Botón 3 (GPIO5): Archivo anterior
   - Botón 4 (GPIO16): Archivo siguiente
   - Botón 5 (GPIO18): Volumen -
   - Botón 6 (GPIO23): Volumen +
2. **LEDs de estado**:
   - LED D1 (GPIO2): Sistema encendido
   - LED D3 (GPIO14): Reproduciendo
   - LED D4 (GPIO15): Error/pausa

## Configuración de Hardware

### Pines I2S para AudioKit v2.2:
- **MCLK (Master Clock)**: GPIO0
- **BCLK (Bit Clock)**: GPIO27
- **LRC (Left/Right Clock)**: GPIO26
- **DOUT (Data Output)**: GPIO25
- **DIN (Data Input)**: GPIO35 (para grabación, opcional)

### Tarjeta SD:
- **CS (Chip Select)**: GPIO5
- **MOSI**: GPIO23
- **MISO**: GPIO19
- **SCK**: GPIO18

### Configuración del Codec AC101:
- **I2C SDA**: GPIO33
- **I2C SCL**: GPIO32
- **Dirección I2C**: 0x1A

## Fases de Implementación

### Fase 1: Configuración Base del Audio
- Configurar comunicación I2S con codec AC101
- Implementar inicialización de tarjeta SD
- Crear estructura básica del programa
- Verificar salida de audio con tono de prueba

### Fase 2: Reproducción WAV Básica
- Implementar lectura de archivos WAV desde SD
- Desarrollar parser de header WAV
- Crear sistema de streaming de audio
- Implementar reproducción básica sin controles

### Fase 3: Sistema de Control
- Agregar manejo de botones del AudioKit
- Implementar funciones Play/Pause/Stop
- Crear navegación entre archivos
- Agregar control de volumen

### Fase 4: Funcionalidades Avanzadas
- Implementar lista de reproducción
- Agregar soporte para múltiples formatos de WAV
- Crear indicadores visuales con LEDs
- Optimizar rendimiento y gestión de memoria

## Especificaciones de Archivos WAV Soportados

### Formatos Recomendados:
- **Frecuencia de muestreo**: 8kHz, 16kHz, 22kHz, 44.1kHz
- **Resolución**: 8-bit, 16-bit
- **Canales**: Mono (recomendado), Estéreo
- **Formato**: PCM sin compresión
- **Tamaño máximo**: Limitado por capacidad de tarjeta SD

### Limitaciones del ESP32:
- Memoria RAM limitada para buffers grandes
- Velocidad de lectura SD como cuello de botella
- Procesamiento en tiempo real requerido

## Configuraciones de PlatformIO

### Dependencias Principales:
```ini
lib_deps = 
    schreibfaul1/ESP32-audioI2S@^3.0.8
    arduino-libraries/SD@^1.2.4
    adafruit/Adafruit NeoPixel@^1.12.0
```

### Configuración de Particiones:
- Usar partición `huge_app.csv` para mayor espacio de programa
- Reservar memoria suficiente para buffers de audio

### Parámetros de Compilación:
- Optimización para velocidad (`-O2`)
- Configuración de frecuencia de CPU a 240MHz para mejor rendimiento de audio

## Consideraciones de Rendimiento

### Optimizaciones Necesarias:
1. **Buffer circular** para streaming continuo
2. **Lectura por bloques** desde tarjeta SD
3. **Gestión de memoria** para archivos grandes
4. **Prioridad de tareas** para audio en tiempo real

### Limitaciones Conocidas:
- Archivos WAV muy grandes pueden requerir streaming
- Latencia mínima determinada por tamaño de buffer
- Velocidad de tarjeta SD afecta calidad de reproducción

Este plan proporciona una base sólida para implementar un reproductor WAV funcional en el ESP32 AudioKit v2.2, manteniendo compatibilidad con el ecosistema existente del proyecto Centinelas del Bosque.
