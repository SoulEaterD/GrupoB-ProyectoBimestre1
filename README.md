<center>
  <h1>
    <strong>GRUPO B - PROYECTO BIMESTRE 1: Ejecución de múltiples tareas con procesos y con hilos.</strong>
  </h1>
</center>

<p align="center">
  <a href="https://www.epn.edu.ec/">
    <img src="https://img.shields.io/badge/Escuela%20Polit%C3%A9cnica%20Nacional-blue?style=for-the-badge">
  </a>
  <a href="https://fis.epn.edu.ec/index.php/es/">
    <img src="https://img.shields.io/badge/Facultad%20de%20Ingenier%C3%ADa%20en%20Sistemas-purple?style=for-the-badge">
  </a>
  <a href="https://www.epn.edu.ec/">
    <img src="https://img.shields.io/badge/Software-darkgreen?style=for-the-badge">
  </a>
</p>

***
### **Integrantes**

<ul>
  <li>Carrera Fernando</li>
  <li>Cevallos Guillermo</li>
  <li>Dávila Gissel</li>
  <li>Rodríguez Ariel</li>
</ul>

***
## **Problema**

<p>
  Desarrollar dos versiones de un programa en C que procese un conjunto de transacciones simuladas (pueden representar consultas a una base de datos, procesamiento de archivos, cálculos, etc.).
  Ambas versiones deben atender los mismo grupos de transacciones.
</p>

***

## Simulador de Venta de Entradas: Concierto Don Medardo y sus Players

Este proyecto implementa un simulador concurrente de venta de entradas para un concierto, utilizando dos enfoques diferentes:

- Versión con **procesos** (fork) y memoria compartida (mmap)
- Versión con **hilos** (pthread) y mutexes

### Contenido del Repositorio
- README.md 

- hilos.c  

- procesosMMAP.c 

### Versión con Hilos (hilos.c)

En esta versión se utilizan cinco hilos (pthread) que representan cinco cajeros. Cada hilo se encarga de procesar una parte de las transacciones.

- Sincronización: Se emplean pthread_mutex_t para asegurar el acceso exclusivo a cada zona de entradas.
- Estructura compartida: Una estructura común en memoria local.
- Ventaja: Mayor eficiencia por no requerir la creación de procesos.

### Versión con Procesos (procesosMMAP.c)

En esta versión se utilizan cinco procesos hijos creados con fork(). Todos los procesos comparten una estructura de entradas mediante mmap.

- Sincronización: Se utiliza un semáforo (sem_t) para asegurar el acceso mutuo a las zonas.
- Estructura compartida: Memoria compartida mediante mmap.
- Ventaja: Aislamiento entre procesos y simulación de entornos independientes.

### Medición de Tiempo

Ambas versiones emplean la función clock_gettime() con CLOCK_MONOTONIC para calcular el tiempo total que toma realizar la venta completa de entradas.

***

## Compilación y Ejecución

### Versión con Hilos
gcc -pthread hilos.c -o hilos 

./hilos

### Versión con Procesos
gcc -pthread procesosMMAP.c -o procesos 

./procesos

