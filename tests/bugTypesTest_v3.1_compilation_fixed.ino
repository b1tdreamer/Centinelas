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

// Declaración adelantada de la estructura para las funciones
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

// Variables para pausas obligatorias de insectos caóticos
bool inMandatoryPause = false;
unsigned long pauseStartTime = 0;
unsigned long pauseDuration = 0;
unsigned long sequenceStartTime = 0;
unsigned long maxSequenceDuration = 0;

// Escalas musicales para cada insecto caótico (frecuencias en Hz)
// Do Mayor (C Major): Do, Re, Mi, Fa, Sol, La, Si
const int scaleCMajor[] = {262, 294, 330, 349, 392, 440, 494, 523, 587, 659, 698, 784};
// La menor (A Minor): La, Si, Do, Re, Mi, Fa, Sol
const int scaleAMinor[] = {220, 247, 262, 294, 330, 349, 392, 440, 494, 523, 587, 659};
// Sol Mayor (G Major): Sol, La, Si, Do, Re, Mi, Fa#
const int scaleGMajor[] = {196, 220, 247, 262, 294, 330, 370, 392, 440, 494, 523, 587};
// Mi menor (E Minor): Mi, Fa#, Sol, La, Si, Do, Re
const int scaleEMinor[] = {165, 185, 196, 220, 247, 262, 294, 330, 370, 392, 440, 494};
// Re Mayor (D Major): Re, Mi, Fa#, Sol, La, Si, Do#
const int scaleDMajor[] = {147, 165, 185, 196, 220, 247, 277, 294, 330, 370, 392, 440};
// Si menor (B Minor): Si, Do#, Re, Mi, Fa#, Sol, La
const int scaleBMinor[] = {123, 139, 147, 165, 185, 196, 220, 247, 277, 294, 330, 370};
// Fa Mayor (F Major): Fa, Sol, La, Sib, Do, Re, Mi
const int scaleFMajor[] = {175, 196, 220, 233, 262, 294, 330, 349, 392, 440, 466, 523};
// Do# menor (C# Minor): Do#, Re#, Mi, Fa#, Sol#, La, Si
const int scaleCSMinor[] = {139, 156, 165, 185, 208, 220, 247, 277, 311, 330, 370, 415};
// Pentatónica (Pentatonic): Do, Re, Mi, Sol, La
const int scalePentatonic[] = {262, 294, 330, 392, 440, 523, 587, 659, 784, 880, 1047, 1175};
// Cromática caótica (notas aleatorias pero musicales)
const int scaleChaotic[] = {138, 207, 277, 311, 415, 466, 554, 622, 740, 831, 932, 1109};
// Blues (escala de blues): Do, Mib, Fa, Fa#, Sol, Sib
const int scaleBlues[] = {262, 311, 349, 370, 392, 466, 523, 622, 698, 740, 784, 932};

const int scaleLength = 12; // Todas las escalas tienen 12 notas

// Función para obtener la escala musical de cada insecto
const int* getInsectScale(BugType insect) {
  switch (insect) {
    case CHAOS_CRICKET: return scaleCMajor;        // Do Mayor - clásico pero caótico
    case GLITCH_CICADA: return scaleChaotic;       // Cromática caótica - digital
    case GRASSHOPPER_GREEN: return scaleGMajor;    // Sol Mayor - natural/hierba
    case ERRATIC_DRAGONFLY: return scaleBlues;     // Blues - fluido como agua
    case STUTTER_BEE: return scaleFMajor;          // Fa Mayor - cálido como flores
    case RANDOM_WASP: return scaleEMinor;          // Mi menor - agresivo/tenso
    case BUMBLEBEE: return scaleBMinor;            // Si menor - grave profundo
    case NIGHT_CRAWLER: return scaleCSMinor;       // Do# menor - nocturno/misterioso
    case PHANTOM_MOTH: return scaleAMinor;         // La menor - fantasmal
    case CHAOS_BEETLE: return scaleDMajor;         // Re Mayor - percusivo/terrestre
    case MANTIS_RELIGIOUS: return scalePentatonic; // Pentatónica - precisa/oriental
    case FRENZY_MOSQUITO: return scaleChaotic;     // Cromática caótica - frenético
    case ANT_RED: return scaleGMajor;              // Sol Mayor - social/natural
    case VOID_FIREFLY: return scaleAMinor;         // La menor - melancólico/vacío
    case STATIC_BUG: return scaleChaotic;          // Cromática caótica - puro caos
    default: return scaleCMajor;
  }
}

// Función para determinar si un insecto necesita pausas obligatorias
bool needsMandatoryPauses(BugType insect) {
  switch (insect) {
    case CHAOS_CRICKET:
    case GLITCH_CICADA:
    case ERRATIC_DRAGONFLY:
    case STUTTER_BEE:
    case RANDOM_WASP:
    case NIGHT_CRAWLER:
    case PHANTOM_MOTH:
    case CHAOS_BEETLE:
    case FRENZY_MOSQUITO:
    case VOID_FIREFLY:
    case STATIC_BUG:
      return true; // Insectos caóticos necesitan pausas
    default:
      return false; // Insectos conservados no necesitan pausas
  }
}

// Función para obtener duración máxima de secuencia antes de pausa obligatoria
unsigned long getMaxSequenceDuration(BugType insect) {
  switch (insect) {
    case CHAOS_CRICKET: return random(2000, 4000);    // 2-4 segundos
    case GLITCH_CICADA: return random(1000, 2500);    // 1-2.5 segundos (glitch rápido)
    case ERRATIC_DRAGONFLY: return random(3000, 6000); // 3-6 segundos (aleteo largo)
    case STUTTER_BEE: return random(1500, 3000);      // 1.5-3 segundos (tartamudeo)
    case RANDOM_WASP: return random(4000, 8000);      // 4-8 segundos (secuencias largas)
    case NIGHT_CRAWLER: return random(2000, 5000);    // 2-5 segundos (nocturno)
    case PHANTOM_MOTH: return random(1000, 3000);     // 1-3 segundos (fantasmal)
    case CHAOS_BEETLE: return random(3000, 7000);     // 3-7 segundos (percusión intensa)
    case FRENZY_MOSQUITO: return random(500, 1500);   // 0.5-1.5 segundos (frenético)
    case VOID_FIREFLY: return random(800, 2000);      // 0.8-2 segundos (pulsos cortos)
    case STATIC_BUG: return random(2000, 6000);       // 2-6 segundos (estático variable)
    default: return 0; // Sin límite para insectos conservados
  }
}

// Función para obtener duración de pausa obligatoria
unsigned long getMandatoryPauseDuration(BugType insect) {
  switch (insect) {
    case CHAOS_CRICKET: return random(3000, 8000);    // 3-8 segundos de pausa
    case GLITCH_CICADA: return random(2000, 5000);    // 2-5 segundos
    case ERRATIC_DRAGONFLY: return random(4000, 10000); // 4-10 segundos
    case STUTTER_BEE: return random(2000, 6000);      // 2-6 segundos
    case RANDOM_WASP: return random(5000, 12000);     // 5-12 segundos
    case NIGHT_CRAWLER: return random(6000, 15000);   // 6-15 segundos (nocturno largo)
    case PHANTOM_MOTH: return random(4000, 12000);    // 4-12 segundos (fantasmal)
    case CHAOS_BEETLE: return random(3000, 8000);     // 3-8 segundos
    case FRENZY_MOSQUITO: return random(1000, 3000);  // 1-3 segundos (rápido)
    case VOID_FIREFLY: return random(8000, 20000);    // 8-20 segundos (vacío profundo)
    case STATIC_BUG: return random(3000, 9000);       // 3-9 segundos
    default: return 0;
  }
}

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
  
  // Inicializar variables de pausas obligatorias
  inMandatoryPause = false;
  pauseStartTime = 0;
  pauseDuration = 0;
  sequenceStartTime = millis();
  
  // Configurar duración máxima de secuencia si el insecto necesita pausas
  if (needsMandatoryPauses(currentBug)) {
    maxSequenceDuration = getMaxSequenceDuration(currentBug);
    Serial.print("Insecto caótico - Secuencia máx: ");
    Serial.print(maxSequenceDuration);
    Serial.println("ms");
  } else {
    maxSequenceDuration = 0; // Sin límite para insectos conservados
  }
  
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
  
  // Mostrar escala musical asignada
  String scaleName = "";
  const int* scale = getInsectScale(currentBug);
  if (scale == scaleCMajor) scaleName = "Do Mayor";
  else if (scale == scaleAMinor) scaleName = "La menor";
  else if (scale == scaleGMajor) scaleName = "Sol Mayor";
  else if (scale == scaleEMinor) scaleName = "Mi menor";
  else if (scale == scaleDMajor) scaleName = "Re Mayor";
  else if (scale == scaleBMinor) scaleName = "Si menor";
  else if (scale == scaleFMajor) scaleName = "Fa Mayor";
  else if (scale == scaleCSMinor) scaleName = "Do# menor";
  else if (scale == scalePentatonic) scaleName = "Pentatónica";
  else if (scale == scaleBlues) scaleName = "Blues";
  else if (scale == scaleChaotic) scaleName = "Cromática Caótica";
  
  Serial.print("Escala: ");
  Serial.println(scaleName);
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
  
  // MANEJO DE PAUSAS OBLIGATORIAS para insectos caóticos
  if (needsMandatoryPauses(currentBug)) {
    // Si estamos en pausa obligatoria
    if (inMandatoryPause) {
      if (currentTime - pauseStartTime >= pauseDuration) {
        // Terminar pausa obligatoria
        inMandatoryPause = false;
        sequenceStartTime = currentTime; // Reiniciar tiempo de secuencia
        maxSequenceDuration = getMaxSequenceDuration(currentBug); // Nueva duración
        Serial.print("*** FIN PAUSA - Nueva secuencia: ");
        Serial.print(maxSequenceDuration);
        Serial.println("ms ***");
      } else {
        // Mantener silencio durante la pausa
        noTone(PIEZO_PIN);
        digitalWrite(PIEZO_PIN, LOW);
        return;
      }
    }
    
    // Verificar si necesitamos iniciar pausa obligatoria
    if (!inMandatoryPause && (currentTime - sequenceStartTime >= maxSequenceDuration)) {
      // Iniciar pausa obligatoria
      inMandatoryPause = true;
      pauseStartTime = currentTime;
      pauseDuration = getMandatoryPauseDuration(currentBug);
      soundActive = false;
      noTone(PIEZO_PIN);
      digitalWrite(PIEZO_PIN, LOW);
      
      Serial.print("*** INICIO PAUSA OBLIGATORIA: ");
      Serial.print(pauseDuration);
      Serial.println("ms ***");
      return;
    }
  }
  
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
  
  // Añadir probabilidad de SILENCIO LARGO (pero menor para insectos caóticos con pausas)
  int silenceProbability = needsMandatoryPauses(currentBug) ? (bug.stochasticFactor * 15) : (bug.stochasticFactor * 30);
  if (random(100) < silenceProbability) {
    actualInterval = random(1000, 5000); // Silencios más cortos si ya hay pausas obligatorias
  }
  
  if (currentTime - lastSoundTime >= actualInterval) {
    // Iniciar nuevo sonido
    startNewSound(currentTime, bug);
    
    lastSoundTime = currentTime;
    soundCounter++;
    
    // Debug menos frecuente para reducir interferencia
    if (soundCounter % 25 == 0) {
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
  const int* scale = getInsectScale(currentBug);
  int freq;
  
  // Variaciones por insecto usando escalas musicales
  switch (currentBug) {
    case CHAOS_CRICKET:
      // Patrones erráticos en Do Mayor
      freq = scale[random(0, scaleLength)];
      tone(PIEZO_PIN, freq, random(10, 100));
      break;
      
    case GRASSHOPPER_GREEN:
      // CONSERVADO - Clicks rápidos en Sol Mayor
      freq = scale[random(0, scaleLength)];
      tone(PIEZO_PIN, freq, 30);
      break;
      
    case PHANTOM_MOTH:
      // Clicks fantasmales en La menor
      if (random(100) < 60) {
        freq = scale[random(0, scaleLength)];
        tone(PIEZO_PIN, freq, random(5, 80));
      }
      break;
      
    case CHAOS_BEETLE:
      // Percusión caótica en Re Mayor (graves y agudos)
      freq = scale[random(0, 6)]; // Preferir notas más graves
      tone(PIEZO_PIN, freq, random(50, 300));
      break;
      
    case MANTIS_RELIGIOUS:
      // CONSERVADO - Clicks precisos en Pentatónica
      freq = scale[random(6, scaleLength)]; // Preferir notas agudas
      tone(PIEZO_PIN, freq, 60);
      break;
      
    case ANT_RED:
      // CONSERVADO - Patrones en grupos en Sol Mayor
      freq = scale[random(0, scaleLength)];
      tone(PIEZO_PIN, freq, 40);
      break;
      
    case VOID_FIREFLY:
      // Pulsos melancólicos en La menor
      if (random(100) < 30) {
        freq = scale[random(0, scaleLength)];
        tone(PIEZO_PIN, freq, random(50, 150));
      }
      break;
      
    default:
      // Sonido en escala asignada
      freq = scale[random(0, scaleLength)];
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
  const int* scale = getInsectScale(currentBug);
  
  switch (currentBug) {
    case BUMBLEBEE:
      // CONSERVADO - FM muy grave en Si menor
      {
        float modulator = sin(2 * PI * bug.modulatorFreq * timeElapsed);
        // Usar notas graves de la escala como base
        int baseNote = scale[random(0, 4)]; // Primeras 4 notas (más graves)
        int currentFreq = baseNote + (int)(bug.modulationIndex * baseNote * modulator * 0.1); // Modulación sutil
        currentFreq = constrain(currentFreq, 50, 500);
        tone(PIEZO_PIN, currentFreq, 50);
      }
      break;
      
    case STUTTER_BEE:
      // FM entrecortado en Fa Mayor con pausas
      {
        // Crear "tartamudeo" con modulación irregular
        float stutter = sin(timeElapsed * 15) > 0.3 ? 1.0 : 0.0; // On/off irregular
        if (stutter > 0) {
          int baseNote = scale[random(2, 8)]; // Notas medias de Fa Mayor
          float modulator = sin(2 * PI * bug.modulatorFreq * timeElapsed * random(80, 120) / 100.0);
          int currentFreq = baseNote + (int)(bug.modulationIndex * baseNote * modulator * 0.2);
          currentFreq = constrain(currentFreq, 200, 1200);
          tone(PIEZO_PIN, currentFreq, 30);
        } else {
          noTone(PIEZO_PIN);
        }
      }
      break;
      
    case FRENZY_MOSQUITO:
      // FM caótico muy rápido en Cromática Caótica
      {
        int baseNote = scale[random(0, scaleLength)];
        float chaosModulator = sin(2 * PI * bug.modulatorFreq * timeElapsed * random(50, 200) / 100.0);
        float noiseModulator = (random(200) - 100) / 100.0 * 0.3; // Ruido adicional
        int currentFreq = baseNote + (int)((bug.modulationIndex + noiseModulator) * baseNote * chaosModulator * 0.3);
        currentFreq = constrain(currentFreq, 800, 4000);
        tone(PIEZO_PIN, currentFreq, 20);
      }
      break;
      
    default:
      // FM caótico genérico usando escala asignada
      {
        int baseNote = scale[random(0, scaleLength)];
        float wildModulator = sin(2 * PI * random(50, 500) * timeElapsed);
        int currentFreq = baseNote + (int)(wildModulator * baseNote * 0.2); // Modulación del 20%
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

// Función para generar ruido blanco MUSICAL sin bloqueos
void updateChaoticNoise(unsigned long currentTime, BugCharacteristics bug) {
  // Generar LFSR en tiempo real
  unsigned bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5)) & 1;
  lfsr = (lfsr >> 1) | (bit << 15);
  
  const int* scale = getInsectScale(currentBug);
  
  switch (currentBug) {
    case GLITCH_CICADA:
      // Ruido glitch usando Cromática Caótica
      {
        // Usar LFSR para seleccionar nota de la escala + variación
        int scaleNote = scale[lfsr % scaleLength];
        int variation = (lfsr % 200) - 100; // ±100Hz de variación
        int noiseFreq = scaleNote + variation;
        if (random(100) < 70) { // 70% probabilidad de sonido
          tone(PIEZO_PIN, noiseFreq, random(5, 30));
        }
      }
      break;
      
    case ERRATIC_DRAGONFLY:
      // Aleteo errático usando Blues
      {
        int scaleNote = scale[lfsr % scaleLength];
        int variation = (lfsr % 300) - 150; // ±150Hz para efecto de aleteo
        int noiseFreq = scaleNote + variation;
        if (random(100) < 60) { // 60% probabilidad
          tone(PIEZO_PIN, noiseFreq, random(10, 80));
        }
      }
      break;
      
    case NIGHT_CRAWLER:
      // Ruido nocturno en Do# menor
      {
        // Preferir notas graves de la escala
        int scaleNote = scale[lfsr % 6]; // Solo primeras 6 notas (graves)
        int variation = (lfsr % 100) - 50; // Variación mínima
        int noiseFreq = scaleNote + variation;
        if (random(100) < 40) { // Solo 40% probabilidad (más silencios)
          tone(PIEZO_PIN, noiseFreq, random(20, 150));
        }
      }
      break;
      
    case STATIC_BUG:
      // Estático caótico usando Cromática Caótica
      {
        int scaleNote = scale[lfsr % scaleLength];
        int variation = (lfsr % 400) - 200; // ±200Hz de variación extrema
        int noiseFreq = scaleNote + variation;
        if (random(100) < 85) { // 85% probabilidad (muy activo)
          tone(PIEZO_PIN, noiseFreq, random(3, 50));
        }
      }
      break;
      
    default:
      // Ruido musical genérico usando escala asignada
      {
        int scaleNote = scale[lfsr % scaleLength];
        int variation = (lfsr % 150) - 75; // ±75Hz de variación
        int noiseFreq = scaleNote + variation;
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

// Función para actualizar sonidos híbridos MUSICALES sin bloqueos
void updateHybridChaos(unsigned long currentTime, BugCharacteristics bug) {
  float timeElapsed = (currentTime - soundStartTime) / 1000.0;
  const int* scale = getInsectScale(currentBug);
  
  switch (currentBug) {
    case CHAOS_CRICKET:
      // Combinar tonos de Do Mayor con ruido musical
      {
        if (random(100) < 60) {
          // Tono caótico en Do Mayor
          int freq = scale[random(0, scaleLength)];
          tone(PIEZO_PIN, freq, random(15, 80));
        } else {
          // Ruido musical basado en escala
          unsigned bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5)) & 1;
          lfsr = (lfsr >> 1) | (bit << 15);
          int scaleNote = scale[lfsr % scaleLength];
          int noiseFreq = scaleNote + ((lfsr % 200) - 100);
          tone(PIEZO_PIN, noiseFreq, random(5, 40));
        }
      }
      break;
      
    case RANDOM_WASP:
      // Secuencias caóticas en Mi menor alternando técnicas
      {
        int technique = random(3);
        switch (technique) {
          case 0: // FM musical
            {
              int baseNote = scale[random(0, scaleLength)];
              float wildMod = sin(timeElapsed * random(10, 50));
              int freq = baseNote + (int)(wildMod * baseNote * 0.3);
              tone(PIEZO_PIN, freq, random(10, 60));
            }
            break;
          case 1: // Ruido musical
            {
              int scaleNote = scale[lfsr % scaleLength];
              int noiseFreq = scaleNote + ((lfsr % 300) - 150);
              tone(PIEZO_PIN, noiseFreq, random(8, 45));
            }
            break;
          case 2: // Clicks en escala
            {
              int freq = scale[random(6, scaleLength)]; // Notas agudas
              tone(PIEZO_PIN, freq, random(5, 25));
            }
            break;
        }
      }
      break;
      
    case STUTTER_BEE:
      // FM tartamudo en Fa Mayor + ruido musical ocasional
      {
        if (random(100) < 75) {
          // FM tartamudo musical
          float stutter = sin(timeElapsed * 12) > 0.2 ? 1.0 : 0.0;
          if (stutter > 0) {
            int baseNote = scale[random(3, 9)]; // Notas medias de Fa Mayor
            float mod = sin(2 * PI * bug.modulatorFreq * timeElapsed);
            int freq = baseNote + (int)(bug.modulationIndex * baseNote * mod * 0.15);
            tone(PIEZO_PIN, freq, 25);
          }
        } else {
          // Ruido musical ocasional
          int scaleNote = scale[lfsr % scaleLength];
          int noiseFreq = scaleNote + ((lfsr % 150) - 75);
          tone(PIEZO_PIN, noiseFreq, random(10, 30));
        }
      }
      break;
      
    case CHAOS_BEETLE:
      // Percusión en Re Mayor + glitches musicales
      {
        if (random(100) < 80) {
          // Percusión musical (notas graves)
          int freq = scale[random(0, 4)]; // Notas graves de Re Mayor
          tone(PIEZO_PIN, freq, random(30, 200));
        } else {
          // Glitch musical (notas agudas)
          int glitchFreq = scale[random(8, scaleLength)]; // Notas agudas
          tone(PIEZO_PIN, glitchFreq, random(3, 15));
        }
      }
      break;
      
    default:
      // Híbrido musical genérico
      {
        int mode = random(4);
        switch (mode) {
          case 0: // Tono de escala
            tone(PIEZO_PIN, scale[random(0, scaleLength)], random(5, 100)); 
            break;
          case 1: // Ruido musical
            {
              int scaleNote = scale[lfsr % scaleLength];
              tone(PIEZO_PIN, scaleNote + ((lfsr % 200) - 100), random(8, 50));
            }
            break;
          case 2: // FM musical
            {
              int baseNote = scale[random(0, scaleLength)];
              int freq = baseNote + (int)(sin(timeElapsed * 20) * baseNote * 0.2);
              tone(PIEZO_PIN, freq, 30);
            }
            break;
          case 3: /* Silencio ocasional */ 
            break;
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