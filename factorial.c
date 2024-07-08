#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

//Estructura para pasar argumentos a los hilos
typedef struct {
    int num;
    long int result;
    int index;
} thread;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;      //Declaración del mutex para asegurar la salida en orden
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;         //Declaración de la variable de condición para sincronizar los hilos
int current_index = 0;                                  // Variable global para el índice actual de salida

//Función para calcular el factorial
void factorial(void *arg) {
    thread *data = (thread *)arg;
    int num = data->num;
    long int result = 1;

    //Calcular el factorial
    for (int i = 1; i <= num; ++i) {
        result *= i;
    }
    data->result = result;

    pthread_mutex_lock(&mutex);                     //Bloquear el mutex
    while (data->index != current_index) {          //Esperar a que sea el turno del hilo actual
        pthread_cond_wait(&cond, &mutex);           //Esperar a que se cumpla la condición
    }

    //Imprimir el resultado del factorial
    printf("Factorial de %d: %ld\n", data->num, data->result);
    current_index++;                    //Incrementar el índice global para el siguiente hilo
    pthread_mutex_unlock(&mutex);       //Desbloquear el mutex
    pthread_cond_broadcast(&cond);      //Notificar a los hilos en espera

    //Terminar el hilo
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    int num_args = argc - 1;        //Número de argumentos
    pthread_t threads[num_args];    //Arreglo de hilos
    thread thread_args[num_args];   //Arreglo de estructuras hilo


    //Crear hilos
    for (int i = 0; i < num_args; ++i) {
        thread_args[i].num = atoi(argv[i + 1]);     //Convertir el argumento a entero
        thread_args[i].index = i;                   //Asignar el índice del hilo
        
        pthread_create(&threads[i], NULL, (void *)factorial, (void *)&thread_args[i]);
    }

    //Esperar a que los hilos terminen
    for (int i = 0; i < num_args; ++i) {
        pthread_join(threads[i], NULL);
    }

    //Destruir el mutex
    pthread_mutex_destroy(&mutex);

    return 0;
}
