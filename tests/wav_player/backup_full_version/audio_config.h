/*
 * audio_config.h - Configuración de pines y parámetros de audio
 * ESP32 AudioKit v2.2 con codec AC101
 * BACKUP DE LA VERSIÓN COMPLETA
 */

#ifndef AUDIO_CONFIG_H
#define AUDIO_CONFIG_H

// ===============================================
// CONFIGURACIÓN DE PINES I2S - ESP32 AudioKit v2.2
// ===============================================

// Pines I2S para comunicación con codec AC101
#define I2S_MCLK      0     // Master Clock
#define I2S_BCLK      27    // Bit Clock (Serial Clock)
#define I2S_LRC       26    // Left/Right Clock (Word Select)
#define I2S_DOUT      25    // Data Output (hacia auriculares)
#define I2S_DIN       35    // Data Input (desde micrófono, opcional)

// ===============================================
// CONFIGURACIÓN DE TARJETA SD
// ===============================================

// Pines SPI para tarjeta microSD
#define SD_CS         5     // Chip Select
#define SPI_MOSI      23    // Master Out Slave In
#define SPI_MISO      19    // Master In Slave Out
#define SPI_SCK       18    // Serial Clock

// ===============================================
// CONFIGURACIÓN DEL CODEC AC101
// ===============================================

// Pines I2C para control del codec
#define I2C_SDA       33    // Serial Data
#define I2C_SCL       32    // Serial Clock
#define AC101_ADDR    0x1A  // Dirección I2C del codec AC101

// ===============================================
// CONFIGURACIÓN DE BOTONES - AudioKit
// ===============================================

// Botones integrados en el AudioKit v2.2
#define BUTTON_1      36    // KEY1 - Play/Pause (ADC)
#define BUTTON_2      13    // KEY2 - Stop (comparte con SD DATA3)
#define BUTTON_3      5     // KEY3 - Anterior (comparte con SD CS)
#define BUTTON_4      16    // KEY4 - Siguiente
#define BUTTON_5      18    // KEY5 - Volumen - (comparte con SPI SCK)
#define BUTTON_6      23    // KEY6 - Volumen + (comparte con SPI MOSI)

// Umbrales para botones analógicos
#define BUTTON_ADC_THRESHOLD  100   // Umbral para detectar pulsación en ADC

// ===============================================
// CONFIGURACIÓN DE LEDS DE ESTADO
// ===============================================

// LEDs integrados en el AudioKit
#define LED_SYSTEM    2     // LED D1 - Sistema encendido (Verde)
#define LED_PLAYING   14    // LED D3 - Reproduciendo (Verde/Rojo)
#define LED_ERROR     15    // LED D4 - Error/Pausa (Rojo)

// LEDs alternativos si los integrados no funcionan
#define LED_ALT1      16    // GPIO16 disponible
#define LED_ALT2      17    // GPIO17 disponible
#define LED_ALT3      4     // GPIO4 disponible

// ===============================================
// PARÁMETROS DE AUDIO
// ===============================================

// Configuración de volumen
#define VOLUME_MIN    0     // Volumen mínimo
#define VOLUME_MAX    21    // Volumen máximo
#define VOLUME_DEFAULT 12   // Volumen inicial

// Configuración de buffer de audio
#define AUDIO_BUFFER_SIZE 1024  // Tamaño del buffer en bytes

// Frecuencias de muestreo soportadas
#define SAMPLE_RATE_8K    8000
#define SAMPLE_RATE_16K   16000
#define SAMPLE_RATE_22K   22050
#define SAMPLE_RATE_44K   44100

// ===============================================
// CONFIGURACIÓN DE ARCHIVOS SOPORTADOS
// ===============================================

// Extensiones de archivo soportadas
#define SUPPORTED_EXTENSIONS  {".wav", ".ogg", ".mp3", ".flac"}
#define MAX_FILENAME_LENGTH   64
#define MAX_PLAYLIST_SIZE     20

// Archivos objetivo prioritarios
#define TARGET_FILES {"example.wav", "example.ogg", "EXAMPLE.WAV", "EXAMPLE.OGG"}

// ===============================================
// CONFIGURACIÓN DE TIMING
// ===============================================

// Delays para evitar rebotes y optimizar rendimiento
#define BUTTON_DEBOUNCE_MS    200   // Antirebote de botones
#define STATUS_UPDATE_MS      1000  // Actualización de estado
#define LED_BLINK_MS          500   // Parpadeo de LEDs
#define MAIN_LOOP_DELAY_MS    10    // Delay principal del loop

// ===============================================
// CONFIGURACIÓN DE DEBUG
// ===============================================

// Habilitar/deshabilitar diferentes tipos de debug
#define DEBUG_AUDIO_INFO      true  // Información de audio
#define DEBUG_BUTTON_STATES   false // Estados de botones
#define DEBUG_SD_OPERATIONS   true  // Operaciones de SD
#define DEBUG_MEMORY_USAGE    true  // Uso de memoria

// ===============================================
// MACROS ÚTILES
// ===============================================

// Macro para verificar si un archivo es de audio
#define IS_AUDIO_FILE(filename) \
  (filename.endsWith(".wav") || filename.endsWith(".WAV") || \
   filename.endsWith(".ogg") || filename.endsWith(".OGG") || \
   filename.endsWith(".mp3") || filename.endsWith(".MP3") || \
   filename.endsWith(".flac") || filename.endsWith(".FLAC"))

// Macro para debug condicional
#define DEBUG_PRINT(condition, message) \
  if (condition) { Serial.println(message); }

// Macro para manejo seguro de arrays
#define SAFE_ARRAY_ACCESS(array, index, size) \
  ((index >= 0 && index < size) ? array[index] : "")

#endif // AUDIO_CONFIG_H
