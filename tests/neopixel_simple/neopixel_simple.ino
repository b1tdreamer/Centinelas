/*
 * NeoPixel Fade Wave - ESP32 AudioKit 2.2
 * 
 * Efecto de onda con fade en aro LED NeoPixel de 8 píxeles
 * - Cada LED hace un fade de 0% a 50% de brillo
 * - Los LEDs empiezan en secuencia creando una onda
 * - Ciclo continuo alrededor del aro
 * 
 * Hardware:
 * - ESP32 AudioKit v2.2
 * - Aro NeoPixel WS2812B de 8 LEDs
 * - Pin de datos: GPIO23
 * 
 * Librería requerida:
 * - Adafruit NeoPixel (instalar desde Library Manager)
 */

#include <Adafruit_NeoPixel.h>

// ===== CONFIGURACIÓN DE HARDWARE =====
#define LED_PIN       23      // GPIO23 - Pin de datos NeoPixel
#define LED_COUNT     8       // 8 LEDs en el aro

// ===== CONFIGURACIÓN DE TIMING =====
#define CYCLE_TIME    3000    // Tiempo de ciclo completo por LED (ms) - 3.5 segundos
#define LED_DELAY     700     // Retraso entre el inicio de cada LED (ms)

// ===== CONFIGURACIÓN DE BRILLO =====
#define MAX_BRIGHTNESS_PERCENT  30    // Brillo máximo en porcentaje (0-100) - Modo pasivo
#define EVENT_BRIGHTNESS_PERCENT 50   // Brillo durante evento de sonido (0-100)

// ===== CONFIGURACIÓN DE COLOR =====
// Valores RGB base (0-255) - Se aplicará el fade sobre estos valores
#define COLOR_RED      20     // Componente rojo
#define COLOR_GREEN    150    // Componente verde (dominante para verde claro)
#define COLOR_BLUE     20     // Componente azul

// Color durante evento de sonido (blanco)
#define EVENT_COLOR_RED    255
#define EVENT_COLOR_GREEN  255
#define EVENT_COLOR_BLUE   255

// ===== CONFIGURACIÓN DE EVENTOS DE SONIDO =====
#define SOUND_EVENT_INTERVAL  4000    // Intervalo entre eventos (ms) - 4 segundos
#define SOUND_EVENT_DURATION  700     // Duración del evento (ms) - 0.7 segundos

// Calcular brillo máximo en escala 0-255
const uint8_t MAX_BRIGHTNESS = (255 * MAX_BRIGHTNESS_PERCENT) / 100;
const uint8_t EVENT_BRIGHTNESS = (255 * EVENT_BRIGHTNESS_PERCENT) / 100;

// Crear objeto NeoPixel
Adafruit_NeoPixel pixels(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// Variables de tiempo
unsigned long startTime = 0;
unsigned long lastEventTime = 0;

// Variables de estado
bool isSoundEventActive = false;

void setup() {
  // Inicializar comunicación serial para debug
  Serial.begin(115200);
  delay(100);
  
  Serial.println("\n==========================================");
  Serial.println("NeoPixel Fade Wave - ESP32 AudioKit");
  Serial.println("==========================================");
  Serial.print("Pin de datos: GPIO");
  Serial.println(LED_PIN);
  Serial.print("Cantidad de LEDs: ");
  Serial.println(LED_COUNT);
  Serial.println("------------------------------------------");
  Serial.print("Brillo pasivo: ");
  Serial.print(MAX_BRIGHTNESS_PERCENT);
  Serial.println("%");
  Serial.print("Color pasivo RGB: (");
  Serial.print(COLOR_RED);
  Serial.print(", ");
  Serial.print(COLOR_GREEN);
  Serial.print(", ");
  Serial.print(COLOR_BLUE);
  Serial.println(")");
  Serial.println("------------------------------------------");
  Serial.print("Brillo evento: ");
  Serial.print(EVENT_BRIGHTNESS_PERCENT);
  Serial.println("%");
  Serial.print("Color evento RGB: (");
  Serial.print(EVENT_COLOR_RED);
  Serial.print(", ");
  Serial.print(EVENT_COLOR_GREEN);
  Serial.print(", ");
  Serial.print(EVENT_COLOR_BLUE);
  Serial.println(") - Blanco");
  Serial.println("------------------------------------------");
  Serial.print("Tiempo de ciclo: ");
  Serial.print(CYCLE_TIME);
  Serial.println(" ms");
  Serial.print("Retraso entre LEDs: ");
  Serial.print(LED_DELAY);
  Serial.println(" ms");
  Serial.print("Evento cada: ");
  Serial.print(SOUND_EVENT_INTERVAL);
  Serial.println(" ms");
  Serial.print("Duración evento: ");
  Serial.print(SOUND_EVENT_DURATION);
  Serial.println(" ms");
  Serial.println("==========================================\n");
  
  // Inicializar NeoPixel
  pixels.begin();
  pixels.setBrightness(150);  // Control de brillo manual
  pixels.clear();
  pixels.show();
  
  Serial.println("✓ NeoPixel inicializado correctamente");
  Serial.println("✓ Iniciando efecto de onda con fade...\n");
  
  // Guardar tiempo de inicio
  startTime = millis();
  
  delay(500);
}

/**
 * Calcula el brillo de un LED basado en su fase en el ciclo
 * Usa una función sinusoidal para un fade suave
 * 
 * @param phase: valor entre 0.0 y 1.0 representando la posición en el ciclo
 * @param maxBrightness: brillo máximo a alcanzar
 * @return: valor de brillo entre 0 y maxBrightness
 */
uint8_t calculateBrightness(float phase, uint8_t maxBrightness) {
  // Asegurar que phase está en rango [0, 1]
  while (phase > 1.0f) phase -= 1.0f;
  while (phase < 0.0f) phase += 1.0f;
  
  // Usar función sinusoidal para fade suave
  // sin() va de -1 a 1, ajustamos a 0 a 1
  float brightness = (sin(phase * 2.0f * PI - PI/2.0f) + 1.0f) / 2.0f;
  
  // Escalar al brillo máximo especificado
  return (uint8_t)(brightness * maxBrightness);
}

void loop() {
  // Obtener tiempo transcurrido desde el inicio
  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - startTime;
  
  // ===== GESTIÓN DE EVENTOS DE SONIDO =====
  // Verificar si debe iniciarse un nuevo evento
  if (!isSoundEventActive && (currentTime - lastEventTime >= SOUND_EVENT_INTERVAL)) {
    isSoundEventActive = true;
    lastEventTime = currentTime;
    Serial.println("🔊 EVENTO DE SONIDO INICIADO - LEDs activos cambiarán a BLANCO (50%)");
  }
  
  // Verificar si debe finalizar el evento actual
  if (isSoundEventActive && (currentTime - lastEventTime >= SOUND_EVENT_DURATION)) {
    isSoundEventActive = false;
    Serial.println("🟢 Evento finalizado - Todos vuelven a modo pasivo\n");
  }
  
  // ===== ACTUALIZAR CADA LED =====
  for (int i = 0; i < LED_COUNT; i++) {
    // Calcular el offset de tiempo para este LED
    unsigned long ledOffset = i * LED_DELAY;
    
    // Calcular el tiempo efectivo para este LED
    long ledTime = elapsedTime - ledOffset;
    
    // Si el LED aún no ha comenzado su ciclo, mantenerlo apagado
    if (ledTime < 0) {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
      continue;
    }
    
    // Calcular la fase del ciclo (0.0 a 1.0)
    float phase = (float)(ledTime % CYCLE_TIME) / (float)CYCLE_TIME;
    
    // ===== DETERMINAR SI ESTE LED ESPECÍFICO DEBE USAR MODO EVENTO =====
    // Un LED usa modo evento si está "encendiéndose" durante el período del evento
    // Calculamos cuánto tiempo hace que este LED comenzó su ciclo actual
    unsigned long timeSinceThisLedCycleStart = ledTime % CYCLE_TIME;
    
    // Verificar si este LED comenzó su ciclo durante el evento activo
    bool thisLedUsesEventMode = false;
    if (isSoundEventActive) {
      // Calcular cuánto tiempo ha pasado desde que empezó el evento
      unsigned long timeSinceEventStart = currentTime - lastEventTime;
      
      // Tiempo absoluto cuando este LED comenzó su ciclo actual
      unsigned long thisLedCycleStartTime = elapsedTime - timeSinceThisLedCycleStart;
      unsigned long eventStartTime = elapsedTime - timeSinceEventStart;
      
      // Este LED usa modo evento si su ciclo comenzó durante el evento
      if (thisLedCycleStartTime >= eventStartTime && 
          thisLedCycleStartTime < (eventStartTime + SOUND_EVENT_DURATION)) {
        thisLedUsesEventMode = true;
      }
    }
    
    // Seleccionar parámetros según si este LED específico está en modo evento
    uint8_t currentMaxBrightness;
    uint8_t currentColorR, currentColorG, currentColorB;
    
    if (thisLedUsesEventMode) {
      // Modo evento para este LED: Blanco al 50%
      currentMaxBrightness = EVENT_BRIGHTNESS;
      currentColorR = EVENT_COLOR_RED;
      currentColorG = EVENT_COLOR_GREEN;
      currentColorB = EVENT_COLOR_BLUE;
    } else {
      // Modo pasivo para este LED: Verde claro al 30%
      currentMaxBrightness = MAX_BRIGHTNESS;
      currentColorR = COLOR_RED;
      currentColorG = COLOR_GREEN;
      currentColorB = COLOR_BLUE;
    }
    
    // Calcular el brillo usando la función de fade con el brillo máximo actual
    uint8_t brightness = calculateBrightness(phase, currentMaxBrightness);
    
    // Calcular factor de brillo (0.0 a 1.0)
    float brightnessFactor = (float)brightness / (float)currentMaxBrightness;
    
    // Aplicar el factor de brillo a cada componente de color
    uint8_t r = (uint8_t)(currentColorR * brightnessFactor);
    uint8_t g = (uint8_t)(currentColorG * brightnessFactor);
    uint8_t b = (uint8_t)(currentColorB * brightnessFactor);
    
    // Aplicar el color con fade
    pixels.setPixelColor(i, pixels.Color(r, g, b));
  }
  
  // Aplicar todos los cambios
  pixels.show();
  
  // Pequeño delay para estabilidad (no bloqueante en términos prácticos)
  delay(10);
}


