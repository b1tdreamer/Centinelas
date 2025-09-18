/*
 * PiezoSoundTest v1.0 - Pruebas de Síntesis de Sonido con Piezoeléctricos
 * Diferentes técnicas de generación de sonido para ESP32
 * 
 * Botón 1 (GPIO36): Cambiar al siguiente tipo de sonido
 * Cada prueba dura 5 segundos
 */

// Pines
#define PIEZO_PIN 21        // Pin del piezoeléctrico
#define BUTTON_1_PIN 36     // Botón para cambiar sonido (ADC)
#define LED_PIN 2           // LED indicador

// Estados de las pruebas de sonido
enum SoundTestType {
  TONE_SIMPLE = 0,          // Tonos simples
  FM_SYNTHESIS = 1,         // Síntesis FM
  WHITE_NOISE = 2,          // Ruido blanco
  FILTERED_NOISE = 3,       // Ruido filtrado
  GRANULAR = 4,             // Síntesis granular
  GLISSANDO = 5,            // Deslizamiento de frecuencia
  TREMOLO = 6,              // Modulación de amplitud
  COMPLEX_WAVE = 7,         // Ondas complejas (armónicos)
  ARPEGGIOS = 8,            // Arpegios rápidos
  RHYTHMIC_PATTERNS = 9,    // Patrones rítmicos
  INSECT_SOUNDS = 10,       // Sonidos de insectos realistas
  AMBIENT_TEXTURE = 11      // Texturas ambientales
};

// Variables globales
SoundTestType currentTest = TONE_SIMPLE;
const int TOTAL_TESTS = 12;
unsigned long testStartTime = 0;
const unsigned long TEST_DURATION = 5000; // 5 segundos por prueba
bool testActive = false;

// Variables para botón
bool buttonPressed = false;
unsigned long buttonPressStart = 0;

// Escalas y frecuencias
const int pentatonicScale[] = {
  262, 294, 330, 370, 440,    // Do4 - La4
  523, 587, 659, 740, 880,    // Do5 - La5
  1047, 1175, 1319, 1480, 1760 // Do6 - La6
};

void setup() {
  Serial.begin(115200);
  Serial.println("=== PiezoSoundTest v2.0 - Síntesis de Sonido Continua ===");
  Serial.println("Presiona el botón 1 para cambiar entre diferentes tipos de sonido");
  Serial.println("Cada prueba se reproduce en LOOP CONTINUO con sonidos extendidos");
  Serial.println("Duraciones: 50ms, 100ms, 200ms, 500ms según el tipo");
  Serial.println();
  
  // Configurar pines
  pinMode(PIEZO_PIN, OUTPUT);
  pinMode(BUTTON_1_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  
  // LED indicador
  digitalWrite(LED_PIN, HIGH);
  
  // Mostrar lista de pruebas
  printTestList();
  
  // Iniciar primera prueba
  startTest();
}

void loop() {
  unsigned long currentTime = millis();
  
  // Manejar botón
  handleButton(currentTime);
  
  // Ejecutar prueba actual (loop continuo)
  if (testActive) {
    executeCurrentTest(currentTime);
    // Nota: Las pruebas ahora se ejecutan en loop continuo hasta cambio manual
  }
  
  delay(1);
}

void printTestList() {
  Serial.println("Lista de pruebas disponibles:");
  Serial.println("0. Tonos Simples - Frecuencias puras básicas");
  Serial.println("1. Síntesis FM - Modulación de frecuencia");
  Serial.println("2. Ruido Blanco - Frecuencias aleatorias");
  Serial.println("3. Ruido Filtrado - Ruido en banda específica");
  Serial.println("4. Síntesis Granular - Texturas granulares");
  Serial.println("5. Glissando - Deslizamiento de frecuencia");
  Serial.println("6. Tremolo - Modulación de amplitud");
  Serial.println("7. Ondas Complejas - Simulación de armónicos");
  Serial.println("8. Arpegios - Secuencias rápidas");
  Serial.println("9. Patrones Rítmicos - Ritmos complejos");
  Serial.println("10. Sonidos de Insectos - Síntesis realista");
  Serial.println("11. Texturas Ambientales - Paisajes sonoros");
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
    
    // Cambiar al siguiente test
    nextTest();
  }
}

void nextTest() {
  stopTest();
  currentTest = (SoundTestType)((currentTest + 1) % TOTAL_TESTS);
  delay(500); // Pausa entre pruebas
  startTest();
}

void startTest() {
  testStartTime = millis();
  testActive = true;
  
  Serial.print("=== INICIANDO PRUEBA ");
  Serial.print((int)currentTest);
  Serial.print(": ");
  Serial.print(getTestName(currentTest));
  Serial.println(" (LOOP CONTINUO) ===");
  Serial.println("Presiona el botón para cambiar a la siguiente prueba");
  
  // LED parpadeo para indicar inicio
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, LOW);
    delay(100);
    digitalWrite(LED_PIN, HIGH);
    delay(100);
  }
}

void stopTest() {
  testActive = false;
  noTone(PIEZO_PIN);
  Serial.println("Prueba terminada\n");
}

String getTestName(SoundTestType type) {
  switch (type) {
    case TONE_SIMPLE: return "Tonos Simples";
    case FM_SYNTHESIS: return "Síntesis FM";
    case WHITE_NOISE: return "Ruido Blanco";
    case FILTERED_NOISE: return "Ruido Filtrado";
    case GRANULAR: return "Síntesis Granular";
    case GLISSANDO: return "Glissando";
    case TREMOLO: return "Tremolo";
    case COMPLEX_WAVE: return "Ondas Complejas";
    case ARPEGGIOS: return "Arpegios";
    case RHYTHMIC_PATTERNS: return "Patrones Rítmicos";
    case INSECT_SOUNDS: return "Sonidos de Insectos";
    case AMBIENT_TEXTURE: return "Texturas Ambientales";
    default: return "Desconocido";
  }
}

void executeCurrentTest(unsigned long currentTime) {
  switch (currentTest) {
    case TONE_SIMPLE:
      testToneSimple(currentTime);
      break;
    case FM_SYNTHESIS:
      testFMSynthesis(currentTime);
      break;
    case WHITE_NOISE:
      testWhiteNoise(currentTime);
      break;
    case FILTERED_NOISE:
      testFilteredNoise(currentTime);
      break;
    case GRANULAR:
      testGranular(currentTime);
      break;
    case GLISSANDO:
      testGlissando(currentTime);
      break;
    case TREMOLO:
      testTremolo(currentTime);
      break;
    case COMPLEX_WAVE:
      testComplexWave(currentTime);
      break;
    case ARPEGGIOS:
      testArpeggios(currentTime);
      break;
    case RHYTHMIC_PATTERNS:
      testRhythmicPatterns(currentTime);
      break;
    case INSECT_SOUNDS:
      testInsectSounds(currentTime);
      break;
    case AMBIENT_TEXTURE:
      testAmbientTexture(currentTime);
      break;
  }
}

// ===== IMPLEMENTACIONES DE PRUEBAS =====

void testToneSimple(unsigned long currentTime) {
  static unsigned long lastChange = 0;
  static int currentNote = 0;
  
  if (currentTime - lastChange > 600) { // Intervalo entre tonos
    tone(PIEZO_PIN, pentatonicScale[currentNote], 500); // 0.5s duración
    currentNote = (currentNote + 1) % 15;
    lastChange = currentTime;
    Serial.print("Tono: ");
    Serial.print(pentatonicScale[currentNote]);
    Serial.println(" Hz (500ms)");
  }
}

void testFMSynthesis(unsigned long currentTime) {
  static unsigned long lastUpdate = 0;
  static unsigned long lastFreqChange = 0;
  static int carrierFreq = 440;
  
  // Cambiar frecuencia portadora cada 2 segundos
  if (currentTime - lastFreqChange > 2000) {
    carrierFreq = random(200, 800);
    lastFreqChange = currentTime;
    Serial.print("Nueva portadora FM: ");
    Serial.println(carrierFreq);
  }
  
  if (currentTime - lastUpdate > 50) { // 0.05s entre actualizaciones
    float time = (currentTime - testStartTime) / 1000.0;
    int modulatorFreq = 30;
    float modulationIndex = 0.8;
    
    // Calcular frecuencia modulada
    float modulation = sin(2 * PI * modulatorFreq * time) * modulationIndex;
    int currentFreq = carrierFreq + (int)(modulation * carrierFreq);
    currentFreq = constrain(currentFreq, 100, 2000);
    
    tone(PIEZO_PIN, currentFreq, 100); // 0.1s duración
    lastUpdate = currentTime;
  }
}

void testWhiteNoise(unsigned long currentTime) {
  static unsigned long lastUpdate = 0;
  
  if (currentTime - lastUpdate > 60) { // 0.05s + pausa
    int randomFreq = random(200, 4000);
    tone(PIEZO_PIN, randomFreq, 50); // 0.05s duración
    lastUpdate = currentTime;
    
    // Debug cada 10 tonos
    static int counter = 0;
    if (++counter % 10 == 0) {
      Serial.print("Ruido blanco: ");
      Serial.print(randomFreq);
      Serial.println(" Hz (50ms)");
    }
  }
}

void testFilteredNoise(unsigned long currentTime) {
  static unsigned long lastUpdate = 0;
  static unsigned long lastCenterChange = 0;
  static int centerFreq = 800;
  
  // Cambiar centro cada 3 segundos
  if (currentTime - lastCenterChange > 3000) {
    centerFreq = random(400, 1500);
    lastCenterChange = currentTime;
    Serial.print("Nuevo centro filtrado: ");
    Serial.print(centerFreq);
    Serial.println(" Hz");
  }
  
  if (currentTime - lastUpdate > 110) { // 0.1s + pausa
    int bandwidth = 200;
    int randomOffset = random(-bandwidth/2, bandwidth/2);
    int freq = centerFreq + randomOffset;
    freq = constrain(freq, 200, 3000);
    
    tone(PIEZO_PIN, freq, 100); // 0.1s duración
    lastUpdate = currentTime;
  }
}

void testGranular(unsigned long currentTime) {
  static unsigned long lastGrain = 0;
  static unsigned long lastBaseChange = 0;
  static int baseFreq = 600;
  
  // Cambiar frecuencia base cada 2 segundos
  if (currentTime - lastBaseChange > 2000) {
    baseFreq = random(400, 1200);
    lastBaseChange = currentTime;
    Serial.print("Nueva base granular: ");
    Serial.println(baseFreq);
  }
  
  if (currentTime - lastGrain > random(80, 150)) {
    if (random(100) < 80) { // 80% probabilidad de grano
      int grainFreq = baseFreq + random(-100, 100);
      grainFreq = constrain(grainFreq, 300, 2000);
      tone(PIEZO_PIN, grainFreq, 200); // 0.2s duración para granos
    }
    lastGrain = currentTime;
  }
}

void testGlissando(unsigned long currentTime) {
  static unsigned long lastUpdate = 0;
  static unsigned long cycleStart = 0;
  static int startFreq = 300;
  static int endFreq = 1500;
  
  // Nuevo ciclo cada 4 segundos
  if (currentTime - cycleStart > 4000) {
    startFreq = random(200, 600);
    endFreq = random(800, 2000);
    cycleStart = currentTime;
    Serial.print("Nuevo glissando: ");
    Serial.print(startFreq);
    Serial.print(" -> ");
    Serial.println(endFreq);
  }
  
  if (currentTime - lastUpdate > 100) { // 0.1s entre pasos
    float progress = ((currentTime - cycleStart) % 4000) / 4000.0;
    int currentFreq = startFreq + (int)((endFreq - startFreq) * progress);
    
    tone(PIEZO_PIN, currentFreq, 200); // 0.2s duración
    lastUpdate = currentTime;
  }
}

void testTremolo(unsigned long currentTime) {
  static unsigned long lastUpdate = 0;
  static unsigned long lastFreqChange = 0;
  static int baseFreq = 800;
  
  // Cambiar frecuencia base cada 3 segundos
  if (currentTime - lastFreqChange > 3000) {
    baseFreq = random(400, 1200);
    lastFreqChange = currentTime;
    Serial.print("Nueva base tremolo: ");
    Serial.println(baseFreq);
  }
  
  if (currentTime - lastUpdate > 150) { // Pausa entre pulsos
    float time = (currentTime - testStartTime) / 1000.0;
    float tremoloRate = 4.0; // 4 Hz
    
    // Simular amplitud variable con duración de pulsos
    float amplitude = (sin(2 * PI * tremoloRate * time) + 1) / 2;
    int pulseDuration = (int)(amplitude * 400) + 100; // 100-500ms
    
    tone(PIEZO_PIN, baseFreq, pulseDuration);
    lastUpdate = currentTime;
  }
}

void testComplexWave(unsigned long currentTime) {
  static unsigned long lastUpdate = 0;
  static unsigned long lastFundamentalChange = 0;
  static int harmonicIndex = 0;
  static int fundamental = 220;
  
  // Cambiar fundamental cada 4 segundos
  if (currentTime - lastFundamentalChange > 4000) {
    fundamental = random(150, 400);
    lastFundamentalChange = currentTime;
    Serial.print("Nuevo fundamental: ");
    Serial.println(fundamental);
  }
  
  if (currentTime - lastUpdate > 250) { // 0.2s + pausa
    int harmonics[] = {fundamental * 2, fundamental * 3, fundamental * 4, fundamental * 5}; // Armónicos
    
    int freq = (harmonicIndex == 0) ? fundamental : harmonics[harmonicIndex - 1];
    freq = constrain(freq, 100, 2000); // Limitar frecuencias
    tone(PIEZO_PIN, freq, 200); // 0.2s duración
    
    harmonicIndex = (harmonicIndex + 1) % 5;
    lastUpdate = currentTime;
  }
}

void testArpeggios(unsigned long currentTime) {
  static unsigned long lastNote = 0;
  static unsigned long lastScaleChange = 0;
  static int noteIndex = 0;
  static int scaleType = 0;
  
  // Cambiar escala cada 5 segundos
  if (currentTime - lastScaleChange > 5000) {
    scaleType = (scaleType + 1) % 3;
    lastScaleChange = currentTime;
    Serial.print("Nueva escala de arpegio: ");
    Serial.println(scaleType == 0 ? "Do Mayor" : scaleType == 1 ? "La menor" : "Pentatónica");
  }
  
  if (currentTime - lastNote > 300) { // 0.2s + pausa
    int scales[][6] = {
      {262, 330, 392, 523, 392, 330}, // Do mayor
      {220, 262, 330, 440, 330, 262}, // La menor
      {262, 294, 330, 440, 523, 440}  // Pentatónica
    };
    
    tone(PIEZO_PIN, scales[scaleType][noteIndex], 200); // 0.2s duración
    
    noteIndex = (noteIndex + 1) % 6;
    lastNote = currentTime;
  }
}

void testRhythmicPatterns(unsigned long currentTime) {
  static unsigned long lastBeat = 0;
  static unsigned long lastPatternChange = 0;
  static int beatIndex = 0;
  static int patternType = 0;
  
  // Cambiar patrón cada 6 segundos
  if (currentTime - lastPatternChange > 6000) {
    patternType = (patternType + 1) % 3;
    lastPatternChange = currentTime;
    Serial.print("Nuevo patrón rítmico: ");
    Serial.println(patternType == 0 ? "Rock" : patternType == 1 ? "Valz" : "Syncopado");
  }
  
  // Diferentes patrones rítmicos
  int patterns[][6] = {
    {300, 150, 280, 120, 300, 0},    // Rock: FUERTE-débil-FUERTE-débil-FUERTE-pausa
    {250, 200, 200, 250, 200, 200},  // Valz: ¾ tiempo
    {200, 0, 250, 150, 0, 300}       // Syncopado: con silencios
  };
  int durations[][6] = {
    {200, 100, 200, 100, 500, 300},  // Rock
    {500, 200, 200, 500, 200, 200},  // Valz
    {100, 100, 200, 100, 200, 500}   // Syncopado
  };
  
  if (currentTime - lastBeat > durations[patternType][beatIndex] + 50) {
    if (patterns[patternType][beatIndex] > 0) {
      int freq = 400 + patterns[patternType][beatIndex];
      tone(PIEZO_PIN, freq, durations[patternType][beatIndex]);
    }
    
    beatIndex = (beatIndex + 1) % 6;
    lastBeat = currentTime;
  }
}

void testInsectSounds(unsigned long currentTime) {
  static unsigned long lastSound = 0;
  static unsigned long lastTypeChange = 0;
  static int insectType = 0;
  
  // Cambiar tipo de insecto cada 8 segundos
  if (currentTime - lastTypeChange > 8000) {
    insectType = (insectType + 1) % 3;
    lastTypeChange = currentTime;
    Serial.print("Nuevo insecto: ");
    Serial.println(insectType == 0 ? "Grillo" : insectType == 1 ? "Araña" : "Escarabajo");
  }
  
  if (currentTime - lastSound > random(800, 1500)) {
    switch (insectType) {
      case 0: // Grillo
        playGrilloSoundExtended();
        break;
      case 1: // Araña
        playSpiderSoundExtended();
        break;
      case 2: // Escarabajo
        playBeetleSoundExtended();
        break;
    }
    
    lastSound = currentTime;
  }
}

void testAmbientTexture(unsigned long currentTime) {
  static unsigned long lastLayer = 0;
  static unsigned long lastTextureChange = 0;
  static int textureType = 0;
  
  // Cambiar tipo de textura cada 10 segundos
  if (currentTime - lastTextureChange > 10000) {
    textureType = (textureType + 1) % 3;
    lastTextureChange = currentTime;
    Serial.print("Nueva textura ambiental: ");
    Serial.println(textureType == 0 ? "Bosque" : textureType == 1 ? "Océano" : "Viento");
  }
  
  if (currentTime - lastLayer > random(200, 600)) {
    // Múltiples capas de sonido ambiental con duraciones extendidas
    if (textureType == 0) { // Bosque
      if (random(100) < 40) {
        tone(PIEZO_PIN, random(150, 300), 500); // Graves largos
      } else {
        tone(PIEZO_PIN, random(800, 1500), 200); // Agudos cortos
      }
    } else if (textureType == 1) { // Océano
      if (random(100) < 60) {
        tone(PIEZO_PIN, random(100, 250), random(200, 500)); // Ondas
      } else {
        tone(PIEZO_PIN, random(300, 600), 100); // Espuma
      }
    } else { // Viento
      tone(PIEZO_PIN, random(200, 800), random(50, 200)); // Ráfagas variables
    }
    
    lastLayer = currentTime;
  }
}

// ===== SONIDOS DE INSECTOS ESPECÍFICOS EXTENDIDOS =====

void playGrilloSoundExtended() {
  for (int i = 0; i < random(3, 6); i++) {
    tone(PIEZO_PIN, random(200, 400), 200); // 0.2s duración
    delay(250); // Pausa entre chirridos
  }
}

void playSpiderSoundExtended() {
  for (int i = 0; i < random(4, 10); i++) {
    tone(PIEZO_PIN, random(800, 2000), random(50, 100)); // 0.05s - 0.1s
    delay(random(60, 150)); // Pausa variable
  }
}

void playBeetleSoundExtended() {
  for (int i = 0; i < random(2, 4); i++) {
    tone(PIEZO_PIN, random(100, 250), 500); // 0.5s duración larga
    delay(random(300, 600)); // Pausa larga
  }
}

// ===== FUNCIONES ORIGINALES (compatibilidad) =====

void playGrilloSound() {
  playGrilloSoundExtended();
}

void playSpiderSound() {
  playSpiderSoundExtended();
}

void playBeetleSound() {
  playBeetleSoundExtended();
}
