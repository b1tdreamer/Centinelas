/*
 * BugTypesTest v1.1 - 15 Tipos de Insectos con Síntesis Avanzada
 * Basado en características reales de insectos y patrones naturales
 * 
 * Técnicas de síntesis utilizadas:
 * - Ruido Blanco (LFSR)
 * - Síntesis FM (Modulación de Frecuencia)
 * - Tonos Simples con patrones complejos
 * - Ritmos estocásticos y polirritmias naturales
 * 
 * Botón 1 (GPIO36): Cambiar al siguiente tipo de insecto
 * Cada insecto suena en bucle continuo hasta cambio manual
 */

#include <math.h>

// Pines
#define PIEZO_PIN 21        // Pin del piezoeléctrico
#define BUTTON_1_PIN 36     // Botón para cambiar insecto (ADC)
#define LED_PIN 2           // LED indicador

// 15 Tipos de insectos basados en especies reales
enum BugType {
  CRICKET_COMMON = 0,       // Grillo Común - Gryllus campestris - Ritmo regular 4/4
  CICADA_MOUNTAIN = 1,      // Cigarra de Montaña - Cicada montana - FM compleja, frecuencias altas
  GRASSHOPPER_GREEN = 2,    // Saltamontes Verde - Tettigonia viridissima - Clicks rápidos
  DRAGONFLY_BLUE = 3,       // Libélula Azul - Aeshna cyanea - Ruido blanco suave (aleteo)
  BEE_HONEY = 4,            // Abeja Melífera - Apis mellifera - FM grave constante
  WASP_COMMON = 5,          // Avispa Común - Vespula vulgaris - FM agudo agresivo
  BUMBLEBEE = 6,            // Moscardón - Bombus terrestris - FM muy grave
  COCKROACH_BLACK = 7,      // Cucaracha Negra - Blatta orientalis - Clicks esporádicos
  MOTH_NIGHT = 8,           // Mariposa Nocturna - Moth - Ruido blanco ultra suave
  BEETLE_DUNG = 9,          // Escarabajo Pelotero - Scarabeus - Percusión grave
  MANTIS_RELIGIOUS = 10,    // Mantis Religiosa - Mantis religiosa - Clicks precisos
  MOSQUITO_COMMON = 11,     // Mosquito Común - Culex pipiens - FM muy agudo
  ANT_RED = 12,             // Hormiga Roja - Formica rufa - Clicks en grupos
  FIREFLY = 13,             // Luciérnaga - Lampyris noctiluca - Pulsos suaves
  STINK_BUG_GREEN = 14      // Chinche Verde - Nezara viridula - Ruido estocástico
};

// Variables globales
BugType currentBug = CRICKET_COMMON;
const int TOTAL_BUGS = 15;
unsigned long bugStartTime = 0;
bool bugActive = true;

// Variables para botón
bool buttonPressed = false;
unsigned long buttonPressStart = 0;

// Variables para síntesis
unsigned long lastSoundTime = 0;
unsigned long lastModulationTime = 0;
int soundCounter = 0;

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

// Características basadas en insectos reales y sus hábitats
BugCharacteristics bugData[15] = {
  // Nombre, Síntesis, BaseFreq, ModFreq, ModIndex, MinInt, MaxInt, BurstLen, Stochastic, Habitat, Rhythm
  {"Grillo Común", 0, 4000, 0, 0.0, 800, 1200, 3, 0.1, "Pradera", "4/4 Regular"},
  {"Cigarra de Montaña", 1, 8000, 1000, 0.8, 2000, 8000, 1, 0.3, "Bosque", "Continuo"},
  {"Saltamontes Verde", 0, 12000, 0, 0.0, 50, 150, 5, 0.4, "Hierba", "Staccato"},
  {"Libélula Azul", 2, 0, 0, 0.0, 200, 500, 8, 0.6, "Agua", "Irregular"},
  {"Abeja Melífera", 1, 300, 50, 0.6, 3000, 6000, 1, 0.2, "Flores", "Continuo"},
  {"Avispa Común", 1, 800, 200, 1.2, 1000, 3000, 2, 0.5, "Jardín", "Agresivo"},
  {"Moscardón", 1, 150, 30, 0.4, 4000, 8000, 1, 0.1, "Campo", "Grave profundo"},
  {"Cucaracha Negra", 0, 200, 0, 0.0, 2000, 10000, 1, 0.8, "Interior", "Esporádico"},
  {"Mariposa Nocturna", 2, 0, 0, 0.0, 1000, 3000, 3, 0.9, "Noche", "Etéreo"},
  {"Escarabajo Pelotero", 0, 80, 0, 0.0, 1500, 4000, 4, 0.3, "Tierra", "Percusivo"},
  {"Mantis Religiosa", 0, 6000, 0, 0.0, 3000, 8000, 2, 0.2, "Arbusto", "Preciso"},
  {"Mosquito Común", 1, 1500, 800, 0.9, 100, 300, 1, 0.4, "Húmedo", "Agudo molesto"},
  {"Hormiga Roja", 0, 8000, 0, 0.0, 200, 500, 6, 0.3, "Colonia", "En grupos"},
  {"Luciérnaga", 0, 2000, 0, 0.0, 2000, 5000, 1, 0.6, "Jardín nocturno", "Pulsos"},
  {"Chinche Verde", 2, 0, 0, 0.0, 500, 1500, 4, 0.7, "Plantas", "Estocástico"}
};

void setup() {
  Serial.begin(115200);
  Serial.println("=== BugTypesTest v1.1 - 15 Tipos de Insectos ===");
  Serial.println("Síntesis avanzada basada en características reales");
  Serial.println("Presiona el botón 1 para cambiar de insecto");
  Serial.println("Variables corregidas - Solo caracteres ASCII válidos");
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
  int buttonValue = analogRead(BUTTON_1_PIN);
  bool buttonState = (buttonValue < 100);
  
  if (buttonState && !buttonPressed) {
    buttonPressed = true;
    buttonPressStart = currentTime;
  } else if (!buttonState && buttonPressed) {
    buttonPressed = false;
    
    // Cambiar al siguiente insecto
    nextBug();
  }
}

void nextBug() {
  stopBug();
  currentBug = (BugType)((currentBug + 1) % TOTAL_BUGS);
  delay(300); // Pausa entre insectos
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
  
  // LED parpadeo para indicar cambio
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, LOW);
    delay(80);
    digitalWrite(LED_PIN, HIGH);
    delay(80);
  }
}

void stopBug() {
  bugActive = false;
  noTone(PIEZO_PIN);
}

void executeBugSound(unsigned long currentTime) {
  BugCharacteristics bug = bugData[currentBug];
  
  // Calcular intervalo con factor estocástico
  int baseInterval = random(bug.minInterval, bug.maxInterval);
  int stochasticVariation = (int)(baseInterval * bug.stochasticFactor * (random(200) - 100) / 100.0);
  int actualInterval = baseInterval + stochasticVariation;
  actualInterval = max(actualInterval, 50); // Mínimo 50ms
  
  if (currentTime - lastSoundTime >= actualInterval) {
    // Generar sonido según el tipo de síntesis
    switch (bug.synthesisType) {
      case 0: // Tonos simples
        generateSimpleTone(bug);
        break;
      case 1: // Síntesis FM
        generateFMSynthesis(bug);
        break;
      case 2: // Ruido blanco
        generateWhiteNoise(bug);
        break;
      case 3: // Híbrido
        generateHybridSound(bug);
        break;
    }
    
    lastSoundTime = currentTime;
    soundCounter++;
    
    // Debug cada cierto número de sonidos
    if (soundCounter % 10 == 0) {
      Serial.print(bug.name);
      Serial.print(" - Sonido #");
      Serial.print(soundCounter);
      Serial.print(" | Intervalo: ");
      Serial.print(actualInterval);
      Serial.println("ms");
    }
  }
}

void generateSimpleTone(BugCharacteristics bug) {
  // Frecuencia base con variación según el insecto
  int freq = bug.baseFreq;
  
  // Variaciones específicas por insecto
  switch (currentBug) {
    case CRICKET_COMMON:
      // Patrón 4/4 regular con ligera variación
      freq += random(-100, 100);
      tone(PIEZO_PIN, freq, 150);
      break;
      
    case GRASSHOPPER_GREEN:
      // Clicks rápidos y cortos
      freq += random(-500, 500);
      tone(PIEZO_PIN, freq, 30);
      break;
      
    case COCKROACH_BLACK:
      // Clicks esporádicos graves
      freq = random(150, 250);
      tone(PIEZO_PIN, freq, 80);
      break;
      
    case BEETLE_DUNG:
      // Percusión grave
      freq = random(60, 120);
      tone(PIEZO_PIN, freq, 200);
      break;
      
    case MANTIS_RELIGIOUS:
      // Clicks precisos y agudos
      freq += random(-200, 200);
      tone(PIEZO_PIN, freq, 60);
      break;
      
    case ANT_RED:
      // Patrones en grupos
      for (int i = 0; i < random(3, 7); i++) {
        tone(PIEZO_PIN, freq + random(-1000, 1000), 40);
        delay(50);
      }
      break;
      
    case FIREFLY:
      {
        // Pulsos suaves con patrón fibonacci
        int fibPattern[] = {1, 1, 2, 3, 5, 8};
        int pulses = fibPattern[random(0, 6)];
        for (int i = 0; i < pulses; i++) {
          tone(PIEZO_PIN, freq, 100);
          delay(120);
        }
      }
      break;
      
    default:
      tone(PIEZO_PIN, freq, 100);
      break;
  }
}

void generateFMSynthesis(BugCharacteristics bug) {
  unsigned long startTime = millis();
  int duration = 0;
  
  switch (currentBug) {
    case CICADA_MOUNTAIN:
      // FM compleja continua
      duration = random(1000, 3000);
      break;
    case BEE_HONEY:
      // FM grave constante
      duration = random(2000, 5000);
      break;
    case WASP_COMMON:
      // FM agudo agresivo
      duration = random(500, 1500);
      break;
    case BUMBLEBEE:
      // FM muy grave
      duration = random(3000, 6000);
      break;
    case MOSQUITO_COMMON:
      // FM agudo molesto
      duration = random(200, 800);
      break;
    default:
      duration = 1000;
      break;
  }
  
  // Generar FM durante la duración especificada
  while (millis() - startTime < duration) {
    float time = (millis() - startTime) / 1000.0;
    float modulator = sin(2 * PI * bug.modulatorFreq * time);
    int currentFreq = bug.baseFreq + (int)(bug.modulationIndex * bug.baseFreq * modulator);
    currentFreq = constrain(currentFreq, 50, 8000);
    
    tone(PIEZO_PIN, currentFreq, 20);
    delay(10);
  }
  noTone(PIEZO_PIN);
}

void generateWhiteNoise(BugCharacteristics bug) {
  int duration = 0;
  int pulseDelay = 50;
  
  switch (currentBug) {
    case DRAGONFLY_BLUE:
      // Ruido suave como aleteo
      duration = random(200, 600);
      pulseDelay = 60;
      break;
    case MOTH_NIGHT:
      // Ruido ultra suave etéreo
      duration = random(300, 800);
      pulseDelay = 80;
      break;
    case STINK_BUG_GREEN:
      // Ruido estocástico
      duration = random(100, 400);
      pulseDelay = random(30, 100);
      break;
    default:
      duration = 300;
      break;
  }
  
  // Generar ruido blanco usando LFSR
  unsigned long startTime = millis();
  while (millis() - startTime < duration) {
    // Algoritmo LFSR para generar secuencia pseudoaleatoria
    unsigned bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5)) & 1;
    lfsr = (lfsr >> 1) | (bit << 15);
    
    digitalWrite(PIEZO_PIN, lfsr & 1);
    delayMicroseconds(pulseDelay);
  }
  digitalWrite(PIEZO_PIN, LOW);
}

void generateHybridSound(BugCharacteristics bug) {
  // Sonidos híbridos que combinan técnicas
  // Por ahora, alternar entre diferentes tipos
  if (random(100) < 50) {
    generateSimpleTone(bug);
  } else {
    generateWhiteNoise(bug);
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