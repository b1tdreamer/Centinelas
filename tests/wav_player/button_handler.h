/*
 * button_handler.h - Manejo avanzado de botones para ESP32 AudioKit v2.2
 * Incluye antirebote, pulsaciones largas/cortas y manejo de pines compartidos
 */

#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

#include "audio_config.h"

// ===============================================
// ESTRUCTURAS Y TIPOS
// ===============================================

// Tipos de pulsación
enum ButtonPressType {
  PRESS_NONE,
  PRESS_SHORT,
  PRESS_LONG,
  PRESS_DOUBLE
};

// Estado de un botón
struct ButtonState {
  bool currentState;
  bool lastState;
  unsigned long pressStartTime;
  unsigned long lastPressTime;
  bool isPressed;
  bool longPressTriggered;
  int consecutivePresses;
};

// ===============================================
// CONFIGURACIÓN DE BOTONES
// ===============================================

// Tiempos para diferentes tipos de pulsación
#define LONG_PRESS_TIME       1000    // 1 segundo para pulsación larga
#define DOUBLE_PRESS_WINDOW   400     // 400ms máximo entre pulsaciones dobles
#define DEBOUNCE_TIME         50      // 50ms de antirebote

// ===============================================
// CLASE PARA MANEJO DE BOTONES
// ===============================================

class ButtonHandler {
private:
  ButtonState buttons[6];
  unsigned long lastUpdateTime;
  
  // Funciones privadas
  bool readButtonRaw(int buttonIndex);
  void updateButtonState(int buttonIndex);
  ButtonPressType getButtonPress(int buttonIndex);
  
public:
  // Constructor
  ButtonHandler();
  
  // Inicialización
  void begin();
  
  // Actualización (llamar en loop principal)
  void update();
  
  // Verificar estado de botones
  bool isPressed(int buttonIndex);
  bool wasPressed(int buttonIndex);
  bool wasReleased(int buttonIndex);
  ButtonPressType getPressType(int buttonIndex);
  
  // Funciones específicas para cada botón
  bool playPausePressed();
  bool stopPressed();
  bool previousPressed();
  bool nextPressed();
  bool volumeUpPressed();
  bool volumeDownPressed();
  
  // Debug
  void printButtonStates();
};

// ===============================================
// IMPLEMENTACIÓN DE LA CLASE
// ===============================================

ButtonHandler::ButtonHandler() {
  lastUpdateTime = 0;
  
  // Inicializar estados de botones
  for (int i = 0; i < 6; i++) {
    buttons[i].currentState = false;
    buttons[i].lastState = false;
    buttons[i].pressStartTime = 0;
    buttons[i].lastPressTime = 0;
    buttons[i].isPressed = false;
    buttons[i].longPressTriggered = false;
    buttons[i].consecutivePresses = 0;
  }
}

void ButtonHandler::begin() {
  Serial.println("Inicializando ButtonHandler...");
  
  // Configurar pines de botones
  pinMode(BUTTON_1, INPUT);           // ADC, no necesita pull-up
  pinMode(BUTTON_2, INPUT_PULLUP);    // Digital con pull-up
  pinMode(BUTTON_4, INPUT_PULLUP);    // Digital con pull-up
  
  // NOTA: BUTTON_3, BUTTON_5, BUTTON_6 comparten pines con SD/SPI
  // Se configurarán dinámicamente cuando sea necesario
  
  Serial.println("Botones configurados:");
  Serial.println("- Botón 1 (GPIO36): Play/Pause - ADC");
  Serial.println("- Botón 2 (GPIO13): Stop - Digital");
  Serial.println("- Botón 4 (GPIO16): Siguiente - Digital");
  Serial.println("- Botones 3,5,6: Pines compartidos con SD/SPI");
}

void ButtonHandler::update() {
  unsigned long currentTime = millis();
  
  // Actualizar solo si ha pasado suficiente tiempo
  if (currentTime - lastUpdateTime >= DEBOUNCE_TIME) {
    for (int i = 0; i < 6; i++) {
      updateButtonState(i);
    }
    lastUpdateTime = currentTime;
  }
}

bool ButtonHandler::readButtonRaw(int buttonIndex) {
  switch (buttonIndex) {
    case 0: // BUTTON_1 - ADC
      return analogRead(BUTTON_1) < BUTTON_ADC_THRESHOLD;
      
    case 1: // BUTTON_2 - Digital
      return !digitalRead(BUTTON_2); // Invertido por pull-up
      
    case 2: // BUTTON_3 - Compartido con SD CS
      // Solo leer si no estamos usando SD activamente
      return false; // Deshabilitado por conflicto con SD
      
    case 3: // BUTTON_4 - Digital
      return !digitalRead(BUTTON_4); // Invertido por pull-up
      
    case 4: // BUTTON_5 - Compartido con SPI SCK
      // Solo disponible cuando no se usa SPI
      return false; // Deshabilitado por conflicto con SPI
      
    case 5: // BUTTON_6 - Compartido con SPI MOSI
      // Solo disponible cuando no se usa SPI
      return false; // Deshabilitado por conflicto con SPI
      
    default:
      return false;
  }
}

void ButtonHandler::updateButtonState(int buttonIndex) {
  if (buttonIndex < 0 || buttonIndex >= 6) return;
  
  ButtonState &btn = buttons[buttonIndex];
  unsigned long currentTime = millis();
  
  // Leer estado actual
  btn.currentState = readButtonRaw(buttonIndex);
  
  // Detectar flanco de subida (pulsación)
  if (btn.currentState && !btn.lastState) {
    btn.pressStartTime = currentTime;
    btn.isPressed = true;
    btn.longPressTriggered = false;
    
    // Verificar pulsación doble
    if (currentTime - btn.lastPressTime < DOUBLE_PRESS_WINDOW) {
      btn.consecutivePresses++;
    } else {
      btn.consecutivePresses = 1;
    }
    
    btn.lastPressTime = currentTime;
  }
  
  // Detectar pulsación larga
  if (btn.isPressed && btn.currentState && !btn.longPressTriggered) {
    if (currentTime - btn.pressStartTime >= LONG_PRESS_TIME) {
      btn.longPressTriggered = true;
    }
  }
  
  // Detectar flanco de bajada (liberación)
  if (!btn.currentState && btn.lastState) {
    btn.isPressed = false;
  }
  
  btn.lastState = btn.currentState;
}

ButtonPressType ButtonHandler::getPressType(int buttonIndex) {
  if (buttonIndex < 0 || buttonIndex >= 6) return PRESS_NONE;
  
  ButtonState &btn = buttons[buttonIndex];
  
  // Pulsación larga detectada
  if (btn.longPressTriggered && btn.currentState) {
    return PRESS_LONG;
  }
  
  // Pulsación corta al liberar
  if (!btn.currentState && btn.lastState && !btn.longPressTriggered) {
    if (btn.consecutivePresses >= 2) {
      btn.consecutivePresses = 0; // Reset contador
      return PRESS_DOUBLE;
    }
    return PRESS_SHORT;
  }
  
  return PRESS_NONE;
}

bool ButtonHandler::isPressed(int buttonIndex) {
  if (buttonIndex < 0 || buttonIndex >= 6) return false;
  return buttons[buttonIndex].currentState;
}

bool ButtonHandler::wasPressed(int buttonIndex) {
  if (buttonIndex < 0 || buttonIndex >= 6) return false;
  return buttons[buttonIndex].currentState && !buttons[buttonIndex].lastState;
}

bool ButtonHandler::wasReleased(int buttonIndex) {
  if (buttonIndex < 0 || buttonIndex >= 6) return false;
  return !buttons[buttonIndex].currentState && buttons[buttonIndex].lastState;
}

// Funciones específicas para cada botón
bool ButtonHandler::playPausePressed() {
  ButtonPressType press = getPressType(0); // BUTTON_1
  return (press == PRESS_SHORT);
}

bool ButtonHandler::stopPressed() {
  ButtonPressType press = getPressType(1); // BUTTON_2
  return (press == PRESS_SHORT);
}

bool ButtonHandler::previousPressed() {
  ButtonPressType press = getPressType(2); // BUTTON_3
  return (press == PRESS_SHORT);
}

bool ButtonHandler::nextPressed() {
  ButtonPressType press = getPressType(3); // BUTTON_4
  return (press == PRESS_SHORT);
}

bool ButtonHandler::volumeUpPressed() {
  ButtonPressType press = getPressType(5); // BUTTON_6
  return (press == PRESS_SHORT);
}

bool ButtonHandler::volumeDownPressed() {
  ButtonPressType press = getPressType(4); // BUTTON_5
  return (press == PRESS_SHORT);
}

void ButtonHandler::printButtonStates() {
  if (!DEBUG_BUTTON_STATES) return;
  
  Serial.println("=== Estado de Botones ===");
  for (int i = 0; i < 6; i++) {
    Serial.print("Botón ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(buttons[i].currentState ? "PRESIONADO" : "LIBERADO");
    
    ButtonPressType press = getPressType(i);
    switch (press) {
      case PRESS_SHORT:
        Serial.print(" [CORTA]");
        break;
      case PRESS_LONG:
        Serial.print(" [LARGA]");
        break;
      case PRESS_DOUBLE:
        Serial.print(" [DOBLE]");
        break;
      default:
        break;
    }
    Serial.println();
  }
  Serial.println("========================");
}

// ===============================================
// FUNCIONES AUXILIARES PARA PINES COMPARTIDOS
// ===============================================

// Función para temporalmente configurar un pin compartido como entrada
void temporaryConfigureAsInput(int pin) {
  pinMode(pin, INPUT_PULLUP);
  delay(10); // Pequeña pausa para estabilizar
}

// Función para restaurar configuración SPI después de leer botón
void restoreSPIConfiguration() {
  // Reconfigurar pines SPI si es necesario
  pinMode(SPI_MOSI, OUTPUT);
  pinMode(SPI_MISO, INPUT);
  pinMode(SPI_SCK, OUTPUT);
  pinMode(SD_CS, OUTPUT);
}

// Función para leer botón compartido de forma segura
bool readSharedButton(int pin) {
  // Guardar configuración actual
  temporaryConfigureAsInput(pin);
  
  // Leer estado del botón
  bool state = !digitalRead(pin); // Invertido por pull-up
  
  // Restaurar configuración SPI
  restoreSPIConfiguration();
  
  return state;
}

#endif // BUTTON_HANDLER_H
