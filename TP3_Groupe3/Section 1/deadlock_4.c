/*
Exclusion mutuelle: Lignes : pthread_mutex_lock(&locks[left]); et pthread_mutex_lock(&locks[right]); assure l'exclusion mutuelle.
Détention et attente: Après avoir verrouillé locks[left], chaque thread le détient tout en essayant de verrouiller locks[right].
Pas de réquisition :Les mutex sont libérés uniquement lorsque le thread appelle pthread_mutex_unlock.
Attente circulaire :Chaque thread id détient locks[id] et attend locks[(id + 1) % NUM_THREADS], créant une dépendance circulaire.

Correction de l'interblocage:

 while (rounds < MAX_ROUNDS) {
        sleep(1);
        int left = id;
        int right = (id + 1) % NUM_THREADS;

        int first = (left < right) ? left : right;
        int second = (left < right) ? right : left;

        pthread_mutex_lock(&locks[first]);
        sleep(1);
        pthread_mutex_lock(&locks[second]);

        sleep(1);
        rounds++;
        pthread_mutex_unlock(&locks[second]);
        pthread_mutex_unlock(&locks[first]);
    }

    Ce code resoud le probleme car, tous les threads acquièrent maintenant les verrous dans un ordre global cohérent qui empeche une formation d'un cycle.

*/
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define NUM_THREADS 5
#define MAX_ROUNDS 3

pthread_mutex_t locks[NUM_THREADS];

void* cons(void* arg) {
    int id = *(int*)arg;
    int rounds = 0;

    while (rounds < MAX_ROUNDS) {
        sleep(1);
        int left = id;
        int right = (id + 1) % NUM_THREADS;
        //int first = (left < right) ? left : right;
        //int second = (left < right) ? right : left;
        
        pthread_mutex_lock(&locks[left]); //pthread_mutex_lock(&locks[first]); //possibilité de deadlock
        sleep(1);
        pthread_mutex_lock(&locks[right]); //pthread_mutex_lock(&locks[second]);

        sleep(1);
        rounds++;
        pthread_mutex_unlock(&locks[left]); //pthread_mutex_unlock(&locks[second]);
        pthread_mutex_unlock(&locks[right]);//pthread_mutex_unlock(&locks[first]);
    }

    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_mutex_init(&locks[i], NULL);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, cons, &ids[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}