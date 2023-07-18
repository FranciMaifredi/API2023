//
// Created by Francesca Maifredi on 18/07/23.
//
#include <stdio.h>
#include <stdlib.h>

void aggiungiStazione();
void aggiungiAuto();
void pianificaPercorso();
void rottamaAuto();
void demolisciStazione();

int main(){
    char comando[19];
    while(!feof(stdin)){
        scanf("%s", comando);
        if(strcmp(comando, "aggiungi-stazione")==0)
            aggiungiStazione();
        else if(strcmp(comando, "aggiungi-auto")==0)
            aggiungiAuto();
        else if(strcmp(comando, "pianifica-percorso")==0)
            pianificaPercorso();
        else if(strcmp(comando, "rottama-auto")==0)
            rottamaAuto();
        else if(strcmp(comando, "demolisci-stazione")==0)
            demolisciStazione();
    }
    return 0;
}

void aggiungiStazione(){
    int distanza, numAuto;
    scanf("%d %d", &distanza, &numAuto);
    for(int i=0; i<numAuto; i++){
        // TODO: aggiungi autonomie auto
    }
    // TODO
}

void aggiungiAuto(){
    int distanza, autonomia;
    scanf("%d %d", &distanza, &autonomia);
    // TODO
}

void pianificaPercorso(){
    int partenza, arrivo;
    scanf("%d %d", &partenza, &arrivo);
    // TODO
}

void rottamaAuto(){
    int distanza, autonomia;
    scanf("%d %d", &distanza, &autonomia);
    // TODO
}

void demolisciStazione(){
    int distanza;
    scanf("%d", &distanza);
    // TODO
}
