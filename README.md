<center>
  <h1>
    <strong>GRUPO B - PROYECTO BIMESTRE 1: Ejecución de múltiples tareas con procesos y con hilos.</strong>
  </h1>
</center>

  <a href="https://www.epn.edu.ec/">
    <img src="https://img.shields.io/badge/Escuela%20Polit%C3%A9cnica%20Nacional-blue?style=for-the-badge">
  </a>
  <a href="https://fis.epn.edu.ec/index.php/es/">
    <img src="https://img.shields.io/badge/Facultad%20de%20Ingenier%C3%ADa%20en%20Sistemas-purple?style=for-the-badge">
  </a>
  <a href="https://www.epn.edu.ec/">
    <img src="https://img.shields.io/badge/Software-darkgreen?style=for-the-badge">
  </a>

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

### Requisitos del Sistema
<ul>
  <li>Sistema operativo tipo UNIX (Linux)</li>
  <li>Compilador compatible con C (gcc)</li>
</ul>

### Comandos de compilación y ejecución
En esta sección se detallan los comandos necesarios para la compilación y ejecución de cada una de las versiones del proyecto.
Es importante asegurarse de ejecutar los comandos desde el mismo directorio donde se encuentran los archivos fuente (.c), ya que al compilar se generará un archivo ejecutable en dicho lugar.
#### Versión con procesos
Ejcutar los comandos en el orden que se muestra:
```bash
gcc procesosMMAP.c -o procesosMMAP
```
```bash
./procesosMMAP
```
#### Versión con hilos
Ejcutar los comandos en el orden que se muestra:
```bash
gcc -pthread hilos.c -o hilos
```
```bash
./hilos
```
### Resultados de la ejecución
En ambas versiones se espera obtener lo siguiente: 
```bash
************ CONCIERTO DON MEDARDO Y SUS PLAYERS ************

*************** INICIO VENTA ENTRADAS: 0.000000 ***************

[Cajero: 5] Ventas por zona:
Tribuna: 20000
Golden Box: 20000
VIP: 10000
Top Box: 6000
Players Box: 4000

[Cajero: 3] Ventas por zona:
Tribuna: 20000
Golden Box: 20000
VIP: 10000
Top Box: 6000
Players Box: 4000

[Cajero: 1] Ventas por zona:
Tribuna: 20000
Golden Box: 20000
VIP: 10000
Top Box: 6000
Players Box: 4000

[Cajero: 2] Ventas por zona:
Tribuna: 20000
Golden Box: 20000
VIP: 10000
Top Box: 6000
Players Box: 4000

[Cajero: 4] Ventas por zona:
Tribuna: 20000
Golden Box: 20000
VIP: 10000
Top Box: 6000
Players Box: 4000

*************** TODAS LAS ENTRADAS HAN SIDO VENDIDAS ***************

*************** FIN VENTA DE ENTRADAS: 0.026526 ***************
```
<p>
  Además, el tiempo total que tarda en ejecutarse el programa se refleja en el mensaje final:
  *************** FIN VENTA DE ENTRADAS: x.xxxxxx ***************
  Este valor representa el tiempo total, en segundos, que tomó el sistema en completar la venta de todas las entradas.
</p>
<p>Nota: En la salida se puede llegar a presentar un comportamiento de superposición debido a que los múltiples procesos e hilos están escribiendo al mismo tiempo en la consola.</p>

***
### Conclusiones
#### Comparación tiempos de ejecución
<p>
Se realizaron múltiples ejecuciones del sistema de venta de entradas usando tanto procesos como hilos. Los hilos obtuvieron consistentemente mejores tiempos de ejecución, debido a que comparten el mismo       espacio de memoria y requieren menos sobrecarga para la creación, gestión y sincronización. Al no necesitar mecanismos de comunicación interprocesos como memoria compartida y semáforos entre procesos, los hilos pueden operar con mayor eficiencia y menor latencia.
</p>

#### Diferencias encontradas
<p>
La principal diferencia radica en la forma en que los hilos y procesos gestionan el acceso compartido a los recursos. Los procesos requieren mecanismos de sincronización como semáforos en memoria compartida, mientras que los hilos utilizan mutexes para proteger secciones críticas. Los hilos comparten el mismo espacio de direcciones, lo que los hace más livianos y rápidos para acceder a memoria compartida, mientras que los procesos, al estar aislados, necesitan más infraestructura para sincronizarse correctamente.
</p>

#### Cuándo usar procesos o hilos
<ul>
  <li>Procesos: Son recomendables cuando se necesita aislamiento fuerte entre unidades de ejecución, por ejemplo, en casos donde una falla en una parte del sistema no debe afectar al resto.</li>
  <li>Hilos: Son preferibles cuando se requiere alto rendimiento, bajo consumo de memoria y los hilos pueden compartir eficientemente datos.</li>
</ul>

#### Problemas de sincronización encontrados
<p>
Uno de los problemas más relevantes fue la condición de carrera al acceder simultáneamente a los contadores de entradas por zona. En la versión con procesos, se solucionó utilizando un único semáforo en memoria compartida, protegiendo el acceso a toda la estructura de entradas. En la versión con hilos, se implementó un mutex por zona, lo que permitió una mayor paralelización y mejor rendimiento.
</p>
