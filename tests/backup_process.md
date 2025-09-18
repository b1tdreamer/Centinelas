# Proceso de Backup - CentinelasBosque

## Estructura de Backups

Todos los backups se guardan en la carpeta `tests/` siguiendo esta organización:

### **Backups de PiezoBugs**
```
tests/piezoBugs_backups/
├── README.md                          # Documentación de versiones
├── piezoBugs_spider16_v3_final.ino   # ⭐ Versión actual estable
├── piezoBugs_corregido_v2.ino        # Corrección de race conditions
├── piezoBugs_backup_seguro.ino       # Backup de seguridad
└── piezoBugs_v0.x.ino                # Versiones históricas
```

### **Backups de PiezoSoundTest**
```
tests/
├── piezoSoundTest/                    # Carpeta principal del proyecto
│   └── piezoSoundTest.ino            # Versión de trabajo
├── piezoSoundTest_v2_extended.ino    # Backup con sonidos extendidos
└── test_piezo.ino                    # Pruebas básicas
```

## Comandos de Backup

### **Para PiezoBugs:**
```powershell
# Crear backup con timestamp
copy piezoBugs\piezoBugs.ino tests\piezoBugs_backups\piezoBugs_vX.X_descripcion.ino

# Ejemplos:
copy piezoBugs\piezoBugs.ino tests\piezoBugs_backups\piezoBugs_v4.0_fm_synthesis.ino
copy piezoBugs\piezoBugs.ino tests\piezoBugs_backups\piezoBugs_v4.1_granular_fix.ino
```

### **Para PiezoSoundTest:**
```powershell
# Crear backup de nuevas versiones
copy tests\piezoSoundTest\piezoSoundTest.ino tests\piezoSoundTest_vX.X_feature.ino

# Ejemplo:
copy tests\piezoSoundTest\piezoSoundTest.ino tests\piezoSoundTest_v3.0_advanced_fm.ino
```

## Convención de Nombres

### **Formato:**
`archivo_vX.X_descripcion.ino`

### **Ejemplos:**
- `piezoBugs_v3.1_spider16.ino` - Versión 3.1 con secuencias de araña de 16 notas
- `piezoBugs_v3.2_fm_synthesis.ino` - Versión 3.2 con síntesis FM
- `piezoSoundTest_v2.1_continuous.ino` - Versión 2.1 con loop continuo

### **Sufijos Especiales:**
- `_final` - Versión estable lista para producción
- `_backup` - Backup de seguridad antes de cambios importantes  
- `_fix` - Corrección de bug específico
- `_test` - Versión experimental
- `_noFunca` - Versión con problemas conocidos (mantener para referencia)

## Proceso Recomendado

1. **Antes de cambios importantes:**
   ```powershell
   copy piezoBugs\piezoBugs.ino tests\piezoBugs_backups\piezoBugs_vX.X_backup.ino
   ```

2. **Después de nueva funcionalidad:**
   ```powershell
   copy piezoBugs\piezoBugs.ino tests\piezoBugs_backups\piezoBugs_vX.X_feature.ino
   ```

3. **Versión estable:**
   ```powershell
   copy piezoBugs\piezoBugs.ino tests\piezoBugs_backups\piezoBugs_vX.X_final.ino
   ```

4. **Actualizar README:**
   - Documentar cambios en `tests/piezoBugs_backups/README.md`
   - Marcar versión actual con ⭐
   - Anotar estado (estable/experimental/deprecated)

## Restaurar desde Backup

```powershell
# Restaurar versión específica
copy tests\piezoBugs_backups\piezoBugs_vX.X.ino piezoBugs\piezoBugs.ino

# Ejemplo:
copy tests\piezoBugs_backups\piezoBugs_spider16_v3_final.ino piezoBugs\piezoBugs.ino
```

## Limpieza Periódica

- Mantener máximo 10 backups por proyecto
- Eliminar versiones `_test` obsoletas
- Conservar siempre las versiones `_final` y `_backup`
- Mantener versiones con `_noFunca` como referencia de problemas
