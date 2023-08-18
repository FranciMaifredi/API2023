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

// LISTA DEL PERCORSO
struct nodo3{
    int distanza;
    struct nodo3* next;
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
struct nodo* treePredecessor(struct nodo* x);
struct nodo* treeMinimum(struct nodo* x);
struct nodo* treeMaximum(struct nodo* x);
struct nodo* treeSearch(struct nodo* x, int distanza);
// LIST FUNCTIONS
struct nodo2* listInsert(struct nodo* treeNode, int autonomia);
struct nodo2* listDelete(struct nodo* treeNode, int autonomia);


int main(){
    struct nodo* treeRoot = NULL;
    char comando[19];
    while(scanf("%s", comando)!=EOF){
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

void pianificaPercorso(struct nodo** root){
    int partenza=0, arrivo=0;
    helper = scanf("%d %d", &partenza, &arrivo);
    struct nodo* startStation = treeSearch(*root, partenza);
    struct nodo* endStation = treeSearch(*root, arrivo);
    if(partenza==arrivo) { // stessa stazione
        printf("%d\n", partenza);
    }
    else if(partenza<arrivo){ // da sinistra a destra
        struct nodo3* percorso = (struct nodo3*)malloc(sizeof(struct nodo3)); // creo lista per percorso
        percorso->distanza = arrivo;
        percorso->next = NULL;
        struct nodo3* help = percorso;

        struct nodo* tmp = treePredecessor(endStation);
        while(tmp!=NULL && tmp->distanza>=partenza){ // l'ultima iterazione è per tmp = startStation
            help = percorso;
            if(tmp->distanza+tmp->autonomiaMax>=help->distanza){
                while(help->next!=NULL && tmp->distanza+tmp->autonomiaMax>=help->next->distanza){
                    percorso = help->next;
                    struct nodo3* prec = help;
                    help = help->next;
                    free(prec);
                }
                struct nodo3* newNode = (struct nodo3*)malloc(sizeof(struct nodo3));
                newNode->distanza = tmp->distanza; // inserisco in testa
                newNode->next = help;
                percorso = newNode;
            }
            tmp = treePredecessor(tmp);
        }

        if(percorso->distanza==partenza){ // esiste percorso (PERCORSO NON PUÒ ESSERE NULL)
            help = percorso;
            printf("%d", help->distanza);
            help = help->next;
            while(help!=NULL){
                printf(" %d", help->distanza);
                struct nodo3* prec = help;
                help = help->next;
                free(prec);
            }
            printf("\n");
        }
        else
            printf("nessun percorso\n");
    }
    else{ // da destra a sinistra
        struct nodo3* percorso = (struct nodo3*)malloc(sizeof(struct nodo3)); // creo lista per percorso
        percorso->distanza = arrivo;
        percorso->next = NULL;
        struct nodo3* help = percorso;

        struct nodo* tmp = treeSuccessor(endStation);
        while(tmp!=NULL && tmp->distanza<=partenza){ // l'ultima iterazione è per tmp = startStation
            help = percorso;
            if(tmp->distanza-tmp->autonomiaMax<=help->distanza){
                while(help->next!=NULL && tmp->distanza-tmp->autonomiaMax<=help->next->distanza){
                    percorso = help->next;
                    struct nodo3* prec = help;
                    help = help->next;
                    free(prec);
                }
                struct nodo3* newNode = (struct nodo3*)malloc(sizeof(struct nodo3));
                newNode->distanza = tmp->distanza; // inserisco in testa
                newNode->next = help;
                percorso = newNode;
            }
            tmp = treeSuccessor(tmp);
        }

        if(percorso->distanza!=partenza){
            printf("nessun percorso\n");
        }
        else{ // percorso c'è
            int numTappe = 0;
            int distanzaLim = 0, tappa = 0, minTappa = 0;
            struct nodo3* proxTappa = percorso->next;
            struct nodo* maxTappa = NULL;
            tmp = startStation;
            help = percorso;
            while(help!=NULL){ // conto le tappe
                numTappe++;
                help=help->next;
            }
            numTappe--;
            help = percorso;
            // printf("\n");

            // int percorsoFinale[numTappe];
            // percorsoFinale[arrayInd] = tmp->distanza;
            // arrayInd++;
            printf("%d ", tmp->distanza);
            maxTappa = treeSearch(*root, tmp->distanza-tmp->autonomiaMax);
            int i=0;
            while(maxTappa==NULL && tmp->distanza-tmp->autonomiaMax+i<=tmp->distanza){
                i++;
                maxTappa = treeSearch(*root, tmp->distanza-tmp->autonomiaMax+i);
            } // trovata la max stazione raggiungibile
            distanzaLim = maxTappa->distanza;
            minTappa = distanzaLim;
            // printf("%d: %d tappe -> ", tmp->distanza, numTappe);
            // printf("raggiunge %d\n", maxTappa->distanza);

            while(tmp!=endStation){ // percorso = startStation
                if(tmp->distanza<=proxTappa->distanza){
                    numTappe--;
                    help = help->next;
                    proxTappa = proxTappa->next;
                    if(tappa!=0)
                        printf("%d ", tappa);
                    // arrayInd++;
                    distanzaLim = minTappa;
                }
                if(help!=percorso && tmp->distanza-tmp->autonomiaMax<=proxTappa->distanza && tmp->distanza>=distanzaLim){
                    // tappa raggiungibile e che riesce ad arrivare alla prossima tappa
                    // percorsoFinale[arrayInd] = tmp->distanza;
                    maxTappa = NULL; // sovrabbondante
                    maxTappa = treeSearch(*root, tmp->distanza-tmp->autonomiaMax);
                    i=0;
                    while(maxTappa==NULL && tmp->distanza-tmp->autonomiaMax+i<=tmp->distanza){
                        i++;
                        maxTappa = treeSearch(*root, tmp->distanza-tmp->autonomiaMax+i);
                    } // trovata la max stazione raggiungibile
                    if(maxTappa->distanza<=minTappa || numTappe==1){
                        tappa = tmp->distanza;
                        minTappa = maxTappa->distanza;
                    }

                    // printf("%d: %d tappe -> ", tmp->distanza, numTappe);
                    // printf("raggiunge %d\n", maxTappa->distanza);
                }
                tmp = treePredecessor(tmp);
            }
            printf("%d ", tappa);
            printf("%d\n", tmp->distanza);
            // percorsoFinale[arrayInd+1] = tmp->distanza;
            // printf("%d\n\n", tmp->distanza); // è l'arrivo
        }
        help = percorso;
        while(help!=NULL){
            struct nodo3* prec = help;
            help = help->next;
            free(prec);
        }
    }
}

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
        //viene ritornata la testa del sottoalbero senza quel nodo, non il nodo eliminato!!
        /* if(z->autonomiesHead!=NULL){ // necessario?
            struct nodo2* tmp = z->autonomiesHead->next;
            while(tmp!=NULL){
                free(z->autonomiesHead);
                z->autonomiesHead = tmp;
                tmp = tmp->next;
            }
            free(z->autonomiesHead);
        }*/
        //free(z); da problemi di invalid read
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
        if(y->p==z){
            if(x!=NULL)
                x->p=y;
        }
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

    if(z->autonomiesHead!=NULL){
        struct nodo2* tmp = z->autonomiesHead;
        while(tmp!=NULL){
            struct nodo2* prec = tmp;
            tmp = tmp->next;
            free(prec);
        }
    }
    free(z);
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
        // if(v->p!=NULL)
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
                    if(w!=NULL && w->color==0){
                        w->color = 1;
                        z->p->color = 0;
                        leftRotate(root, z->p);
                        w = z->p->right;
                    }
                    if(w->left!=NULL && w->right!=NULL && w->left->color==1 && w->right->color==1){
                        w->color = 0;
                        z = z->p;
                    }
                    else{
                        if(w->right!=NULL && w->right->color==1){
                            if(w->left!=NULL)
                                w->left->color = 1;
                            if(w!=NULL)
                                w->color = 0;
                            rightRotate(root, w);
                            w = z->p->right;
                        }
                        if(w!=NULL)
                            w->color = z->p->color;
                        z->p->color = 1;
                        if(w->right!=NULL)
                            w->right->color = 1;
                        leftRotate(root, z->p);
                        z = *root;
                    }
                }
                else{
                    w = z->p->left;
                    if(w!=NULL && w->color==0){
                        w->color = 1;
                        z->p->color = 0;
                        rightRotate(root, z->p);
                        w = z->p->left;
                    }
                    if(w->left!=NULL && w->right!=NULL && w->right->color==1 && w->left->color==1){
                        w->color = 0;
                        z = z->p;
                    }
                    else{
                        if(w->left!=NULL && w->left->color==1){
                            if(w->right!=NULL)
                                w->right->color = 1;
                            if(w!=NULL)
                                w->color = 0;
                            leftRotate(root, w);
                            w = z->p->left;
                        }
                        if(w!=NULL)
                            w->color = z->p->color;
                        z->p->color = 1;
                        if(w->left!=NULL)
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

struct nodo* treePredecessor(struct nodo* x){
    struct nodo* y = NULL;
    if(x->left!=NULL)
        return treeMaximum(x->left);
    y = x->p;
    while(y!=NULL && x==y->left){
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

struct nodo* treeMaximum(struct nodo* x){
    while(x->right!=NULL)
        x = x->right;
    return x;
}

struct nodo* treeSearch(struct nodo* x, int distanza){
    if(x==NULL)
        return x;
    if(distanza==x->distanza)
        return x;
    if(distanza<x->distanza)
        return treeSearch(x->left, distanza);
    else
        return treeSearch(x->right, distanza);
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



