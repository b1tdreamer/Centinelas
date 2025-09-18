/*
 * Test básico de piezoeléctricos para ESP32 Audio Kit
 * Este archivo es para pruebas rápidas en Arduino IDE
 */

// Pines para piezoeléctricos
#define PIEZO_1_PIN 21
#define PIEZO_2_PIN 22

void setup() {
  Serial.begin(115200);
  Serial.println("=== Test de Piezoeléctricos ===");
  
  // Configurar pines
  pinMode(PIEZO_1_PIN, OUTPUT);
  pinMode(PIEZO_2_PIN, OUTPUT);
  
  delay(1000);
}

void loop() {
  Serial.println("\n--- Menú de Pruebas ---");
  Serial.println("1. Test Piezo 1 (Grave)");
  Serial.println("2. Test Piezo 2 (Agudo)");
  Serial.println("3. Test Simultáneo");
  Serial.println("4. Test de Frecuencias");
  Serial.println("5. Test de Duración");
  Serial.println("6. Test de Intensidad");
  Serial.println("0. Salir");
  
  while (!Serial.available()) {
    delay(100);
  }
  
  int choice = Serial.parseInt();
  Serial.read(); // Limpiar buffer
  
  switch (choice) {
    case 1:
      testPiezo1();
      break;
    case 2:
      testPiezo2();
      break;
    case 3:
      testSimultaneo();
      break;
    case 4:
      testFrecuencias();
      break;
    case 5:
      testDuracion();
      break;
    case 6:
      testIntensidad();
      break;
    case 0:
      Serial.println("Saliendo...");
      return;
    default:
      Serial.println("Opción no válida");
  }
  
  delay(2000);
}

void testPiezo1() {
  Serial.println("Test Piezo 1 - Sonidos graves");
  
  int frecuencias[] = {200, 300, 400, 500, 600};
  
  for (int i = 0; i < 5; i++) {
    Serial.print("Frecuencia: ");
    Serial.print(frecuencias[i]);
    Serial.println(" Hz");
    
    tone(PIEZO_1_PIN, frecuencias[i], 1000);
    delay(1200);
  }
  
  noTone(PIEZO_1_PIN);
  Serial.println("Test Piezo 1 completado");
}

void testPiezo2() {
  Serial.println("Test Piezo 2 - Sonidos agudos");
  
  int frecuencias[] = {2000, 2500, 3000, 3500, 4000};
  
  for (int i = 0; i < 5; i++) {
    Serial.print("Frecuencia: ");
    Serial.print(frecuencias[i]);
    Serial.println(" Hz");
    
    tone(PIEZO_2_PIN, frecuencias[i], 1000);
    delay(1200);
  }
  
  noTone(PIEZO_2_PIN);
  Serial.println("Test Piezo 2 completado");
}

void testSimultaneo() {
  Serial.println("Test Simultáneo - Ambos piezoeléctricos");
  
  // Secuencia de acordes
  int graves[] = {200, 300, 400, 500};
  int agudos[] = {2000, 2500, 3000, 3500};
  
  for (int i = 0; i < 4; i++) {
    Serial.print("Acorde: ");
    Serial.print(graves[i]);
    Serial.print(" + ");
    Serial.print(agudos[i]);
    Serial.println(" Hz");
    
    tone(PIEZO_1_PIN, graves[i], 2000);
    tone(PIEZO_2_PIN, agudos[i], 2000);
    delay(2200);
  }
  
  noTone(PIEZO_1_PIN);
  noTone(PIEZO_2_PIN);
  Serial.println("Test simultáneo completado");
}

void testFrecuencias() {
  Serial.println("Test de Rango de Frecuencias");
  
  // Barrido de frecuencias
  for (int freq = 100; freq <= 5000; freq += 100) {
    Serial.print("Frecuencia: ");
    Serial.print(freq);
    Serial.println(" Hz");
    
    if (freq <= 1000) {
      tone(PIEZO_1_PIN, freq, 200);
    } else {
      tone(PIEZO_2_PIN, freq, 200);
    }
    
    delay(300);
  }
  
  noTone(PIEZO_1_PIN);
  noTone(PIEZO_2_PIN);
  Serial.println("Test de frecuencias completado");
}

void testDuracion() {
  Serial.println("Test de Duración de Notas");
  
  int duraciones[] = {100, 250, 500, 1000, 2000};
  
  for (int i = 0; i < 5; i++) {
    Serial.print("Duración: ");
    Serial.print(duraciones[i]);
    Serial.println(" ms");
    
    tone(PIEZO_1_PIN, 1000, duraciones[i]);
    delay(duraciones[i] + 200);
  }
  
  noTone(PIEZO_1_PIN);
  Serial.println("Test de duración completado");
}

void testIntensidad() {
  Serial.println("Test de Intensidad (PWM)");
  
  // Simular diferentes intensidades con PWM
  for (int intensity = 50; intensity <= 255; intensity += 50) {
    Serial.print("Intensidad: ");
    Serial.print(intensity);
    Serial.println("/255");
    
    // Usar PWM para simular intensidad
    analogWrite(PIEZO_1_PIN, intensity);
    delay(1000);
  }
  
  analogWrite(PIEZO_1_PIN, 0);
  Serial.println("Test de intensidad completado");
}
