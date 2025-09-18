/*
 * PiezoBugs v0.2 - Simulación de insectos con escala pentatónica menor en Si
 * Piezo 1: Insecto con sonidos rápidos usando frecuencias aleatorias de la escala
 * Piezo 2: Grillo con secuencia fija de 4 frecuencias de la escala
 */

// Pines para piezoeléctricos
#define PIEZO_1_PIN 21  // Insecto con sonidos rápidos
#define PIEZO_2_PIN 22  // Grillo

// Escala pentatónica menor en Si (frecuencias en octavas altas para insectos)
// Si, Re, Mi, Fa#, La (en octava 4 y 5)
const int pentatonicScale[] = {
  494,   // Si4 (246.94 * 2) - redondeado
  587,   // Re4 (293.66 * 2) - redondeado
  659,   // Mi4 (329.63 * 2) - redondeado
  740,   // Fa#4 (369.99 * 2) - redondeado
  880,   // La4 (440.00 * 2)
  988,   // Si5 (246.94 * 4) - redondeado
  1175,  // Re5 (293.66 * 4) - redondeado
  1319,  // Mi5 (329.63 * 4) - redondeado
  1480,  // Fa#5 (369.99 * 4) - redondeado
  1760   // La5 (440.00 * 4)
};

// Secuencia fija del grillo (4 frecuencias de la escala)
const int cricketSequence[] = {
  pentatonicScale[1], // Re4 (587 Hz)
  pentatonicScale[3], // Fa#4 (740 Hz)
  pentatonicScale[0], // Si4 (494 Hz)
  pentatonicScale[4]  // La4 (880 Hz)
};

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

void setup() {
  Serial.begin(115200);
  Serial.println("=== PiezoBugs v0.2 - Escala Pentatónica Menor en Si ===");
  Serial.println("Piezo 1: Insecto con frecuencias aleatorias de la escala");
  Serial.println("Piezo 2: Grillo con secuencia fija: Re-Fa#-Si-La");
  
  // Mostrar frecuencias de la escala
  Serial.println("\nFrecuencias de la escala pentatónica menor en Si:");
  String noteNames[] = {"Si4", "Re4", "Mi4", "Fa#4", "La4", "Si5", "Re5", "Mi5", "Fa#5", "La5"};
  for (int i = 0; i < 10; i++) {
    Serial.print("Nota ");
    Serial.print(i + 1);
    Serial.print(" (");
    Serial.print(noteNames[i]);
    Serial.print("): ");
    Serial.print(pentatonicScale[i]);
    Serial.println(" Hz");
  }
  
  Serial.println("\nSecuencia del grillo:");
  String cricketNoteNames[] = {"Re4", "Fa#4", "Si4", "La4"};
  for (int i = 0; i < 4; i++) {
    Serial.print("Sonido ");
    Serial.print(i + 1);
    Serial.print(" (");
    Serial.print(cricketNoteNames[i]);
    Serial.print("): ");
    Serial.print(cricketSequence[i]);
    Serial.println(" Hz");
  }
  
  // Configurar pines
  pinMode(PIEZO_1_PIN, OUTPUT);
  pinMode(PIEZO_2_PIN, OUTPUT);
  
  // Inicializar tiempos
  lastBug1Time = millis();
  lastCricketTime = millis();
  
  // Generar primer intervalo aleatorio para el insecto 1
  generateNewBug1Interval();
  
  Serial.println("\nIniciando simulación...");
  delay(2000);
}

void loop() {
  unsigned long currentTime = millis();
  
  // Comportamiento del primer insecto (sonidos rápidos con frecuencias aleatorias de la escala)
  handleBug1(currentTime);
  
  // Comportamiento del grillo (secuencia fija de 4 frecuencias)
  handleCricket(currentTime);
  
  delay(10); // Pequeña pausa para evitar sobrecarga
}

void handleBug1(unsigned long currentTime) {
  if (currentTime - lastBug1Time >= bug1Interval) {
    if (!bug1Active) {
      // Iniciar secuencia de sonidos rápidos
      bug1Active = true;
      bug1SoundCount = 0;
      bug1MaxSounds = random(3, 8); // Entre 3 y 7 sonidos por secuencia
      Serial.println("Insecto 1: Iniciando secuencia de sonidos rápidos");
    } else {
      // Reproducir siguiente sonido en la secuencia
      if (bug1SoundCount < bug1MaxSounds) {
        playBug1Sound();
        bug1SoundCount++;
        lastBug1Time = currentTime;
        bug1Interval = random(80, 200); // Entre 80ms y 200ms entre sonidos
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
      cricketInterval = random(120, 180); // Entre 120ms y 180ms entre sonidos del grillo
      Serial.println("Grillo: Iniciando secuencia fija de 4 sonidos");
    } else {
      // Reproducir siguiente sonido del grillo
      if (cricketSequenceIndex < 4) {
        playCricketSound();
        cricketSequenceIndex++;
        lastCricketTime = currentTime;
        cricketInterval = random(120, 180);
      } else {
        // Terminar secuencia del grillo
        cricketActive = false;
        noTone(PIEZO_2_PIN);
        lastCricketTime = currentTime;
        cricketInterval = 1700; // Volver al intervalo de 1.7s
        Serial.println("Grillo: Secuencia completada, esperando 1.7s");
      }
    }
  }
}

void playBug1Sound() {
  // Seleccionar frecuencia aleatoria de la escala pentatónica
  int freqIndex = random(0, 10);
  int freq = pentatonicScale[freqIndex];
  int duration = random(50, 150); // Entre 50ms y 150ms
  
  // Nombres de las notas para el debug
  String noteNames[] = {"Si4", "Re4", "Mi4", "Fa#4", "La4", "Si5", "Re5", "Mi5", "Fa#5", "La5"};
  
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
  // Usar la secuencia fija del grillo
  int freq = cricketSequence[cricketSequenceIndex];
  int duration = random(100, 200); // Entre 100ms y 200ms
  
  // Nombres de las notas del grillo
  String cricketNoteNames[] = {"Re4", "Fa#4", "Si4", "La4"};
  
  tone(PIEZO_2_PIN, freq, duration);
  Serial.print("Grillo: Sonido ");
  Serial.print(cricketSequenceIndex + 1);
  Serial.print(" (");
  Serial.print(cricketNoteNames[cricketSequenceIndex]);
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

