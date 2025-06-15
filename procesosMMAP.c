#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <time.h>

#define TOTAL_ENTRADAS 300000
#define NUM_PROCESOS 5

typedef struct {
	int tribuna;
	int golden;
	int vip;
	int top;
	int players;
	sem_t semaforo;
} Entradas;

int tipoTransaccion(int transaccion) {
	if (transaccion < 20000) return 0;
	else if (transaccion < 50000) return 1;
	else if (transaccion < 100000) return 2;
	else if (transaccion < 200000) return 3;
	else return 4;
}

void procesarTransaccion(Entradas *e, int tipo) {
	sem_wait(&e->semaforo);
	switch (tipo) {
		case 0:
			e->players--;
			break;
		case 1:
			e->top--;
			break;
		case 2:
			e->vip--;
			break;
		case 3:
			e->golden--;
			break;
		case 4:
			e->tribuna--;
			break;
	}
	sem_post(&e->semaforo);
}

int main() {
	Entradas *e = mmap(NULL, sizeof(Entradas), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (e == MAP_FAILED) {
		perror("mmap");
		exit(EXIT_FAILURE);
	}

	// Inicializar entradas
	e->tribuna = 100000;
	e->golden = 100000;
	e->vip = 50000;
	e->top = 30000;
	e->players = 20000;

	// Inicializar semáforos
	sem_init(&e->semaforo, 1, 1);

	struct timespec t_inicio, t_fin;
	clock_gettime(CLOCK_MONOTONIC, &t_inicio);

	printf("\n************ CONCIERTO DON MEDARDO Y SUS PLAYERS ************\n");
	printf("\n*************** INICIO VENTA ENTRADAS: 0.000000 ***************\n");

	for (int i = 0; i < NUM_PROCESOS; i++) {
		if (fork() == 0) {
			int ventas_por_zona[5] = {0, 0, 0, 0, 0};

			for (int j = i; j < TOTAL_ENTRADAS; j += NUM_PROCESOS) {
				int tipo = tipoTransaccion(j);
				procesarTransaccion(e, tipo);
				ventas_por_zona[tipo]++;
			}

			printf("\n[Cajero: %d] Ventas por zona:\n", i + 1);
			printf("Tribuna: %d\n", ventas_por_zona[4]);
			printf("Golden Box: %d\n", ventas_por_zona[3]);
			printf("VIP: %d\n", ventas_por_zona[2]);
			printf("Top Box: %d\n", ventas_por_zona[1]);
			printf("Players Box: %d\n", ventas_por_zona[0]);

			munmap(e, sizeof(Entradas));
			exit(0);
		}
	}

	for (int i = 0; i < NUM_PROCESOS; i++) {
		wait(NULL);
	}

	clock_gettime(CLOCK_MONOTONIC, &t_fin);
	double tiempo_total = (t_fin.tv_sec - t_inicio.tv_sec) + (t_fin.tv_nsec - t_inicio.tv_nsec) / 1e9;

	printf("\n*************** TODAS LAS ENTRADAS HAN SIDO VENDIDAS ***************\n");
	printf("\n*************** FIN VENTA DE ENTRADAS: %.6f ***************\n", tiempo_total);

	// Destruir semáforos
	sem_destroy(&e->semaforo);
	munmap(e, sizeof(Entradas));

	return 0;
}
