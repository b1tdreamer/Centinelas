/*
 * Test Secuencial NeoPixel con Fade - ESP32 AudioKit 2.2
 * 
 * Descripción:
 * Control de aro LED NeoPixel de 8 píxeles conectado al GPIO23.
 * Efecto de "ola verde" secuencial con transiciones suaves (fade).
 * 
 * Hardware:
 * - ESP32 AudioKit 2.2
 * - Aro NeoPixel WS2812B de 8 LEDs
 * - Pin de datos: GPIO23
 * 
 * Comportamiento:
 * - Cada LED se enciende durante 3 segundos en color verde
 * - Fade in suave de 1000ms al aparecer (igual al intervalo)
 * - Fade out suave de 1000ms al desaparecer (igual al intervalo)
 * - Nuevo LED comienza cada 1000ms (intervalo ajustable)
 * - Múltiples LEDs encendidos simultáneamente creando efecto "ola"
 * - Ciclo se repite indefinidamente sin cortes
 */

#include <Adafruit_NeoPixel.h>

// ============================================
// DEFINICIONES HARDWARE
// ============================================
#define LED_PIN       23      // GPIO23 - Pin de datos NeoPixel
#define LED_COUNT     8       // 8 LEDs en el aro

// ============================================
// CONFIGURACIÓN - Modificar aquí los valores
// ============================================

// Parámetros de temporización
const unsigned long LED_DURATION = 3000;      // Tiempo que cada LED permanece encendido (ms)
const unsigned long LED_INTERVAL = 600;      // Intervalo entre inicio de LEDs consecutivos (ms)

// Parámetros de fade (transiciones suaves)
// Fade in y fade out iguales al intervalo para efecto simétrico
const unsigned long FADE_IN_TIME = LED_INTERVAL;   // Tiempo de aparición (fade in) = intervalo
const unsigned long FADE_OUT_TIME = LED_INTERVAL;  // Tiempo de desaparición (fade out) = intervalo

// Parámetros de apariencia
const uint8_t BRIGHTNESS = 50;                // Brillo 0-255 (76 = 30%)
const uint8_t COLOR_R = 0;                    // Componente rojo (para verde = 0)
const uint8_t COLOR_G = 255;                  // Componente verde (para verde = 255)
const uint8_t COLOR_B = 0;                    // Componente azul (para verde = 0)

// Debug (cambiar a true para ver valores de fade en monitor serial)
const bool DEBUG_FADE = false;                // Activar debug del fade

// ============================================

// Inicialización del objeto NeoPixel
Adafruit_NeoPixel pixels(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);


/**
 * Calcula el nivel de brillo con fade in/out
 * @param timeInState: Tiempo transcurrido desde que el LED comenzó a encenderse (ms)
 * @param totalDuration: Duración total del encendido (ms)
 * @return: Factor de brillo entre 0.0 y 1.0
 */
float calculateFadeBrightness(unsigned long timeInState, unsigned long totalDuration) {
  float brightness = 0.0;
  
  // Fase 1: Fade in (primeros FADE_IN_TIME ms)
  if (timeInState <= FADE_IN_TIME) {
    brightness = (float)timeInState / (float)FADE_IN_TIME;
    // Asegurar que no exceda 1.0
    if (brightness > 1.0) brightness = 1.0;
    if (brightness < 0.0) brightness = 0.0;
    return brightness;
  }
  
  // Fase 3: Fade out (últimos FADE_OUT_TIME ms)
  if (timeInState >= (totalDuration - FADE_OUT_TIME)) {
    unsigned long timeUntilEnd = totalDuration - timeInState;
    brightness = (float)timeUntilEnd / (float)FADE_OUT_TIME;
    // Asegurar que no exceda 1.0 ni sea negativo
    if (brightness > 1.0) brightness = 1.0;
    if (brightness < 0.0) brightness = 0.0;
    return brightness;
  }
  
  // Fase 2: Estado completo (entre fade in y fade out)
  return 1.0;
}


void setup() {
  // Inicializar comunicación serial
  Serial.begin(115200);
  delay(500);  // Pequeña pausa para estabilizar serial
  
  // Inicializar NeoPixel
  pixels.begin();
  pixels.setBrightness(BRIGHTNESS);
  pixels.clear();
  pixels.show();
  
  // Calcular y mostrar timing del ciclo
  unsigned long cycleTime = LED_COUNT * LED_INTERVAL;
  
  // Calcular LEDs simultáneos aproximados
  int simultaneousLEDs = (LED_DURATION / LED_INTERVAL) + 1;
  if (simultaneousLEDs > LED_COUNT) {
    simultaneousLEDs = LED_COUNT;
  }

}


void loop() {
  // Obtener tiempo actual
  unsigned long currentTime = millis();
  unsigned long cycleTime = LED_COUNT * LED_INTERVAL;
  
  // Actualizar estado de cada LED con fade
  for(int i = 0; i < LED_COUNT; i++) {
    // Calcular cuántos ciclos completos han pasado
    unsigned long currentCycle = currentTime / cycleTime;
    
    // Calcular el tiempo de inicio de este LED en el ciclo actual
    unsigned long ledStartTimeInCycle = i * LED_INTERVAL;
    unsigned long ledStartTimeAbsolute = (currentCycle * cycleTime) + ledStartTimeInCycle;
    
    // También verificar el ciclo anterior (para permitir que los LEDs completen su fade out)
    unsigned long ledStartTimePreviousCycle = 0;
    bool checkPreviousCycle = false;
    if (currentCycle > 0) {
      ledStartTimePreviousCycle = ((currentCycle - 1) * cycleTime) + ledStartTimeInCycle;
      checkPreviousCycle = true;
    }
    
    // Variables para gestionar el estado del LED
    bool shouldBeOn = false;
    unsigned long timeInState = 0;
    
    // Verificar si el LED está activo en el ciclo actual
    if (currentTime >= ledStartTimeAbsolute && 
        currentTime < (ledStartTimeAbsolute + LED_DURATION)) {
      shouldBeOn = true;
      timeInState = currentTime - ledStartTimeAbsolute;
    }
    // Verificar si el LED todavía está activo del ciclo anterior
    else if (checkPreviousCycle && 
             currentTime >= ledStartTimePreviousCycle && 
             currentTime < (ledStartTimePreviousCycle + LED_DURATION)) {
      shouldBeOn = true;
      timeInState = currentTime - ledStartTimePreviousCycle;
    }
    
    // Calcular brillo con fade y aplicar el color
    if (shouldBeOn) {
      // Calcular factor de fade (0.0 a 1.0)
      float fadeFactor = calculateFadeBrightness(timeInState, LED_DURATION);
      
      // Debug: Imprimir valores de fade (solo para LED 0 y durante fade in)
      if (DEBUG_FADE && i == 0 && timeInState <= FADE_IN_TIME) {
        Serial.println((uint8_t)(COLOR_G * fadeFactor));
      }
      
      // Aplicar fade al color
      uint8_t fadedR = (uint8_t)(COLOR_R * fadeFactor);
      uint8_t fadedG = (uint8_t)(COLOR_G * fadeFactor);
      uint8_t fadedB = (uint8_t)(COLOR_B * fadeFactor);
      
      pixels.setPixelColor(i, pixels.Color(fadedR, fadedG, fadedB));
    } else {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));  // Apagado
    }
  }
  
  // Actualizar físicamente los LEDs
  pixels.show();
  
  // Pequeño delay para estabilidad (no afecta el timing basado en millis)
  // Reducido a 5ms para mejor resolución en el fade
  delay(5);
}

