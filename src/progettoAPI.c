//
// Created by Francesca Maifredi on 18/07/23.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int helper=0;

// ALBERO DELLE STAZIONI
struct nodo{
    int distanza; // key
    int autonomiaMax;
    char color; // RED=0, BLACK=1
    struct nodo* p;
    struct nodo* left;
    struct nodo* right;
    struct nodo2* autonomiesHead;
};

// LISTA DELLE AUTONOMIE: ordinata in ordine decrescente di autonomia
struct nodo2{
    int autonomia;
    struct nodo2* next;
};


void aggiungiStazione(struct nodo** root);
void aggiungiAuto(struct nodo** root);
void pianificaPercorso(struct nodo** root);
void rottamaAuto(struct nodo** root);
void demolisciStazione(struct nodo** root);
// RED BLACK TREE FUNCTIONS
void insert(struct nodo** root, struct nodo* z);
struct nodo* delete(struct nodo** root, struct nodo* z);
void rbTransplant(struct nodo** root, struct nodo* u, struct nodo* v);
void leftRotate(struct nodo** root, struct nodo* x);
void rightRotate(struct nodo** root, struct nodo* x);
void rbInsertFixup(struct nodo** root, struct nodo* z);
void rbDeleteFixup(struct nodo** root, struct nodo* z);
struct nodo* treeSuccessor(struct nodo* x);
struct nodo* treeMinimum(struct nodo* x);
struct nodo* treeSearch(struct nodo* root, int distanza);
// LIST FUNCTIONS
struct nodo2* listInsert(struct nodo* treeNode, int autonomia);
struct nodo2* listDelete(struct nodo* treeNode, int autonomia);


int main(){
    struct nodo* treeRoot = NULL;
    char comando[19];
    while(!feof(stdin)){
        helper = scanf("%s", comando);
        if(strcmp(comando, "aggiungi-stazione")==0)
            aggiungiStazione(&treeRoot);
        else if(strcmp(comando, "aggiungi-auto")==0)
            aggiungiAuto(&treeRoot);
        else if(strcmp(comando, "pianifica-percorso")==0)
            pianificaPercorso(&treeRoot);
        else if(strcmp(comando, "rottama-auto")==0)
            rottamaAuto(&treeRoot);
        else if(strcmp(comando, "demolisci-stazione")==0)
            demolisciStazione(&treeRoot);
    }
    return 0;
}

void aggiungiStazione(struct nodo** root){
    int distanza=0, numAuto=0, autonomia=0;
    helper = scanf("%d %d", &distanza, &numAuto);

    // controllo se la stazione è già presente
    struct nodo* z = treeSearch(*root, distanza);
    if(z!=NULL){ // stazione già presente
        for(int i=0; i<numAuto; i++) // necessario?
            helper = scanf("%d", &autonomia);
        printf("non aggiunta\n");
        return; // necessario?
    }
    else{ // z = NULL, quindi la stazione non è presente:
        // creo nodo da inserire nell'albero delle stazioni
        z = (struct nodo*)malloc(sizeof(struct nodo));
        z->distanza = distanza;
        z->autonomiaMax = 0;
        z->color = 0; // necessario?
        z->p = NULL; // necessario?
        z->left = NULL; // necessario?
        z->right = NULL; // necessario?
        z->autonomiesHead = NULL;
        insert(root, z);

        // inserisco le autonomie delle auto
        int flag=0, m=0;
        for(int i=0; i<numAuto; i++){
            helper = scanf("%d", &autonomia);
            z->autonomiesHead = listInsert(z, autonomia);
        }

        printf("aggiunta\n");
    }
}

void aggiungiAuto(struct nodo** root){
    int distanza=0, autonomia=0;
    helper = scanf("%d %d", &distanza, &autonomia);
    struct nodo* z = treeSearch(*root, distanza);
    if(z!=NULL){
        // aggiungo l'auto
        z->autonomiesHead = listInsert(z, autonomia);
        printf("aggiunta\n");
    }
    else
        printf("non aggiunta\n");
}

/*
void pianificaPercorso(struct nodo** root){
    int partenza=0, arrivo=0;
    char flag=0;
    helper = scanf("%d %d", &partenza, &arrivo);
    struct nodo* startStation = treeSearch(*root, partenza);
    struct nodo* endStation = treeSearch(*root, arrivo);
    if(partenza==arrivo) { // stessa stazione
        printf("%d\n", partenza);
    }
    else if(partenza<arrivo){ // da sinistra a destra, complessità fattoriale
        int* percorso = (int*)calloc(4, sizeof(int));
        percorso[0] = arrivo;
        int lastOcc = 0; // indice dell'ultima posizione occupata dell'array
        struct nodo* tmp = NULL;
        while(percorso[lastOcc]!=partenza){
            tmp = startStation;
            flag=0;
            while(tmp!=endStation) {
                if (tmp->autonomies[0] >= arrivo - tmp->distanza) { // inserisco in coda nell'array
                    if(sizeof(percorso)==lastOcc+2){ // array ha solo ultima posizione vuota
                        int size = sizeof(percorso);
                        percorso = realloc(percorso, size+2*sizeof(int));
                    }
                    percorso[lastOcc+1]=tmp->distanza;
                    lastOcc++;
                    endStation = tmp;
                    arrivo = tmp->distanza;
                    flag=1;
                    break;
                }
                tmp = treeSuccessor(tmp);
            }
            if(tmp==endStation && flag==0){ // non esiste percorso
                printf("nessun percorso\n");
                // free(percorso);
                break;
            }
            if(percorso[lastOcc]==partenza){ // esiste percorso
                for(int i=lastOcc; i>=0; i--){
                    printf("%d ", percorso[i]);
                }
                printf("\n");
                // free(percorso);
                break; // non necessario
            }
        }
    }
    else{ // da destra a sinistra
        int* percorso = (int*)calloc(4, sizeof(int));
        percorso[0] = partenza;
        int lastOcc = 0; // indice dell'ultima posizione occupata dell'array
        struct nodo* tmp = NULL;
        while(percorso[lastOcc]!=arrivo){
            tmp = endStation;
            flag=0;
            while(tmp!=startStation){
                if(startStation->autonomies[0] >= partenza - tmp->distanza){ // inserisco in coda
                    if(sizeof(percorso)==lastOcc+2){ // array ha solo ultima posizione vuota
                        int size = sizeof(percorso);
                        percorso = realloc(percorso, size+2*sizeof(int));
                    }
                    percorso[lastOcc+1]=tmp->distanza;
                    lastOcc++;
                    startStation = tmp;
                    partenza = tmp->distanza;
                    flag=1;
                    break;
                }
                tmp = treeSuccessor(tmp);
            }
            if(tmp==startStation && flag==0){
                printf("nessun percorso\n");
                // free(percorso);
                break;
            }
            if(percorso[lastOcc]==arrivo){
                for(int i=0; i<=lastOcc; i++){
                    printf("%d ", percorso[i]);
                }
                printf("\n");
                // free(percorso);
                break; // non necessario
            }
        }
    }
}
*/

void rottamaAuto(struct nodo** root){
    int distanza=0, autonomia=0;
    helper = scanf("%d %d", &distanza, &autonomia);
    struct nodo* z = treeSearch(*root, distanza);
    if(z!=NULL){
        z->autonomiesHead = listDelete(z, autonomia);
    }
    else
        printf("non rottamata\n");
}

void demolisciStazione(struct nodo** root){
    int distanza=0;
    helper = scanf("%d", &distanza);
    struct nodo* z = treeSearch(*root, distanza);
    if(z!=NULL){ // stazione esiste
        z = delete(root, z);
        if(z->autonomiesHead!=NULL){ // necessario?
            struct nodo2* tmp = z->autonomiesHead->next;
            while(tmp!=NULL){
                free(z->autonomiesHead);
                z->autonomiesHead = tmp;
                tmp = tmp->next;
            }
            free(z->autonomiesHead);
        }
        free(z);
        printf("demolita\n");
    }
    else
        printf("non demolita\n");
}

void insert(struct nodo** root, struct nodo* z){
    struct nodo* y = NULL;
    struct nodo* x = *root;
    while(x!=NULL){
        y = x;
        if(z->distanza < x->distanza)
            x = x->left;
        else
            x = x->right;
    }
    z->p = y;
    if(y==NULL){
        *root = z;
        // printf("%d", z->distanza);
    }
    else if(z->distanza < y->distanza)
        y->left = z;
    else
        y->right = z;
    z->left = NULL;
    z->right = NULL;
    z->color = 0;
    rbInsertFixup(root, z);
}

struct nodo* delete(struct nodo** root, struct nodo* z){
    struct nodo* x = NULL;
    struct nodo* y = z;
    char yOriginalColor = y->color;
    if(z->left==NULL){
        x = z->right;
        rbTransplant(root, z, z->right);
    }
    else if(z->right==NULL){
        x = z->left;
        rbTransplant(root, z, z->left);
    }
    else{
        y = treeMinimum(z->right);
        yOriginalColor = y->color;
        x = y->right;
        if(y->p==z && x!=NULL)
            x->p=y;
        else{
            rbTransplant(root, y, y->right);
            y->right = z->right;
            if(y->right!=NULL)
                y->right->p = y;
        }
        rbTransplant(root, z, y);
        y->left = z->left;
        if(y->left!=NULL)
            y->left->p = y;
        y->color = z->color;
    }
    if(yOriginalColor==1)
        rbDeleteFixup(root, x);
    return y;
}

void rbTransplant(struct nodo** root, struct nodo* u, struct nodo* v){
    if(u->p==NULL)
        *root = v;
    else if(u==u->p->left){
        u->p->left = v;
    }
    else
        u->p->right = v;
    if(v!=NULL){
        if(v->p!=NULL)
            v->p = u->p;
    }
}

void leftRotate(struct nodo** root, struct nodo* x){
    struct nodo* y = x->right;
    if(x!=NULL && y!=NULL){
        x->right = y->left;
        if(y->left!=NULL)
            y->left->p = x;
        y->p = x->p;
        if(x->p==NULL)
            *root = y;
        else if(x==x->p->left)
            x->p->left = y;
        else
            x->p->right = y;
        y->left = x;
        x->p = y;
    }
}

void rightRotate(struct nodo** root, struct nodo* x){
    struct nodo* y = x->left;
    if(x!=NULL && y!=NULL){
        x->left = y->right;
        if(y->right!=NULL)
            y->right->p = x;
        y->p = x->p;
        if(x->p==NULL)
            *root = y;
        else if(x==x->p->right)
            x->p->right = y;
        else
            x->p->left = y;
        y->right = x;
        x->p = y;
    }
}

void rbInsertFixup(struct nodo** root, struct nodo* z){
    struct nodo* y = NULL;
    // if(z==root)
    //    root->color = BLACK;
    //else{
    while(z->p!=NULL && z->p->color==0){
        if(z->p->p!=NULL){
            if(z->p==z->p->p->left){
                y = z->p->p->right;
                if(y!=NULL && y->color==0){
                    z->p->color = 1;
                    y->color = 1;
                    z->p->p->color = 0;
                    z = z->p->p;
                }
                else{
                    if(z==z->p->right){
                        z = z->p;
                        leftRotate(root, z);
                    }
                    z->p->color = 1;
                    z->p->p->color = 0;
                    rightRotate(root, z->p->p);
                }

            }
            else{
                y = z->p->p->left;
                if(y!=NULL && y->color==0){
                    z->p->color = 1;
                    y->color = 1;
                    z->p->p->color = 0;
                    z = z->p->p;
                }
                else{
                    if(z==z->p->left){
                        z = z->p;
                        rightRotate(root, z);
                    }
                    z->p->color = 1;
                    z->p->p->color = 0;
                    leftRotate(root, z->p->p);
                }
            }
        }
    }
    // }
    (*root)->color = 1;
}

void rbDeleteFixup(struct nodo** root, struct nodo* z){
    struct nodo* w = NULL;
    if(z!=NULL){
        while(z!=*root && z->color==1){
            // if(x->color==RED || x->p==NULL)
            //    x->color = BLACK;
            if(z->p!=NULL){
                if(z==z->p->left){
                    w = z->p->right;
                    if(w->color==0){
                        w->color = 1;
                        z->p->color = 0;
                        leftRotate(root, z->p);
                        w = z->p->right;
                    }
                    if(w->left->color==1 && w->right->color==1){
                        w->color = 0;
                        z = z->p;
                    }
                    else{
                        if(w->right->color==1){
                            w->left->color = 1;
                            w->color = 0;
                            rightRotate(root, w);
                            w = z->p->right;
                        }
                        w->color = z->p->color;
                        z->p->color = 1;
                        w->right->color = 1;
                        leftRotate(root, z->p);
                        z = *root;
                    }
                }
                else{
                    w = z->p->left;
                    if(w->color==0){
                        w->color = 1;
                        z->p->color = 0;
                        rightRotate(root, z->p);
                        w = z->p->left;
                    }
                    if(w->right->color==1 && w->left->color==1){
                        w->color = 0;
                        z = z->p;
                    }
                    else{
                        if(w->left->color==1){
                            w->right->color = 1;
                            w->color = 0;
                            leftRotate(root, w);
                            w = z->p->left;
                        }
                        w->color = z->p->color;
                        z->p->color = 1;
                        w->left->color = 1;
                        rightRotate(root, z->p);
                        z = *root;
                    }
                }
            }
        }
        z->color = 1;
    }
}

struct nodo* treeSuccessor(struct nodo* x){
    struct nodo* y = NULL;
    if(x->right!=NULL)
        return treeMinimum(x->right);
    y = x->p;
    while(y!=NULL && x==y->right){
        x = y;
        y = y->p;
    }
    return y;
}

struct nodo* treeMinimum(struct nodo* x){
    while(x->left!=NULL)
        x = x->left;
    return x;
}

struct nodo* treeSearch(struct nodo* root, int distanza){
    if(root==NULL || distanza==root->distanza)
        return root;
    if(distanza<root->distanza)
        return treeSearch(root->left, distanza);
    else
        return treeSearch(root->right, distanza);
}

struct nodo2* listInsert(struct nodo* treeNode, int autonomia){ // inserisco in testa
    struct nodo2* newNode = (struct nodo2*)malloc(sizeof(struct nodo2));
    newNode->autonomia = autonomia;
    newNode->next = treeNode->autonomiesHead;
    if(autonomia>treeNode->autonomiaMax)
        treeNode->autonomiaMax = autonomia;
    return newNode;
}

struct nodo2* listDelete(struct nodo* treeNode, int autonomia){
    if(autonomia<=treeNode->autonomiaMax){ // potrebbe esserci l'auto
        struct nodo2* prec = NULL;
        struct nodo2* tmp = treeNode->autonomiesHead;
        while(tmp!=NULL && tmp->autonomia!=autonomia){
            prec = tmp;
            tmp = tmp->next;
        }
        if(tmp!=NULL && tmp->autonomia==autonomia){ // auto c'è
            int max=0;
            if(prec!=NULL)
                prec->next = tmp->next;
            else
                treeNode->autonomiesHead = tmp->next;
            free(tmp);
            if(autonomia==treeNode->autonomiaMax){ // auto aveva autonomia max, quindi cerco la nuova autonomia max
                prec = treeNode->autonomiesHead;
                while(prec!=NULL){
                    if(prec->autonomia>max)
                        max = prec->autonomia;
                    prec = prec->next;
                }
                treeNode->autonomiaMax = max;
            }
            printf("rottamata\n");
            return treeNode->autonomiesHead;
        }
    }
    // auto non c'è
    printf("non rottamata\n");
    return treeNode->autonomiesHead;
}



