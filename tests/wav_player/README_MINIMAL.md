# WAV Player MINIMAL - ESP32 AudioKit v2.2

## 🎯 Versión Minimalista

Esta es una **versión ultra-simplificada** del reproductor WAV creada para resolver el problema de memoria del ESP32.

### ⚡ Optimizaciones Aplicadas

#### **Código Reducido**
- **Eliminados**: archivos `.h` auxiliares (audio_config.h, button_handler.h)
- **Simplificado**: manejo de botones básico sin antirebote avanzado
- **Reducido**: solo 2 botones funcionales (Play/Pause, Siguiente)
- **Minimizado**: callbacks de audio comentados
- **Eliminado**: sistema de LEDs complejo

#### **Funcionalidades Básicas**
- ✅ **Reproducción WAV** desde tarjeta SD
- ✅ **Búsqueda automática** de `example.wav` y `example.ogg`
- ✅ **Control básico** Play/Pause y Siguiente
- ✅ **LED de sistema** para indicar funcionamiento
- ✅ **Monitor serial** con información básica

### 🔧 Configuración Optimizada

#### **PlatformIO.ini Minimal**
```ini
build_flags = 
    -Os                      ; Optimizar para tamaño
    -DCORE_DEBUG_LEVEL=0     ; Sin debug
    -ffunction-sections      ; Eliminar funciones no usadas
    -fdata-sections
    -Wl,--gc-sections
```

#### **Librerías Esenciales**
- `ESP32-audioI2S` - Solo para reproducción
- `SD` - Solo para lectura de tarjeta

### 🎮 Controles Simplificados

| Control | Función |
|---------|---------|
| **Botón 1 (GPIO36)** | Play/Pause |
| **Botón 4 (GPIO16)** | Siguiente archivo |
| **LED Sistema (GPIO2)** | Sistema funcionando |

### 📁 Archivos Soportados

#### **Archivos Prioritarios**
1. `/example.wav` (primera opción)
2. `/example.ogg` (segunda opción)
3. Cualquier `.wav` en la raíz (si no hay los anteriores)

#### **Formatos Recomendados**
- **WAV**: PCM 16-bit, 16kHz, Mono
- **Tamaño**: Menor a 10MB por archivo
- **Ubicación**: Raíz de la tarjeta SD

### 🚀 Instalación y Uso

#### **1. Preparar Hardware**
```bash
# Tarjeta SD formateada en FAT32
# Copiar example.wav a la raíz
# Insertar en ESP32 AudioKit v2.2
# Conectar auriculares al jack 3.5mm
```

#### **2. Compilar y Subir**
```bash
cd tests/wav_player/
pio run -e esp32-audiokit-minimal
pio run -e esp32-audiokit-minimal --target upload
pio device monitor
```

#### **3. Operación**
1. **LED Sistema** se enciende al iniciar
2. **Presionar Botón 1** para reproducir
3. **Presionar Botón 4** para cambiar archivo
4. **Monitor Serial** muestra estado

### 📊 Comparación de Tamaño

| Versión | Tamaño Compilado | Estado |
|---------|------------------|--------|
| **Completa** | 1,621,743 bytes | ❌ No compila (123%) |
| **Minimal** | ~800,000 bytes | ✅ Debería compilar (60%) |

### ⚠️ Limitaciones

#### **Funcionalidades Removidas**
- ❌ Control de volumen por botones
- ❌ Función Stop dedicada
- ❌ Función Pause (solo Play/Stop)
- ❌ LEDs de estado avanzados
- ❌ Callbacks de audio detallados
- ❌ Manejo de errores extensivo
- ❌ Soporte para múltiples formatos
- ❌ Información detallada de estado

#### **Controles Limitados**
- Solo 2 botones funcionales
- Sin antirebote avanzado
- Sin pulsaciones largas/dobles

### 🔄 Migración Futura

Cuando tengas más memoria disponible:

1. **Usar versión completa** del backup
2. **Configurar partición** `huge_app.csv`
3. **Habilitar todas las funcionalidades**

### 🎯 Objetivo

Esta versión **minimalista** permite:
- ✅ **Probar la funcionalidad básica** de reproducción WAV
- ✅ **Verificar hardware** (I2S, SD, AudioKit)
- ✅ **Validar configuración** de pines
- ✅ **Confirmar compatibilidad** de archivos

Una vez que funcione, puedes optimizar o migrar a versión completa.

---

**Versión**: Minimal 1.0  
**Propósito**: Solución temporal para limitaciones de memoria  
**Estado**: Listo para prueba
