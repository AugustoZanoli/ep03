/*
Nome: Ep03
------------
Programadores: Augusto de Camargos Zanoli e Caio Henrique Pessoa De Lima
------------
Matrícula: 2023001020 e 2022012333
------------
Objetivo: Um programa principal para criar vários fans e apenas um demonstrador e verifique 
se o algoritmo funciona, poderá utilizar a função sleep() nas funções de assisteFilme(), 
telefona() e exibeFilme(), assisteFilme() e exibeFilme() devem demorar o mesmo tempo.
------------
Data: 02/10/2024
*/


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
        printf("| %-5d | %-32s | %-26d |\n", id, "Chegou ao stand", nFans);
        sem_post(&mutex);

        sem_post(&dem);
        sem_wait(&fila);

        printf("| %-5d | %-32s | %-26s |\n", id, "Assistindo ao filme", "-");
        sleep(2);
        printf("| %-5d | %-33s | %-26s |\n", id, "Telefonando para a mãe", "-");
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
        printf("| %-5s | %-35s | %-26d |\n", "DEMO", "Iniciando exibição para 10 fãs", nFans);
        sem_post(&mutex);

        for (int i = 0; i < N; i++) {
            sem_post(&fila);  // Libera os fãs para assistirem ao filme
        }

        printf("| %-5s | %-32s | %-26s |\n", "DEMO", "Exibindo o filme", "-");
        sleep(2);
    }
}

int main() {
    pthread_t demoThread;
    pthread_t fanThreads[20];  // Mais de 10 fãs

    // Inicializar semáforos
    sem_init(&mutex, 0, 1);
    sem_init(&dem, 0, 0);
    sem_init(&fila, 0, 0);

    // Cabeçalho da tabela
    printf("| %-5s | %-34s | %-27s |\n", "ID", "Ação  ", "Total de Fãs no Stand");
    printf("| %-5s|%-34s|%-26s|\n", "------", "----------------------------------", "----------------------------");

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
