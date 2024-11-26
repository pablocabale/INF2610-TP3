/*
Exclusion Mutuelle: Les descripteurs de fichier du pipe f[0] (exemple ligne 48) pour la lecture et f[1] (exemple ligne 61) pour l'écriture sont des ressources exclusives. 
Un seul processus peut lire ou écrire dans le pipe.

Detention et attente: Le processus parent detient le descripteur d'ecriture du pipe f[1] (ligne 61) et attend la terminaison des processus enfants avec wait().
En meme temps, les processus enfants detiennent le descripteur de lecture du pipe f[0] (ligne 48) et attendent la lecture du message du parent.

Pas de réquisition: Les descripteurs de fichier du pipe ne peuvent pas être requisitionnés par un autre processus.

Attente circulaire: Le processus parent attend la terminaison des processus enfants avec wait() (ligne 59) tandis que le premier enfant attend la lecture du message du parent (ligne 48).
Ceci crée une attente circulaire.

Correction de l'interblocage: Il faut corriger l'attente circulaire.
Au lieu de ce code:
    while(wait(NULL) > 0);
    char string[6] = "Salut!";
    write(fd[1], &string, strlen(string));
    printf("Process completed!\n");
    close(fd[0]);
    close(fd[1]);

On peut remplacer par ce code:
    close(fd[0]);
    char string[6] = "Salut!";
    write(fd[1], string, strlen(string));
    printf("Process completed!\n");
    close(fd[1]);
    while(wait(NULL) > 0);

Ce code permet au parent d'écrire dans le pipe pour que le premier enfant puisse lire le message. Ensuite, le parent attend la terminaison des processus enfants.
Ce qui corrige l'attente circulaire.
*/


#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main() {
    int fd[2];

    pipe(fd);

    if(!fork()){
        close(fd[1]); 
        char* string[6];    
        read(fd[0], string, 6); // f[0] ressource exclusive // Detient et attend
        exit(0);
    }

    if(!fork()){
        close(fd[1]);
        close(fd[0]);
        execlp("ls", "ls", "-l", NULL);
        exit(0);
    }

    while(wait(NULL) > 0); // Attente de la terminaison des processus enfants // Il faut attendre a la fin
    char string[6] = "Salut!";
    write(fd[1], &string, strlen(string)); // f[1] ressource exclusive // Detient et attend
    printf("Process completed!\n");
    close(fd[0]);
    close(fd[1]);
    return 0;
}