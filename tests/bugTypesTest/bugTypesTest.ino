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

// 5 Insectos seleccionados con características específicas
enum BugType {
  GRASSHOPPER_GREEN = 0,    // Saltamontes Verde - Clicks con pausas cada 8-22 notas
  BUMBLEBEE = 1,            // Moscardón - FM grave profundo (conservado)
  PHANTOM_MOTH = 2,         // Polilla Fantasma - Código anterior recuperado
  MANTIS_RELIGIOUS = 3,     // Mantis Religiosa - Polirritmias sin escala
  ANT_RED = 4               // Hormiga Roja - Grupos sin escala
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
BugType currentBug = GRASSHOPPER_GREEN;
const int TOTAL_BUGS = 5;
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

// Escalas musicales menos comunes para sonidos más interesantes
// Frigia (Do Frigio): Do, Reb, Mib, Fa, Sol, Lab, Sib
const int scalePhrygian[] = {262, 277, 311, 349, 392, 415, 466, 523, 554, 622, 698, 784};
// Eólica (La Eólico/Natural menor): La, Si, Do, Re, Mi, Fa, Sol
const int scaleAeolian[] = {220, 247, 262, 294, 330, 349, 392, 440, 494, 523, 587, 659};
// Dórica (Re Dórico): Re, Mi, Fa, Sol, La, Si, Do
const int scaleDorian[] = {147, 165, 175, 196, 220, 247, 262, 294, 330, 349, 392, 440};
// Mixolidia (Sol Mixolidio): Sol, La, Si, Do, Re, Mi, Fa
const int scaleMixolydian[] = {196, 220, 247, 262, 294, 330, 349, 392, 440, 494, 523, 587};
// Lidia (Fa Lidio): Fa, Sol, La, Si, Do, Re, Mi
const int scaleLydian[] = {175, 196, 220, 247, 262, 294, 330, 349, 392, 440, 494, 523};
// Locria (Si Locrio): Si, Do, Re, Mib, Fa, Sol, Lab
const int scaleLocrian[] = {123, 131, 147, 156, 175, 196, 208, 247, 262, 294, 311, 349};
// Harmónica menor (La harmónica): La, Si, Do, Re, Mi, Fa, Sol#
const int scaleHarmonicMinor[] = {220, 247, 262, 294, 330, 349, 415, 440, 494, 523, 587, 659};

const int scaleLength = 12; // Todas las escalas tienen 12 notas

// Función para obtener la escala musical de cada insecto (solo los que usan escalas)
const int* getInsectScale(BugType insect) {
  switch (insect) {
    case GRASSHOPPER_GREEN: return scalePhrygian;    // Frigia - exótico/hierba
    case BUMBLEBEE: return scaleAeolian;             // Eólica - grave/melancólico
    case PHANTOM_MOTH: return scaleDorian;           // Dórica - fantasmal/etéreo
    // Mantis y Hormiga NO usan escalas (polirritmias libres)
    default: return scalePhrygian; // Frigia por defecto
  }
}

// Función para determinar si un insecto necesita pausas obligatorias
bool needsMandatoryPauses(BugType insect) {
  switch (insect) {
    case GRASSHOPPER_GREEN:  // Pausas cada 8-22 notas
    case PHANTOM_MOTH:       // Pausas frecuentes cortas
      return true;
    default:
      return false; // Moscardón, Mantis y Hormiga sin pausas obligatorias
  }
}

// Función para obtener duración máxima de secuencia antes de pausa obligatoria
unsigned long getMaxSequenceDuration(BugType insect) {
  switch (insect) {
    case GRASSHOPPER_GREEN: return random(800, 2000);  // 0.8-2 segundos (8-22 notas)
    case PHANTOM_MOTH: return random(600, 1500);       // 0.6-1.5 segundos (frecuente)
    default: return 0; // Sin límite para otros insectos
  }
}

// Función para obtener duración de pausa obligatoria (más cortas)
unsigned long getMandatoryPauseDuration(BugType insect) {
  switch (insect) {
    case GRASSHOPPER_GREEN: return random(300, 800);   // 0.3-0.8 segundos (corta)
    case PHANTOM_MOTH: return random(200, 600);        // 0.2-0.6 segundos (muy corta)
    default: return 0;
  }
}

// Características para los 5 insectos seleccionados (simplificados)
BugCharacteristics bugData[5] = {
  // Nombre, Síntesis, BaseFreq, ModFreq, ModIndex, MinInt, MaxInt, BurstLen, Stochastic, Habitat, Rhythm
  {"Saltamontes Verde", 0, 12000, 0, 0.0, 80, 200, 12, 0.3, "Hierba", "Clicks con pausas"},
  {"Moscardón", 1, 150, 30, 0.4, 4000, 8000, 1, 0.1, "Campo", "FM grave profundo"},
  {"Polilla Fantasma", 0, 1500, 0, 0.0, 300, 1200, 4, 0.6, "Sombras", "Clicks etéreos"},
  {"Mantis Religiosa", 0, 6000, 0, 0.0, 200, 2000, 3, 0.4, "Arbusto", "Polirritmias libres"},
  {"Hormiga Roja", 0, 8000, 0, 0.0, 150, 400, 5, 0.2, "Colonia", "Grupos libres"}
};

void setup() {
  Serial.begin(115200);
  Serial.println("=== BugTypesTest v4.0 - 5 INSECTOS SIMPLIFICADOS ===");
  Serial.println("Escalas modales menos comunes + Polirritmias libres");
  Serial.println("Pausas frecuentes pero cortas - Sonidos menos caóticos");
  Serial.println("Presiona el botón 1 para cambiar de insecto");
  Serial.println("Seleccionados: Saltamontes, Moscardón, Polilla, Mantis, Hormiga");
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
  
  // Mostrar escala musical asignada (solo para insectos que usan escalas)
  String scaleName = "";
  if (currentBug == MANTIS_RELIGIOUS || currentBug == ANT_RED) {
    scaleName = "Sin escala (Polirritmias libres)";
  } else {
    const int* scale = getInsectScale(currentBug);
    if (scale == scalePhrygian) scaleName = "Frigia";
    else if (scale == scaleAeolian) scaleName = "Eólica";
    else if (scale == scaleDorian) scaleName = "Dórica";
    else if (scale == scaleMixolydian) scaleName = "Mixolidia";
    else if (scale == scaleLydian) scaleName = "Lidia";
    else if (scale == scaleLocrian) scaleName = "Locria";
    else if (scale == scaleHarmonicMinor) scaleName = "Harmónica menor";
    else scaleName = "Frigia (por defecto)";
  }
  
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
  int freq;
  
  // Comportamientos específicos por insecto
  switch (currentBug) {
    case GRASSHOPPER_GREEN:
      {
        // Clicks en escala Frigia con pausas cada 8-22 notas
        const int* scale = getInsectScale(currentBug);
        freq = scale[random(0, scaleLength)];
        tone(PIEZO_PIN, freq, 40);
      }
      break;
      
    case BUMBLEBEE:
      // Este caso no se ejecuta porque usa FM, pero por seguridad
      freq = 150;
      tone(PIEZO_PIN, freq, 100);
      break;
      
    case PHANTOM_MOTH:
      {
        // Clicks etéreos en escala Dórica (código anterior recuperado)
        const int* scale = getInsectScale(currentBug);
        if (random(100) < 70) { // 70% probabilidad de sonido
          freq = scale[random(0, scaleLength)];
          tone(PIEZO_PIN, freq, random(10, 80));
        }
      }
      break;
      
    case MANTIS_RELIGIOUS:
      {
        // Polirritmias libres SIN ESCALA - Clicks precisos con patrones complejos
        freq = random(4000, 8000); // Frecuencias agudas libres
        
        // Patrones polirrítmicos complejos
        static int rhythmPattern = 0;
        static int polyrhythmCounter = 0;
        
        switch (rhythmPattern) {
          case 0: // Patrón 3 contra 2
            if (polyrhythmCounter % 3 == 0) tone(PIEZO_PIN, freq, 30);
            if (polyrhythmCounter % 2 == 0) tone(PIEZO_PIN, freq + 200, 20);
            break;
          case 1: // Patrón 5 contra 3
            if (polyrhythmCounter % 5 == 0) tone(PIEZO_PIN, freq, 40);
            if (polyrhythmCounter % 3 == 0) tone(PIEZO_PIN, freq - 300, 25);
            break;
          case 2: // Patrón 7 contra 4
            if (polyrhythmCounter % 7 == 0) tone(PIEZO_PIN, freq + 100, 35);
            if (polyrhythmCounter % 4 == 0) tone(PIEZO_PIN, freq - 100, 30);
            break;
        }
        
        polyrhythmCounter++;
        if (polyrhythmCounter > 21) { // Cambiar patrón cada 21 beats
          polyrhythmCounter = 0;
          rhythmPattern = (rhythmPattern + 1) % 3;
        }
      }
      break;
      
    case ANT_RED:
      {
        // Grupos libres SIN ESCALA - Patrones sociales complejos
        static int groupSize = random(3, 8);
        static int groupCounter = 0;
        
        freq = random(6000, 10000); // Frecuencias agudas libres
        
        // Simular comunicación de grupo
        if (groupCounter < groupSize) {
          // Dentro del grupo - frecuencias relacionadas
          int baseFreq = 7000 + (groupCounter * 200);
          tone(PIEZO_PIN, baseFreq + random(-100, 100), 35);
          groupCounter++;
        } else {
          // Fin de grupo - pausa y nuevo grupo
          groupCounter = 0;
          groupSize = random(3, 8);
          // No hacer sonido esta vez (pausa de grupo)
        }
      }
      break;
      
    default:
      // Sonido genérico
      freq = random(1000, 5000);
      tone(PIEZO_PIN, freq, random(20, 100));
      break;
  }
}

// Función para iniciar FM sin bloqueos
void startChaoticFM(BugCharacteristics bug) {
  // El sonido será actualizado en updateChaoticFM
}

// Función para actualizar FM de forma continua sin bloqueos (solo Moscardón)
void updateChaoticFM(unsigned long currentTime, BugCharacteristics bug) {
  float timeElapsed = (currentTime - soundStartTime) / 1000.0;
  
  switch (currentBug) {
    case BUMBLEBEE:
      // FM grave profundo en escala Eólica
      {
        const int* scale = getInsectScale(currentBug);
        float modulator = sin(2 * PI * bug.modulatorFreq * timeElapsed);
        // Usar notas graves de la escala Eólica
        int baseNote = scale[random(0, 6)]; // Primeras 6 notas (más graves)
        int currentFreq = baseNote + (int)(bug.modulationIndex * baseNote * modulator * 0.15);
        currentFreq = constrain(currentFreq, 80, 400);
        tone(PIEZO_PIN, currentFreq, 60);
      }
      break;
      
    default:
      // Caso por defecto (no debería ejecutarse)
      tone(PIEZO_PIN, 200, 50);
      break;
  }
}

// Función para iniciar ruido blanco sin bloqueos
void startChaoticNoise(BugCharacteristics bug) {
  // El ruido será generado en updateChaoticNoise
}

// Función para ruido blanco (no usada en estos 5 insectos, pero mantenida por compatibilidad)
void updateChaoticNoise(unsigned long currentTime, BugCharacteristics bug) {
  // Caso genérico simple
  int noiseFreq = random(500, 3000);
  if (random(100) < 50) {
    tone(PIEZO_PIN, noiseFreq, random(10, 50));
  }
}

// Función para iniciar sonidos híbridos caóticos
void startHybridChaos(BugCharacteristics bug) {
  // Los híbridos combinan múltiples técnicas en tiempo real
}

// Función para sonidos híbridos (no usada en estos 5 insectos, pero mantenida por compatibilidad)
void updateHybridChaos(unsigned long currentTime, BugCharacteristics bug) {
  // Caso genérico simple
  int freq = random(1000, 4000);
  tone(PIEZO_PIN, freq, random(20, 80));
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