#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <time.h>

// constantes globales
#define TOTAL_ENTRADAS 300000 //cantidad total de transacciones que se realizaran 
//(Se uso' esta cantidad de iteraciones para obtener una comparacion mas evidente en los tiempos)

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
//(esta estructura facilita el uso de las entradas y lograr un bloqueo mas sencillo con el semaforo)


//clasifica la transaccion en un tipo de entrada segun su indice
int tipoTransaccion(int transaccion) {
	if (transaccion < 20000) return 0; //retorno 0 si la transaccion es menor a 20000
	else if (transaccion < 50000) return 1; //retorno 1 si la transaccion es menor a 50000
	else if (transaccion < 100000) return 2; //retorno 2 si la transaccion es menor a 100000
	else if (transaccion < 200000) return 3; //retorno 3 si la transaccion es menor a 200000
	else return 4; //retorno 4 si la transaccion es >= 200000
}
//esta funcion genera los procesos que iran dando uso a fragmentos de las iteraciones totales y se le da un id independiente

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
//esta funcion evita una condicion de carrera que sincroniza apropiadamente a los procesos a traves de un semaforo

int main() {
	// se reserva una region de memoria compartida para la estructura de entradas
	Entradas *e = mmap(NULL, sizeof(Entradas), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0); // se puede leer y escribir, memoria compartida anonima, no se asocia a ningun archivo
	if (e == MAP_FAILED) {
		perror("mmap"); // si falla la asignacion, se muestra un error
		exit(EXIT_FAILURE);
	}
	
	
	// Inicializar entradas
	e->tribuna = 100000;
	e->golden = 100000;
	e->vip = 50000;
	e->top = 30000;
	e->players = 20000;

	// Inicializar semáforos
	//el primer parametro define el id del semaforo
	// el segundo parametro en 1 indica que el semaforo es compartido entre procesos
	//el tercer parametro inicializa el semaforo como desbloqueado/preparado
	sem_init(&e->semaforo, 1, 1);

	// se captura el tiempo antes de iniciar las ventas
	//structur se usa para declarar variables que almacenen tiempo
	struct timespec t_inicio, t_fin;
	
	//clock_gettime define el metodo con el que se calculara el tiempo
	//en este caso CLOCK_MONOTONIC define un punto de inicio, siendo este el entregado en el segundo parametro
	clock_gettime(CLOCK_MONOTONIC, &t_inicio);
	
	
	// se imprime mensaje inicial de bienvenida
	printf("\n************ CONCIERTO DON MEDARDO Y SUS PLAYERS ************\n");
	printf("\n*************** INICIO VENTA ENTRADAS: 0.000000 ***************\n");
	//cabe recalcar que el tiempo inicial de el programa, no es absolutamente cero,
	//pero es un margen de error que se puede despreciar sin causar complicaciones
	
	//crea los procesos hijos que actuaran como cajeros
	for (int i = 0; i < NUM_PROCESOS; i++) {
		if (fork() == 0) { //codigo hijo
			int ventas_por_zona[5] = {0, 0, 0, 0, 0}; // contadores locales de ventas por zona
			
			//procesa un subconjunto de transacciones
			for (int j = i; j < TOTAL_ENTRADAS; j += NUM_PROCESOS) {
				int tipo = tipoTransaccion(j); // se obtiene el tipo de boleto
				procesarTransaccion(e, tipo); // se descuenta el boleto en memoria compartida
				ventas_por_zona[tipo]++; // se actualiza el contador 
			}
			//este ciclo envia las iteraciones de forma intercalada a cada hilo usando un planificador de procesos round robin usando su id independiente
			
			// muestra resumen de ventas del cajero
			printf("\n[Cajero: %d] Ventas por zona:\n", i + 1);
			printf("Tribuna: %d\n", ventas_por_zona[4]);
			printf("Golden Box: %d\n", ventas_por_zona[3]);
			printf("VIP: %d\n", ventas_por_zona[2]);
			printf("Top Box: %d\n", ventas_por_zona[1]);
			printf("Players Box: %d\n", ventas_por_zona[0]);
			
			munmap(e, sizeof(Entradas));
			//este metodo saca a los procesos de la memoria compartida
			
			exit(0); //finaliza el hijo
		}
	}

	//proceso padre espera a que todos los hijos finalicen
	for (int i = 0; i < NUM_PROCESOS; i++) {
		wait(NULL);
	}
	
	clock_gettime(CLOCK_MONOTONIC, &t_fin); // se toma el tiempo final luego de que todos los boletos fueron vendidos
	
	double tiempo_total = (t_fin.tv_sec - t_inicio.tv_sec) + (t_fin.tv_nsec - t_inicio.tv_nsec) / 1e9; //calculo del tiempo total
	//el calculo se realiza al obtener el tiempo inicial, sumado al tiempo final que se divide a 1e9 para obtener su valor en segundos
	
	
	printf("\n*************** TODAS LAS ENTRADAS HAN SIDO VENDIDAS ***************\n");
	printf("\n*************** FIN VENTA DE ENTRADAS: %.6f ***************\n", tiempo_total);
	
	// Destruir semáforos
	sem_destroy(&e->semaforo);
	munmap(e, sizeof(Entradas));

	return 0; //fin del programa
}
