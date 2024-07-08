#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define MAX 30 // define el tamaño del vector

// estructura para pasar datos a los hilos
typedef struct {
    int *vector;
    int num_a_buscar;
    int inicio;
    int fin;
    int hilo_id;
} thread;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int indice_encontrado = -1;       // indice donde se encuentra el numero (si se encuentra)
int terminados = 0;         // numero de hilos que han terminado
int num_threads;            // numero de hilos
int hilo_que_encontro;      // bandera para saber si un hilo ya encontro el numero

// funcion que ejecutaran los hilos
void search(void *arg) {
    thread *hilo = (thread *)arg;

    for (int i = hilo->inicio; i <= hilo->fin; ++i) {           // para cada elemento en el rango asignado al hilo
        pthread_mutex_lock(&mutex);                             // bloquear el mutex
        if (indice_encontrado != -1) {                                // si el numero ya fue encontrado por otro hilo
            pthread_mutex_unlock(&mutex);                       // desbloquear el mutex
            break;                                              // salir del ciclo
        }                     
        // si el numero no fue encontrado                      
        if (hilo->vector[i] == hilo->num_a_buscar) {            // si el numero se encuentra en la posicion actual
            indice_encontrado = i;                              // guardar la posicion
            hilo_que_encontro = hilo->hilo_id;                  // guardar el id del hilo que encontro el numero
            printf("\n");
            pthread_mutex_unlock(&mutex);
            break;
        }
        // si el numero no fue encontrado
        pthread_mutex_unlock(&mutex);
    }

    // cuando termina de buscar en su rango
    pthread_mutex_lock(&mutex);
    terminados++;             
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}



int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <número_a_buscar> <número_de_hilos>\n", argv[0]);
        return 1;
    }

    int num_a_buscar = atoi(argv[1]);
    num_threads = atoi(argv[2]);

    // Inicializar el vector con números aleatorios
    int vector[MAX];
    srand(time(NULL));
    for (int i = 0; i < MAX; ++i) {
        vector[i] = rand() % 10;
    }

    //imprimir el vector con indice de cada elemento
    printf("\n");
    printf("Indice: |");
    for (int i = 0; i < MAX; ++i) {
        printf("%d |", i);
    }
    printf("\n");
    printf("Vector: |");
    for (int i = 0; i < MAX; ++i) {
        if (i >= 10) {
            printf("%d  |", vector[i]);
        } else {
        printf("%d |", vector[i]);
        }
    }
    printf("\n");
    printf("\n");

    
    pthread_t threads[num_threads];
    thread thread_args[num_threads];

    int tam_busqueda = MAX / num_threads;
    // en caso de que el tamaño del vector no sea divisible entre el numero de hilos
    int remaining = MAX % num_threads;

    // Crear hilos
    for (int i = 0; i < num_threads; ++i) {
        thread_args[i].vector = vector;
        thread_args[i].num_a_buscar = num_a_buscar;
        thread_args[i].inicio = i * tam_busqueda;
        thread_args[i].fin = (i == num_threads - 1) ? (i + 1) * tam_busqueda + remaining - 1 : (i + 1) * tam_busqueda - 1;      // operador ternario para asignar el resto al ultimo hilo
        thread_args[i].hilo_id = i+1;

        pthread_create(&threads[i], NULL, (void *)search, (void *)&thread_args[i]);
    }

    // Esperar a que los hilos terminen
    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], NULL);
    }

    if (terminados == num_threads){
        if(indice_encontrado != -1){
            printf("Número %d encontrado en el vector en la posición %d por el hilo %d.\n", num_a_buscar, indice_encontrado, hilo_que_encontro);
            printf("\n");
        } else {
            printf("Número %d no encontrado en el vector.\n", num_a_buscar);
            printf("\n");
        }
    }

    // Destruir el mutex
    pthread_mutex_destroy(&mutex);

    return 0;
}
