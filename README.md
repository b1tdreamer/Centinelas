# Centinelas del Bosque - ESP32 Ecosystem Sonification

## Descripción
Proyecto de sonificación de ecosistemas forestales usando ESP32, con múltiples programas para explorar diferentes técnicas de síntesis sonora y simulación de insectos. Desde escalas pentatónicas básicas hasta polirritmias complejas y escalas modales.

## Estado del Proyecto
- **Programa Principal**: `piezoBugs/piezoBugs.ino` - Sistema modular básico
- **Tests Avanzados**: Múltiples programas de prueba en carpeta `tests/`
- **Síntesis Sonora**: FM, ruido blanco, tonos simples y híbridos
- **Escalas Musicales**: Pentatónica, modales (Frigia, Eólica, Dórica, etc.)

## Hardware Requerido
- ESP32 Audio Kit v2.2
- Aro LED Neopixel de 24 LEDs (5V)
- 2 Piezoeléctricos
- 6 Botones táctiles (integrados en AudioKit)
- LEDs de estado (integrados en AudioKit)

## Configuración

### 1. Instalar Arduino IDE
- Descargar desde: https://www.arduino.cc/en/software
- Instalar el core ESP32:
  - Abrir Arduino IDE
  - Ir a File → Preferences
  - Añadir URL: `https://dl.espressif.com/dl/package_esp32_index.json`
  - Ir a Tools → Board → Boards Manager
  - Buscar "ESP32" e instalar "ESP32 by Espressif Systems"

### 2. Seleccionar Board en Arduino IDE
- Board: "ESP32 Audio Kit"
- Si no aparece, usar: "ESP32 Dev Module"
- Port: Seleccionar el puerto COM correspondiente

### 3. Configurar WiFi (opcional)
Editar en `piezoBugs/piezoBugs.ino` o tests correspondientes:
```cpp
const char* ssid = "TU_WIFI_SSID";
const char* password = "TU_WIFI_PASSWORD";
```

### 4. Conexiones Hardware

#### ESP32 Audio Kit v2.2 - Pines
- **GPIO21**: Piezoeléctrico 1 (Insecto 1 - Araña por defecto)
- **GPIO22**: Piezoeléctrico 2 (Insecto 2 - Grillo por defecto)
- **GPIO23**: Aro LED Neopixel (24 LEDs)

#### Botones (integrados en AudioKit)
- **GPIO36**: Botón 1 (ON/OFF + Cambio de frecuencia)
- **GPIO35**: Botón 2 (Cambiar tipo Insecto 1)
- **GPIO27**: Botón 3 (Cambiar tipo Insecto 2)
- **GPIO33**: Botón 4 (Reservado)
- **GPIO25**: Botón 5 (Reservado)
- **GPIO26**: Botón 6 (Reservado)

#### LEDs de Estado (integrados en AudioKit)
- **GPIO2**: LED D1 (Sistema encendido)
- **GPIO14**: LED D3 (Actividad de audio)
- **GPIO15**: LED D4 (Estado mute)

#### Piezoeléctricos
- Conectar el pin positivo al GPIO correspondiente
- Conectar el pin negativo a GND
- Usar resistencia de 1kΩ en serie (opcional)

#### Aro LED Neopixel
- VCC → 3.3V (AudioKit no tiene 5V)
- GND → GND
- DIN → GPIO23

## 🧪 TESTS Y EXPERIMENTOS DESARROLLADOS

### 📁 `tests/piezoSoundTest.ino` - Laboratorio de Síntesis Sonora
**Programa de pruebas para explorar diferentes técnicas de generación de sonidos con piezoeléctricos.**

#### 🎵 **12 Tests de Síntesis Implementados:**
1. **Tonos Simples** - Frecuencias básicas con duración variable
2. **FM Synthesis** - Modulación de frecuencia para sonidos complejos
3. **White Noise** - Ruido blanco usando LFSR (Linear Feedback Shift Register)
4. **Filtered Noise** - Ruido filtrado en rangos específicos
5. **Granular Synthesis** - Texturas granulares con micro-sonidos
6. **Glissando** - Deslizamientos suaves de frecuencia
7. **Tremolo** - Modulación de amplitud simulada
8. **Complex Waves** - Simulación de armónicos y ondas complejas
9. **Arpeggios** - Secuencias rápidas de notas
10. **Rhythmic Patterns** - Patrones rítmicos complejos con acentos
11. **Insect Sounds** - Sonidos de insectos con síntesis avanzada
12. **Ambient Texture** - Texturas ambientales multicapa

#### ⚙️ **Características:**
- **Duración Variable**: 0.05s, 0.1s, 0.2s, 0.5s por sonido
- **Loop Continuo**: Reproducción hasta cambio de test
- **Control por Botón**: Botón 1 para ciclar entre tests
- **Parámetros Dinámicos**: Auto-variación de parámetros en tiempo real

---

### 📁 `tests/bugTypesTest.ino` - Ecosistema de Insectos Virtuales
**Simulación avanzada de 15 tipos de insectos con comportamientos estocásticos y escalas musicales.**

#### 🐛 **Evolución del Programa:**

##### **v1.0** - 15 Insectos Básicos
- 15 especies con nombres en español
- Síntesis: FM, ruido blanco, tonos simples
- Patrones estocásticos básicos

##### **v2.0** - Caos Extremo
- **11 insectos caóticos** + 4 conservados
- Sonidos **no-bloqueantes** para responsividad
- **Ruido blanco audible** con LFSR
- **Silencios variables** y secuencias largas
- **Botón responsivo** con detección mejorada

##### **v3.0** - Escalas Musicales y Pausas Obligatorias
- **11 escalas musicales** asignadas por insecto
- **Pausas obligatorias** para insectos caóticos
- Secuencias activas seguidas de silencios mandatorios
- Escalas: Do Mayor, La menor, Blues, Cromática, etc.

##### **v4.0** - 5 Insectos Simplificados (ACTUAL)
- **Reducido a 5 insectos** seleccionados
- **Escalas modales menos comunes**: Frigia, Eólica, Dórica
- **Pausas frecuentes pero cortas** (0.2-2s actividad, 0.2-0.8s pausa)
- **Polirritmias matemáticas** sin escalas (Mantis, Hormiga)

#### 🎭 **Insectos Actuales (v4.0):**

##### **1. Saltamontes Verde** 🦗
- **Escala**: Frigia (Do, Reb, Mib, Fa, Sol, Lab, Sib)
- **Comportamiento**: Clicks con pausas cada 8-22 notas
- **Pausas**: 0.8-2s actividad → 0.3-0.8s pausa

##### **2. Moscardón** 🐝
- **Escala**: Eólica (La, Si, Do, Re, Mi, Fa, Sol)
- **Comportamiento**: FM grave profundo continuo
- **Síntesis**: Modulación sutil en frecuencias graves

##### **3. Polilla Fantasma** 🦋
- **Escala**: Dórica (Re, Mi, Fa, Sol, La, Si, Do)
- **Comportamiento**: Clicks etéreos intermitentes
- **Pausas**: 0.6-1.5s actividad → 0.2-0.6s pausa

##### **4. Mantis Religiosa** 🙏
- **Sin Escala** - Polirritmias libres
- **Comportamiento**: **Patrones matemáticos complejos**:
  - Patrón 0: **3 contra 2**
  - Patrón 1: **5 contra 3**
  - Patrón 2: **7 contra 4**
- **Frecuencias**: 4000-8000Hz libres

##### **5. Hormiga Roja** 🐜
- **Sin Escala** - Comunicación social
- **Comportamiento**: **Grupos de 3-8 sonidos**
- **Simulación**: Comunicación de colonia con frecuencias relacionadas
- **Frecuencias**: 6000-10000Hz libres

#### 🎼 **Escalas Modales Implementadas:**
| Escala | Notas | Carácter |
|--------|-------|----------|
| **Frigia** | Do, Reb, Mib, Fa, Sol, Lab, Sib | Exótico, oriental |
| **Eólica** | La, Si, Do, Re, Mi, Fa, Sol | Natural menor |
| **Dórica** | Re, Mi, Fa, Sol, La, Si, Do | Fantasmal, etéreo |
| **Mixolidia** | Sol, La, Si, Do, Re, Mi, Fa | Dominante, blues |
| **Lidia** | Fa, Sol, La, Si, Do, Re, Mi | Brillante, suspendido |
| **Locria** | Si, Do, Re, Mib, Fa, Sol, Lab | Inestable, disonante |
| **Harmónica menor** | La, Si, Do, Re, Mi, Fa, Sol# | Exótico, dramático |

---

### 📁 Backups y Versiones
**Sistema de backup organizado en carpeta `tests/`:**

#### **PiezoBugs Principal:**
- `tests/piezoBugs_backup_seguro.ino` - Backup estable del programa principal
- `tests/piezoBugs_v0.X.ino` - Versiones históricas numeradas

#### **BugTypesTest Versiones:**
- `tests/bugTypesTest_v1.2_fixed.ino` - Primera versión estable
- `tests/bugTypesTest_v2.0_chaotic.ino` - Versión caótica extrema
- `tests/bugTypesTest_v3.1_compilation_fixed.ino` - Con escalas musicales
- `tests/bugTypesTest_v4.0_simplified.ino` - Versión actual simplificada

#### **PiezoSoundTest:**
- `tests/piezoSoundTest.ino` - Laboratorio de síntesis (eliminado)
- Documentado en `tests/piezoSoundTest_README.md`

#### **Documentación:**
- `tests/backup_process.md` - Procedimientos de backup estandarizados
- `tests/BugTypesTest_Documentation.md` - Características de cada insecto
- `tests/piezoBugs_backups/README.md` - Historial de versiones

---

## Funcionalidades Actuales

### Sistema de Insectos Modulares
- **3 tipos de insectos**: Araña, Grillo, Escarabajo
- **Comportamientos únicos**: Cada tipo tiene patrones de sonido, duración y frecuencia específicos
- **Escala pentatónica menor**: 35 notas desde octava 2 hasta octava 8
- **Secuencias aleatorias**: Cada insecto genera secuencias únicas

### Tipos de Insectos

#### 🕷️ Araña (Insecto 1)
- **Rango**: Octavas 5, 6, 7, 8 (frecuencias agudas)
- **Secuencias**: 3-15 notas por secuencia
- **Intervalo entre notas**: 20-260ms
- **Intervalo entre secuencias**: 2-8 segundos (con multiplicador)

#### 🦗 Grillo (Insecto 2)
- **Rango**: Octavas 2, 3, 4 (frecuencias graves)
- **Secuencias**: 3-4 notas por secuencia
- **Intervalo entre notas**: 80-200ms
- **Intervalo entre secuencias**: 1.5-2 segundos (con multiplicador)

#### 🪲 Escarabajo
- **Rango**: Octavas pares (2, 4, 6, 8) - solo notas Si
- **Secuencias**: 4-8 notas por secuencia
- **Intervalo entre notas**: 18-40ms
- **Intervalo entre secuencias**: 10-30 segundos (con multiplicador)

### Control por Botones
- **Botón 1**: Pulsación larga (1s) = ON/OFF, Pulsación corta = Cambiar frecuencia
- **Botón 2**: Cambiar tipo del Insecto 1 (Araña → Grillo → Escarabajo)
- **Botón 3**: Cambiar tipo del Insecto 2 (Grillo → Escarabajo → Araña)
- **Estados de frecuencia**: Normal (x1) → Lento (x3) → Muy Lento (x5) → Extremo (x7)

### Efectos Visuales
- **Aro LED Neopixel**: 24 LEDs con patrón de 1 de cada 5 encendidos
- **Colores**: Verde (sonido activo) / Rojo (sonido desactivado)
- **LEDs de estado**: D1 (sistema), D3 (audio), D4 (mute)
- **LEDs alternativos**: GPIO16-19 para pruebas

### Escala Musical
- **Escala pentatónica menor en Si**: Si, Re, Mi, Fa#, La
- **Rango completo**: 7 octavas (123 Hz - 14,080 Hz)
- **Distribución por insectos**: Octavas graves para Grillo, agudas para Araña

---

## 🎯 TÉCNICAS DE SÍNTESIS EXPLORADAS

### 🔊 **Síntesis de Frecuencia Modulada (FM)**
- **Implementación**: Modulador sinusoidal aplicado a frecuencia portadora
- **Casos de Uso**: Moscardón (FM grave), síntesis de insectos complejos
- **Parámetros**: Frecuencia portadora, moduladora, índice de modulación
- **Resultado**: Sonidos complejos con armónicos dinámicos

### 🌊 **Generación de Ruido Blanco**
- **Implementación**: LFSR (Linear Feedback Shift Register) de 16-bit
- **Técnica**: Retroalimentación lineal para secuencias pseudo-aleatorias
- **Aplicación**: Ruido musical filtrado por escalas
- **Variaciones**: Ruido filtrado, texturas ambientales

### 🎼 **Escalas Modales Avanzadas**
- **7 modos implementados**: Jónico, Dórico, Frigio, Lidio, Mixolidio, Eólico, Locrio
- **Escala Harmónica Menor**: Para efectos dramáticos y exóticos
- **Aplicación Musical**: Cada insecto con carácter modal único
- **Mapeo Frecuencial**: Escalas de 12 notas por octava

### 🔄 **Polirritmias Matemáticas**
- **Patrones Implementados**: 3:2, 5:3, 7:4
- **Algoritmo**: Contadores modulares independientes
- **Aplicación**: Mantis Religiosa (patrones complejos)
- **Evolución**: Cambio automático de patrones cada 21 beats

### 👥 **Simulación de Comunicación Social**
- **Grupos Dinámicos**: 3-8 sonidos por grupo
- **Frecuencias Relacionadas**: Progresión matemática dentro del grupo
- **Pausas Grupales**: Silencios automáticos entre grupos
- **Aplicación**: Hormiga Roja (comunicación de colonia)

---

## 🚀 PRÓXIMOS PASOS

### 🎵 **Síntesis Avanzada**
- [x] **Síntesis FM**: ✅ Implementada con modulación sinusoidal
- [x] **Ruido Blanco**: ✅ Implementado con LFSR de 16-bit  
- [x] **Tremolo**: ✅ Implementado en piezoSoundTest
- [x] **Glissando**: ✅ Deslizamientos suaves implementados
- [x] **Granular**: ✅ Texturas granulares implementadas
- [x] **Arpeggios**: ✅ Secuencias rápidas implementadas
- [ ] **Síntesis Aditiva**: Combinación de múltiples senos para timbres complejos
- [ ] **Filtros Digitales**: Filtros pasa-bajos/altos para esculpir timbres
- [ ] **Envolventes ADSR**: Attack, Decay, Sustain, Release para sonidos naturales

### 🎼 **Escalas y Teoría Musical**
- [x] **Escala Pentatónica**: ✅ Implementada en programa principal
- [x] **Escalas Modales**: ✅ 7 modos implementados (Frigia, Eólica, Dórica, etc.)
- [x] **Escala Harmónica Menor**: ✅ Implementada para efectos dramáticos
- [x] **Polirritmias**: ✅ Patrones 3:2, 5:3, 7:4 implementados
- [ ] **Cambio de Nota Raíz**: Implementar botones para cambiar afinación
- [ ] **12 Semitonos**: Permitir cambiar de Si a Do, Do#, Re, etc.
- [ ] **Persistencia**: Guardar la afinación seleccionada en memoria

### 🐛 **Comportamientos de Insectos**
- [x] **3 Insectos Básicos**: ✅ Araña, Grillo, Escarabajo (programa principal)
- [x] **15 Insectos Avanzados**: ✅ Implementados en bugTypesTest v1.0-v3.0
- [x] **5 Insectos Refinados**: ✅ Versión actual simplificada v4.0
- [x] **Patrones Estocásticos**: ✅ Implementados con variación aleatoria
- [x] **Comunicación Social**: ✅ Simulación de grupos (Hormiga Roja)
- [x] **Pausas Inteligentes**: ✅ Pausas frecuentes pero cortas implementadas
- [ ] **Comportamientos Temporales**: Variaciones según hora del día
- [ ] **Interacciones entre Especies**: Insectos que reaccionan entre sí

### 🌐 **Conectividad Ecosistémica**
- [ ] **Datos Biológicos Reales**: Integración con sensores de árboles
- [ ] **Protocolo IoT**: Comunicación entre múltiples centinelas
- [ ] **Sincronización Temporal**: Coordinación de patrones entre dispositivos
- [ ] **Base de Datos**: Almacenamiento y recuperación de datos sonoros
- [ ] **WiFi Mesh**: Red distribuida de centinelas

### 🎧 **Sonificación de Biodatos**
- [ ] **Mapeo Paramétrico**: Humedad → Frecuencia, Temperatura → Tempo
- [ ] **Entrada MIDI**: Recepción de datos externos por minijack
- [ ] **Algoritmos Adaptativos**: Respuesta dinámica a cambios ambientales
- [ ] **Visualización Sonora**: LEDs sincronizados con datos biológicos
- [ ] **Sensores Ambientales**: Integración con sensores de humedad, temperatura, luz

### 🌟 **Red de Centinelas Distribuida**
- [ ] **Protocolo de Comunicación**: WiFi/LoRa para transmisión a larga distancia
- [ ] **Comportamiento Emergente**: Patrones colectivos sin control central
- [ ] **Geolocalización**: GPS para mapeo espacial de la red
- [ ] **Efectos Espaciales**: Pan, eco y reverb distribuido geográficamente
- [ ] **Sincronización Visual**: Coordinar efectos LED entre dispositivos

### 🔧 **Mejoras Técnicas**
- [x] **Código No-Bloqueante**: ✅ Implementado en bugTypesTest v2.0+
- [x] **Botón Responsivo**: ✅ Detección mejorada implementada
- [x] **Sistema de Backups**: ✅ Organizado en carpeta tests/
- [x] **Documentación Completa**: ✅ README actualizado con todos los tests
- [ ] **Optimización de Memoria**: Reducir uso de RAM para más complejidad
- [ ] **Gestión de Energía**: Modos de bajo consumo para funcionamiento autónomo
- [ ] **OTA Updates**: Actualización inalámbrica de firmware

## Comandos Útiles en Arduino IDE

### Compilar y subir
- **Compilar**: Ctrl+R o botón ✓
- **Subir**: Ctrl+U o botón →
- **Compilar y subir**: Shift+Ctrl+U

### Monitor serial
- **Abrir monitor**: Ctrl+Shift+M o Tools → Serial Monitor
- **Baudrate**: Configurar a 115200 (según el programa)
- **Plotter serie**: Tools → Serial Plotter

### Gestión de archivos
- **Abrir archivo**: Ctrl+O
- **Guardar**: Ctrl+S
- **Nuevo**: Ctrl+N

## Troubleshooting

### Problemas de Botones
- **Botones no responden**: Verificar que los pines estén correctamente configurados
- **Detección errónea**: Ajustar umbrales de ADC (valores < 500 para botones analógicos)
- **Pines flotando**: Usar pines digitales con pull-up interno para botones problemáticos

### Problemas de Audio
- **Sin sonido**: Verificar conexiones de piezoeléctricos en GPIO21 y GPIO22
- **Sonido distorsionado**: Comprobar voltaje de alimentación y resistencias
- **Frecuencias incorrectas**: Verificar que la escala pentatónica esté bien configurada

### Problemas de LEDs
- **Aro LED no funciona**: Verificar conexión en GPIO23 y alimentación 3.3V
- **LEDs completamente encendidos**: Problema de timing en la comunicación Neopixel
- **Patrón incorrecto**: Verificar que se envíen los datos correctos (1 de cada 5 LEDs)

### Problemas de Insectos
- **Secuencias no cambian**: Verificar que los intervalos de cambio estén configurados
- **Tipos no cambian**: Comprobar que los botones 2 y 3 estén funcionando
- **Frecuencias no se aplican**: Verificar que el multiplicador se aplique correctamente

## 📋 VERSIONES Y ESTADO ACTUAL

### **Programa Principal:** `piezoBugs/piezoBugs.ino` v0.9
- Sistema modular básico de 3 insectos (Araña, Grillo, Escarabajo)
- Escala pentatónica menor en Si
- Control por botones y efectos LED
- **Estado**: Estable, funcional

### **Programa de Tests:** `tests/bugTypesTest.ino` v4.0
- 5 insectos con escalas modales avanzadas
- Polirritmias matemáticas y comunicación social
- Pausas frecuentes pero cortas
- **Estado**: Activo, en desarrollo

### **Laboratorio de Síntesis:** `tests/piezoSoundTest.ino`
- 12 técnicas de síntesis diferentes
- Tests de FM, ruido blanco, granular, etc.
- **Estado**: Documentado, archivado

---

## 🎼 **RESUMEN TÉCNICO FINAL**

**El proyecto Centinelas del Bosque ha evolucionado desde un sistema básico de simulación de insectos hasta un laboratorio completo de síntesis sonora y ecosistemas virtuales:**

### **🔬 Investigación Desarrollada:**
- **12 técnicas de síntesis** implementadas y probadas
- **7 escalas modales** + escala harmónica menor
- **Algoritmos matemáticos** para polirritmias complejas
- **Simulación social** de comunicación de insectos
- **Ruido blanco musical** con LFSR de 16-bit

### **🎯 Logros Técnicos:**
- **Síntesis no-bloqueante** para máxima responsividad
- **Escalas musicales auténticas** implementadas en frecuencias
- **Comportamientos emergentes** de comunicación grupal
- **Sistema de backups organizado** con documentación completa
- **Modularidad extrema** para fácil extensión

### **🚀 Potencial Futuro:**
- Base sólida para **sonificación de datos biológicos reales**
- Arquitectura preparada para **redes distribuidas de sensores**
- **Escalabilidad** a ecosistemas complejos multiespecie
- **Integración IoT** para monitoreo forestal en tiempo real

**Estado del Proyecto**: **ACTIVO** - Listo para integración con datos biológicos y expansión a redes distribuidas.
