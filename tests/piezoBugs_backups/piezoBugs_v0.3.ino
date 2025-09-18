/*
 * PiezoBugs v0.3 - Simulación de insectos con escala pentatónica menor en Si
 * Piezo 1: Insecto con sonidos rápidos usando frecuencias aleatorias de la escala
 * Piezo 2: Grillo con secuencia fija de 4 frecuencias de la escala
 */

// Pines para piezoeléctricos
#define PIEZO_1_PIN 21  // Insecto con sonidos rápidos
#define PIEZO_2_PIN 22  // Grillo

// Escala pentatónica menor en Si (frecuencias hasta octava 8 para insectos)
// Si, Re, Mi, Fa#, La (octavas 4, 5, 6, 7, 8)
const int pentatonicScale[] = {
  // Octava 4
  494,   // Si4 (246.94 * 2)
  587,   // Re4 (293.66 * 2)
  659,   // Mi4 (329.63 * 2)
  740,   // Fa#4 (369.99 * 2)
  880,   // La4 (440.00 * 2)
  
  // Octava 5
  988,   // Si5 (246.94 * 4)
  1175,  // Re5 (293.66 * 4)
  1319,  // Mi5 (329.63 * 4)
  1480,  // Fa#5 (369.99 * 4)
  1760,  // La5 (440.00 * 4)
  
  // Octava 6
  1976,  // Si6 (246.94 * 8)
  2349,  // Re6 (293.66 * 8)
  2637,  // Mi6 (329.63 * 8)
  2960,  // Fa#6 (369.99 * 8)
  3520,  // La6 (440.00 * 8)
  
  // Octava 7
  3951,  // Si7 (246.94 * 16)
  4699,  // Re7 (293.66 * 16)
  5274,  // Mi7 (329.63 * 16)
  5920,  // Fa#7 (369.99 * 16)
  7040,  // La7 (440.00 * 16)
  
  // Octava 8 (frecuencias muy agudas)
  7902,  // Si8 (246.94 * 32)
  9398,  // Re8 (293.66 * 32)
  10548, // Mi8 (329.63 * 32)
  11840, // Fa#8 (369.99 * 32)
  14080  // La8 (440.00 * 32)
};

// Múltiples secuencias para el grillo (cada una con 3-4 notas, usando frecuencias agudas)
const int cricketSequences[][4] = {
  {pentatonicScale[1], pentatonicScale[3], pentatonicScale[0], pentatonicScale[4]}, // Re4, Fa#4, Si4, La4
  {pentatonicScale[6], pentatonicScale[8], pentatonicScale[9]}, // Si5, Mi5, La5 (3 notas)
  {pentatonicScale[12], pentatonicScale[14], pentatonicScale[16], pentatonicScale[19]}, // Si6, Mi6, La6, Fa#7
  {pentatonicScale[7], pentatonicScale[9], pentatonicScale[11]}, // Re5, La5, Fa#6 (3 notas)
  {pentatonicScale[10], pentatonicScale[13], pentatonicScale[15], pentatonicScale[18]}, // Re6, Fa#6, Mi7, La7
  {pentatonicScale[5], pentatonicScale[8], pentatonicScale[12]}, // Si5, Mi5, Si6 (3 notas)
  {pentatonicScale[15], pentatonicScale[17], pentatonicScale[19], pentatonicScale[22]}, // Mi7, Fa#7, La7, Re8
  {pentatonicScale[11], pentatonicScale[14], pentatonicScale[16]}, // Fa#6, Mi6, La6 (3 notas)
  {pentatonicScale[20], pentatonicScale[22], pentatonicScale[24]}, // Si8, Re8, La8 (3 notas)
  {pentatonicScale[13], pentatonicScale[16], pentatonicScale[18], pentatonicScale[21]}  // Fa#6, La6, La7, Mi8
};

// Longitudes de cada secuencia (número de notas)
const int cricketSequenceLengths[] = {4, 3, 4, 3, 4, 3, 4, 3, 3, 4};

// Variables para el comportamiento del primer insecto
unsigned long lastBug1Time = 0;
unsigned long bug1Interval = 0;
bool bug1Active = false;
int bug1SoundCount = 0;
int bug1MaxSounds = 0;

// Variables para el grillo
unsigned long lastCricketTime = 0;
unsigned long cricketInterval = 1700; // 1.7 segundos
int cricketSequenceIndex = 0;
bool cricketActive = false;
int currentCricketSequence = 0; // Secuencia actual (0-7)
unsigned long lastSequenceChange = 0; // Último cambio de secuencia
unsigned long sequenceChangeInterval = 0; // Intervalo para cambio (40-50s)

void setup() {
  Serial.begin(115200);
  Serial.println("=== PiezoBugs v0.2 - Escala Pentatónica Menor en Si ===");
  Serial.println("Piezo 1: Insecto con frecuencias aleatorias de la escala");
  Serial.println("Piezo 2: Grillo con secuencia fija: Re-Fa#-Si-La");
  
  // Mostrar frecuencias de la escala
  Serial.println("\nFrecuencias de la escala pentatónica menor en Si (octavas 4-8):");
  String noteNames[] = {
    "Si4", "Re4", "Mi4", "Fa#4", "La4",     // Octava 4
    "Si5", "Re5", "Mi5", "Fa#5", "La5",     // Octava 5
    "Si6", "Re6", "Mi6", "Fa#6", "La6",     // Octava 6
    "Si7", "Re7", "Mi7", "Fa#7", "La7",     // Octava 7
    "Si8", "Re8", "Mi8", "Fa#8", "La8"      // Octava 8
  };
  for (int i = 0; i < 25; i++) {
    Serial.print("Nota ");
    Serial.print(i + 1);
    Serial.print(" (");
    Serial.print(noteNames[i]);
    Serial.print("): ");
    Serial.print(pentatonicScale[i]);
    Serial.println(" Hz");
  }
  
  Serial.println("\nSecuencias del grillo (cambia cada 40-50s):");
  String allNoteNames[] = {
    "Si4", "Re4", "Mi4", "Fa#4", "La4",     // Octava 4
    "Si5", "Re5", "Mi5", "Fa#5", "La5",     // Octava 5
    "Si6", "Re6", "Mi6", "Fa#6", "La6",     // Octava 6
    "Si7", "Re7", "Mi7", "Fa#7", "La7",     // Octava 7
    "Si8", "Re8", "Mi8", "Fa#8", "La8"      // Octava 8
  };
  for (int seq = 0; seq < 10; seq++) {
    Serial.print("Secuencia ");
    Serial.print(seq + 1);
    Serial.print(" (");
    Serial.print(cricketSequenceLengths[seq]);
    Serial.print(" notas): ");
    for (int i = 0; i < cricketSequenceLengths[seq]; i++) {
      // Encontrar el índice en la escala pentatónica
      for (int j = 0; j < 25; j++) {
        if (cricketSequences[seq][i] == pentatonicScale[j]) {
          Serial.print(allNoteNames[j]);
          if (i < cricketSequenceLengths[seq] - 1) Serial.print("-");
          break;
        }
      }
    }
    Serial.println();
  }
  
  // Configurar pines
  pinMode(PIEZO_1_PIN, OUTPUT);
  pinMode(PIEZO_2_PIN, OUTPUT);
  
  // Inicializar tiempos
  lastBug1Time = millis();
  lastCricketTime = millis();
  lastSequenceChange = millis();
  
  // Generar primer intervalo aleatorio para el insecto 1
  generateNewBug1Interval();
  
  // Generar primer intervalo para cambio de secuencia del grillo
  generateNewSequenceChangeInterval();
  
  Serial.println("\nIniciando simulación...");
  delay(2000);
}

void loop() {
  unsigned long currentTime = millis();
  
  // Comportamiento del primer insecto (sonidos rápidos con frecuencias aleatorias de la escala)
  handleBug1(currentTime);
  
  // Comportamiento del grillo (secuencias variables que cambian cada 40-50s)
  handleCricket(currentTime);
  
  // Verificar si es hora de cambiar la secuencia del grillo
  checkSequenceChange(currentTime);
  
  delay(10); // Pequeña pausa para evitar sobrecarga
}

void handleBug1(unsigned long currentTime) {
  if (currentTime - lastBug1Time >= bug1Interval) {
    if (!bug1Active) {
      // Iniciar secuencia de sonidos rápidos
      bug1Active = true;
      bug1SoundCount = 0;
      bug1MaxSounds = random(3, 16); // Entre 3 y 7 sonidos por secuencia
      Serial.println("Insecto 1: Iniciando secuencia de sonidos rápidos");
    } else {
      // Reproducir siguiente sonido en la secuencia
      if (bug1SoundCount < bug1MaxSounds) {
        playBug1Sound();
        bug1SoundCount++;
        lastBug1Time = currentTime;
        bug1Interval = random(20, 260); // Entre 80ms y 200ms entre sonidos
      } else {
        // Terminar secuencia y generar nueva pausa aleatoria
        bug1Active = false;
        noTone(PIEZO_1_PIN);
        generateNewBug1Interval();
        lastBug1Time = currentTime;
        Serial.println("Insecto 1: Secuencia completada, pausa aleatoria");
      }
    }
  }
}

void handleCricket(unsigned long currentTime) {
  if (currentTime - lastCricketTime >= cricketInterval) {
    if (!cricketActive) {
      // Iniciar secuencia del grillo
      cricketActive = true;
      cricketSequenceIndex = 0;
      lastCricketTime = currentTime;
      cricketInterval = random(80, 200); // Entre 80ms y 200ms entre sonidos del grillo (variable)
      Serial.print("Grillo: Iniciando secuencia ");
      Serial.print(currentCricketSequence + 1);
      Serial.print(" (");
      Serial.print(cricketSequenceLengths[currentCricketSequence]);
      Serial.println(" notas)");
    } else {
      // Reproducir siguiente sonido del grillo
      if (cricketSequenceIndex < cricketSequenceLengths[currentCricketSequence]) {
        playCricketSound();
        cricketSequenceIndex++;
        lastCricketTime = currentTime;
        cricketInterval = random(80, 200); // Silencios variables entre notas
      } else {
        // Terminar secuencia del grillo
        cricketActive = false;
        noTone(PIEZO_2_PIN);
        lastCricketTime = currentTime;
        cricketInterval = random(1500, 2000); // Pausa variable entre secuencias (1.5-2s)
        Serial.println("Grillo: Secuencia completada, pausa variable");
      }
    }
  }
}

void playBug1Sound() {
  // Seleccionar frecuencia aleatoria de la escala pentatónica (ahora 25 notas)
  int freqIndex = random(0, 25);
  int freq = pentatonicScale[freqIndex];
  int duration = random(50, 150); // Entre 50ms y 150ms
  
  // Nombres de las notas para el debug
  String noteNames[] = {
    "Si4", "Re4", "Mi4", "Fa#4", "La4",     // Octava 4
    "Si5", "Re5", "Mi5", "Fa#5", "La5",     // Octava 5
    "Si6", "Re6", "Mi6", "Fa#6", "La6",     // Octava 6
    "Si7", "Re7", "Mi7", "Fa#7", "La7",     // Octava 7
    "Si8", "Re8", "Mi8", "Fa#8", "La8"      // Octava 8
  };
  
  tone(PIEZO_1_PIN, freq, duration);
  Serial.print("Insecto 1: ");
  Serial.print(freq);
  Serial.print(" Hz (");
  Serial.print(noteNames[freqIndex]);
  Serial.print(") por ");
  Serial.print(duration);
  Serial.println(" ms");
}

void playCricketSound() {
  // Usar la secuencia actual del grillo
  int freq = cricketSequences[currentCricketSequence][cricketSequenceIndex];
  int duration = random(60, 180); // Duración variable entre 60ms y 180ms
  
  // Nombres de todas las notas
  String allNoteNames[] = {
    "Si4", "Re4", "Mi4", "Fa#4", "La4",     // Octava 4
    "Si5", "Re5", "Mi5", "Fa#5", "La5",     // Octava 5
    "Si6", "Re6", "Mi6", "Fa#6", "La6",     // Octava 6
    "Si7", "Re7", "Mi7", "Fa#7", "La7",     // Octava 7
    "Si8", "Re8", "Mi8", "Fa#8", "La8"      // Octava 8
  };
  
  // Encontrar el nombre de la nota actual
  String currentNote = "";
  for (int i = 0; i < 25; i++) {
    if (freq == pentatonicScale[i]) {
      currentNote = allNoteNames[i];
      break;
    }
  }
  
  tone(PIEZO_2_PIN, freq, duration);
  Serial.print("Grillo: Sonido ");
  Serial.print(cricketSequenceIndex + 1);
  Serial.print(" (");
  Serial.print(currentNote);
  Serial.print(") - ");
  Serial.print(freq);
  Serial.print(" Hz por ");
  Serial.print(duration);
  Serial.println(" ms");
}

void generateNewBug1Interval() {
  // Generar pausa aleatoria entre secuencias (entre 2 y 8 segundos)
  bug1Interval = random(2000, 8000);
  Serial.print("Insecto 1: Nueva pausa de ");
  Serial.print(bug1Interval);
  Serial.println(" ms");
}

void generateNewSequenceChangeInterval() {
  // Generar intervalo para cambio de secuencia (entre 30 y 58 segundos)
  sequenceChangeInterval = random(30000, 58000);
  Serial.print("Grillo: Próximo cambio de secuencia en ");
  Serial.print(sequenceChangeInterval / 1000);
  Serial.println(" segundos");
}

void checkSequenceChange(unsigned long currentTime) {
  if (currentTime - lastSequenceChange >= sequenceChangeInterval) {
    // Cambiar a la siguiente secuencia (ahora tenemos 10 secuencias)
    currentCricketSequence = (currentCricketSequence + 1) % 10;
    lastSequenceChange = currentTime;
    generateNewSequenceChangeInterval();
    
    Serial.print("Grillo: Cambiando a secuencia ");
    Serial.print(currentCricketSequence + 1);
    Serial.print(" (");
    Serial.print(cricketSequenceLengths[currentCricketSequence]);
    Serial.println(" notas)");
  }
}
