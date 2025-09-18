# PiezoBugs - Historial de Backups

## Versiones Principales

### **piezoBugs_spider16_v3_final.ino** ⭐ **[VERSIÓN ACTUAL]**
- **Fecha**: 17/09/2025
- **Características**:
  - Secuencias de araña expandidas a **16 notas máximo**
  - Corrección de índices corruptos (problema race conditions)
  - Información de octava en salidas seriales
  - Arrays: Insecto1[16], Insecto2[8]
  - Validaciones adaptativas según tipo de insecto
- **Estado**: Estable, sin errores críticos

### **piezoBugs_corregido_v2.ino**
- **Fecha**: 17/09/2025  
- **Características**:
  - Corrección del problema de índices corruptos
  - Lógica de incremento mejorada en handleInsect1/2
  - Arrays: Insecto1[8], Insecto2[8]
- **Estado**: Corrección específica del problema de overflow

### **piezoBugs_backup_seguro.ino**
- **Fecha**: 17/09/2025
- **Características**:
  - Versión original con información de octava añadida
  - Base estable antes de correcciones mayores
  - Arrays: Insecto1[8], Insecto2[8]
- **Estado**: Backup de seguridad

## Versiones de Desarrollo Anteriores

### **piezoBugs_v0.4_sintes_noFunca.ino**
- Intento de síntesis que no funcionó
- Mantener para referencia de qué NO hacer

### **piezoBugs_v0.3.ino**
- Versión estable anterior
- Base para desarrollos

### **piezoBugs_v0.2.ino** / **piezoBugs_v0.1.ino**
- Versiones iniciales de desarrollo
- Funcionalidad básica

## Archivos de Prueba

### **piezoBugs.ino** (en esta carpeta)
- Copia de trabajo para tests locales
- No usar como referencia principal

## Organización del Proyecto

```
CentinelasBosque/
├── piezoBugs/
│   └── piezoBugs.ino              # Archivo principal de trabajo
├── tests/
│   ├── piezoBugs_backups/         # Esta carpeta - historial completo
│   ├── piezoSoundTest/            # Programa de pruebas de síntesis
│   └── forestData/                # Datos forestales
└── README.md
```

## Notas de Desarrollo

- **Problema resuelto**: Índices corruptos por race conditions
- **Mejora implementada**: Secuencias de araña de 3-16 notas
- **Próximas mejoras**: Síntesis FM, ruido blanco, texturas granulares
- **Testing**: Usar piezoSoundTest/ para experimentar nuevas técnicas

## Uso Recomendado

1. **Para producción**: Usar `piezoBugs_spider16_v3_final.ino`
2. **Para desarrollo**: Copiar desde backup a `../piezoBugs/`
3. **Para experimentar**: Usar `../piezoSoundTest/`
4. **Para rollback**: Cualquier versión anterior disponible aquí
