/*
 * BugTypesTest v2.0 - 15 Insectos Caóticos y Estocásticos
 * Rediseñado para sonidos más variables, impredecibles y caóticos
 * 
 * Técnicas de síntesis mejoradas:
 * - Ruido Blanco LFSR con tonos audibles
 * - Síntesis FM con modulación caótica
 * - Patrones estocásticos extremos con silencios
 * - Secuencias largas y muy variables
 * - Manejo del botón sin bloqueos
 * 
 * Conservados: Saltamontes Verde, Moscardón, Mantis Religiosa, Hormiga Roja
 * Rediseñados: 11 insectos con sonidos más caóticos y estocásticos
 * 
 * Botón 1 (GPIO36): Cambiar al siguiente tipo de insecto
 */

#include <math.h>

// Pines
#define PIEZO_PIN 21        // Pin del piezoeléctrico
#define BUTTON_1_PIN 36     // Botón para cambiar insecto (ADC)
#define LED_PIN 2           // LED indicador

// 15 Tipos de insectos: 4 conservados + 11 rediseñados para caos extremo
enum BugType {
  CHAOS_CRICKET = 0,        // Grillo Caótico - Patrones erráticos con silencios
  GLITCH_CICADA = 1,        // Cigarra Glitch - Ruido blanco con clicks
  GRASSHOPPER_GREEN = 2,    // Saltamontes Verde - CONSERVADO - Clicks rápidos
  ERRATIC_DRAGONFLY = 3,    // Libélula Errática - Ruido blanco muy variable
  STUTTER_BEE = 4,          // Abeja Tartamuda - FM entrecortado con pausas
  RANDOM_WASP = 5,          // Avispa Aleatoria - Secuencias caóticas largas
  BUMBLEBEE = 6,            // Moscardón - CONSERVADO - FM muy grave
  NIGHT_CRAWLER = 7,        // Rastrero Nocturno - Ruido blanco grave con silencios
  PHANTOM_MOTH = 8,         // Polilla Fantasma - Clicks fantasmales irregulares
  CHAOS_BEETLE = 9,         // Escarabajo Caótico - Percusión aleatoria extrema
  MANTIS_RELIGIOUS = 10,    // Mantis Religiosa - CONSERVADO - Clicks precisos
  FRENZY_MOSQUITO = 11,     // Mosquito Frenético - FM caótico muy rápido
  ANT_RED = 12,             // Hormiga Roja - CONSERVADO - Clicks en grupos con silencios
  VOID_FIREFLY = 13,        // Luciérnaga del Vacío - Pulsos con largos silencios
  STATIC_BUG = 14           // Chinche Estático - Ruido blanco puro caótico
};

// Variables globales
BugType currentBug = CHAOS_CRICKET;
const int TOTAL_BUGS = 15;
unsigned long bugStartTime = 0;
bool bugActive = true;

// Variables para botón mejoradas
bool buttonPressed = false;
bool lastButtonState = false;
unsigned long buttonPressStart = 0;
unsigned long lastButtonCheck = 0;
const unsigned long BUTTON_CHECK_INTERVAL = 10; // Revisar botón cada 10ms

// Variables para síntesis sin bloqueos
unsigned long lastSoundTime = 0;
unsigned long soundStartTime = 0;
unsigned long soundDuration = 0;
bool soundActive = false;
int soundCounter = 0;
int currentSequenceStep = 0;

// LFSR para ruido blanco
unsigned long lfsr = 0xACE1;

// Variables específicas por insecto
struct BugCharacteristics {
  const char* name;
  int synthesisType; // 0=Tono, 1=FM, 2=Ruido Blanco, 3=Híbrido
  int baseFreq;
  int modulatorFreq;
  float modulationIndex;
  int minInterval;
  int maxInterval;
  int burstLength;
  float stochasticFactor;
  const char* habitat;
  const char* rhythm;
};

// Características rediseñadas para CAOS EXTREMO y sonidos estocásticos
BugCharacteristics bugData[15] = {
  // Nombre, Síntesis, BaseFreq, ModFreq, ModIndex, MinInt, MaxInt, BurstLen, Stochastic, Habitat, Rhythm
  {"Grillo Caótico", 3, 3000, 0, 0.0, 50, 3000, 15, 0.9, "Caos", "Errático extremo"},
  {"Cigarra Glitch", 2, 0, 0, 0.0, 10, 500, 25, 0.95, "Digital", "Glitch rápido"},
  {"Saltamontes Verde", 0, 12000, 0, 0.0, 50, 150, 5, 0.4, "Hierba", "Staccato"},      // CONSERVADO
  {"Libélula Errática", 2, 0, 0, 0.0, 5, 800, 30, 0.98, "Aire", "Caótico variable"},
  {"Abeja Tartamuda", 3, 400, 80, 1.5, 10, 1000, 20, 0.85, "Flores", "Entrecortado"},
  {"Avispa Aleatoria", 3, 900, 300, 2.0, 5, 2000, 40, 0.92, "Caos", "Secuencias largas"},
  {"Moscardón", 1, 150, 30, 0.4, 4000, 8000, 1, 0.1, "Campo", "Grave profundo"},       // CONSERVADO
  {"Rastrero Nocturno", 2, 0, 0, 0.0, 100, 5000, 12, 0.88, "Oscuridad", "Silencios largos"},
  {"Polilla Fantasma", 0, 1500, 0, 0.0, 200, 8000, 8, 0.94, "Sombras", "Clicks fantasmales"},
  {"Escarabajo Caótico", 3, 120, 0, 0.0, 20, 4000, 35, 0.96, "Tierra", "Percusión extrema"},
  {"Mantis Religiosa", 0, 6000, 0, 0.0, 3000, 8000, 2, 0.2, "Arbusto", "Preciso"},    // CONSERVADO
  {"Mosquito Frenético", 1, 2000, 1200, 1.8, 5, 100, 50, 0.9, "Aire", "Frenético rápido"},
  {"Hormiga Roja", 0, 8000, 0, 0.0, 200, 500, 6, 0.3, "Colonia", "En grupos"},        // CONSERVADO
  {"Luciérnaga del Vacío", 0, 1000, 0, 0.0, 1000, 15000, 3, 0.85, "Vacío", "Silencios profundos"},
  {"Chinche Estático", 2, 0, 0, 0.0, 1, 200, 100, 0.99, "Estático", "Ruido puro caótico"}
};

void setup() {
  Serial.begin(115200);
  Serial.println("=== BugTypesTest v2.0 - INSECTOS CAÓTICOS ===");
  Serial.println("Sonidos estocásticos extremos con ruido blanco audible");
  Serial.println("Secuencias largas, silencios variables, patrones impredecibles");
  Serial.println("Presiona el botón 1 para cambiar de insecto");
  Serial.println("Conservados: Saltamontes, Moscardón, Mantis, Hormiga");
  Serial.println();
  
  // Configurar pines
  pinMode(PIEZO_PIN, OUTPUT);
  pinMode(BUTTON_1_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  
  // LED indicador
  digitalWrite(LED_PIN, HIGH);
  
  // Mostrar lista de insectos
  printBugList();
  
  // Iniciar primer insecto
  startBug();
}

void loop() {
  unsigned long currentTime = millis();
  
  // Manejar botón
  handleButton(currentTime);
  
  // Ejecutar sonido del insecto actual
  if (bugActive) {
    executeBugSound(currentTime);
  }
  
  delay(1);
}

void printBugList() {
  Serial.println("=== CATÁLOGO DE INSECTOS ===");
  for (int i = 0; i < TOTAL_BUGS; i++) {
    Serial.print(i);
    Serial.print(". ");
    Serial.print(bugData[i].name);
    Serial.print(" - ");
    Serial.print(bugData[i].habitat);
    Serial.print(" - ");
    Serial.println(bugData[i].rhythm);
  }
  Serial.println();
}

void handleButton(unsigned long currentTime) {
  // Revisar botón cada cierto intervalo para mayor responsividad
  if (currentTime - lastButtonCheck < BUTTON_CHECK_INTERVAL) {
    return;
  }
  lastButtonCheck = currentTime;
  
  int buttonValue = analogRead(BUTTON_1_PIN);
  bool currentButtonState = (buttonValue < 100);
  
  // Detectar flanco descendente (botón soltado)
  if (lastButtonState && !currentButtonState) {
    Serial.println("*** BOTÓN PRESIONADO - Cambiando insecto ***");
    nextBug();
  }
  
  lastButtonState = currentButtonState;
}

void nextBug() {
  stopBug();
  currentBug = (BugType)((currentBug + 1) % TOTAL_BUGS);
  delay(100); // Pausa mínima entre insectos
  startBug();
}

void startBug() {
  bugStartTime = millis();
  bugActive = true;
  lastSoundTime = 0;
  soundCounter = 0;
  
  BugCharacteristics bug = bugData[currentBug];
  
  Serial.print("=== INICIANDO: ");
  Serial.print(bug.name);
  Serial.println(" ===");
  Serial.print("Hábitat: ");
  Serial.print(bug.habitat);
  Serial.print(" | Ritmo: ");
  Serial.println(bug.rhythm);
  
  String synthType = "";
  switch (bug.synthesisType) {
    case 0: synthType = "Tonos Simples"; break;
    case 1: synthType = "Síntesis FM"; break;
    case 2: synthType = "Ruido Blanco"; break;
    case 3: synthType = "Híbrido"; break;
  }
  Serial.print("Síntesis: ");
  Serial.println(synthType);
  Serial.println();
  
  // LED parpadeo rápido para indicar cambio
  for (int i = 0; i < 2; i++) {
    digitalWrite(LED_PIN, LOW);
    delay(50);
    digitalWrite(LED_PIN, HIGH);
    delay(50);
  }
}

void stopBug() {
  bugActive = false;
  noTone(PIEZO_PIN);
}

void executeBugSound(unsigned long currentTime) {
  BugCharacteristics bug = bugData[currentBug];
  
  // Si hay un sonido activo, continuar reproduciéndolo
  if (soundActive) {
    if (currentTime - soundStartTime >= soundDuration) {
      // Sonido terminado
      soundActive = false;
      noTone(PIEZO_PIN);
      digitalWrite(PIEZO_PIN, LOW);
    } else {
      // Continuar sonido activo (para FM y ruido blanco)
      updateActiveSound(currentTime, bug);
    }
    return;
  }
  
  // Calcular intervalo EXTREMAMENTE estocástico
  int baseInterval = random(bug.minInterval, bug.maxInterval);
  int stochasticVariation = (int)(baseInterval * bug.stochasticFactor * (random(300) - 150) / 100.0);
  int actualInterval = baseInterval + stochasticVariation;
  
  // Permitir intervalos muy cortos para caos extremo
  actualInterval = max(actualInterval, 1); 
  
  // Añadir probabilidad de SILENCIO LARGO
  if (random(100) < (bug.stochasticFactor * 30)) {
    actualInterval = random(2000, 10000); // Silencio de 2-10 segundos
  }
  
  if (currentTime - lastSoundTime >= actualInterval) {
    // Iniciar nuevo sonido
    startNewSound(currentTime, bug);
    
    lastSoundTime = currentTime;
    soundCounter++;
    
    // Debug menos frecuente para reducir interferencia
    if (soundCounter % 20 == 0) {
      Serial.print(bug.name);
      Serial.print(" - Sonido #");
      Serial.print(soundCounter);
      Serial.print(" | Caos: ");
      Serial.print((int)(bug.stochasticFactor * 100));
      Serial.println("%");
    }
  }
}

// Función para iniciar un nuevo sonido sin bloqueos
void startNewSound(unsigned long currentTime, BugCharacteristics bug) {
  soundStartTime = currentTime;
  soundActive = true;
  currentSequenceStep = 0;
  
  // Determinar duración del sonido basada en el tipo
  switch (bug.synthesisType) {
    case 0: // Tonos simples
      soundDuration = random(20, 200); // Muy corto para clicks rápidos
      generateChaoticTone(bug);
      break;
    case 1: // Síntesis FM
      soundDuration = random(100, 2000); // Variable para FM
      startChaoticFM(bug);
      break;
    case 2: // Ruido blanco
      soundDuration = random(50, 800); // Ráfagas de ruido
      startChaoticNoise(bug);
      break;
    case 3: // Híbrido
      soundDuration = random(10, 1500); // Muy variable
      startHybridChaos(bug);
      break;
  }
}

// Función para actualizar sonidos activos sin bloqueos
void updateActiveSound(unsigned long currentTime, BugCharacteristics bug) {
  switch (bug.synthesisType) {
    case 1: // FM continuo
      updateChaoticFM(currentTime, bug);
      break;
    case 2: // Ruido blanco continuo
      updateChaoticNoise(currentTime, bug);
      break;
    case 3: // Híbrido continuo
      updateHybridChaos(currentTime, bug);
      break;
  }
}

void generateChaoticTone(BugCharacteristics bug) {
  int freq = bug.baseFreq;
  
  // Variaciones EXTREMAS por insecto
  switch (currentBug) {
    case CHAOS_CRICKET:
      // Patrones erráticos extremos
      freq = random(500, 8000);
      tone(PIEZO_PIN, freq, random(10, 100));
      break;
      
    case GRASSHOPPER_GREEN:
      // CONSERVADO - Clicks rápidos y cortos
      freq += random(-500, 500);
      tone(PIEZO_PIN, freq, 30);
      break;
      
    case PHANTOM_MOTH:
      // Clicks fantasmales muy irregulares
      if (random(100) < 60) { // 60% probabilidad de sonido
        freq = random(800, 4000);
        tone(PIEZO_PIN, freq, random(5, 80));
      }
      break;
      
    case CHAOS_BEETLE:
      // Percusión caótica extrema
      freq = random(30, 300);
      tone(PIEZO_PIN, freq, random(50, 300));
      break;
      
    case MANTIS_RELIGIOUS:
      // CONSERVADO - Clicks precisos y agudos
      freq += random(-200, 200);
      tone(PIEZO_PIN, freq, 60);
      break;
      
    case ANT_RED:
      // CONSERVADO - Patrones en grupos pero SIN DELAYS bloqueantes
      freq += random(-1000, 1000);
      tone(PIEZO_PIN, freq, 40);
      break;
      
    case VOID_FIREFLY:
      // Pulsos con largos silencios
      if (random(100) < 30) { // Solo 30% probabilidad de sonido
        freq = random(800, 2500);
        tone(PIEZO_PIN, freq, random(50, 150));
      }
      break;
      
    default:
      // Sonido caótico por defecto
      freq = random(100, 5000);
      tone(PIEZO_PIN, freq, random(20, 150));
      break;
  }
}

// Función para iniciar FM sin bloqueos
void startChaoticFM(BugCharacteristics bug) {
  // El sonido será actualizado en updateChaoticFM
}

// Función para actualizar FM de forma continua sin bloqueos
void updateChaoticFM(unsigned long currentTime, BugCharacteristics bug) {
  float timeElapsed = (currentTime - soundStartTime) / 1000.0;
  
  switch (currentBug) {
    case BUMBLEBEE:
      // CONSERVADO - FM muy grave
      {
        float modulator = sin(2 * PI * bug.modulatorFreq * timeElapsed);
        int currentFreq = bug.baseFreq + (int)(bug.modulationIndex * bug.baseFreq * modulator);
        currentFreq = constrain(currentFreq, 50, 500);
        tone(PIEZO_PIN, currentFreq, 50);
      }
      break;
      
    case STUTTER_BEE:
      // FM entrecortado con pausas
      {
        // Crear "tartamudeo" con modulación irregular
        float stutter = sin(timeElapsed * 15) > 0.3 ? 1.0 : 0.0; // On/off irregular
        if (stutter > 0) {
          float modulator = sin(2 * PI * bug.modulatorFreq * timeElapsed * random(80, 120) / 100.0);
          int currentFreq = bug.baseFreq + (int)(bug.modulationIndex * bug.baseFreq * modulator);
          currentFreq = constrain(currentFreq, 200, 1200);
          tone(PIEZO_PIN, currentFreq, 30);
        } else {
          noTone(PIEZO_PIN);
        }
      }
      break;
      
    case FRENZY_MOSQUITO:
      // FM caótico muy rápido
      {
        float chaosModulator = sin(2 * PI * bug.modulatorFreq * timeElapsed * random(50, 200) / 100.0);
        float noiseModulator = (random(200) - 100) / 100.0 * 0.3; // Ruido adicional
        int currentFreq = bug.baseFreq + (int)((bug.modulationIndex + noiseModulator) * bug.baseFreq * chaosModulator);
        currentFreq = constrain(currentFreq, 800, 4000);
        tone(PIEZO_PIN, currentFreq, 20);
      }
      break;
      
    default:
      // FM caótico genérico
      {
        float wildModulator = sin(2 * PI * random(50, 500) * timeElapsed);
        int currentFreq = random(200, 2000) + (int)(wildModulator * random(100, 800));
        currentFreq = constrain(currentFreq, 100, 6000);
        tone(PIEZO_PIN, currentFreq, 25);
      }
      break;
  }
}

// Función para iniciar ruido blanco sin bloqueos
void startChaoticNoise(BugCharacteristics bug) {
  // El ruido será generado en updateChaoticNoise
}

// Función para generar ruido blanco AUDIBLE sin bloqueos
void updateChaoticNoise(unsigned long currentTime, BugCharacteristics bug) {
  // Generar LFSR en tiempo real
  unsigned bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5)) & 1;
  lfsr = (lfsr >> 1) | (bit << 15);
  
  switch (currentBug) {
    case GLITCH_CICADA:
      // Ruido blanco con clicks audibles
      {
        // Convertir LFSR a frecuencia audible
        int noiseFreq = 1000 + (lfsr % 3000); // 1000-4000 Hz
        if (random(100) < 70) { // 70% probabilidad de sonido
          tone(PIEZO_PIN, noiseFreq, random(5, 30));
        }
      }
      break;
      
    case ERRATIC_DRAGONFLY:
      // Ruido muy variable simulando aleteo caótico
      {
        int noiseFreq = 500 + (lfsr % 2000); // 500-2500 Hz
        if (random(100) < 60) { // 60% probabilidad
          tone(PIEZO_PIN, noiseFreq, random(10, 80));
        }
      }
      break;
      
    case NIGHT_CRAWLER:
      // Ruido grave con silencios
      {
        int noiseFreq = 100 + (lfsr % 800); // 100-900 Hz (más grave)
        if (random(100) < 40) { // Solo 40% probabilidad (más silencios)
          tone(PIEZO_PIN, noiseFreq, random(20, 150));
        }
      }
      break;
      
    case STATIC_BUG:
      // Ruido puro caótico muy audible
      {
        int noiseFreq = 200 + (lfsr % 6000); // 200-6200 Hz (rango amplio)
        if (random(100) < 85) { // 85% probabilidad (muy activo)
          tone(PIEZO_PIN, noiseFreq, random(3, 50));
        }
      }
      break;
      
    default:
      // Ruido genérico audible
      {
        int noiseFreq = 800 + (lfsr % 2400); // 800-3200 Hz
        if (random(100) < 65) {
          tone(PIEZO_PIN, noiseFreq, random(8, 60));
        }
      }
      break;
  }
}

// Función para iniciar sonidos híbridos caóticos
void startHybridChaos(BugCharacteristics bug) {
  // Los híbridos combinan múltiples técnicas en tiempo real
}

// Función para actualizar sonidos híbridos sin bloqueos
void updateHybridChaos(unsigned long currentTime, BugCharacteristics bug) {
  float timeElapsed = (currentTime - soundStartTime) / 1000.0;
  
  switch (currentBug) {
    case CHAOS_CRICKET:
      // Combinar tonos erráticos con ruido
      {
        if (random(100) < 60) {
          // Tono caótico
          int freq = random(800, 6000);
          tone(PIEZO_PIN, freq, random(15, 80));
        } else {
          // Ruido convertido a frecuencia
          unsigned bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5)) & 1;
          lfsr = (lfsr >> 1) | (bit << 15);
          int noiseFreq = 1500 + (lfsr % 2500);
          tone(PIEZO_PIN, noiseFreq, random(5, 40));
        }
      }
      break;
      
    case RANDOM_WASP:
      // Secuencias caóticas largas alternando técnicas
      {
        int technique = random(3);
        switch (technique) {
          case 0: // FM caótico
            {
              float wildMod = sin(timeElapsed * random(10, 50));
              int freq = 600 + (int)(wildMod * random(300, 1200));
              tone(PIEZO_PIN, freq, random(10, 60));
            }
            break;
          case 1: // Ruido audible
            {
              int noiseFreq = 400 + (lfsr % 3000);
              tone(PIEZO_PIN, noiseFreq, random(8, 45));
            }
            break;
          case 2: // Clicks extremos
            {
              int freq = random(1000, 8000);
              tone(PIEZO_PIN, freq, random(5, 25));
            }
            break;
        }
      }
      break;
      
    case ABEJA_TARTAMUDA:
      // FM entrecortado + ruido ocasional
      {
        if (random(100) < 75) {
          // FM tartamudo
          float stutter = sin(timeElapsed * 12) > 0.2 ? 1.0 : 0.0;
          if (stutter > 0) {
            float mod = sin(2 * PI * bug.modulatorFreq * timeElapsed);
            int freq = bug.baseFreq + (int)(bug.modulationIndex * bug.baseFreq * mod);
            tone(PIEZO_PIN, freq, 25);
          }
        } else {
          // Ruido ocasional
          int noiseFreq = 800 + (lfsr % 1200);
          tone(PIEZO_PIN, noiseFreq, random(10, 30));
        }
      }
      break;
      
    case CHAOS_BEETLE:
      // Percusión extrema + glitches
      {
        if (random(100) < 80) {
          // Percusión caótica
          int freq = random(50, 400);
          tone(PIEZO_PIN, freq, random(30, 200));
        } else {
          // Glitch noise
          int glitchFreq = random(2000, 8000);
          tone(PIEZO_PIN, glitchFreq, random(3, 15));
        }
      }
      break;
      
    default:
      // Híbrido genérico extremo
      {
        int mode = random(4);
        switch (mode) {
          case 0: tone(PIEZO_PIN, random(100, 8000), random(5, 100)); break;
          case 1: tone(PIEZO_PIN, 500 + (lfsr % 4000), random(8, 50)); break;
          case 2: tone(PIEZO_PIN, (int)(sin(timeElapsed * 20) * 2000 + 1500), 30); break;
          case 3: /* Silencio ocasional */ break;
        }
      }
      break;
  }
}

// Función para generar patrones basados en secuencias naturales
int getFibonacciInterval(int n) {
  if (n <= 1) return n;
  int a = 0, b = 1, temp;
  for (int i = 2; i <= n; i++) {
    temp = a + b;
    a = b;
    b = temp;
  }
  return b * 100; // Convertir a milisegundos
}

// Función para generar variaciones estocásticas
int getStochasticVariation(int base, float factor) {
  return base + (int)(base * factor * (random(200) - 100) / 100.0);
}