/*
 * WAV Player MINIMAL v1.0 - ESP32 AudioKit v2.2
 * Reproductor básico de archivos WAV desde tarjeta SD
 * Versión minimalista con librería ESP8266Audio (más liviana)
 * 
 * INSTALACIÓN DE LIBRERÍA EN ARDUINO IDE:
 * 1. Ir a Tools → Manage Libraries
 * 2. Buscar: "ESP8266Audio"
 * 3. Instalar: "ESP8266Audio by Earle F. Philhower"
 * 
 * Busca: example.wav en la raíz de la SD
 * 
 * Hardware:
 * - ESP32 AudioKit v2.2 
 * - Tarjeta microSD con example.wav
 * - Auriculares en jack 3.5mm
 * 
 * Controles básicos:
 * - Botón 1 (GPIO36): Play/Pause
 * - Botón 4 (GPIO16): Siguiente archivo
 */

#include "AudioFileSourceSD.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2S.h"
#include "SD.h"

// Pines I2S 
#define I2S_DOUT      25
#define I2S_BCLK      27
#define I2S_LRC       26

// SD
#define SD_CS         5

// Botones
#define BUTTON_1      36    // Play/Pause
#define BUTTON_4      16    // Siguiente

// LEDs
#define LED_SYSTEM    2

// Objetos de audio ESP8266Audio
AudioGeneratorWAV *wav;
AudioFileSourceSD *file;
AudioOutputI2S *out;

// Estado simple
bool isPlaying = false;
int fileIndex = 0;
String files[5];  // Máximo 5 archivos
int numFiles = 0;

// Timing
unsigned long lastButton = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("=== WAV Player MINIMAL v1.0 ===");
  
  // LED sistema
  pinMode(LED_SYSTEM, OUTPUT);
  digitalWrite(LED_SYSTEM, HIGH);
  
  // Botones
  pinMode(BUTTON_1, INPUT);
  pinMode(BUTTON_4, INPUT_PULLUP);
  
  // Inicializar SD
  if (!SD.begin(SD_CS)) {
    Serial.println("Error SD");
    return;
  }
  
  // Buscar archivos
  findFiles();
  
  // Configurar audio I2S
  out = new AudioOutputI2S();
  out->SetPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  out->SetGain(0.1);  // Volumen bajo inicial
  
  wav = new AudioGeneratorWAV();
  
  Serial.println("Listo!");
  Serial.println("Archivos: " + String(numFiles));
  Serial.println("Botón 1: Play/Pause");
  Serial.println("Botón 4: Siguiente");
}

void loop() {
  // Mantener audio corriendo
  if (wav->isRunning()) {
    if (!wav->loop()) {
      wav->stop();
      if (file) {
        file->close();
        delete file;
        file = nullptr;
      }
      isPlaying = false;
      Serial.println("Archivo terminado");
    }
  }
  
  // Botones simples
  if (millis() - lastButton > 300) {
    if (analogRead(BUTTON_1) < 100) {
      togglePlay();
      lastButton = millis();
    }
    if (!digitalRead(BUTTON_4)) {
      nextFile();
      lastButton = millis();
    }
  }
  
  delay(10);
}

void findFiles() {
  numFiles = 0;
  
  // Buscar solo archivos WAV (ESP8266Audio solo soporta WAV bien)
  if (SD.exists("/example.wav")) {
    files[numFiles++] = "/example.wav";
    Serial.println("Encontrado: example.wav");
  }
  
  // Si no hay archivos específicos, buscar otros WAV
  if (numFiles == 0) {
    File root = SD.open("/");
    File file = root.openNextFile();
    while (file && numFiles < 5) {
      String name = file.name();
      if (name.endsWith(".wav") || name.endsWith(".WAV")) {
        files[numFiles++] = "/" + name;
        Serial.println("Encontrado: " + name);
      }
      file = root.openNextFile();
    }
    root.close();
  }
}

void togglePlay() {
  if (numFiles == 0) {
    Serial.println("No hay archivos");
    return;
  }
  
  if (isPlaying) {
    wav->stop();
    if (file) {
      file->close();
      delete file;
      file = nullptr;
    }
    isPlaying = false;
    Serial.println("Stop");
  } else {
    String filename = files[fileIndex];
    Serial.println("Play: " + filename);
    
    file = new AudioFileSourceSD(filename.c_str());
    if (wav->begin(file, out)) {
      isPlaying = true;
    } else {
      Serial.println("Error al reproducir");
      delete file;
      file = nullptr;
    }
  }
}

void nextFile() {
  if (numFiles == 0) return;
  
  fileIndex = (fileIndex + 1) % numFiles;
  Serial.println("Siguiente: " + files[fileIndex]);
  
  if (isPlaying) {
    // Detener archivo actual
    wav->stop();
    if (file) {
      file->close();
      delete file;
    }
    
    // Reproducir nuevo archivo
    String filename = files[fileIndex];
    file = new AudioFileSourceSD(filename.c_str());
    if (wav->begin(file, out)) {
      Serial.println("Reproduciendo: " + filename);
    } else {
      Serial.println("Error al cambiar archivo");
      isPlaying = false;
      delete file;
      file = nullptr;
    }
  }
}