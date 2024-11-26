#include<stdbool.h>
#include "./libs/lib.h"

const int TAILLE_CADRE = 1024;

unsigned int calculerNumeroDePage(unsigned long adresse) {
	return adresse / TAILLE_CADRE;
}

unsigned long calculerDeplacementDansLaPage(unsigned long adresse) {
	return adresse % TAILLE_CADRE;
}

unsigned long calculerAdresseComplete(unsigned int numeroDePage, unsigned long deplacementDansLaPage) {
    return (numeroDePage * TAILLE_CADRE) + deplacementDansLaPage;
}

void rechercherTLB(struct RequeteMemoire* req, struct SystemeMemoire* mem) {
    unsigned int pageRecherche = calculerNumeroDePage(req->adresseVirtuelle);
    
    req->estDansTLB = 0;
    for(int j = 0; j < TAILLE_TLB; j++) {
        if(mem->tlb->entreeValide[j]) {
            if(mem->tlb->numeroPage[j] == pageRecherche) {
                req->estDansTLB = 1;
                
                unsigned int numeroCadre = mem->tlb->numeroCadre[j];
                unsigned long deplacement = calculerDeplacementDansLaPage(req->adresseVirtuelle);
                unsigned long adressePhysique = calculerAdresseComplete(numeroCadre, deplacement);
                
                req->adressePhysique = adressePhysique;
                mem->tlb->dernierAcces[j] = req->date;
                return;
            }
        }
    }
    
    req->adressePhysique = 0;
    req->estDansTLB = 0;
}

void rechercherTableDesPages(struct RequeteMemoire* req, struct SystemeMemoire* mem) {
    unsigned int pageRecherche = calculerNumeroDePage(req->adresseVirtuelle);
    unsigned long deplacement = calculerDeplacementDansLaPage(req->adresseVirtuelle);
    
    if (mem->tp->entreeValide[pageRecherche]) {
        req->estDansTablePages = 1;
        
        unsigned int numeroCadre = mem->tp->numeroCadre[pageRecherche];
        unsigned long adressePhysique = calculerAdresseComplete(numeroCadre, deplacement);
        req->adressePhysique = adressePhysique;
    }
    else {
        req->estDansTablePages = 0;
        req->adressePhysique = 0;
    }
}

void ajouterDansMemoire(struct RequeteMemoire* req, struct SystemeMemoire* mem) {
    unsigned int pageRecherche = calculerNumeroDePage(req->adresseVirtuelle);
    unsigned long deplacement = calculerDeplacementDansLaPage(req->adresseVirtuelle);
    
    int cadreLibre = -1;
    for(int i = 0; i < TAILLE_MEMOIRE; i++) {
        if(mem->memoire->utilisee[i] == 0) {
            cadreLibre = i;
            break;
        }
    }
    
    if(cadreLibre != -1) {
        mem->memoire->numeroPage[cadreLibre] = pageRecherche;
        mem->memoire->dernierAcces[cadreLibre] = req->date;
        mem->memoire->dateCreation[cadreLibre] = req->date;
        mem->memoire->utilisee[cadreLibre] = 1;
        
        unsigned long adressePhysique = calculerAdresseComplete(cadreLibre, deplacement);
        
        req->adressePhysique = adressePhysique;
    }
}

void mettreAJourTLB(struct RequeteMemoire* req, struct SystemeMemoire* mem) {
    unsigned int pageRecherche = calculerNumeroDePage(req->adresseVirtuelle);
    unsigned int numeroCadre = calculerNumeroDePage(req->adressePhysique);

    int indexARemplacer = -1; 
    for(int j = 0; j < TAILLE_TLB; j++) {
        if(mem->tlb->entreeValide[j] == 0) {
            indexARemplacer = j;
            break;
        }
    }

    if(indexARemplacer == -1) {
        unsigned long plusVielleDate = mem->tlb->dateCreation[0];
        indexARemplacer = 0;

        for(int j = 1; j < TAILLE_TLB; j++) {
            if(mem->tlb->dateCreation[j] < plusVielleDate) {
                plusVielleDate = mem->tlb->dateCreation[j];
                indexARemplacer = j;
            }
        }
    }

    mem->tlb->numeroPage[indexARemplacer] = pageRecherche;
    mem->tlb->numeroCadre[indexARemplacer] = numeroCadre;
    mem->tlb->entreeValide[indexARemplacer] = 1;
    mem->tlb->dernierAcces[indexARemplacer] = req->date;
    mem->tlb->dateCreation[indexARemplacer] = req->date;
}

// NE PAS MODIFIER
int main() {
    evaluate(
		&calculerNumeroDePage, 
		&calculerDeplacementDansLaPage, 
		&calculerAdresseComplete, 
        &rechercherTLB, 
		&rechercherTableDesPages,
		&mettreAJourTLB,
		&ajouterDansMemoire
    );
    return 0;
}
