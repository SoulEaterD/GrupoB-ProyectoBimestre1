#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define TOTAL_ENTRADAS 300000
#define NUM_HILOS 5

typedef struct {
	int tribuna;
	int golden;
	int vip;
	int top;
	int players;
	pthread_mutex_t mutex_tribuna;
	pthread_mutex_t mutex_golden;
	pthread_mutex_t mutex_vip;
	pthread_mutex_t mutex_top;
	pthread_mutex_t mutex_players;
} Entradas;

typedef struct {
	int id;
	Entradas *e;
} DatosHilo;

int tipoTransaccion(int transaccion) {
	if(transaccion < 20000) return 0;
	else if(transaccion < 50000) return 1;
	else if(transaccion < 100000) return 2;
	else if(transaccion < 200000) return 3;
	else return 4;
}

void procesarTransaccion(Entradas *e, int tipo) {
	switch(tipo) {
		case 0:
			pthread_mutex_lock(&e->mutex_players);
			e->players--;
			pthread_mutex_unlock(&e->mutex_players);
			break;
		case 1:
			pthread_mutex_lock(&e->mutex_top);
			e->top--;
			pthread_mutex_unlock(&e->mutex_top);
			break;
		case 2:
			pthread_mutex_lock(&e->mutex_vip);
			e->vip--;
			pthread_mutex_unlock(&e->mutex_vip);
			break;
		case 3:
			pthread_mutex_lock(&e->mutex_golden);
			e->golden--;
			pthread_mutex_unlock(&e->mutex_golden);
			break;
		case 4:
			pthread_mutex_lock(&e->mutex_tribuna);
			e->tribuna--;
			pthread_mutex_unlock(&e->mutex_tribuna);
			break;
	}
}

void *trabajoHilo(void *arg) {
	DatosHilo *dato = (DatosHilo *)arg;
	int id = dato->id;
	Entradas *e = dato->e;

	int ventas_por_zona[5] = {0, 0, 0, 0, 0};

	for(int i = id; i < TOTAL_ENTRADAS; i += NUM_HILOS) {
		int tipo = tipoTransaccion(i);
		procesarTransaccion(e, tipo);
		ventas_por_zona[tipo]++;
	}

	printf("\n[Cajero: %d] Ventas por zona:\n", id+1);
	printf("Tribuna: %d\n", ventas_por_zona[4]);
	printf("Golden Box: %d\n", ventas_por_zona[3]);
	printf("VIP: %d\n", ventas_por_zona[2]);
	printf("Top Box: %d\n", ventas_por_zona[1]);
	printf("Players Box: %d\n", ventas_por_zona[0]);

	return NULL;
}

int main() {
	Entradas e = {100000, 100000, 50000, 30000, 20000, PTHREAD_MUTEX_INITIALIZER,
	       	PTHREAD_MUTEX_INITIALIZER,
		PTHREAD_MUTEX_INITIALIZER,
		PTHREAD_MUTEX_INITIALIZER,
		PTHREAD_MUTEX_INITIALIZER};

	pthread_t hilos[NUM_HILOS];
	DatosHilo datos[NUM_HILOS];

	struct timespec t_inicio, t_fin;
	clock_gettime(CLOCK_MONOTONIC, &t_inicio);
	
	printf("\n************ CONCIERTO DON MEDARDO Y SUS PLAYERS ************\n");
	printf("\n*************** INICIO VENTA ENTRADAS: 0.000000 ***************\n");

	for(int i = 0; i < NUM_HILOS; i++) {
		datos[i].id = i;
		datos[i].e = &e;
		pthread_create(&hilos[i], NULL, trabajoHilo, &datos[i]);
	}

	for(int i = 0; i < NUM_HILOS; i++) {
		pthread_join(hilos[i], NULL);
	}

	clock_gettime(CLOCK_MONOTONIC, &t_fin);
	double tiempo_total = (t_fin.tv_sec - t_inicio.tv_sec) + (t_fin.tv_nsec - t_inicio.tv_nsec) / 1e9;

	printf("\n*************** TODAS LAS ENTRADAS HAN SIDO VENDIDAS ***************\n");
	printf("\n*************** FIN VENTA DE ENTRADAS: %.6f ***************\n", tiempo_total);

	pthread_mutex_destroy(&e.mutex_tribuna);
	pthread_mutex_destroy(&e.mutex_golden);
	pthread_mutex_destroy(&e.mutex_vip);
	pthread_mutex_destroy(&e.mutex_top);
	pthread_mutex_destroy(&e.mutex_players);
	return 0;
}

