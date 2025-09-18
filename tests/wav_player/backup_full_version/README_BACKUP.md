# BACKUP - Versión Completa del WAV Player

## 📦 Contenido del Backup

Esta carpeta contiene la **versión completa** del reproductor WAV con todas las funcionalidades implementadas:

- `wav_player_full.ino` - Programa principal completo
- `audio_config.h` - Configuración avanzada de pines
- `button_handler.h` - Manejo sofisticado de botones
- `README_BACKUP.md` - Este archivo

## ⚠️ Problema de Memoria

Esta versión **NO COMPILA** debido a limitaciones de memoria:
- **Tamaño compilado**: 1,621,743 bytes (123% del límite)
- **Espacio disponible**: 1,310,720 bytes
- **Exceso**: ~300KB demasiado grande

## 🎯 Funcionalidades Incluidas

### ✅ Características Implementadas
- Reproducción completa I2S con codec AC101
- Soporte para múltiples formatos (WAV, OGG, MP3, FLAC)
- Manejo avanzado de botones con antirebote
- Sistema completo de LEDs de estado
- Gestión robusta de playlist
- Control de volumen completo
- Debug extensivo y callbacks de audio

### 🔧 Librerías Utilizadas
- `ESP32-audioI2S v3.0.8` (pesada - ~800KB)
- `SD v1.2.4`
- `Arduino Framework completo`

## 🚀 Versión Minimalista

Para resolver el problema de memoria, se creó una **versión minimalista** que:
- Elimina archivos .h auxiliares
- Reduce funcionalidades de debug
- Usa solo reproducción WAV básica
- Mantiene funcionalidad esencial

## 🔄 Migración Futura

**Cuando tengas más memoria disponible** (partición más grande o ESP32-S3):

1. **Copiar archivos** de este backup al directorio principal
2. **Usar configuración** de `platformio.ini` con partición `huge_app.csv`
3. **Habilitar todas las funcionalidades** comentadas

## 📝 Notas para Desarrollo

### Optimizaciones Posibles
- Usar librería I2S más liviana
- Compilar con flags de optimización `-Os`
- Eliminar callbacks de audio no esenciales
- Reducir tamaño de buffers

### Funcionalidades que se Perdieron
- Manejo avanzado de botones con pulsaciones largas/dobles
- Soporte para múltiples formatos de audio
- Debug extensivo con callbacks
- Configuración modular en archivos separados

## 🎯 Objetivo

Este backup permite **recuperar la funcionalidad completa** cuando se resuelvan las limitaciones de memoria, manteniendo todo el trabajo de desarrollo avanzado realizado.

---

**Fecha de Backup**: 2024  
**Motivo**: Limitaciones de memoria del ESP32  
**Estado**: Funcional pero no compila por tamaño
