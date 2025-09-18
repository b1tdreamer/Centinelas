/*
 * PiezoBugs - Simulación de insectos con piezoeléctricos
 * Piezo 1: Insecto con sonidos rápidos y pausas aleatorias
 * Piezo 2: Grillo con secuencias de 4 sonidos cada 1.7s
 */

// Pines para piezoeléctricos
#define PIEZO_1_PIN 21  // Insecto con sonidos rápidos
#define PIEZO_2_PIN 22  // Grillo

// Variables para el comportamiento del primer insecto
unsigned long lastBug1Time = 0;
unsigned long bug1Interval = 0;
bool bug1Active = false;
int bug1SoundCount = 0;
int bug1MaxSounds = 0;

// Variables para el grillo
unsigned long lastCricketTime = 0;
unsigned long cricketInterval = 1700; // 1.7 segundos
int cricketSequence = 0;
bool cricketActive = false;

void setup() {
  Serial.begin(115200);
  Serial.println("=== PiezoBugs - Simulación de Insectos ===");
  Serial.println("Piezo 1: Insecto con sonidos rápidos y pausas aleatorias");
  Serial.println("Piezo 2: Grillo con secuencias de 4 sonidos");
  
  // Configurar pines
  pinMode(PIEZO_1_PIN, OUTPUT);
  pinMode(PIEZO_2_PIN, OUTPUT);
  
  // Inicializar tiempos
  lastBug1Time = millis();
  lastCricketTime = millis();
  
  // Generar primer intervalo aleatorio para el insecto 1
  generateNewBug1Interval();
  
  Serial.println("Iniciando simulación...");
  delay(1000);
}

void loop() {
  unsigned long currentTime = millis();
  
  // Comportamiento del primer insecto (sonidos rápidos y pausas aleatorias)
  handleBug1(currentTime);
  
  // Comportamiento del grillo (secuencias de 4 sonidos cada 1.7s)
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
      cricketSequence = 0;
      lastCricketTime = currentTime;
      cricketInterval = random(120, 180); // Entre 120ms y 180ms entre sonidos del grillo
      Serial.println("Grillo: Iniciando secuencia de 4 sonidos");
    } else {
      // Reproducir siguiente sonido del grillo
      if (cricketSequence < 4) {
        playCricketSound();
        cricketSequence++;
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
  // Frecuencias agudas y medias para el primer insecto
  int frequencies[] = {1200, 1500, 1800, 2000, 2200, 2500, 2800, 3000};
  int freq = frequencies[random(0, 8)];
  int duration = random(50, 100); // Entre 50ms y 150ms
  
  tone(PIEZO_1_PIN, freq, duration);
  Serial.print("Insecto 1: ");
  Serial.print(freq);
  Serial.print(" Hz por ");
  Serial.print(duration);
  Serial.println(" ms");
}

void playCricketSound() {
  // Frecuencias medias para el grillo
  int frequencies[] = {800, 1000, 1200, 1400, 1600};
  int freq = frequencies[random(0, 5)];
  int duration = random(30, 10); // Entre 100ms y 200ms
  
  tone(PIEZO_2_PIN, freq, duration);
  Serial.print("Grillo: Sonido ");
  Serial.print(cricketSequence + 1);
  Serial.print(" - ");
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
