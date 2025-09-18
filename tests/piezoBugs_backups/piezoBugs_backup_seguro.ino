/*
 * PiezoBugs v0.9 - Simulación de insectos con escala pentatónica menor en Si
 * Sistema modular de insectos con diferentes comportamientos
 * Soporte para aro LED Neopixel y control por botones
 */

// Pines para piezoeléctricos
#define PIEZO_1_PIN 21  // Insecto 1 (Araña por defecto)
#define PIEZO_2_PIN 22  // Insecto 2 (Grillo por defecto)

// Pin para aro LED Neopixel (24 LEDs)
#define NEOPIXEL_PIN 23  // GPIO23 - Pin de datos para Neopixel
#define NEOPIXEL_COUNT 24 // Número de LEDs en el aro

// Escala pentatónica menor en Do (frecuencias desde octava 2 hasta octava 8)
// Do, Re, Mi, Fa#, La (octavas 2, 3, 4, 5, 6, 7, 8)
const int pentatonicScale[] = {
  // Octava 2
  65,    // Do2 (261.63 / 4)
  73,    // Re2 (293.66 / 4)
  82,    // Mi2 (329.63 / 4)
  92,    // Fa#2 (369.99 / 4)
  110,   // La2 (440.00 / 4)
  
  // Octava 3
  131,   // Do3 (261.63 / 2)
  147,   // Re3 (293.66 / 2)
  165,   // Mi3 (329.63 / 2)
  185,   // Fa#3 (369.99 / 2)
  220,   // La3 (440.00 / 2)
  
  // Octava 4
  262,   // Do4 (261.63)
  294,   // Re4 (293.66)
  330,   // Mi4 (329.63)
  370,   // Fa#4 (369.99)
  440,   // La4 (440.00)
  
  // Octava 5
  523,   // Do5 (261.63 * 2)
  587,   // Re5 (293.66 * 2)
  659,   // Mi5 (329.63 * 2)
  740,   // Fa#5 (369.99 * 2)
  880,   // La5 (440.00 * 2)
  
  // Octava 6
  1047,  // Do6 (261.63 * 4)
  1175,  // Re6 (293.66 * 4)
  1319,  // Mi6 (329.63 * 4)
  1480,  // Fa#6 (369.99 * 4)
  1760,  // La6 (440.00 * 4)
  
  // Octava 7
  2093,  // Do7 (261.63 * 8)
  2349,  // Re7 (293.66 * 8)
  2637,  // Mi7 (329.63 * 8)
  2960,  // Fa#7 (369.99 * 8)
  3520,  // La7 (440.00 * 8)
  
  // Octava 8 (frecuencias muy agudas)
  4186,  // Do8 (261.63 * 16)
  4699,  // Re8 (293.66 * 16)
  5274,  // Mi8 (329.63 * 16)
  5920,  // Fa#8 (369.99 * 16)
  7040   // La8 (440.00 * 16)
};

// Tipos de insectos disponibles
enum InsectType {
  SPIDER = 0,    // Araña (octavas 5, 6, 7, 8)
  CRICKET = 1,   // Grillo (octavas 2, 3, 4)
  BEETLE = 2     // Escarabajo (octavas 2, 3, 4 - solo Si)
  // BUMBLEBEE = 3  // Abejorro (octavas 2, 3 - melodías largas) - DESACTIVADO
};

// Variables para tipos de insectos
InsectType insect1Type = SPIDER;  // Tipo del insecto 1
InsectType insect2Type = CRICKET; // Tipo del insecto 2

// Variables para secuencias de insectos
int insect1Sequence[8]; // Secuencia actual del insecto 1 (máximo 8 notas)
int insect1SequenceLength = 0; // Longitud de la secuencia actual
int insect2Sequence[8]; // Secuencia actual del insecto 2 (máximo 8 notas)
int insect2SequenceLength = 0; // Longitud de la secuencia actual

// Pines de botones del ESP32 AudioKit v2.2 (según documentación oficial)
#define BUTTON_1_PIN 36  // Botón 1 (GPIO36 - KEY1) - FUNCIONA
#define BUTTON_2_PIN 13  // Botón 2 (GPIO13 - KEY2) - Comparte con SD DATA3
#define BUTTON_3_PIN 4    // Botón 3 (GPIO4 - DATA1 SDCard)
#define BUTTON_4_PIN 23  // Botón 4 (GPIO23 - KEY4) - FUNCIONA
#define BUTTON_5_PIN 0   // Botón 5 (GPIO0 - BOOT Button)
#define BUTTON_6_PIN 5   // Botón 6 (GPIO5 - KEY6) - FUNCIONA

// Pines de LEDs del ESP32 AudioKit v2.2 (verificar en documentación)
// Posibles pines alternativos si los actuales no funcionan
#define LED_D1_PIN 2     // LED D1 (GPIO2) - Verde - Sistema encendido
#define LED_D3_PIN 14    // LED D3 (GPIO14) - Verde/Rojo - Actividad de audio
#define LED_D4_PIN 15    // LED D4 (GPIO15) - Rojo - Estado de error/mute

// LEDs alternativos disponibles (GPIOs libres)
#define LED_ALT1_PIN 16  // GPIO16 - LED alternativo 1
#define LED_ALT2_PIN 17  // GPIO17 - LED alternativo 2
#define LED_ALT3_PIN 18  // GPIO18 - LED alternativo 3
// LED_ALT4_PIN eliminado para evitar conflictos

// Estados del sistema
enum SystemState {
  SOUND_OFF = 0,    // Sonido desactivado
  FREQ_NORMAL = 1,  // Frecuencia normal (parámetros por defecto)
  FREQ_SLOW = 2,    // Frecuencia lenta (tiempos x3)
  FREQ_VERY_SLOW = 3, // Frecuencia muy lenta (tiempos x5)
  FREQ_EXTREMELY_SLOW = 4 // Frecuencia extremadamente lenta (tiempos x7)
};

SystemState currentState = FREQ_NORMAL;
bool insect1Muted = false; // Mute individual del insecto 1
bool insect2Muted = false; // Mute individual del insecto 2

// Variables para cambio de nota raíz
int rootNoteOffset = 0; // Offset en semitonos desde Do (0 = Do, 1 = Do#, 2 = Re, etc.)
const char* noteNames[] = {"Do", "Do#", "Re", "Re#", "Mi", "Fa", "Fa#", "Sol", "Sol#", "La", "La#", "Si"};

// Variables para manejo de botones
unsigned long buttonPressStart = 0;
bool buttonPressed = false;
const unsigned long LONG_PRESS_TIME = 1000; // 1 segundo para pulsación larga

// Variables para otros botones
bool button2Pressed = false;
bool button3Pressed = false;
bool button4Pressed = false;
bool button5Pressed = false;
bool button6Pressed = false;
unsigned long button2PressStart = 0;
unsigned long button3PressStart = 0;
unsigned long button4PressStart = 0;
unsigned long button5PressStart = 0;
unsigned long button6PressStart = 0;

// Variables para el comportamiento de los insectos
unsigned long lastInsect1Time = 0;
unsigned long insect1Interval = 0;
bool insect1Active = false;
int insect1SoundCount = 0;
int insect1MaxSounds = 0;
int insect1SequenceIndex = 0;

unsigned long lastInsect2Time = 0;
unsigned long insect2Interval = 0;
bool insect2Active = false;
int insect2SoundCount = 0;
int insect2MaxSounds = 0;
int insect2SequenceIndex = 0;

unsigned long lastSequenceChange1 = 0; // Último cambio de secuencia insecto 1
unsigned long lastSequenceChange2 = 0; // Último cambio de secuencia insecto 2
unsigned long sequenceChangeInterval1 = 0; // Intervalo para cambio insecto 1
unsigned long sequenceChangeInterval2 = 0; // Intervalo para cambio insecto 2

void setup() {
  Serial.begin(115200);
  Serial.println("=== PiezoBugs v0.9 - Sistema Modular de Insectos ===");
  
  // Valores por defecto (sin EEPROM)
  insect1Type = SPIDER;
  insect2Type = CRICKET;
  rootNoteOffset = 0;
  currentState = FREQ_NORMAL;
  Serial.println("Piezo 1: Insecto 1 (Araña por defecto) - Botón 2 para cambiar tipo");
  Serial.println("Piezo 2: Insecto 2 (Grillo por defecto) - Botón 4 para cambiar tipo");
  Serial.println("Tipos: Araña(octavas 5-8), Grillo(octavas 2-4), Escarabajo(octavas 2-4, solo Si)");
  Serial.println("Botón 1: Pulsación corta = Cambiar frecuencia, Larga (1s) = Reset a valores por defecto");
  Serial.println("NOTA: No se guardan preferencias - valores por defecto al reiniciar");
  Serial.println("Botón 2: Pulsación larga (1s) = Mute/Unmute Insecto 1, Pulsación corta = Cambiar tipo");
  Serial.println("Botón 4: Pulsación larga (1s) = Mute/Unmute Insecto 2, Pulsación corta = Cambiar tipo");
  Serial.println("Botón 6: Pulsación corta = Cambiar nota raíz, Larga (1s) = Reset a Si");
  Serial.println("Estados: Normal(x1) -> Lento(x3) -> Muy Lento(x5) -> Extremo(x7) -> Normal");
  Serial.println("Neopixel: Aro de 24 LEDs en GPIO23");
  Serial.println("LEDs: D1=Sistema, D3=Audio, D4=Mute + Alternativos GPIO16-19");
  
  // Mostrar frecuencias de la escala
  Serial.println("\nFrecuencias de la escala pentatónica menor en Do (octavas 2-8):");
  String noteNames[] = {
    "Do2", "Re2", "Mi2", "Fa#2", "La2",     // Octava 2
    "Do3", "Re3", "Mi3", "Fa#3", "La3",     // Octava 3
    "Do4", "Re4", "Mi4", "Fa#4", "La4",     // Octava 4
    "Do5", "Re5", "Mi5", "Fa#5", "La5",     // Octava 5
    "Do6", "Re6", "Mi6", "Fa#6", "La6",     // Octava 6
    "Do7", "Re7", "Mi7", "Fa#7", "La7",     // Octava 7
    "Do8", "Re8", "Mi8", "Fa#8", "La8"      // Octava 8
  };
  for (int i = 0; i < 35; i++) {
    Serial.print("Nota ");
    Serial.print(i + 1);
    Serial.print(" (");
    Serial.print(noteNames[i]);
    Serial.print("): ");
    Serial.print(pentatonicScale[i]);
    Serial.println(" Hz");
  }
  
  Serial.println("\nGrillo: Usa secuencias aleatorias de 3-4 notas");
  Serial.println("Rango: Solo octavas 2, 3 y 4 (índices 0-14 de la escala)");
  Serial.println("Cambia de secuencia cada 30-58 segundos");
  Serial.print("Nota raíz actual: ");
  Serial.println(noteNames[rootNoteOffset]);
  
  // Configurar pines
  pinMode(PIEZO_1_PIN, OUTPUT);
  pinMode(PIEZO_2_PIN, OUTPUT);
  pinMode(NEOPIXEL_PIN, OUTPUT);
  
  // Configurar botones como entradas
  pinMode(BUTTON_1_PIN, INPUT);      // ADC (GPIO36)
  pinMode(BUTTON_2_PIN, INPUT_PULLUP); // Digital con pull-up (GPIO13 - comparte con SD)
  pinMode(BUTTON_3_PIN, INPUT_PULLUP); // Digital con pull-up (GPIO4)
  pinMode(BUTTON_4_PIN, INPUT_PULLUP); // Digital con pull-up (GPIO23)
  pinMode(BUTTON_5_PIN, INPUT_PULLUP); // Digital con pull-up (GPIO18)
  pinMode(BUTTON_6_PIN, INPUT_PULLUP); // Digital con pull-up (GPIO5)
  
  // Configurar LEDs originales
  pinMode(LED_D1_PIN, OUTPUT);
  pinMode(LED_D3_PIN, OUTPUT);
  pinMode(LED_D4_PIN, OUTPUT);
  
  // Configurar LEDs alternativos
  pinMode(LED_ALT1_PIN, OUTPUT);
  pinMode(LED_ALT2_PIN, OUTPUT);
  pinMode(LED_ALT3_PIN, OUTPUT);
  
  // Inicializar LEDs originales
  digitalWrite(LED_D1_PIN, HIGH);  // D1 encendido (sistema activo)
  digitalWrite(LED_D3_PIN, LOW);   // D3 apagado (sin actividad de audio)
  digitalWrite(LED_D4_PIN, LOW);   // D4 apagado (sin mute)
  
  // Inicializar LEDs alternativos (para prueba)
  digitalWrite(LED_ALT1_PIN, HIGH); // LED alternativo 1 encendido
  digitalWrite(LED_ALT2_PIN, LOW);  // LED alternativo 2 apagado
  digitalWrite(LED_ALT3_PIN, LOW);  // LED alternativo 3 apagado
  
  Serial.println("LEDs configurados - Prueba visual:");
  Serial.println("D1 (GPIO2), D3 (GPIO14), D4 (GPIO15) - Originales");
  Serial.println("ALT1 (GPIO16), ALT2 (GPIO17), ALT3 (GPIO18) - Alternativos");
  Serial.println("NOTA: Botón 2 comparte GPIO13 con SD DATA3");
  Serial.println("NOTA: Botón 5 usa GPIO0 (BOOT Button)");
  
  /* Debug de botones al inicio
  Serial.println("\n=== DEBUG DE BOTONES ===");
  Serial.print("Botón 1 (GPIO36): ");
  Serial.println(analogRead(BUTTON_1_PIN));
  Serial.print("Botón 2 (GPIO13): ");
  Serial.println(digitalRead(BUTTON_2_PIN) ? "HIGH" : "LOW");
  Serial.print("Botón 3 (GPIO4): ");
  Serial.println(digitalRead(BUTTON_3_PIN) ? "HIGH" : "LOW");
  Serial.print("Botón 4 (GPIO23): ");
  Serial.println(digitalRead(BUTTON_4_PIN) ? "HIGH" : "LOW");
  Serial.print("Botón 5 (GPIO0): ");
  Serial.println(digitalRead(BUTTON_5_PIN) ? "HIGH" : "LOW");
  Serial.print("Botón 6 (GPIO5): ");
  Serial.println(digitalRead(BUTTON_6_PIN) ? "HIGH" : "LOW");
  Serial.println("ADC: < 100, Digital: LOW = presionado (pull-up invertido)");
  Serial.println("========================");
  */
  
  // Inicializar Neopixel (comentado temporalmente)
  // initNeopixel();
  
  // Inicializar tiempos
  lastInsect1Time = millis();
  lastInsect2Time = millis();
  lastSequenceChange1 = millis();
  lastSequenceChange2 = millis();
  
  // Generar primeros intervalos y secuencias
  generateNewInsect1Interval();
  generateNewInsect2Interval();
  generateNewSequenceChangeInterval1();
  generateNewSequenceChangeInterval2();
  generateRandomInsect1Sequence();
  generateRandomInsect2Sequence();
  
  Serial.println("\nIniciando simulación...");
  delay(2000);
}

// Funci�n para validar el estado del sistema y prevenir corrupci�n
void validateSystemState() {
  // Validar tipos de insectos
  if (insect1Type < 0 || insect1Type > 2) {
    Serial.print("CR�TICO: Tipo insecto1 corrupto: ");
    Serial.print((int)insect1Type);
    Serial.println(", reseteando sistema");
    insect1Type = SPIDER;
    insect1SequenceIndex = 0;
    insect1SequenceLength = 0;
    insect1Active = false;
    generateRandomInsect1Sequence();
  }

  if (insect2Type < 0 || insect2Type > 2) {
    Serial.print("CR�TICO: Tipo insecto2 corrupto: ");
    Serial.print((int)insect2Type);
    Serial.println(", reseteando sistema");
    insect2Type = CRICKET;
    insect2SequenceIndex = 0;
    insect2SequenceLength = 0;
    insect2Active = false;
    generateRandomInsect2Sequence();
  }

  // Validar longitudes de secuencias
  if (insect1SequenceLength < 0 || insect1SequenceLength > 8) {
    Serial.print("CR�TICO: Longitud secuencia1 corrupta: ");
    Serial.print(insect1SequenceLength);
    Serial.println(", reseteando");
    insect1SequenceLength = 0;
    insect1SequenceIndex = 0;
    insect1Active = false;
    generateRandomInsect1Sequence();
  }

  if (insect2SequenceLength < 0 || insect2SequenceLength > 8) {
    Serial.print("CR�TICO: Longitud secuencia2 corrupta: ");
    Serial.print(insect2SequenceLength);
    Serial.println(", reseteando");
    insect2SequenceLength = 0;
    insect2SequenceIndex = 0;
    insect2Active = false;
    generateRandomInsect2Sequence();
  }

  // Validar �ndices de secuencias
  if (insect1SequenceIndex < 0 || insect1SequenceIndex > insect1SequenceLength) {
    Serial.print("CR�TICO: �ndice secuencia1 corrupto: ");
    Serial.print(insect1SequenceIndex);
    Serial.println(", reseteando");
    insect1SequenceIndex = 0;
    insect1Active = false;
  }

  if (insect2SequenceIndex < 0 || insect2SequenceIndex > insect2SequenceLength) {
    Serial.print("CR�TICO: �ndice secuencia2 corrupto: ");
    Serial.print(insect2SequenceIndex);
    Serial.println(", reseteando");
    insect2SequenceIndex = 0;
    insect2Active = false;
  }
}

void loop() {
  // Validar estado del sistema para prevenir corrupci�n
  validateSystemState();

  unsigned long currentTime = millis();
  
  // Manejar botones
  handleButtons(currentTime);
  
  // Comportamiento de los insectos (respetando mute individual)
  if (!insect1Muted) {
    handleInsect1(currentTime);
    checkSequenceChange1(currentTime);
  }
  if (!insect2Muted) {
    handleInsect2(currentTime);
    checkSequenceChange2(currentTime);
  }
  
  // Actualizar Neopixel (comentado temporalmente)
  // updateNeopixel(currentTime);
  
  delay(10); // Pequeña pausa para evitar sobrecarga
}






// Funciones para manejo de botones
void handleButtons(unsigned long currentTime) {
  // Manejar botón 1
  handleButton1(currentTime);
  
  // Agregar manejo de otros botones para debug
  handleButton2(currentTime);
  handleButton3(currentTime);
  handleButton4(currentTime);
  handleButton5(currentTime);
  handleButton6(currentTime);
}

void handleButton1(unsigned long currentTime) {
  // Leer valor analógico del botón (pines ADC)
  int buttonValue = analogRead(BUTTON_1_PIN);
  bool buttonState = (buttonValue < 100); // Umbral para detectar pulsación
  
  if (buttonState && !buttonPressed) {
    // Botón presionado por primera vez
    buttonPressed = true;
    buttonPressStart = currentTime;
  } else if (!buttonState && buttonPressed) {
    // Botón liberado
    buttonPressed = false;
    unsigned long pressDuration = currentTime - buttonPressStart;
    
    // Funcionalidad del botón 1
    if (pressDuration >= LONG_PRESS_TIME) {
      // Pulsación larga (1s): resetear a valores por defecto
      Serial.println("=== RESETEO A VALORES POR DEFECTO ===");
      insect1Type = SPIDER;
      insect2Type = CRICKET;
      rootNoteOffset = 0;
      currentState = FREQ_NORMAL;
      insect1Muted = false;
      insect2Muted = false;
      
      // Regenerar secuencias
      generateRandomInsect1Sequence();
      generateRandomInsect2Sequence();
      generateNewInsect1Interval();
      generateNewInsect2Interval();
      generateNewSequenceChangeInterval1();
      generateNewSequenceChangeInterval2();
      
      Serial.println("Sistema reseteado a valores por defecto");
    } else {
      // Pulsación corta: cambiar frecuencia
      changeFrequency();
    }
  }
}


void changeFrequency() {
  // Cambiar entre los cuatro estados de frecuencia
  switch (currentState) {
    case FREQ_NORMAL:
      currentState = FREQ_SLOW;
      Serial.println("Estado: Frecuencia LENTA (x3)");
      break;
    case FREQ_SLOW:
      currentState = FREQ_VERY_SLOW;
      Serial.println("Estado: Frecuencia MUY LENTA (x5)");
      break;
    case FREQ_VERY_SLOW:
      currentState = FREQ_EXTREMELY_SLOW;
      Serial.println("Estado: Frecuencia EXTREMADAMENTE LENTA (x7)");
      break;
    case FREQ_EXTREMELY_SLOW:
      currentState = FREQ_NORMAL;
      Serial.println("Estado: Frecuencia NORMAL (x1)");
      break;
  }
  
  // Regenerar intervalos con el nuevo multiplicador
  generateNewInsect1Interval();
  generateNewInsect2Interval();
  generateNewSequenceChangeInterval1();
  generateNewSequenceChangeInterval2();
}

int getFrequencyMultiplier() {
  switch (currentState) {
    case FREQ_NORMAL:
      return 1;
    case FREQ_SLOW:
      return 3;
    case FREQ_VERY_SLOW:
      return 5;
    case FREQ_EXTREMELY_SLOW:
      return 7;
    default:
      return 1;
  }
}

// Funciones de debug para otros botones
void handleButton2(unsigned long currentTime) {
  bool buttonState = !digitalRead(BUTTON_2_PIN); // Invertir porque usamos pull-up
  
  // Debug: mostrar valor del botón cada 5 segundos (comentado)
  /*
  static unsigned long lastDebug2 = 0;
  if (currentTime - lastDebug2 > 5000) {
    lastDebug2 = currentTime;
    Serial.print("Botón 2 - Estado: ");
    Serial.println(buttonState ? "PRESIONADO" : "LIBERADO");
  }
  */
  
  if (buttonState && !button2Pressed) {
    button2Pressed = true;
    button2PressStart = currentTime;
  } else if (!buttonState && button2Pressed) {
    button2Pressed = false;
    unsigned long pressDuration = currentTime - button2PressStart;
    
    // Funcionalidad del botón 2
    if (pressDuration >= LONG_PRESS_TIME) {
      // Pulsación larga: mute/unmute insecto 1
      toggleInsect1Mute();
    } else {
      // Pulsación corta: cambiar tipo del insecto 1
      changeInsect1Type();
    }
  }
}

void handleButton3(unsigned long currentTime) {
  bool buttonState = !digitalRead(BUTTON_3_PIN); // Invertir porque usamos pull-up
  
  if (buttonState && !button3Pressed) {
    button3Pressed = true;
    button3PressStart = currentTime;
  } else if (!buttonState && button3Pressed) {
    button3Pressed = false;
    unsigned long pressDuration = currentTime - button3PressStart;
    
    // Botón 3 reservado para futuras funcionalidades
  }
}

void handleButton4(unsigned long currentTime) {
  bool buttonState = !digitalRead(BUTTON_4_PIN); // Invertir porque usamos pull-up
  
  if (buttonState && !button4Pressed) {
    button4Pressed = true;
    button4PressStart = currentTime;
  } else if (!buttonState && button4Pressed) {
    button4Pressed = false;
    unsigned long pressDuration = currentTime - button4PressStart;
    
    // Funcionalidad del botón 4 (Insecto 2)
    if (pressDuration >= LONG_PRESS_TIME) {
      // Pulsación larga: mute/unmute insecto 2
      toggleInsect2Mute();
    } else {
      // Pulsación corta: cambiar tipo del insecto 2
      changeInsect2Type();
    }
  }
}

void handleButton5(unsigned long currentTime) {
  bool buttonState = !digitalRead(BUTTON_5_PIN); // Invertir porque usamos pull-up
  
  if (buttonState && !button5Pressed) {
    button5Pressed = true;
    button5PressStart = currentTime;
  } else if (!buttonState && button5Pressed) {
    button5Pressed = false;
    unsigned long pressDuration = currentTime - button5PressStart;
  }
}

void handleButton6(unsigned long currentTime) {
  bool buttonState = !digitalRead(BUTTON_6_PIN); // Invertir porque usamos pull-up
  
  if (buttonState && !button6Pressed) {
    button6Pressed = true;
    button6PressStart = currentTime;
  } else if (!buttonState && button6Pressed) {
    button6Pressed = false;
    unsigned long pressDuration = currentTime - button6PressStart;
    
    // Funcionalidad del botón 6
    if (pressDuration >= LONG_PRESS_TIME) {
      // Pulsación larga: resetear nota raíz a Do
      rootNoteOffset = 0;
      Serial.println("Nota raíz reseteada a: Do");
      generateRandomInsect1Sequence();
      generateRandomInsect2Sequence();
    } else {
      // Pulsación corta: cambiar nota raíz (cada pulsación sube un semitono)
      changeRootNote();
    }
  }
}

// Funciones para Neopixel (comentadas temporalmente)
/*
void initNeopixel() {
  // Inicializar el aro de Neopixels
  Serial.println("Neopixel inicializado en GPIO23 (24 LEDs)");
  Serial.println("Patrón: 1 de cada 5 LEDs encendidos");
  
  // Configurar pin como salida
  pinMode(NEOPIXEL_PIN, OUTPUT);
  digitalWrite(NEOPIXEL_PIN, LOW);
  
  // Mostrar patrón inicial: 1 de cada 5 LEDs encendidos
  showNeopixelPattern();
}
*/

/*
void showNeopixelPattern() {
  // Patrón: 1 de cada 5 LEDs encendidos (LEDs 0, 5, 10, 15, 20)
  // Implementación simplificada para verificar conexión
  
  Serial.println("Mostrando patrón Neopixel:");
  Serial.println("LEDs encendidos: 0, 5, 10, 15, 20");
  
  // Reset: mantener pin bajo por 50μs
  digitalWrite(NEOPIXEL_PIN, LOW);
  delayMicroseconds(50);
  
  // Enviar datos para 24 LEDs
  for (int led = 0; led < 24; led++) {
    if (led % 5 == 0) {
      // LED encendido (verde)
      sendNeopixelByte(0);   // Red
      sendNeopixelByte(50);  // Green
      sendNeopixelByte(0);   // Blue
    } else {
      // LED apagado
      sendNeopixelByte(0);   // Red
      sendNeopixelByte(0);   // Green
      sendNeopixelByte(0);   // Blue
    }
  }
  
  // Finalizar
  digitalWrite(NEOPIXEL_PIN, LOW);
  delayMicroseconds(50);
}
*/

/*
void sendNeopixelByte(uint8_t byte) {
  // Enviar un byte a Neopixel
  for (int i = 7; i >= 0; i--) {
    if (byte & (1 << i)) {
      // Bit 1: HIGH por 0.8μs, LOW por 0.45μs
      digitalWrite(NEOPIXEL_PIN, HIGH);
      delayMicroseconds(1);
      digitalWrite(NEOPIXEL_PIN, LOW);
      delayMicroseconds(1);
    } else {
      // Bit 0: HIGH por 0.4μs, LOW por 0.85μs
      digitalWrite(NEOPIXEL_PIN, HIGH);
      delayMicroseconds(1);
      digitalWrite(NEOPIXEL_PIN, LOW);
      delayMicroseconds(1);
    }
  }
}
*/

/*
void updateNeopixel(unsigned long currentTime) {
  // Actualizar efectos del aro de Neopixels
  static unsigned long lastNeopixelUpdate = 0;
  static bool patternShown = false;
  
  if (!patternShown) {
    // Mostrar patrón inicial una vez
    showNeopixelPattern();
    patternShown = true;
    lastNeopixelUpdate = currentTime;
  }
  
  // Actualizar cada 2 segundos con efectos básicos
  if (currentTime - lastNeopixelUpdate > 2000) {
    lastNeopixelUpdate = currentTime;
    
    // Efecto simple: cambiar color basado en estado del sistema
    if (soundEnabled) {
      // Sonido activo: LEDs verdes
      showNeopixelPattern();
    } else {
      // Sonido desactivado: LEDs rojos
      showNeopixelRedPattern();
    }
  }
}
*/

/*
void showNeopixelRedPattern() {
  // Patrón rojo para cuando el sonido está desactivado
  Serial.println("Mostrando patrón rojo (sonido desactivado)");
  
  digitalWrite(NEOPIXEL_PIN, LOW);
  delayMicroseconds(50);
  
  for (int led = 0; led < 24; led++) {
    if (led % 5 == 0) {
      // LED encendido (rojo)
      sendNeopixelByte(50);  // Red
      sendNeopixelByte(0);   // Green
      sendNeopixelByte(0);   // Blue
    } else {
      // LED apagado
      sendNeopixelByte(0);   // Red
      sendNeopixelByte(0);   // Green
      sendNeopixelByte(0);   // Blue
    }
  }
  
  digitalWrite(NEOPIXEL_PIN, LOW);
  delayMicroseconds(50);
}
*/

// Funciones para manejo de insectos
void handleInsect1(unsigned long currentTime) {
  if (currentTime - lastInsect1Time >= insect1Interval) {
    if (!insect1Active) {
      // Iniciar secuencia del insecto 1
      insect1Active = true;
      insect1SequenceIndex = 0;
      lastInsect1Time = currentTime;
      insect1Interval = getInsectNoteInterval(insect1Type);
      // Serial.print("Insecto 1 (");
      // Serial.print(getInsectTypeName(insect1Type));
      // Serial.println("): Iniciando secuencia");
    } else {
      // Reproducir siguiente sonido del insecto 1
      if (insect1SequenceIndex < insect1SequenceLength && insect1SequenceIndex >= 0 && insect1SequenceLength > 0) {
        playInsect1Sound();
        insect1SequenceIndex++;
        lastInsect1Time = currentTime;
        insect1Interval = getInsectNoteInterval(insect1Type);
      } else {
        // Terminar secuencia del insecto 1
        insect1Active = false;
        noTone(PIEZO_1_PIN);
        lastInsect1Time = currentTime;
        insect1Interval = getInsectSequenceInterval(insect1Type);
        // Serial.print("Insecto 1 (");
        // Serial.print(getInsectTypeName(insect1Type));
        // Serial.println("): Secuencia completada");
      }
    }
  }
}

void handleInsect2(unsigned long currentTime) {
  if (currentTime - lastInsect2Time >= insect2Interval) {
    if (!insect2Active) {
      // Iniciar secuencia del insecto 2
      insect2Active = true;
      insect2SequenceIndex = 0;
      lastInsect2Time = currentTime;
      insect2Interval = getInsectNoteInterval(insect2Type);
      // Serial.print("Insecto 2 (");
      // Serial.print(getInsectTypeName(insect2Type));
      // Serial.println("): Iniciando secuencia");
    } else {
      // Reproducir siguiente sonido del insecto 2
      if (insect2SequenceIndex < insect2SequenceLength && insect2SequenceIndex >= 0 && insect2SequenceLength > 0) {
        playInsect2Sound();
        insect2SequenceIndex++;
        lastInsect2Time = currentTime;
        insect2Interval = getInsectNoteInterval(insect2Type);
      } else {
        // Terminar secuencia del insecto 2
        insect2Active = false;
        noTone(PIEZO_2_PIN);
        lastInsect2Time = currentTime;
        insect2Interval = getInsectSequenceInterval(insect2Type);
        // Serial.print("Insecto 2 (");
        // Serial.print(getInsectTypeName(insect2Type));
        // Serial.println("): Secuencia completada");
      }
    }
  }
}

// Funciones para cambiar tipos de insectos
void toggleInsect1Mute() {
  insect1Muted = !insect1Muted;
  if (insect1Muted) {
    Serial.println("=== INSECTO 1 MUTEADO ===");
    noTone(PIEZO_1_PIN);
  } else {
    Serial.println("=== INSECTO 1 ACTIVADO ===");
  }
}

void toggleInsect2Mute() {
  insect2Muted = !insect2Muted;
  if (insect2Muted) {
    Serial.println("=== INSECTO 2 MUTEADO ===");
    noTone(PIEZO_2_PIN);
  } else {
    Serial.println("=== INSECTO 2 ACTIVADO ===");
  }
}

void changeInsect1Type() {
  /*Serial.print("Cambiando insecto 1 desde: ");
  Serial.print((int)insect1Type);
  Serial.print(" (");
  Serial.print(getInsectTypeName(insect1Type));
  Serial.println(")");*/
  
  // Validar tipo actual antes de cambiar
  if (insect1Type < 0 || insect1Type > 2) {
    Serial.print("Error: Tipo de insecto 1 corrupto: ");
    Serial.print((int)insect1Type);
    Serial.println(", reseteando a SPIDER");
    insect1Type = SPIDER;
  }
  
  insect1Type = (InsectType)((insect1Type + 1) % 3); // Cambiado de 4 a 3
  
  Serial.print("Insecto 1 cambió a ");
  Serial.print(getInsectTypeName(insect1Type));
  
  // Resetear índices para evitar corrupción
  insect1SequenceIndex = 0;
  insect1Active = false;
  
  generateRandomInsect1Sequence();
  generateNewInsect1Interval();
}

void changeInsect2Type() {
  // Validar tipo actual antes de cambiar
  if (insect2Type < 0 || insect2Type > 2) {
    Serial.print("Error: Tipo de insecto 2 corrupto: ");
    Serial.print((int)insect2Type);
    Serial.println(", reseteando a CRICKET");
    insect2Type = CRICKET;
  }
  
  insect2Type = (InsectType)((insect2Type + 1) % 3); // Cambiado de 4 a 3
  Serial.print("Insecto 2 cambió a ");
  Serial.println(getInsectTypeName(insect2Type));
  
  // Resetear índices para evitar corrupción
  insect2SequenceIndex = 0;
  insect2Active = false;
  
  generateRandomInsect2Sequence();
  generateNewInsect2Interval();
}

void changeRootNote() {
  rootNoteOffset = (rootNoteOffset + 1) % 12; // Ciclar de 0 a 11
  Serial.print("Nota raíz cambiada a: ");
  Serial.println(noteNames[rootNoteOffset]);
  
  // Regenerar secuencias con la nueva nota raíz
  generateRandomInsect1Sequence();
  generateRandomInsect2Sequence();
}

int applyRootNoteOffset(int baseFrequency) {
  // Aplicar el offset de nota raíz multiplicando por 2^(offset/12)
  // LÍMITES DE FRECUENCIA RESTAURADOS PARA SEGURIDAD
  
  // Validar frecuencia base
  if (baseFrequency <= 0 || baseFrequency > 20000) {
    Serial.print("Error: Frecuencia base inválida: ");
    Serial.println(baseFrequency);
    return 440; // Frecuencia por defecto
  }
  
  // Validar rootNoteOffset
  if (rootNoteOffset < 0 || rootNoteOffset > 11) {
    Serial.print("Error: rootNoteOffset inválido: ");
    Serial.println(rootNoteOffset);
    return baseFrequency; // Devolver frecuencia original
  }
  
  float multiplier = pow(2.0, rootNoteOffset / 12.0);
  int newFreq = (int)(baseFrequency * multiplier);
  
  // Limitar frecuencias para evitar errores del ESP32
  if (newFreq < 20) {
    Serial.print("Frecuencia muy baja, ajustando: ");
    Serial.print(newFreq);
    Serial.print(" -> 20 Hz");
    newFreq = 20;
  }
  if (newFreq > 8000) {
    Serial.print("Frecuencia muy alta, ajustando: ");
    Serial.print(newFreq);
    Serial.print(" -> 8000 Hz");
    newFreq = 8000;
  }
  
  return newFreq;
}

String getInsectTypeName(InsectType type) {
  switch (type) {
    case SPIDER: return "Araña";
    case CRICKET: return "Grillo";
    case BEETLE: return "Escarabajo";
    // case BUMBLEBEE: return "Abejorro"; // DESACTIVADO
    default: 
      Serial.print("ERROR: Tipo de insecto desconocido: ");
      Serial.println((int)type);
      return "Desconocido";
  }
}

// Funciones para generar secuencias según el tipo de insecto
void generateRandomInsect1Sequence() {
  generateRandomSequence(insect1Type, insect1Sequence, insect1SequenceLength);
}

void generateRandomInsect2Sequence() {
  generateRandomSequence(insect2Type, insect2Sequence, insect2SequenceLength);
}

void generateRandomSequence(InsectType type, int sequence[], int &length) {
  // Validar tipo de insecto
  if (type < 0 || type > 2) {
    Serial.print("Error: Tipo de insecto inválido en generateRandomSequence: ");
    Serial.println((int)type);
    type = SPIDER; // Usar tipo por defecto
  }
  
  switch (type) {
    case SPIDER:
      // Araña: octavas 5, 6, 7, 8 (índices 15-34) - COMPLETAMENTE ALEATORIA
      length = random(3, 8); // Limitado a máximo 8 (tamaño del array)
      for (int i = 0; i < length; i++) {
        int baseIndex = random(15, 35);
        // Validar índice antes de acceder
        if (baseIndex >= 0 && baseIndex < 35) {
          sequence[i] = applyRootNoteOffset(pentatonicScale[baseIndex]);
        } else {
          sequence[i] = 440; // Frecuencia por defecto
        }
      }
      break;
      
    case CRICKET:
      // Grillo: octavas 2, 3, 4 (índices 0-14)
      length = random(3, 5);
      for (int i = 0; i < length; i++) {
        int baseIndex = random(0, 15);
        // Validar índice antes de acceder
        if (baseIndex >= 0 && baseIndex < 35) {
          sequence[i] = applyRootNoteOffset(pentatonicScale[baseIndex]);
        } else {
          sequence[i] = 440; // Frecuencia por defecto
        }
      }
      break;
      
    case BEETLE: {
      // Escarabajo: octavas 2, 3, 4 solo Do (índices 0, 5, 10) - frecuencias audibles
      length = random(4, 8); // Limitado a máximo 8
      int beetleNotes[] = {pentatonicScale[0], pentatonicScale[5], pentatonicScale[10]}; // Do2, Do3, Do4
      for (int i = 0; i < length; i++) {
        int noteIndex = random(0, 3);
        // Validar índice antes de acceder
        if (noteIndex >= 0 && noteIndex < 3) {
          sequence[i] = applyRootNoteOffset(beetleNotes[noteIndex]);
        } else {
          sequence[i] = 440; // Frecuencia por defecto
        }
      }
      break;
    }
  }
  
  // Validar longitud final
  if (length < 1 || length > 8) {
    Serial.print("Error: Longitud de secuencia inválida: ");
    Serial.print(length);
    Serial.println(", ajustando a 3");
    length = 3;
  }
}
  
// Funciones para intervalos según el tipo de insecto
unsigned long getInsectNoteInterval(InsectType type) {
  switch (type) {
    case SPIDER:
      return random(20, 210); // Entre 20ms y 260ms
    case CRICKET:
      return random(80, 250); // Entre 80ms y 200ms
    case BEETLE:
      return random(60, 300); // Entre 60ms y 300ms
    // case BUMBLEBEE:
    //   return random(200, 500); // Entre 200ms y 500ms (más lento)
    default:
      return 100;
  }
}

unsigned long getInsectSequenceInterval(InsectType type) {
  int multiplier = getFrequencyMultiplier();
  switch (type) {
    case SPIDER:
      return random(2000, 8000) * multiplier; // Entre 2-8 segundos
    case CRICKET:
      return random(1500, 2000) * multiplier; // Entre 1.5-2 segundos
    case BEETLE:
      return random(5000, 12000) * multiplier; // Entre 5-30 segundos (reducido)
    // case BUMBLEBEE:
    //   return random(5000, 18000) * multiplier; // Entre 3-22 segundos
    default:
      return 5000 * multiplier;
  }
}

// Funciones para reproducir sonidos
void playInsect1Sound() {
  // Validar índice antes de acceder al array
  if (insect1SequenceIndex < 0 || insect1SequenceIndex >= 8 || insect1SequenceIndex >= insect1SequenceLength) {
    Serial.print("Error: Índice de secuencia 1 inválido: ");
    Serial.print(insect1SequenceIndex);
    Serial.print(" (longitud: ");
    Serial.print(insect1SequenceLength);
    Serial.println(")");
    noTone(PIEZO_1_PIN);
    return;
  }
  
  int freq = insect1Sequence[insect1SequenceIndex];
  int duration = getInsectDuration(insect1Type);
  
  // Validar frecuencia antes de reproducir
  if (freq < 20 || freq > 8000) {
    Serial.print("Error: Frecuencia 1 inválida: ");
    Serial.print(freq);
    Serial.println(" Hz, usando 440 Hz");
    freq = 440;
  }
  
  tone(PIEZO_1_PIN, freq, duration);
  
  // Debug: mostrar tipo de insecto, nota raíz y octava
  if (insect1SequenceIndex == 0) {
    int octave = getOctaveFromFrequency(freq);
    String noteName = getNoteNameFromScale(freq);
    Serial.print("Insecto1 ");
    Serial.print(getInsectTypeName(insect1Type));
    Serial.print(" ");
    Serial.print(noteNames[rootNoteOffset]);
    Serial.print(" - ");
    Serial.print(noteName);
    Serial.print(octave);
    Serial.print(" (");
    Serial.print(freq);
    Serial.println(" Hz)");
  }
}

void playInsect2Sound() {
  // Validar índice antes de acceder al array
  if (insect2SequenceIndex < 0 || insect2SequenceIndex >= 8 || insect2SequenceIndex >= insect2SequenceLength) {
    Serial.print("Error: Índice de secuencia 2 inválido: ");
    Serial.print(insect2SequenceIndex);
    Serial.print(" (longitud: ");
    Serial.print(insect2SequenceLength);
    Serial.println(")");
    noTone(PIEZO_2_PIN);
    return;
  }
  
  int freq = insect2Sequence[insect2SequenceIndex];
  int duration = getInsectDuration(insect2Type);
  
  // Validar frecuencia antes de reproducir
  if (freq < 20 || freq > 8000) {
    Serial.print("Error: Frecuencia 2 inválida: ");
    Serial.print(freq);
    Serial.println(" Hz, usando 440 Hz");
    freq = 440;
  }
  
  tone(PIEZO_2_PIN, freq, duration);
  
  // Debug: mostrar tipo de insecto, nota raíz y octava
  if (insect2SequenceIndex == 0) {
    int octave = getOctaveFromFrequency(freq);
    String noteName = getNoteNameFromScale(freq);
    Serial.print("Insecto2 ");
    Serial.print(getInsectTypeName(insect2Type));
    Serial.print(" ");
    Serial.print(noteNames[rootNoteOffset]);
    Serial.print(" - ");
    Serial.print(noteName);
    Serial.print(octave);
    Serial.print(" (");
    Serial.print(freq);
    Serial.println(" Hz)");
  }
}

int getInsectDuration(InsectType type) {
  switch (type) {
    case SPIDER:
      return random(50, 150); // Entre 50ms y 150ms
    case CRICKET:
      return random(60, 180); // Entre 60ms y 180ms
    case BEETLE:
      return random(18, 40);  // Entre 18ms y 40ms
    // case BUMBLEBEE:
    //   return random(4000, 7000); // Entre 4-7 segundos (melodías largas)
    default:
      return 100;
  }
}

// Función para calcular la octava de una frecuencia
int getOctaveFromFrequency(int frequency) {
  // Frecuencias de referencia para Do en cada octava
  // Do0 = 16.35 Hz, Do1 = 32.70 Hz, Do2 = 65.41 Hz, Do3 = 130.81 Hz
  // Do4 = 261.63 Hz, Do5 = 523.25 Hz, Do6 = 1046.50 Hz, Do7 = 2093.00 Hz
  // Do8 = 4186.01 Hz, Do9 = 8372.02 Hz
  
  if (frequency < 23) return 0;      // Octava 0
  else if (frequency < 46) return 1; // Octava 1
  else if (frequency < 92) return 2; // Octava 2
  else if (frequency < 185) return 3; // Octava 3
  else if (frequency < 370) return 4; // Octava 4
  else if (frequency < 740) return 5; // Octava 5
  else if (frequency < 1480) return 6; // Octava 6
  else if (frequency < 2960) return 7; // Octava 7
  else if (frequency < 5920) return 8; // Octava 8
  else return 9; // Octava 9+
}

// Función para obtener el nombre de la nota desde la escala pentatónica
String getNoteNameFromScale(int frequency) {
  // Encontrar la nota más cercana en la escala
  int closestIndex = 0;
  int minDiff = abs(frequency - pentatonicScale[0]);
  
  for (int i = 1; i < 35; i++) {
    int diff = abs(frequency - pentatonicScale[i]);
    if (diff < minDiff) {
      minDiff = diff;
      closestIndex = i;
    }
  }
  
  // Mapear índice a nombre de nota
  String baseNotes[] = {"Do", "Re", "Mi", "Fa#", "La"};
  int noteInScale = closestIndex % 5;
  
  return baseNotes[noteInScale];
}

// Funciones para generar intervalos
void generateNewInsect1Interval() {
  insect1Interval = getInsectSequenceInterval(insect1Type);
  // Serial.print("Insecto 1: Nueva pausa de ");
  // Serial.print(insect1Interval);
  // Serial.println(" ms");
}

void generateNewInsect2Interval() {
  insect2Interval = getInsectSequenceInterval(insect2Type);
  // Serial.print("Insecto 2: Nueva pausa de ");
  // Serial.print(insect2Interval);
  // Serial.println(" ms");
}

void generateNewSequenceChangeInterval1() {
  int baseInterval = random(30000, 58000);
  int multiplier = getFrequencyMultiplier();
  sequenceChangeInterval1 = baseInterval * multiplier;
  // Serial.print("Insecto 1: Próximo cambio de secuencia en ");
  // Serial.print(sequenceChangeInterval1 / 1000);
  // Serial.println(" segundos");
}

void generateNewSequenceChangeInterval2() {
  int baseInterval = random(30000, 58000);
  int multiplier = getFrequencyMultiplier();
  sequenceChangeInterval2 = baseInterval * multiplier;
  // Serial.print("Insecto 2: Próximo cambio de secuencia en ");
  // Serial.print(sequenceChangeInterval2 / 1000);
  // Serial.println(" segundos");
}

void checkSequenceChange1(unsigned long currentTime) {
  if (currentTime - lastSequenceChange1 >= sequenceChangeInterval1) {
    generateRandomInsect1Sequence();
    lastSequenceChange1 = currentTime;
    generateNewSequenceChangeInterval1();
    // Serial.print("Insecto 1: Nueva secuencia (");
    // Serial.print(insect1SequenceLength);
    // Serial.println(" notas)");
  }
}

void checkSequenceChange2(unsigned long currentTime) {
  if (currentTime - lastSequenceChange2 >= sequenceChangeInterval2) {
    generateRandomInsect2Sequence();
    lastSequenceChange2 = currentTime;
    generateNewSequenceChangeInterval2();
    // Serial.print("Insecto 2: Nueva secuencia (");
    // Serial.print(insect2SequenceLength);
    // Serial.println(" notas)");
  }
}
