#include <stdio.h>      // funciones de entrada y salida
#include <stdlib.h>     // funciones generales como malloc, exit
#include <pthread.h>    // manejo de hilos
#include <time.h>       // medicion de tiempo

#define TOTAL_ENTRADAS 300000  // cantidad total de entradas a vender
#define NUM_HILOS 5            // numero de cajeros (hilos)

//estructura de entradas
typedef struct {
	int tribuna; //entrada de tribuna
	int golden; //entrada golden
	int vip; //entrada vip
	int top; //entrada top
	int players; //entrada player

	//mutex para evitar condiciones de carrera
	pthread_mutex_t mutex_tribuna;
	pthread_mutex_t mutex_golden;
	pthread_mutex_t mutex_vip;
	pthread_mutex_t mutex_top;
	pthread_mutex_t mutex_players;
} Entradas;

typedef struct {
	int id;        // identificador del cajero 
	Entradas *e;   // puntero a la estructura compartida de entradas
} DatosHilo;

int tipoTransaccion(int transaccion) {
	// retorna un valor entre 0 y 4 segun el valor de la transaccion
	if(transaccion < 20000) return 0;         // retorno 0
	else if(transaccion < 50000) return 1;    // retorno 1
	else if(transaccion < 100000) return 2;   // retorno 2
	else if(transaccion < 200000) return 3;   // retorno 3
	else return 4;                            // retorno 4
}

void procesarTransaccion(Entradas *e, int tipo) {
	switch(tipo) {
		case 0:
			// se bloquea el acceso a la zona players con mutex para evitar que otro hilo modifique al mismo tiempo
			pthread_mutex_lock(&e->mutex_players);
			e->players--;  // se descuenta una entrada de la zona players
			pthread_mutex_unlock(&e->mutex_players); // se libera el acceso
			break;

		case 1:
			// se bloquea el acceso a la zona top con mutex para evitar que otro hilo modifique al mismo tiempo
			pthread_mutex_lock(&e->mutex_top);
			e->top--; // se descuenta una entrada de la zona top
			pthread_mutex_unlock(&e->mutex_top); // se libera el acceso
			break;

		case 2:
			// se bloquea el acceso a la zona vip con mutex para evitar que otro hilo modifique al mismo tiempo
			pthread_mutex_lock(&e->mutex_vip);
			e->vip--; // se descuenta una entrada de la zona vip
			pthread_mutex_unlock(&e->mutex_vip); // se libera el acceso
			break;

		case 3:
			// se bloquea el acceso a la zona golden con mutex para evitar que otro hilo modifique al mismo tiempo
			pthread_mutex_lock(&e->mutex_golden);
			e->golden--; // se descuenta una entrada de la zona golden
			pthread_mutex_unlock(&e->mutex_golden); // se libera el acceso
			break;

		case 4:
			// se bloquea el acceso a la zona tribuna con mutex para evitar que otro hilo modifique al mismo tiempo
			pthread_mutex_lock(&e->mutex_tribuna);
			e->tribuna--; // se descuenta una entrada de la zona tribuna
			pthread_mutex_unlock(&e->mutex_tribuna); // se libera el acceso
			break;
	}
}

void *trabajoHilo(void *arg) {
	DatosHilo *dato = (DatosHilo *)arg; // obtiene los datos del hilo
	int id = dato->id;
	Entradas *e = dato->e; //obtiene el puntero a la estructura compartida

	int ventas_por_zona[5] = {0, 0, 0, 0, 0}; // contador local de ventas

	// cada hilo toma transacciones comenzando en su id y saltando de 5 en 5
	for(int i = id; i < TOTAL_ENTRADAS; i += NUM_HILOS) {
		int tipo = tipoTransaccion(i);     // determina el tipo de zona
		procesarTransaccion(e, tipo);      // descuenta una entrada
		ventas_por_zona[tipo]++;           // registra la venta localmente
	}

	// imprime el resumen de ventas de este hilo
	printf("\n[Cajero: %d] Ventas por zona:\n", id+1);
	printf("Tribuna: %d\n", ventas_por_zona[4]);
	printf("Golden Box: %d\n", ventas_por_zona[3]);
	printf("VIP: %d\n", ventas_por_zona[2]);
	printf("Top Box: %d\n", ventas_por_zona[1]);
	printf("Players Box: %d\n", ventas_por_zona[0]);

	return NULL; // fin del hilo
}

int main() {
	// inicializa estructura de entradas con cantidades iniciales
	// cada zona tiene su mutex inicializado
	Entradas e = {100000, 100000, 50000, 30000, 20000, PTHREAD_MUTEX_INITIALIZER, // cantidades por zona
	       	PTHREAD_MUTEX_INITIALIZER,
		PTHREAD_MUTEX_INITIALIZER,
		PTHREAD_MUTEX_INITIALIZER,
		PTHREAD_MUTEX_INITIALIZER};

	pthread_t hilos[NUM_HILOS];     // arreglo de hilos
	DatosHilo datos[NUM_HILOS];     // arreglo con los datos para cada hilo

	struct timespec t_inicio, t_fin; // estructura para medir tiempo
	clock_gettime(CLOCK_MONOTONIC, &t_inicio); // marca el tiempo de inicio

	// mensaje de bienvenida
	printf("\n************ CONCIERTO DON MEDARDO Y SUS PLAYERS ************\n");
	printf("\n*************** INICIO VENTA ENTRADAS: 0.000000 ***************\n");

	// crea los hilos y les asigna su id y acceso a la estructura de entradas
	for(int i = 0; i < NUM_HILOS; i++) {
		datos[i].id = i;
		datos[i].e = &e;
		pthread_create(&hilos[i], NULL, trabajoHilo, &datos[i]);
	}

	// espera a que todos los hilos terminen
	for(int i = 0; i < NUM_HILOS; i++) {
		pthread_join(hilos[i], NULL);
	}

	// mide el tiempo de fin
	clock_gettime(CLOCK_MONOTONIC, &t_fin);
	double tiempo_total = (t_fin.tv_sec - t_inicio.tv_sec) + (t_fin.tv_nsec - t_inicio.tv_nsec) / 1e9;

	// mensaje final de confirmacion
	printf("\n*************** TODAS LAS ENTRADAS HAN SIDO VENDIDAS ***************\n");
	printf("\n*************** FIN VENTA DE ENTRADAS: %.6f ***************\n", tiempo_total);

	// libera los mutex 
	pthread_mutex_destroy(&e.mutex_tribuna);
	pthread_mutex_destroy(&e.mutex_golden);
	pthread_mutex_destroy(&e.mutex_vip);
	pthread_mutex_destroy(&e.mutex_top);
	pthread_mutex_destroy(&e.mutex_players);
	return 0; //fin del programa
}

