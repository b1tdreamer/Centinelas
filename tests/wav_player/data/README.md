# Carpeta de Archivos de Audio

Esta carpeta está destinada para almacenar archivos de audio de ejemplo que pueden ser copiados a la tarjeta SD.

## 📁 Archivos Recomendados

### Archivos Objetivo Prioritarios
- `example.wav` - Archivo principal que buscará el reproductor
- `example.ogg` - Archivo alternativo en formato OGG

### Especificaciones Recomendadas

#### Para Archivos WAV
- **Formato**: WAV PCM (sin compresión)
- **Frecuencia de muestreo**: 16kHz o 22kHz (recomendado)
- **Resolución**: 16-bit
- **Canales**: Mono (para mejor rendimiento)
- **Duración**: 10-60 segundos (para pruebas)

#### Para Archivos OGG
- **Formato**: OGG Vorbis
- **Bitrate**: 96-128 kbps
- **Frecuencia**: 16kHz-44.1kHz
- **Canales**: Mono o Estéreo

## 🛠️ Cómo Preparar Archivos de Audio

### Usando Audacity (Gratuito)
1. **Abrir** tu archivo de audio en Audacity
2. **Convertir a mono**: Tracks → Mix → Mix and Render to New Track
3. **Cambiar frecuencia**: Tracks → Resample... → 16000 Hz
4. **Exportar**: File → Export → Export as WAV
   - Format: WAV (Microsoft)
   - Encoding: Signed 16-bit PCM

### Usando FFmpeg (Línea de comandos)
```bash
# Convertir a WAV 16kHz mono
ffmpeg -i input.mp3 -ar 16000 -ac 1 -acodec pcm_s16le example.wav

# Convertir a OGG 96kbps mono
ffmpeg -i input.mp3 -ar 16000 -ac 1 -b:a 96k example.ogg
```

## 📋 Lista de Verificación

Antes de copiar archivos a la tarjeta SD, verificar:

- [ ] **Nombre correcto**: `example.wav` o `example.ogg`
- [ ] **Formato válido**: WAV PCM o OGG Vorbis
- [ ] **Tamaño razonable**: Menor a 10MB por archivo
- [ ] **Duración apropiada**: 10-60 segundos para pruebas
- [ ] **Sin caracteres especiales** en el nombre del archivo

## 🎵 Archivos de Ejemplo Sugeridos

### Sonidos de Prueba
- **Tono de 440Hz** (La4) - Para verificar salida de audio
- **Sweep de frecuencias** - Para probar rango de audio
- **Música instrumental** - Para probar calidad de reproducción
- **Sonidos de la naturaleza** - Compatible con el tema del proyecto

### Fuentes de Audio Libre
- [Freesound.org](https://freesound.org) - Sonidos libres
- [Zapsplat](https://zapsplat.com) - Efectos de sonido
- [Adobe Audition Samples](https://adobe.com) - Muestras de audio

## ⚠️ Notas Importantes

### Limitaciones del ESP32
- **Memoria limitada**: Archivos muy grandes pueden causar problemas
- **CPU limitada**: Formatos comprimidos requieren más procesamiento
- **Velocidad SD**: Tarjetas lentas pueden causar interrupciones

### Recomendaciones
- **Probar primero** con archivos pequeños (1-2MB)
- **Usar mono** cuando sea posible para mejor rendimiento
- **Evitar archivos** con metadatos complejos
- **Mantener nombres simples** sin espacios ni caracteres especiales

## 🔄 Proceso de Copia a SD

1. **Formatear** tarjeta SD en FAT32
2. **Copiar** archivos directamente a la raíz (no en carpetas)
3. **Verificar** que los archivos son accesibles
4. **Insertar** tarjeta en el ESP32 AudioKit v2.2
5. **Ejecutar** el programa reproductor

---

**Nota**: Esta carpeta `data/` es solo para referencia y organización del proyecto. Los archivos reales deben copiarse a la tarjeta SD física.
