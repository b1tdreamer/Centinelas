# Forest Data - Prueba de Conectividad InfluxDB

Este proyecto es una prueba de concepto para conectar un ESP32 AudioKit 2.2 con InfluxDB Cloud y obtener datos de sensores de árboles en tiempo real.

## Características

- **WiFiManager**: Configuración fácil de red WiFi mediante portal web
- **InfluxDB Cloud**: Conexión HTTPS a https://db.sinfoniabiotica.xyz
- **Bucket**: biodata
- **Polling**: Consultas cada 10 segundos
- **Datos**: Solo los más recientes de sensores del árbol

## Configuración

1. Sube el código al ESP32
2. Al iniciar, si no hay WiFi configurado, se creará un punto de acceso
3. Conecta a la red "ESP32-ForestData"
4. Abre el navegador en http://192.168.4.1
5. Selecciona tu red WiFi e introduce la contraseña
6. El ESP32 se conectará y empezará a consultar InfluxDB

## Monitoreo

Abre el Monitor Serie a 115200 baudios para ver:
- Estado de conexión WiFi
- Consultas a InfluxDB
- Datos recibidos
- Errores de conexión

## Estructura de Datos Esperada

El sistema espera datos en formato InfluxDB con mediciones de sensores de árboles como:
- Humedad del suelo
- Temperatura
- Actividad bioeléctrica
- Luz solar
- etc.
