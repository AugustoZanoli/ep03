

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N 10

int nFans = 0;
sem_t mutex, dem, fila;

void* fan(void* arg) {
    int id = *(int*)arg;
    free(arg); // Liberando memória imediatamente após o uso
    while (1) {
        sem_wait(&mutex);
        nFans++;
        printf("Fã %d chegou ao stand. Total de fãs no stand: %d\n", id, nFans);
        // Se um fã que já assistiu um filme chega no stand, ou seja, o mesmo iD, significa que ele ja tinha saíddo da outra sessão dele, e se funcionar tudo era pra ele ter ligado para a mãe dele também
        sem_post(&mutex);

        sem_post(&dem);
        sem_wait(&fila);

        printf("Fã %d | assistindo ao filme\n", id);
        sleep(2);
        printf("Fã %d | telefonando para a mãe\n", id);
        sleep(1);
    }
}

void* demonstrator(void* arg) {
    while (1) {
        for (int i = 0; i < N; i++) {
            sem_wait(&dem);  // Espera 10 fãs
        }

        sem_wait(&mutex);
        nFans -= N;
        printf("Demonstrador | iniciando exibição para 10 fãs. Fãs restantes: %d\n", nFans);
        sem_post(&mutex);

        for (int i = 0; i < N; i++) {
            sem_post(&fila);  // Libera os fãs para assistirem ao filme
        }

        printf("Demonstrador | exibindo o filme\n");
        sleep(2);
    }
}

int main() {
    pthread_t demoThread;
    pthread_t fanThreads[20];  // Mais de 10 fãs

    sem_init(&mutex, 0, 1);
    sem_init(&dem, 0, 0);
    sem_init(&fila, 0, 0);

    // Thread do demonstrador
    pthread_create(&demoThread, NULL, demonstrator, NULL);

    // Criar 20 fãs
    for (int i = 0; i < 20; i++) {
        int* id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&fanThreads[i], NULL, fan, id);
    }

    // Esperar pelos threads
    pthread_join(demoThread, NULL);
    for (int i = 0; i < 20; i++) {
        pthread_join(fanThreads[i], NULL);
    }

    // Destruir os semáforos
    sem_destroy(&mutex);
    sem_destroy(&dem);
    sem_destroy(&fila);

    return 0;
}
