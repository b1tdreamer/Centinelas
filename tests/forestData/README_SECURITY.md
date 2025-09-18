# 🔐 SEGURIDAD - ForestData

## ⚠️ ADVERTENCIA IMPORTANTE

Este proyecto contiene **información sensible** que NO debe subirse a GitHub:
- Tokens de InfluxDB
- URLs de servidores privados
- Credenciales de base de datos

## 📋 CONFIGURACIÓN INICIAL

### 1. **Configurar Secretos (OBLIGATORIO)**
```bash
# 1. Copiar template
cp secrets_template.h secrets.h

# 2. Editar con tus datos reales
# Completar: INFLUXDB_TOKEN, INFLUXDB_URL, etc.

# 3. VERIFICAR que secrets.h está en .gitignore
git status  # secrets.h NO debe aparecer
```

### 2. **Verificar Seguridad**
```bash
# Verificar que secrets.h está ignorado
git check-ignore secrets.h
# Debe retornar: secrets.h

# Verificar que no hay archivos sensibles
git status
# secrets.h NO debe aparecer en la lista
```

## 🔑 DATOS REQUERIDOS

### **InfluxDB Configuration**
- **INFLUXDB_URL**: URL completa del servidor
- **INFLUXDB_TOKEN**: Token de API (obtener de InfluxDB UI)
- **INFLUXDB_ORG**: Nombre de la organización
- **INFLUXDB_BUCKET**: Nombre del bucket de datos

### **Dónde Obtener los Datos:**
1. **InfluxDB Cloud**: https://cloud2.influxdata.com/
2. **API Tokens**: Data → API Tokens → Generate API Token
3. **Organization**: Settings → About → Organization ID
4. **Bucket**: Data → Buckets → Bucket Name

## 🚨 SI SUBISTE DATOS SENSIBLES A GITHUB

### **Opción 1: Cambiar Datos (RECOMENDADO)**
```bash
# 1. Cambiar tokens/passwords en el servicio
# 2. Regenerar credenciales
# 3. Actualizar secrets.h con nuevos datos
```

### **Opción 2: Limpiar Historial (AVANZADO)**
```bash
# ⚠️ PELIGROSO - Solo si no hay colaboradores
git filter-branch --force --index-filter \
  'git rm --cached --ignore-unmatch tests/forestData/forestData.ino' \
  --prune-empty --tag-name-filter cat -- --all

# Forzar push
git push origin --force --all
git push origin --force --tags
```

### **Opción 3: Repositorio Nuevo (MÁS SEGURO)**
```bash
# 1. Crear nuevo repositorio
# 2. Copiar código limpio (sin secretos)
# 3. Hacer commit inicial
# 4. Eliminar repositorio anterior
```

## ✅ CHECKLIST DE SEGURIDAD

- [ ] `secrets.h` creado y configurado
- [ ] `secrets.h` está en `.gitignore`
- [ ] `git status` no muestra `secrets.h`
- [ ] Datos sensibles removidos del código
- [ ] Token regenerado si fue expuesto
- [ ] README actualizado con instrucciones

## 🆘 EN CASO DE EXPOSICIÓN

### **INMEDIATAMENTE:**
1. **Cambiar/Regenerar** todas las credenciales expuestas
2. **Revocar tokens** comprometidos en InfluxDB
3. **Verificar logs** de acceso en servicios
4. **Notificar** al equipo si aplica

### **DESPUÉS:**
1. Implementar este sistema de secretos
2. Revisar todo el historial de Git
3. Auditar otros proyectos similares
4. Establecer procedimientos de seguridad

## 📞 SOPORTE

Si necesitas ayuda con la configuración de seguridad:
1. Revisar `secrets_template.h` 
2. Verificar documentación de InfluxDB
3. Contactar al administrador del sistema
