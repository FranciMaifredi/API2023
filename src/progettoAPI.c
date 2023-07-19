//
// Created by Francesca Maifredi on 18/07/23.
//
#include <stdio.h>
#include <stdlib.h>

struct nodo{
    int distanza; // key
    int autonomiaMax;
    enum {RED, BLACK} color;
    struct nodo* p;
    struct nodo* left;
    struct nodo* right;
};

struct nodo* treeRoot = NULL;

void aggiungiStazione();
void aggiungiAuto();
void pianificaPercorso();
void rottamaAuto();
void demolisciStazione();
// RED BLACK TREE FUNCTIONS
void insert(struct nodo* root, struct nodo* z);
struct nodo* delete(struct nodo* root, struct nodo* z);
void leftRotate(struct nodo* root, struct nodo* nodo);
void rightRotate(struct nodo* root, struct nodo* nodo);
void rbInsertFixup(struct nodo* root, struct nodo* nodo);
void rbDeleteFixup(struct nodo* root, struct nodo* nodo);

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

void insert(struct nodo* root, struct nodo* z){
    struct nodo* y = NULL;
    struct nodo* x = root;
    while(x!=NULL){
        y = x;
        if(z->distanza < x->distanza)
            x = x->left;
        else
            x = x->right;
    }
    z->p = y;
    if(y==NULL)
        root = z;
    else if(z->distanza < y->distanza)
        y->left = z;
    else
        y->right = z;
    z->left = NULL;
    z->right = NULL;
    z->color = RED;
    rbInsertFixup(root, z);
}

struct nodo* delete(struct nodo* root, struct nodo* z){
    struct nodo* y = NULL;
    struct nodo* x = NULL;
    if(z->left==NULL || z->right==NULL)
        y = z;
    else
        y = treeSuccessor(z);
    if(y->left!=NULL)
        x = y->left;
    else
        x = y->right;
    x->p=y->p;
    if(y->p==NULL)
        root=x;
    else if(y==y->p->left)
        y->p->left = x;
    else
        y->p->right=x;
    if(y!=z){
        z->distanza = y->distanza;
        z->autonomiaMax = y->autonomiaMax; // controlla autonomiaMax!
    }
    if(y->color==BLACK)
        rbDeleteFixup(root, x);
    return y;
}


