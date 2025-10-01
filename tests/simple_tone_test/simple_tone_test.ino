/*
 * Test Simple de Sonido - ESP32-A1S AudioKit 2.2
 * Genera un tono de 440 Hz por la salida LOUT (Left Output)
 * 
 * Hardware: ESP32-A1S AudioKit 2.2
 * Salida: Jack de auriculares (solo canal izquierdo)
 * Frecuencia: 440 Hz (nota La4)
 * 
 * Sin dependencias externas - solo I2S nativo del ESP32
 */

#include "driver/i2s.h"
#include <math.h>

// Pines I2S para ESP32-A1S AudioKit 2.2
#define I2S_BCLK      27    // Bit Clock
#define I2S_LRC       26    // Left/Right Clock  
#define I2S_DOUT      25    // Data Output -> LOUT
#define LED_SYSTEM    2     // LED indicador de actividad

// Parámetros de audio
#define SAMPLE_RATE   16000 // Frecuencia de muestreo (Hz)
#define TONE_FREQ     440   // Frecuencia del tono (Hz)
#define WAVE_SAMPLES  64    // Número de muestras en la tabla de onda
#define AMPLITUDE     16000 // Amplitud del tono (50% del máximo)

// Buffer para la onda senoidal pre-calculada
int16_t sineWave[WAVE_SAMPLES];

// Variables para el loop principal
unsigned long lastLedToggle = 0;
bool ledState = false;

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== Test Simple de Sonido ESP32-A1S AudioKit 2.2 ===");
  Serial.println("Generando tono de 440 Hz por salida LOUT...");
  
  // Configurar LED
  pinMode(LED_SYSTEM, OUTPUT);
  digitalWrite(LED_SYSTEM, LOW);
  
  // Pre-calcular tabla de onda senoidal
  generateSineWave();
  
  // Configurar I2S
  if (setupI2S()) {
    Serial.println("✓ I2S configurado correctamente");
    Serial.println("✓ Conecta auriculares al jack del AudioKit");
    Serial.println("✓ Deberías escuchar un tono de 440 Hz en el canal izquierdo");
  } else {
    Serial.println("✗ Error al configurar I2S");
  }
}

void loop() {
  // Generar y enviar tono continuamente
  playTone();
  
  // Parpadear LED cada segundo
  unsigned long currentTime = millis();
  if (currentTime - lastLedToggle >= 1000) {
    ledState = !ledState;
    digitalWrite(LED_SYSTEM, ledState);
    lastLedToggle = currentTime;
  }
}

void generateSineWave() {
  Serial.print("Generando tabla de onda senoidal (");
  Serial.print(WAVE_SAMPLES);
  Serial.println(" muestras)...");
  
  for (int i = 0; i < WAVE_SAMPLES; i++) {
    // Calcular onda senoidal para 440 Hz
    // Fórmula: sin(2π * frecuencia * i / frecuencia_muestreo)
    float angle = 2.0 * PI * TONE_FREQ * i / SAMPLE_RATE;
    sineWave[i] = (int16_t)(sin(angle) * AMPLITUDE);
  }
  
  Serial.println("✓ Tabla de onda generada");
}

bool setupI2S() {
  // Configuración I2S
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = false,
    .tx_desc_auto_clear = true
  };
  
  // Configuración de pines I2S
  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_BCLK,           // Bit Clock
    .ws_io_num = I2S_LRC,             // Word Select (L/R Clock)
    .data_out_num = I2S_DOUT,         // Data Output
    .data_in_num = I2S_PIN_NO_CHANGE  // No entrada
  };
  
  // Instalar driver I2S
  esp_err_t result = i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  if (result != ESP_OK) {
    Serial.print("Error instalando driver I2S: ");
    Serial.println(result);
    return false;
  }
  
  // Configurar pines I2S
  result = i2s_set_pin(I2S_NUM_0, &pin_config);
  if (result != ESP_OK) {
    Serial.print("Error configurando pines I2S: ");
    Serial.println(result);
    return false;
  }
  
  return true;
}

void playTone() {
  size_t bytes_written;
  
  // Enviar todas las muestras de la tabla de onda
  for (int i = 0; i < WAVE_SAMPLES; i++) {
    // Crear buffer estéreo (2 canales)
    int16_t samples[2];
    samples[0] = sineWave[i];  // Canal izquierdo (LOUT)
    samples[1] = 0;            // Canal derecho (ROUT) - silencio
    
    // Enviar al I2S (4 bytes: 2 canales × 16 bits)
    esp_err_t result = i2s_write(I2S_NUM_0, samples, 4, &bytes_written, portMAX_DELAY);
    
    // Verificar errores (solo en la primera iteración para no saturar serial)
    if (i == 0 && result != ESP_OK) {
      Serial.print("Error escribiendo a I2S: ");
      Serial.println(result);
    }
  }
}

/*
 * INSTRUCCIONES DE USO:
 * 
 * 1. Conecta el ESP32-A1S AudioKit 2.2 a tu computadora
 * 2. Abre este sketch en Arduino IDE
 * 3. Selecciona la placa "ESP32 Dev Module" o "ESP32-A1S AudioKit"
 * 4. Sube el código al ESP32
 * 5. Conecta auriculares al jack de 3.5mm del AudioKit
 * 6. Deberías escuchar un tono continuo de 440 Hz en el auricular izquierdo
 * 
 * VERIFICACIÓN:
 * ✓ LED GPIO2 parpadea cada segundo
 * ✓ Monitor serial muestra mensajes de confirmación
 * ✓ Tono audible en canal izquierdo (LOUT)
 * ✓ Canal derecho en silencio
 * 
 * SOLUCIÓN DE PROBLEMAS:
 * - No se escucha nada: Verificar conexión de auriculares
 * - Tono distorsionado: Reducir AMPLITUDE a 8000
 * - LED no parpadea: Problema con ESP32, no con audio
 */
