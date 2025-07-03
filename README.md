# MATE Analysis Tool

MATE Analysis Tool es una pequeña herramienta en C++ basada en ROOT que facilita el procesamiento y análisis de datos del Muon Andes Telescope (MATE).  
El detector MATE cuenta con tres módulos de detección: M101, M102 y M103, cada uno compuesto por 2 planos ortogonales de barras centelladoras (planos A y B).

<p align="center">
  <img src="https://github.com/user-attachments/assets/aa7dae95-5371-4c08-8705-21bd21c34249" width="300">
  <img src="https://github.com/user-attachments/assets/5d267bf0-3375-40cc-b9f7-96af8e46b52b" width="300">
</p>


## Características principales

- **Validación de fechas** (m101/m102/m103): Detecta archivos faltantes, líneas corruptas y otros posibles errores para cada archivo con datos del detector, genera un informe y separa los datos inválidos en un directorio definido por el usuario.
- **Conversión a ROOT**: Luego de validar los archivos, el programa da la opción de seleccionar una lista o intervalo de fechas para procesar los datos limpios y crear un `TTree` para realizar el análisis con ROOT.
El programa además da la opción de transformar los datos limpios en archivos CSV los cuales contienen la información de los 6 planos del detector, de esta manera se facilita un posterior análisis de los datos con herramientas externas. Los archivos resultantes .CSV y .ROOT poseen el siguiente formato en su columnas/branches: 

| Columna / Branch | Descripción |
|------------------|-------------|
| `ts` | **UNIX Timestamp (resolución de 1 s)**. Estampa temporal generada por el router al recibir el evento. |
| `A1`, `B1` | Barras activadas en los planos **A1** y **B1** del módulo **M101**. |
| `A2`, `B2` | Barras activadas en los planos **A2** y **B2** del módulo **M102**. |
| `A3`, `B3` | Barras activadas en los planos **A3** y **B3** del módulo **M103**. |
| `nA1`, `nB1`, `nA2`, `nB2`, `nA3`, `nB3` | Contadores de hits simultáneos por plano (útiles para filtros de multiplicidad). |
| `ts2_m101`, `ts2_m102`, `ts2_m103` | **eKit Local Timestamp (resolución de 100 ns)** de cada módulo. Puede ser `0` debido a un bug. Se usan en el ajuste exponencial. |
| `evt` | Identificador del evento. |
NOTA: Las columnas Ai y Bi en los archivos .csv corresponden a listas contenidas dentro de un paréntesis (), mientras que las branches del TTree corresponden a vectores de tamaño 12. Es importante mencionar que el método Scan de ROOT solo permite mostrar un único elemento de un vector, por defecto es el primer elemento (Ai[0] y Bi[0]) 
- **Concatenación de árboles**:  El programa permite unir una lista o intervalo de archivos ROOT usando la clase `TChain` con opción de exportar el archivo .root resultante.
  <img width="1333" alt="image" src="https://github.com/user-attachments/assets/3b2cada3-2c53-4fbd-8bb5-419a84366aa2" />

- **Filtrado interactivo**: permite aplicar expresiones ROOT para cortes en los datos de todo tipo, ya sea en memoria o creando un nuevo árbol filtrado.
- **Generación de histogramas 1D/2D**: produce reportes en PDF con estilos predefinidos.
  <img width="800" alt="image" src="https://github.com/user-attachments/assets/08d89b79-c74d-4da2-b0cc-5d6279935275" />
  <img width="500" alt="image" src="https://github.com/user-attachments/assets/e92a4a91-d36f-4683-b5cf-0e299a9f723e" />


- **Ajuste exponencial del rate**: procesa timestamps de los tres planos, calcula la distribución de tiempos entre eventos y ajusta un modelo exponencial, mostrando gráficas y parámetros.
  <img width="600" alt="image" src="https://github.com/user-attachments/assets/1ce5137f-cecf-4c21-992d-6a682f4ae9ac" />



## Requisitos

- Compilador con soporte **C++17**
- **ROOT 6** instalado (disponible como `root-config`)

## Estructura del proyecto

```
MATE-Analysis-Tool/
├── include/       # headers (.hpp)
├── src/           # implementaciones (.cpp)
└── README.md      # este archivo
```

## Compilación

1. Clona el repositorio:
   ```sh
   git clone https://github.com/diegolmos/MATE-Analysis-Tool.git
   cd MATE-Analysis-Tool
   ```
2. Ejecuta:
   ```sh
   g++ -std=c++17 -Iinclude \
     src/PathConfig.cpp src/DataValidator.cpp src/DataProcessor.cpp \
     src/DateSelector.cpp src/TreeLoader.cpp src/DataFilter.cpp \
     src/HistogramMaker.cpp src/RateFitter.cpp src/main.cpp \
     $(root-config --cflags --libs) -o MATE-Analysis-Tool
   ```

El binario `MATE-Analysis-Tool` quedará listo para su uso.

## Instrucciones de uso

Antes de iniciar, prepare dos carpetas:

- **data/**: coloque los archivos de texto con datos del detector, nombrados como `YYYY_MM_DD_06h00_mate-m101.txt`, `YYYY_MM_DD_06h00_mate-m102.txt` y `YYYY_MM_DD_06h00_mate-m103.txt`.
- **badData/**: aquí se moverán las triadas m101/m102/m103 inválidas (faltantes o con errores de formato y escritura ).

Para ejecutar la herramienta, se pedirá especificar también el nombre archivo de reporte, este archivo de texto contiene la información de los datos inválidos, indicando la línea, nombre del archivo y el tipo de problema que presenta.


En este flujo:

1. **Validación**: se comprueba cada fecha en `data/` y se registra en `reporte.txt` cualquier triada incompleta o línea corrupta; las triadas erróneas se mueven a `badData/`.
2. **Procesamiento**: las triadas válidas generan un archivo `YYYYMMDD_combined_output.txt` y un árbol ROOT `YYYYMMDD_output.root` en el directorio actual.
3. **Menú interactivo**: ofrece opciones para:
   - Concatenar árboles ROOT y guardar el resultado.
   - Aplicar filtros dinámicos.
   - Generar histogramas 1D/2D en PDF.
   - Ajustar la distribución de tiempos entre eventos con modelo exponencial.

## Módulos internos

- **PathConfig**: configura rutas de datos y reporte.
- **DataValidator**: inspecciona y reporta triadas de texto.
- **DateSelector**: maneja la selección de fechas.
- **DataProcessor**: convierte triadas en combinados y árboles ROOT.
- **TreeLoader**: concatena y explora árboles ROOT.
- **DataFilter**: construye filtros con expresiones ROOT.
- **HistogramMaker**: crea histogramas 1D/2D en PDF.
- **RateFitter**: implementa la limpieza de timestamps y el ajuste exponencial del rate.

## Flujo de trabajo resumido

1. Validación y separación de datos malos.
2. Procesamiento de triadas válidas.
3. Carga/concatenación de ROOT.
4. Filtrado interactivo.
5. Generación de histogramas.
6. Ajuste exponencial del rate.

---
## Reminder de próximas adiciones

1. Incluir branch/columna de zenith para cada medición/fecha, necesaria para hacer el ajuste de rate vs. ángulo.
2. permitir también concatenar archivos .csv
3. Incluir branch de theta_x junto a theta_y
4. Crear módulo que permita hacer el fitting de rate vs. ángulo
5. Implementación en QT creator
6. Optimizar validación de fechas

