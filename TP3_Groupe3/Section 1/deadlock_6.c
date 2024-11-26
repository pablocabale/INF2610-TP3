/*
Exclusion mutuelle: Il n'y a pas de ressources exclusives dans ce code. Juste la valeur de flag (ligne 30) est partagée entre les threads.
cependant elle n'est pas modifiée par les threads, donc il n'y a pas de problème d'exclusion mutuelle.

Detention et attente: Il n'y a pas de détention et attente dans ce code. Les threads ne détiennent pas de ressources et n'attendent pas d'autres ressources.

Pas de réquisition: Les threads ne réquisitionnent pas de ressources dans ce code.

Attente circulaire: Il n'y a pas d'attente circulaire dans ce code.

Correction de l'interblocage: les conditions d'interblocage de Coffman ne sont pas satisfaites dans ce code.
cependant, le programme sera toujours exécuté indéfiniment car la variable flag n'est jamais modifiée.
car rand() % 10 == 10 n'est jamais vrai (ligne 37). Pour eviter des boucles infinies, il faudrais que la 
fonction changeFlag() (ligne 36) modifie puisse toujours modifier la valeur de flag a 1.
*/

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#define N_THREADS 5

int flag = 0;

void* function_that_does_something(void* arg) {
    while (1) {
        if (flag) // flag est partagé entre les threads mais n'est pas modifié
            break;
    }
    pthread_exit(NULL);
}

void changeFlag(){
    if (rand() % 10 == 10) // Toujours faux
        flag = 1;
    printf("Signal called\n");
}

int main() {
    pthread_t threads[N_THREADS];
 
    signal(SIGALRM, &changeFlag);      

    for(int i = 0; i < N_THREADS; i++){
        pthread_create(&threads[i], NULL, function_that_does_something, NULL);
    }

    alarm(1);

    for(int i = 0; i < N_THREADS; i++){
        pthread_join(threads[i], NULL);
    }

    return 0;
}