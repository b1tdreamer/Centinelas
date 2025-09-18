/*
 * WAV Player v1.0 - ESP32 AudioKit v2.2 - VERSIÓN COMPLETA (BACKUP)
 * Reproductor de archivos WAV desde tarjeta SD
 * Soporte para example.wav y example.ogg en la raíz de la SD
 * 
 * NOTA: Esta es la versión completa con todas las funcionalidades
 * Se mantiene como backup para referencia futura
 * 
 * Hardware requerido:
 * - ESP32 AudioKit v2.2 con codec AC101
 * - Tarjeta microSD con archivos de audio
 * - Auriculares conectados al jack de 3.5mm
 * 
 * Controles:
 * - Botón 1 (GPIO36): Play/Pause
 * - Botón 2 (GPIO13): Stop 
 * - Botón 3 (GPIO5): Archivo anterior
 * - Botón 4 (GPIO16): Archivo siguiente  
 * - Botón 5 (GPIO18): Volumen -
 * - Botón 6 (GPIO23): Volumen +
 */

#include "Audio.h"
#include "SD.h"
#include "FS.h"

// Pines I2S para ESP32 AudioKit v2.2
#define I2S_DOUT      25    // Data Output
#define I2S_BCLK      27    // Bit Clock  
#define I2S_LRC       26    // Left/Right Clock

// Pines para tarjeta SD
#define SD_CS         5     // Chip Select
#define SPI_MOSI      23    // Master Out Slave In
#define SPI_MISO      19    // Master In Slave Out  
#define SPI_SCK       18    // Serial Clock

// Pines de botones del AudioKit
#define BUTTON_1      36    // Play/Pause (ADC)
#define BUTTON_2      13    // Stop
#define BUTTON_3      5     // Archivo anterior (comparte con SD_CS)
#define BUTTON_4      16    // Archivo siguiente
#define BUTTON_5      18    // Volumen - (comparte con SPI_SCK)
#define BUTTON_6      23    // Volumen + (comparte con SPI_MOSI)

// Pines de LEDs de estado
#define LED_SYSTEM    2     // Sistema encendido
#define LED_PLAYING   14    // Reproduciendo
#define LED_ERROR     15    // Error/pausa

// Objeto de audio
Audio audio;

// Variables de control
enum PlayerState {
  STOPPED,
  PLAYING, 
  PAUSED
};

PlayerState currentState = STOPPED;
int currentVolume = 12;     // Volumen inicial (0-21)
int currentFileIndex = 0;   // Índice del archivo actual
String playlist[10];        // Lista de archivos (máximo 10)
int playlistSize = 0;       // Número de archivos encontrados

// Variables para manejo de botones
unsigned long lastButtonPress = 0;
const unsigned long DEBOUNCE_DELAY = 200;

// Variables de tiempo para tareas no bloqueantes
unsigned long lastStatusUpdate = 0;
const unsigned long STATUS_UPDATE_INTERVAL = 1000;

void setup() {
  Serial.begin(115200);
  Serial.println("=== WAV Player v1.0 - ESP32 AudioKit v2.2 ===");
  
  // Configurar LEDs de estado
  pinMode(LED_SYSTEM, OUTPUT);
  pinMode(LED_PLAYING, OUTPUT);
  pinMode(LED_ERROR, OUTPUT);
  
  // LED de sistema encendido
  digitalWrite(LED_SYSTEM, HIGH);
  digitalWrite(LED_PLAYING, LOW);
  digitalWrite(LED_ERROR, LOW);
  
  // Configurar botones
  setupButtons();
  
  // Inicializar tarjeta SD
  if (!initializeSD()) {
    Serial.println("Error: No se pudo inicializar la tarjeta SD");
    digitalWrite(LED_ERROR, HIGH);
    return;
  }
  
  // Escanear archivos de audio
  scanAudioFiles();
  
  // Configurar audio I2S
  if (!initializeAudio()) {
    Serial.println("Error: No se pudo inicializar el sistema de audio");
    digitalWrite(LED_ERROR, HIGH);
    return;
  }
  
  Serial.println("Sistema iniciado correctamente");
  Serial.println("Controles:");
  Serial.println("- Botón 1 (GPIO36): Play/Pause");
  Serial.println("- Botón 2 (GPIO13): Stop");
  Serial.println("- Botón 4 (GPIO16): Siguiente archivo");
  Serial.println("- Volumen actual: " + String(currentVolume) + "/21");
  
  if (playlistSize > 0) {
    Serial.println("Archivos encontrados: " + String(playlistSize));
    for (int i = 0; i < playlistSize; i++) {
      Serial.println("  " + String(i + 1) + ". " + playlist[i]);
    }
  } else {
    Serial.println("No se encontraron archivos de audio en la SD");
    Serial.println("Asegúrate de tener example.wav o example.ogg en la raíz");
  }
}

void loop() {
  // Mantener el loop de audio activo
  audio.loop();
  
  // Manejar botones
  handleButtons();
  
  // Actualizar estado y LEDs
  updateStatus();
  
  // Pequeña pausa para evitar sobrecarga
  delay(10);
}

bool initializeSD() {
  Serial.println("Inicializando tarjeta SD...");
  
  // Inicializar SPI para SD
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SD_CS);
  
  if (!SD.begin(SD_CS)) {
    Serial.println("Error: Tarjeta SD no detectada");
    return false;
  }
  
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("Error: No hay tarjeta SD insertada");
    return false;
  }
  
  Serial.print("Tipo de tarjeta SD: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("Desconocido");
  }
  
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.println("Tamaño de tarjeta SD: " + String(cardSize) + "MB");
  
  return true;
}

bool initializeAudio() {
  Serial.println("Inicializando sistema de audio I2S...");
  
  // Configurar pines I2S
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  
  // Configurar volumen inicial
  audio.setVolume(currentVolume); // 0...21
  
  // Configurar parámetros de audio
  audio.setTone(0, 0, 0); // Sin ecualización inicial
  
  Serial.println("Audio I2S inicializado correctamente");
  Serial.println("Pines I2S: BCLK=" + String(I2S_BCLK) + 
                 ", LRC=" + String(I2S_LRC) + 
                 ", DOUT=" + String(I2S_DOUT));
  
  return true;
}

void scanAudioFiles() {
  Serial.println("Escaneando archivos de audio...");
  
  playlistSize = 0;
  
  // Buscar archivos específicos en la raíz
  String targetFiles[] = {"example.wav", "example.ogg", "EXAMPLE.WAV", "EXAMPLE.OGG"};
  int numTargets = 4;
  
  for (int i = 0; i < numTargets && playlistSize < 10; i++) {
    String filename = "/" + targetFiles[i];
    if (SD.exists(filename)) {
      playlist[playlistSize] = filename;
      playlistSize++;
      Serial.println("Encontrado: " + filename);
    }
  }
  
  // Si no se encuentran los archivos específicos, buscar otros archivos de audio
  if (playlistSize == 0) {
    File root = SD.open("/");
    if (root) {
      File file = root.openNextFile();
      while (file && playlistSize < 10) {
        if (!file.isDirectory()) {
          String fileName = file.name();
          fileName.toLowerCase();
          if (fileName.endsWith(".wav") || fileName.endsWith(".ogg") || 
              fileName.endsWith(".mp3") || fileName.endsWith(".flac")) {
            playlist[playlistSize] = "/" + String(file.name());
            playlistSize++;
            Serial.println("Encontrado: " + playlist[playlistSize - 1]);
          }
        }
        file = root.openNextFile();
      }
      root.close();
    }
  }
  
  if (playlistSize == 0) {
    Serial.println("No se encontraron archivos de audio");
  } else {
    Serial.println("Total de archivos encontrados: " + String(playlistSize));
  }
}

void setupButtons() {
  // Configurar botones como entradas
  pinMode(BUTTON_1, INPUT);      // ADC, no necesita pull-up
  pinMode(BUTTON_2, INPUT_PULLUP);
  pinMode(BUTTON_4, INPUT_PULLUP);
  
  // NOTA: BUTTON_3, BUTTON_5, BUTTON_6 comparten pines con SD/SPI
  // Se manejarán de forma especial para evitar conflictos
  
  Serial.println("Botones configurados");
  Serial.println("NOTA: Algunos botones comparten pines con SD/SPI");
}

void handleButtons() {
  unsigned long currentTime = millis();
  
  // Evitar rebotes
  if (currentTime - lastButtonPress < DEBOUNCE_DELAY) {
    return;
  }
  
  // Botón 1 (GPIO36) - Play/Pause - ADC
  int button1Value = analogRead(BUTTON_1);
  if (button1Value < 100) { // Umbral para detectar pulsación
    handlePlayPause();
    lastButtonPress = currentTime;
    return;
  }
  
  // Botón 2 (GPIO13) - Stop
  if (!digitalRead(BUTTON_2)) {
    handleStop();
    lastButtonPress = currentTime;
    return;
  }
  
  // Botón 4 (GPIO16) - Siguiente archivo
  if (!digitalRead(BUTTON_4)) {
    handleNextFile();
    lastButtonPress = currentTime;
    return;
  }
  
  // Para botones que comparten pines, implementar lógica especial si es necesario
  // Por ahora, enfocarse en funcionalidad básica
}

void handlePlayPause() {
  if (playlistSize == 0) {
    Serial.println("No hay archivos para reproducir");
    return;
  }
  
  switch (currentState) {
    case STOPPED:
      playCurrentFile();
      break;
    case PLAYING:
      pausePlayback();
      break;
    case PAUSED:
      resumePlayback();
      break;
  }
}

void handleStop() {
  if (currentState != STOPPED) {
    stopPlayback();
  }
}

void handleNextFile() {
  if (playlistSize == 0) return;
  
  currentFileIndex = (currentFileIndex + 1) % playlistSize;
  Serial.println("Siguiente archivo: " + playlist[currentFileIndex]);
  
  if (currentState == PLAYING) {
    playCurrentFile(); // Reproducir inmediatamente el nuevo archivo
  }
}

void handleVolumeUp() {
  if (currentVolume < 21) {
    currentVolume++;
    audio.setVolume(currentVolume);
    Serial.println("Volumen: " + String(currentVolume) + "/21");
  }
}

void handleVolumeDown() {
  if (currentVolume > 0) {
    currentVolume--;
    audio.setVolume(currentVolumen);
    Serial.println("Volumen: " + String(currentVolume) + "/21");
  }
}

void playCurrentFile() {
  if (playlistSize == 0) return;
  
  String filename = playlist[currentFileIndex];
  Serial.println("Reproduciendo: " + filename);
  
  bool success = audio.connecttoFS(SD, filename.c_str());
  
  if (success) {
    currentState = PLAYING;
    digitalWrite(LED_PLAYING, HIGH);
    digitalWrite(LED_ERROR, LOW);
    Serial.println("Reproducción iniciada");
  } else {
    Serial.println("Error al reproducir el archivo");
    currentState = STOPPED;
    digitalWrite(LED_PLAYING, LOW);
    digitalWrite(LED_ERROR, HIGH);
  }
}

void pausePlayback() {
  audio.pauseResume();
  currentState = PAUSED;
  digitalWrite(LED_PLAYING, LOW);
  Serial.println("Reproducción pausada");
}

void resumePlayback() {
  audio.pauseResume();
  currentState = PLAYING;
  digitalWrite(LED_PLAYING, HIGH);
  Serial.println("Reproducción reanudada");
}

void stopPlayback() {
  audio.stopSong();
  currentState = STOPPED;
  digitalWrite(LED_PLAYING, LOW);
  digitalWrite(LED_ERROR, LOW);
  Serial.println("Reproducción detenida");
}

void updateStatus() {
  unsigned long currentTime = millis();
  
  if (currentTime - lastStatusUpdate >= STATUS_UPDATE_INTERVAL) {
    lastStatusUpdate = currentTime;
    
    // Verificar si la reproducción ha terminado
    if (currentState == PLAYING && !audio.isRunning()) {
      Serial.println("Archivo terminado, pasando al siguiente");
      handleNextFile();
      if (currentState == PLAYING) {
        playCurrentFile();
      }
    }
    
    // Mostrar información de estado cada 10 segundos
    static int statusCounter = 0;
    statusCounter++;
    if (statusCounter >= 10) {
      statusCounter = 0;
      printStatus();
    }
  }
}

void printStatus() {
  Serial.println("=== Estado del Reproductor ===");
  Serial.println("Estado: " + getStateString());
  Serial.println("Volumen: " + String(currentVolume) + "/21");
  Serial.println("Archivo actual: " + String(currentFileIndex + 1) + "/" + String(playlistSize));
  if (playlistSize > 0) {
    Serial.println("Reproduciendo: " + playlist[currentFileIndex]);
  }
  Serial.println("Memoria libre: " + String(ESP.getFreeHeap()) + " bytes");
  Serial.println("=============================");
}

String getStateString() {
  switch (currentState) {
    case STOPPED: return "DETENIDO";
    case PLAYING: return "REPRODUCIENDO";
    case PAUSED: return "PAUSADO";
    default: return "DESCONOCIDO";
  }
}

// Callbacks opcionales para eventos de audio
void audio_info(const char *info) {
  Serial.print("Info: ");
  Serial.println(info);
}

void audio_id3data(const char *info) {
  Serial.print("ID3: ");
  Serial.println(info);
}

void audio_eof_mp3(const char *info) {
  Serial.print("EOF MP3: ");
  Serial.println(info);
}

void audio_showstation(const char *info) {
  Serial.print("Station: ");
  Serial.println(info);
}

void audio_showstreamtitle(const char *info) {
  Serial.print("Stream Title: ");
  Serial.println(info);
}

void audio_bitrate(const char *info) {
  Serial.print("Bitrate: ");
  Serial.println(info);
}

void audio_commercial(const char *info) {
  Serial.print("Commercial: ");
  Serial.println(info);
}

void audio_icyurl(const char *info) {
  Serial.print("ICY URL: ");
  Serial.println(info);
}

void audio_lasthost(const char *info) {
  Serial.print("Last Host: ");
  Serial.println(info);
}
