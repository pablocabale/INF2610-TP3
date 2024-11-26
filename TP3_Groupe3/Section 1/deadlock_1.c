/*
Exclusion mutuelle: La resource partagée est la variable buffer (ligne 49) . L'accès à cette variable est protégé par le sémaphore sem_critical (exemple ligne 57).

Détention et attente: Les threads peuvent détenir un sémaphore et attendre un autre. Un consommateur peut detenir sem_critical et attendre sur
sem_busy (exemple ligne 71). Un producteur peut detenir sem_criticla et attendre sur sem_initial (exemple ligne 57).

Pas de réquisition: Les semaphores detenus par un thread ne peuvent pas être requisitionnés par un autre thread.

Attente circulaire: Une attente circulaire est possible quand il y a moins de producteurs + la taille du buffer que de consommateurs. Car il n'y
aura pas assez de jetons sem_busy pour que tous les consommateurs finissent leur thread. 

Correction de l'interblocage: 
Ce code :
    for(int i = 0; i < N_THREADS_2; i++){
        buffer[ip] = 0;
        ip = (ip + 1) % BUFFER_SIZE;
    }
Ne poste pas sur sem_busy, donc les consommateurs ne pourront pas finir leur thread.

Solution:
remplacer pour ce nouveau code:
    for(int i = 0; i < N_THREADS_2; i++){
        sem_wait(&sem_initial);
        sem_wait(&sem_critical);
        buffer[ip] = 0;
        ip = (ip + 1) % BUFFER_SIZE;
        sem_post(&sem_critical);
        sem_post(&sem_busy);
    }
Ce code resoud le probleme car, le boucle utilise les semaphores de la meme maniere que les producteurs, donc les consommateurs pourront finir leur thread.
*/

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#define BUFFER_SIZE 5
#define N_THREADS 1
#define N_THREADS_2 10

int buffer[BUFFER_SIZE];
int ip;
int ic;

sem_t sem_initial;
sem_t sem_busy; // resource partagée
sem_t sem_critical;

int flag = 0;

void* producer(void* arg) {
    while (1) {
        sem_wait(&sem_initial); 
        sem_wait(&sem_critical); // Exclusion mutuelle // Peut detenir sem_critical et attendre sur sem_initial
        buffer[ip] = rand() % 9 + 1;
        ip = (ip + 1) % BUFFER_SIZE;
        sem_post(&sem_critical); // Fin de l'exclusion mutuelle
        sem_post(&sem_busy);
        if (flag)
            break;
    }
    pthread_exit(NULL);
}

void* consumer(void* arg) {
    while (1) {
        sem_wait(&sem_busy); // Possibilité de deadlock
        sem_wait(&sem_critical); // Exclusion mutuelle // Peut detenir sem_critical et attendre sur sem_busy
        int x = buffer[ic];
        ic = (ic + 1) % BUFFER_SIZE;
        sem_post(&sem_critical); // Fin de l'exclusion mutuelle
        sem_post(&sem_initial);
        if(x == 0){
            break;
        }
    }
    pthread_exit(NULL);
}

void changeFlag(){
    flag = 1;
    printf("Flag changed\n");
}

int main() {
    pthread_t prod_thread[N_THREADS], cons_thread[N_THREADS_2];

    sem_init(&sem_initial, 0, BUFFER_SIZE);
    sem_init(&sem_busy, 0, 0);           
    sem_init(&sem_critical, 0, 1); 
    signal(SIGALRM, &changeFlag);      

    for(int i = 0; i < N_THREADS; i++){
        pthread_create(&prod_thread[i], NULL, producer, NULL);
    }

    for(int i = 0; i < N_THREADS_2; i++){
        pthread_create(&cons_thread[i], NULL, consumer, NULL);
    }

    alarm(1);

    for(int i = 0; i < N_THREADS; i++){
        pthread_join(prod_thread[i], NULL);
    }

    for(int i = 0; i < N_THREADS_2; i++){
        // sem_wait(&sem_initial);
        // sem_wait(&sem_critical);
        buffer[ip] = 0;
        ip = (ip + 1) % BUFFER_SIZE;
        // sem_post(&sem_critical);
        // sem_post(&sem_busy);
    }

    for(int i = 0; i < N_THREADS_2; i++){
        pthread_join(cons_thread[i], NULL);
    }

    sem_destroy(&sem_initial);
    sem_destroy(&sem_busy);
    sem_destroy(&sem_critical);

    return 0;
}