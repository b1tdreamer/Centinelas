#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>

// Configuración de pines para ESP32 Audio Kit
#define PIEZO_PIN_1 25    // GPIO25 para primer piezoeléctrico
#define PIEZO_PIN_2 26    // GPIO26 para segundo piezoeléctrico
#define LED_PIN 27        // GPIO27 para aro de LEDs Neopixel
#define LED_COUNT 8       // Número de LEDs en el aro

// Configuración WiFi
const char* ssid = "TU_WIFI_SSID";
const char* password = "TU_WIFI_PASSWORD";

// Configuración InfluxDB
const char* influxdb_url = "http://tu-influxdb-server:8086";
const char* database = "arbol_data";

// Objeto para controlar LEDs
Adafruit_NeoPixel pixels(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// Variables para control de piezoeléctricos
unsigned long lastPiezoUpdate = 0;
int piezo1Freq = 1000;
int piezo2Freq = 2000;
bool piezo1Active = false;
bool piezo2Active = false;

void setup() {
  Serial.begin(115200);
  Serial.println("Centinelas del Bosque - Iniciando...");
  
  // Configurar pines de piezoeléctricos
  pinMode(PIEZO_PIN_1, OUTPUT);
  pinMode(PIEZO_PIN_2, OUTPUT);
  
  // Inicializar LEDs
  pixels.begin();
  pixels.setBrightness(50);
  pixels.show();
  
  // Conectar a WiFi
  connectToWiFi();
  
  // Test inicial de componentes
  testPiezoElectrics();
  testLEDs();
  
  Serial.println("Sistema iniciado correctamente");
}

void loop() {
  // Simular datos de árbol (por ahora)
  simulateTreeData();
  
  // Actualizar piezoeléctricos cada 100ms
  if (millis() - lastPiezoUpdate > 100) {
    updatePiezoElectrics();
    lastPiezoUpdate = millis();
  }
  
  // Actualizar LEDs
  updateLEDs();
  
  delay(50);
}

void connectToWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.println("WiFi conectado!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void testPiezoElectrics() {
  Serial.println("Probando piezoeléctricos...");
  
  // Test piezoeléctrico 1 - sonido grave
  Serial.println("Piezo 1 - Sonido grave (500Hz)");
  tone(PIEZO_PIN_1, 500, 1000);
  delay(1200);
  
  // Test piezoeléctrico 2 - sonido agudo
  Serial.println("Piezo 2 - Sonido agudo (3000Hz)");
  tone(PIEZO_PIN_2, 3000, 1000);
  delay(1200);
  
  // Test simultáneo
  Serial.println("Test simultáneo - Ambos piezoeléctricos");
  tone(PIEZO_PIN_1, 800, 2000);
  tone(PIEZO_PIN_2, 2500, 2000);
  delay(2200);
  
  noTone(PIEZO_PIN_1);
  noTone(PIEZO_PIN_2);
  
  Serial.println("Test de piezoeléctricos completado");
}

void testLEDs() {
  Serial.println("Probando LEDs...");
  
  // Test secuencial
  for (int i = 0; i < LED_COUNT; i++) {
    pixels.setPixelColor(i, pixels.Color(255, 0, 0)); // Rojo
    pixels.show();
    delay(200);
  }
  
  // Test todos juntos
  for (int i = 0; i < LED_COUNT; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 255, 0)); // Verde
  }
  pixels.show();
  delay(1000);
  
  // Apagar todos
  pixels.clear();
  pixels.show();
  
  Serial.println("Test de LEDs completado");
}

void simulateTreeData() {
  // Simular datos de actividad del árbol
  static float treeActivity = 0.5;
  static float increment = 0.01;
  
  treeActivity += increment;
  if (treeActivity >= 1.0 || treeActivity <= 0.0) {
    increment = -increment;
  }
  
  // Convertir actividad a frecuencias de piezoeléctricos
  piezo1Freq = 500 + (treeActivity * 1000);  // 500-1500 Hz
  piezo2Freq = 1500 + (treeActivity * 2000); // 1500-3500 Hz
  
  // Activar/desactivar basado en actividad
  piezo1Active = treeActivity > 0.3;
  piezo2Active = treeActivity > 0.7;
}

void updatePiezoElectrics() {
  if (piezo1Active) {
    tone(PIEZO_PIN_1, piezo1Freq, 50);
  } else {
    noTone(PIEZO_PIN_1);
  }
  
  if (piezo2Active) {
    tone(PIEZO_PIN_2, piezo2Freq, 50);
  } else {
    noTone(PIEZO_PIN_2);
  }
}

void updateLEDs() {
  static unsigned long lastLEDUpdate = 0;
  static int ledIndex = 0;
  
  if (millis() - lastLEDUpdate > 200) {
    // Limpiar LEDs
    pixels.clear();
    
    // Encender LED actual con color basado en actividad
    uint32_t color;
    if (piezo1Active && piezo2Active) {
      color = pixels.Color(255, 255, 0); // Amarillo - alta actividad
    } else if (piezo1Active || piezo2Active) {
      color = pixels.Color(0, 255, 0);   // Verde - actividad media
    } else {
      color = pixels.Color(0, 0, 255);   // Azul - baja actividad
    }
    
    pixels.setPixelColor(ledIndex, color);
    pixels.show();
    
    // Mover al siguiente LED
    ledIndex = (ledIndex + 1) % LED_COUNT;
    lastLEDUpdate = millis();
  }
}

// Función para obtener datos reales de InfluxDB (implementar después)
void fetchTreeData() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(influxdb_url);
    http.addHeader("Content-Type", "application/json");
    
    // Query para obtener datos más recientes del árbol
    String query = "SELECT * FROM arbol_vital_signs ORDER BY time DESC LIMIT 1";
    
    int httpResponseCode = http.GET();
    
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Datos recibidos: " + response);
      
      // Parsear JSON y actualizar variables
      // TODO: Implementar parsing de datos reales
    } else {
      Serial.print("Error en HTTP: ");
      Serial.println(httpResponseCode);
    }
    
    http.end();
  }
}
