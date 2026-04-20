# 📊 Validación de datos – CANSAT AURORA

## 📊 VALIDACIÓN DE DATOS

- Relación presión-altitud  
- Comparación con modelos reales  

---

## 📌 Visión general

La validación de datos tiene como objetivo comprobar que las mediciones obtenidas durante la misión son coherentes con el comportamiento físico real de la atmósfera.

Se centra principalmente en el análisis de la relación entre presión, altitud y temperatura.

---

## 🌍 Relación presión-altitud

El sensor BMP280 permite estimar la altitud a partir de la presión atmosférica.

En condiciones normales:

- A mayor altitud → menor presión  
- A menor altitud → mayor presión  

Esta relación sigue un comportamiento no lineal descrito por modelos atmosféricos.

---

## 📐 Modelo teórico

Una aproximación habitual es la fórmula barométrica:

```text
h = 44330 · (1 - (P / P0)^(1/5.255))
Donde:

h = altitud (m)
P = presión medida (hPa)
P0 = presión a nivel del mar (≈ 1013.25 hPa)
```
---

##📊 Comparación con datos reales

Durante el análisis se comparan:

- Altitud calculada por el BMP280
- Variación de presión registrada
- Datos GPS (cuando hay fix)

Esto permite:

- Validar la coherencia de los sensores
- Detectar errores o ruido en las mediciones
- Confirmar eventos como el apogeo

---

##📈 Análisis típico

Se generan gráficas como:

- Presión vs tiempo
- Altitud vs tiempo
- Presión vs altitud

Estas gráficas permiten observar:

- Fase de ascenso (disminución de presión)
- Apogeo (mínimo de presión)
- Descenso (aumento de presión)

---

##⚠️ Fuentes de error
- Ruido en sensores
- Cambios de temperatura
- Errores en calibración
- Retrasos en medición
- Baja precisión del GPS

---

##🧠 Interpretación de resultados

Una validación correcta implica:

- Tendencia coherente presión-altitud
- Identificación clara de fases de vuelo
- Correspondencia entre sensores (BMP280, GPS)

---

##🎯 Objetivo de la validación
- Verificar la calidad de los datos recogidos
- Confirmar el comportamiento físico del sistema
- Garantizar la validez científica del experimento
- Apoyar las conclusiones de la misión

---

##📄 ➡️ Validación

Este apartado forma parte del análisis científico del proyecto CANSAT AURORA y permite evaluar la fiabilidad de los datos obtenidos durante la misión.
