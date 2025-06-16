#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <time.h>

// constantes globales
#define TOTAL_ENTRADAS 300000 //cantidad total de transacciones que se realizaran 
#define NUM_PROCESOS 5 //numero de cajeros

//estructura que guarda los contadores de entradas y el semaforo
typedef struct {
//tipos de entrada existentes
	int tribuna;
	int golden;
	int vip;
	int top;
	int players;
//semaforo
	sem_t semaforo;
} Entradas;

//clasifica la transaccion en un tipo de entrada segun su indice
int tipoTransaccion(int transaccion) {
	if (transaccion < 20000) return 0; //retorno 0 si la transaccion es menor a 20000
	else if (transaccion < 50000) return 1; //retorno 1 si la transaccion es menor a 50000
	else if (transaccion < 100000) return 2; //retorno 2 si la transaccion es menor a 100000
	else if (transaccion < 200000) return 3; //retorno 3 si la transaccion es menor a 200000
	else return 4; //retorno 4 si la transaccion es >= 200000
}

//descuenta una entrada del tipo indicado
void procesarTransaccion(Entradas *e, int tipo) {
	sem_wait(&e->semaforo); //inicio seccion critica
	switch (tipo) {
		case 0: //si es 0, reduce una de players
			e->players--;
			break;
		case 1: //si es 1, reduce una de top
			e->top--;
			break;
		case 2: //si es 2, reduce una de vip
			e->vip--;
			break;
		case 3: //si es 3, reduce una de golden
			e->golden--;
			break;
		case 4: //si es 4, reduce una de tribuna
			e->tribuna--;
			break;
	}
	sem_post(&e->semaforo); //fin seccion critica
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

	//crea los procesos hijos que actuaran como cajeros
	for (int i = 0; i < NUM_PROCESOS; i++) {
		if (fork() == 0) { //codigo hijo
			int ventas_por_zona[5] = {0, 0, 0, 0, 0};
			
			//procesa un subconjunto de transacciones
			for (int j = i; j < TOTAL_ENTRADAS; j += NUM_PROCESOS) {
				int tipo = tipoTransaccion(j);
				procesarTransaccion(e, tipo);
				ventas_por_zona[tipo]++;
			}

			// muestra resumen de ventas del cajero
			printf("\n[Cajero: %d] Ventas por zona:\n", i + 1);
			printf("Tribuna: %d\n", ventas_por_zona[4]);
			printf("Golden Box: %d\n", ventas_por_zona[3]);
			printf("VIP: %d\n", ventas_por_zona[2]);
			printf("Top Box: %d\n", ventas_por_zona[1]);
			printf("Players Box: %d\n", ventas_por_zona[0]);

			munmap(e, sizeof(Entradas));
			exit(0); //finaliza el hijo
		}
	}

	//proceso padre espera a que todos los hijos finalicen
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
