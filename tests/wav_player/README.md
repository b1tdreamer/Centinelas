# WAV Player para ESP32 AudioKit v2.2

## Descripción

Reproductor de archivos de audio WAV/OGG para ESP32 AudioKit v2.2 con codec AC101. Este programa permite reproducir archivos de audio desde una tarjeta microSD a través de la salida de auriculares del AudioKit, con control completo mediante los botones integrados.

## 🎯 Características

- ✅ **Reproducción de audio de alta calidad** mediante I2S y codec AC101
- ✅ **Soporte para múltiples formatos**: WAV, OGG, MP3, FLAC
- ✅ **Control por botones integrados** del AudioKit v2.2
- ✅ **Gestión automática de playlist** desde tarjeta SD
- ✅ **Control de volumen** en 22 niveles (0-21)
- ✅ **Indicadores LED** de estado del sistema
- ✅ **Detección automática** de archivos `example.wav` y `example.ogg`

## 🔧 Hardware Requerido

### Componentes Principales
- **ESP32 AudioKit v2.2** con codec AC101
- **Tarjeta microSD** (formateada en FAT32)
- **Auriculares** con conector de 3.5mm

### Pines Utilizados (Configuración Automática)
- **I2S Audio**: GPIO25 (DOUT), GPIO27 (BCLK), GPIO26 (LRC)
- **Tarjeta SD**: GPIO5 (CS), GPIO23 (MOSI), GPIO19 (MISO), GPIO18 (SCK)
- **Botones**: GPIO36 (Botón 1), GPIO13 (Botón 2), GPIO16 (Botón 4)
- **LEDs**: GPIO2 (Sistema), GPIO14 (Reproduciendo), GPIO15 (Error)

## 📁 Preparación de la Tarjeta SD

### Formato y Estructura
1. **Formatear** la tarjeta microSD en **FAT32**
2. **Copiar archivos** de audio en la **raíz** de la tarjeta
3. **Archivos recomendados**:
   - `example.wav` (prioritario)
   - `example.ogg` (alternativo)

### Especificaciones de Audio Soportadas
- **Formatos**: WAV (PCM), OGG Vorbis, MP3, FLAC
- **Frecuencia de muestreo**: 8kHz - 44.1kHz
- **Resolución**: 8-bit, 16-bit
- **Canales**: Mono (recomendado), Estéreo
- **Tamaño máximo**: Limitado por capacidad de tarjeta SD

## 🎮 Controles

### Botones del AudioKit v2.2
| Botón | GPIO | Función | Estado |
|-------|------|---------|--------|
| **Botón 1** | 36 | Play/Pause | ✅ Disponible |
| **Botón 2** | 13 | Stop | ✅ Disponible |
| **Botón 3** | 5 | Anterior | ⚠️ Compartido con SD |
| **Botón 4** | 16 | Siguiente | ✅ Disponible |
| **Botón 5** | 18 | Volumen - | ⚠️ Compartido con SPI |
| **Botón 6** | 23 | Volumen + | ⚠️ Compartido con SPI |

### Funciones Disponibles
- **Play/Pause**: Iniciar o pausar reproducción
- **Stop**: Detener reproducción completamente
- **Siguiente**: Avanzar al siguiente archivo de la playlist
- **Control de volumen**: Ajustar nivel de 0 a 21

## 💡 Indicadores LED

### LEDs de Estado
- **LED Sistema (GPIO2)**: Encendido = Sistema iniciado correctamente
- **LED Reproduciendo (GPIO14)**: Encendido = Audio reproduciéndose
- **LED Error (GPIO15)**: Encendido = Error o pausa

### Códigos de Estado
- **Solo LED Sistema**: Sistema listo, no hay reproducción
- **Sistema + Reproduciendo**: Reproducción activa
- **Sistema + Error**: Error en archivo o tarjeta SD

## 🚀 Instalación y Uso

### 1. Preparar el Entorno
```bash
# Clonar proyecto
cd tests/wav_player/

# Instalar dependencias (PlatformIO)
pio lib install
```

### 2. Configurar Hardware
1. **Insertar tarjeta microSD** con archivos de audio
2. **Conectar auriculares** al jack de 3.5mm
3. **Conectar ESP32 AudioKit** via USB

### 3. Compilar y Subir
```bash
# Compilar proyecto
pio run

# Subir a ESP32
pio run --target upload

# Monitor serial
pio device monitor
```

### 4. Operación
1. **Encender** el AudioKit (LED Sistema debe encender)
2. **Presionar Botón 1** para iniciar reproducción
3. **Usar Botón 4** para cambiar entre archivos
4. **Presionar Botón 2** para detener

## 📊 Monitor Serial

### Información Mostrada
- Estado del sistema y inicialización
- Archivos encontrados en la tarjeta SD
- Estado actual de reproducción
- Información de audio (bitrate, formato)
- Uso de memoria del ESP32

### Ejemplo de Salida
```
=== WAV Player v1.0 - ESP32 AudioKit v2.2 ===
Inicializando tarjeta SD...
Tipo de tarjeta SD: SDHC
Tamaño de tarjeta SD: 8192MB
Escaneando archivos de audio...
Encontrado: /example.wav
Total de archivos encontrados: 1
Inicializando sistema de audio I2S...
Audio I2S inicializado correctamente
Sistema iniciado correctamente
```

## ⚙️ Configuración Avanzada

### Modificar Volumen Inicial
En `wav_player.ino`, cambiar:
```cpp
int currentVolume = 12;  // Valor de 0 a 21
```

### Agregar Más Formatos de Archivo
En `audio_config.h`, modificar:
```cpp
#define SUPPORTED_EXTENSIONS {".wav", ".ogg", ".mp3", ".flac", ".aac"}
```

### Ajustar Buffer de Audio
En `platformio.ini`, ajustar:
```ini
build_flags = 
    -DAUDIO_BUFFER_SIZE=2048  ; Aumentar para mejor calidad
```

## 🔍 Solución de Problemas

### Problemas Comunes

#### No se Detecta la Tarjeta SD
- **Verificar formato**: Debe ser FAT32
- **Verificar conexiones**: Revisar pines SPI
- **Probar otra tarjeta**: Algunas tarjetas no son compatibles

#### No Hay Sonido en Auriculares
- **Verificar conexión**: Jack de 3.5mm bien conectado
- **Verificar volumen**: Probar aumentar volumen
- **Verificar archivo**: Asegurar formato compatible

#### Botones No Responden
- **Verificar pines**: Algunos botones comparten pines con SD/SPI
- **Verificar conexiones**: Hardware del AudioKit
- **Monitor serial**: Revisar mensajes de debug

#### Archivo No Se Reproduce
- **Verificar formato**: WAV PCM recomendado
- **Verificar nombre**: `example.wav` o `example.ogg`
- **Verificar tamaño**: Archivos muy grandes pueden fallar

### Debug Avanzado

#### Habilitar Debug Detallado
En `audio_config.h`:
```cpp
#define DEBUG_AUDIO_INFO      true
#define DEBUG_SD_OPERATIONS   true
#define DEBUG_MEMORY_USAGE    true
```

#### Monitor de Memoria
El sistema muestra automáticamente el uso de memoria cada 10 segundos.

## 📋 Especificaciones Técnicas

### Rendimiento
- **CPU**: ESP32 a 240MHz
- **Memoria**: Optimizado para uso eficiente de RAM
- **Audio**: I2S de 16-bit a hasta 44.1kHz
- **Storage**: Soporte para tarjetas SD hasta 32GB

### Limitaciones
- **Máximo 20 archivos** en playlist
- **Archivos grandes** pueden requerir buffer mayor
- **Algunos botones** comparten pines con SD/SPI
- **Formatos comprimidos** requieren más CPU

## 🛠️ Desarrollo

### Estructura del Código
```
wav_player/
├── wav_player.ino       # Programa principal
├── platformio.ini       # Configuración del proyecto
├── audio_config.h       # Configuración de pines
├── button_handler.h     # Manejo de botones
└── README.md           # Este archivo
```

### Librerías Utilizadas
- **ESP32-audioI2S v3.0.8**: Reproducción de audio I2S
- **SD v1.2.4**: Manejo de tarjeta SD
- **Arduino Framework**: Base del ESP32

### Contribuir
1. Fork del repositorio
2. Crear rama de feature
3. Implementar mejoras
4. Crear pull request

## 📜 Licencia

Este proyecto forma parte del ecosistema **Centinelas del Bosque** y mantiene compatibilidad con las configuraciones existentes del proyecto principal.

## 🔗 Enlaces Relacionados

- [Proyecto Principal: Centinelas del Bosque](../../README.md)
- [Documentación ESP32 AudioKit v2.2](https://github.com/espressif/esp-dev-kits)
- [Librería ESP32-audioI2S](https://github.com/schreibfaul1/ESP32-audioI2S)

---

**Versión**: 1.0  
**Fecha**: 2024  
**Compatibilidad**: ESP32 AudioKit v2.2 con codec AC101
