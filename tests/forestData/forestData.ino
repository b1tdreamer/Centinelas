/*
 * Forest Data - Prueba de Conectividad InfluxDB
 * ESP32 AudioKit 2.2 con WiFiManager e InfluxDB Cloud
 * 
 * Características:
 * - WiFiManager para configuración fácil de red
 * - Conexión HTTPS a InfluxDB Cloud
 * - Consultas cada 10 segundos
 * - Parsing de datos de sensores de árboles
 */

#include <WiFiManager.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>

// 🔐 Importar configuración sensible desde archivo externo
// ⚠️  VERIFICAR que secrets.h existe y está en .gitignore
#include "secrets.h"

// Configuración de timing
const unsigned long QUERY_INTERVAL = 10000; // 10 segundos
unsigned long lastQueryTime = 0;

// WiFiManager
WiFiManager wm;

// Cliente HTTPS
WiFiClientSecure client;

// Variables para datos del árbol
struct TreeData {
  float humidity = 0.0;
  float temperature = 0.0;
  float bioelectrical_activity = 0.0;
  float light_level = 0.0;
  unsigned long timestamp = 0;
  bool data_valid = false;
} currentTreeData;

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== Forest Data - InfluxDB Test ===");
  
  // Configurar cliente HTTPS
  client.setInsecure(); // Para pruebas, en producción usar certificados
  
  // Configurar WiFiManager
  setupWiFiManager();
  
  Serial.println("Sistema iniciado correctamente");
  Serial.println("Esperando datos de InfluxDB...");
}

void loop() {
  // Verificar conexión WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi desconectado, intentando reconectar...");
    WiFi.reconnect();
    delay(5000);
    return;
  }
  
  // Consultar InfluxDB cada 10 segundos
  if (millis() - lastQueryTime >= QUERY_INTERVAL) {
    fetchTreeData();
    lastQueryTime = millis();
  }
  
  // Mostrar datos actuales cada 5 segundos
  static unsigned long lastDisplayTime = 0;
  if (millis() - lastDisplayTime >= 5000) {
    displayCurrentData();
    lastDisplayTime = millis();
  }
  
  delay(100);
}

void setupWiFiManager() {
  // Configurar WiFiManager
  wm.setConfigPortalTimeout(300); // 5 minutos timeout
  wm.setConnectTimeout(20); // 20 segundos para conectar
  wm.setAPStaticIPConfig(IPAddress(192,168,4,1), IPAddress(192,168,4,1), IPAddress(255,255,255,0));
  
  // Intentar conectar con credenciales guardadas
  if (!wm.autoConnect("ESP32-ForestData")) {
    Serial.println("No se pudo conectar a WiFi");
    Serial.println("Configuración fallida, reiniciando...");
    delay(3000);
    ESP.restart();
  }
  
  Serial.println("WiFi conectado exitosamente!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
}

void fetchTreeData() {
  Serial.println("\n--- Consultando InfluxDB ---");
  Serial.println("Intentando conectar a: db.sinfoniabiotica.xyz:443");
  
  if (!client.connect("db.sinfoniabiotica.xyz", 443)) {
    Serial.println("Error: No se pudo conectar a InfluxDB");
    Serial.println("Verifica la conexión a internet y la URL");
    return;
  }
  
  Serial.println("Conexión HTTPS establecida exitosamente");
  
  // Construir query para obtener los datos más recientes
  String query = "from(bucket: \"biodata\") |> range(start: -1h) |> last()";
  Serial.println("Query: " + query);
  
  // Construir request HTTP
  String httpRequest = "POST /api/v2/query?org=" + String(INFLUXDB_ORG) + " HTTP/1.1\r\n";
  httpRequest += "Host: db.sinfoniabiotica.xyz\r\n";
  httpRequest += "Authorization: Token " + String(INFLUXDB_TOKEN) + "\r\n";
  httpRequest += "Content-Type: application/vnd.flux\r\n";
  httpRequest += "Content-Length: " + String(query.length()) + "\r\n";
  httpRequest += "Connection: close\r\n\r\n";
  httpRequest += query;
  
  Serial.println("Enviando request HTTP...");
  Serial.println("Organización: " + String(INFLUXDB_ORG));
  
  // Enviar request
  client.print(httpRequest);
  Serial.println("Request enviado, esperando respuesta...");
  
  // Leer respuesta
  String response = "";
  unsigned long timeout = millis() + 10000; // 10 segundos timeout
  int bytesReceived = 0;
  
  Serial.println("Leyendo respuesta...");
  while (client.connected() && millis() < timeout) {
    if (client.available()) {
      char c = client.read();
      response += c;
      bytesReceived++;
      
      // Mostrar progreso cada 100 bytes
      if (bytesReceived % 100 == 0) {
        Serial.print(".");
      }
      
      // Limitar tamaño de respuesta
      if (response.length() > 5000) {
        Serial.println("\nRespuesta demasiado grande, truncando...");
        break;
      }
    }
  }
  
  Serial.println("\nConexión cerrada");
  client.stop();
  
  if (response.length() == 0) {
    Serial.println("Error: No se recibió respuesta de InfluxDB");
    Serial.println("Timeout alcanzado o conexión perdida");
    return;
  }
  
  Serial.println("Respuesta recibida (" + String(response.length()) + " bytes)");
  Serial.println("Primeros 200 caracteres de la respuesta:");
  Serial.println(response.substring(0, min(200, (int)response.length())));
  
  // Parsear respuesta
  parseInfluxResponse(response);
}

void parseInfluxResponse(String response) {
  // Buscar el cuerpo de la respuesta (después de headers HTTP)
  int bodyStart = response.indexOf("\r\n\r\n");
  if (bodyStart == -1) {
    Serial.println("Error: No se encontró el cuerpo de la respuesta");
    return;
  }
  
  String body = response.substring(bodyStart + 4);
  
  // Verificar si hay datos
  if (body.length() < 10) {
    Serial.println("No hay datos disponibles en InfluxDB");
    currentTreeData.data_valid = false;
    return;
  }
  
  Serial.println("Datos CSV recibidos:");
  Serial.println(body);
  
  // Parsear datos CSV de InfluxDB
  // Formato: _result,_table,_start,_stop,_time,_value,_field,_measurement
  parseCSVData(body);
}

void parseCSVData(String csvData) {
  // Resetear datos
  currentTreeData.data_valid = false;
  currentTreeData.timestamp = millis();
  
  // Dividir en líneas
  int lineStart = 0;
  int lineEnd = csvData.indexOf('\n');
  
  // Saltar primera línea (headers)
  if (lineEnd != -1) {
    lineStart = lineEnd + 1;
    lineEnd = csvData.indexOf('\n', lineStart);
  }
  
  // Procesar cada línea de datos
  while (lineEnd != -1) {
    String line = csvData.substring(lineStart, lineEnd);
    if (line.length() > 10) { // Ignorar líneas vacías
      parseDataLine(line);
    }
    
    lineStart = lineEnd + 1;
    lineEnd = csvData.indexOf('\n', lineStart);
  }
  
  // Procesar última línea
  if (lineStart < csvData.length()) {
    String line = csvData.substring(lineStart);
    if (line.length() > 10) {
      parseDataLine(line);
    }
  }
  
  currentTreeData.data_valid = true;
  Serial.println("Datos parseados exitosamente");
}

void parseDataLine(String line) {
  // Dividir línea por comas
  int fieldStart = 0;
  int fieldEnd = line.indexOf(',');
  int fieldIndex = 0;
  
  String measurement = "";
  String field = "";
  String value = "";
  
  while (fieldEnd != -1 && fieldIndex < 8) {
    String fieldValue = line.substring(fieldStart, fieldEnd);
    
    switch (fieldIndex) {
      case 6: // _field
        field = fieldValue;
        break;
      case 7: // _measurement
        measurement = fieldValue;
        break;
      case 5: // _value
        value = fieldValue;
        break;
    }
    
    fieldStart = fieldEnd + 1;
    fieldEnd = line.indexOf(',', fieldStart);
    fieldIndex++;
  }
  
  // Procesar último campo
  if (fieldIndex == 7 && fieldStart < line.length()) {
    measurement = line.substring(fieldStart);
  }
  
  // Asignar valores según el campo
  if (field == "humidity" && value.length() > 0) {
    currentTreeData.humidity = value.toFloat();
    Serial.println("Humedad: " + String(currentTreeData.humidity) + "%");
  }
  else if (field == "temperature" && value.length() > 0) {
    currentTreeData.temperature = value.toFloat();
    Serial.println("Temperatura: " + String(currentTreeData.temperature) + "°C");
  }
  else if (field == "bioelectrical_activity" && value.length() > 0) {
    currentTreeData.bioelectrical_activity = value.toFloat();
    Serial.println("Actividad bioeléctrica: " + String(currentTreeData.bioelectrical_activity));
  }
  else if (field == "light_level" && value.length() > 0) {
    currentTreeData.light_level = value.toFloat();
    Serial.println("Nivel de luz: " + String(currentTreeData.light_level));
  }
}

void displayCurrentData() {
  Serial.println("\n=== DATOS ACTUALES DEL ÁRBOL ===");
  Serial.println("Timestamp: " + String(currentTreeData.timestamp));
  Serial.println("Datos válidos: " + String(currentTreeData.data_valid ? "SÍ" : "NO"));
  
  if (currentTreeData.data_valid) {
    Serial.println("Humedad: " + String(currentTreeData.humidity) + "%");
    Serial.println("Temperatura: " + String(currentTreeData.temperature) + "°C");
    Serial.println("Actividad bioeléctrica: " + String(currentTreeData.bioelectrical_activity));
    Serial.println("Nivel de luz: " + String(currentTreeData.light_level));
  } else {
    Serial.println("No hay datos disponibles");
  }
  Serial.println("================================");
}
