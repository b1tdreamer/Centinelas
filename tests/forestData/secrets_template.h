/*
 * SECRETS_TEMPLATE.H - Plantilla de Configuración
 * 
 * 📋 INSTRUCCIONES DE USO:
 * 1. Copiar este archivo como 'secrets.h'
 * 2. Completar con tus datos reales
 * 3. Verificar que 'secrets.h' está en .gitignore
 * 4. NUNCA subir 'secrets.h' a GitHub
 */

#ifndef SECRETS_H
#define SECRETS_H

// ======================================
// 🌐 CONFIGURACIÓN INFLUXDB
// ======================================

// URL del servidor InfluxDB
const char* INFLUXDB_URL = "https://tu-servidor-influxdb.com";

// Token de autenticación InfluxDB
// 🔑 OBTENER DE: InfluxDB UI → Data → API Tokens
const char* INFLUXDB_TOKEN = "TU_TOKEN_INFLUXDB_AQUI";

// Organización InfluxDB
// 🏢 OBTENER DE: InfluxDB UI → About → Organization
const char* INFLUXDB_ORG = "TU_ORGANIZACION";

// Bucket de datos
// 📊 OBTENER DE: InfluxDB UI → Data → Buckets
const char* INFLUXDB_BUCKET = "tu_bucket";

// ======================================
// 📶 CONFIGURACIÓN WIFI (OPCIONAL)
// ======================================
// Descomenta si quieres WiFi automático sin WiFiManager

// const char* WIFI_SSID = "TU_RED_WIFI";
// const char* WIFI_PASSWORD = "TU_PASSWORD_WIFI";

// ======================================
// 🔑 OTROS SECRETOS
// ======================================

// API Keys para otros servicios
// const char* OTHER_API_KEY = "tu_api_key_aqui";

// Certificados SSL personalizados (si necesarios)
// const char* SSL_CERT = "-----BEGIN CERTIFICATE-----\n...\n-----END CERTIFICATE-----";

#endif // SECRETS_H
