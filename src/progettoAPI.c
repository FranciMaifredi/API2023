//
// Created by Francesca Maifredi on 18/07/23.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ALBERO DELLE STAZIONI
struct nodo{
    int distanza; // key
    int autonomiaMax;
    enum {RED, BLACK} color;
    struct nodo* p;
    struct nodo* left;
    struct nodo* right;
    struct nodo2* autonomiesHead; // testa della lista delle autonomie
};

// LISTA DELLE AUTONOMIE
// ordinata in ordine decrescente di autonomia
struct nodo2{
    int autonomia;
    int count;
    struct nodo2* next;
};

// LISTA DELLE TAPPE DEL PERCORSO
struct nodo3{
    int distanza;
    struct nodo3* next;
};

void aggiungiStazione(struct nodo* root);
void aggiungiAuto(struct nodo* root);
void pianificaPercorso(struct nodo* root);
void rottamaAuto(struct nodo* root);
void demolisciStazione(struct nodo* root);
// RED BLACK TREE FUNCTIONS
void insert(struct nodo* root, struct nodo* z);
struct nodo* delete(struct nodo* root, struct nodo* z);
void leftRotate(struct nodo* root, struct nodo* x);
void rightRotate(struct nodo* root, struct nodo* x);
void rbInsertFixup(struct nodo* root, struct nodo* z);
void rbDeleteFixup(struct nodo* root, struct nodo* z);
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
        scanf("%s", comando);
        if(strcmp(comando, "aggiungi-stazione")==0)
            aggiungiStazione(treeRoot);
        else if(strcmp(comando, "aggiungi-auto")==0)
            aggiungiAuto(treeRoot);
        else if(strcmp(comando, "pianifica-percorso")==0)
            pianificaPercorso(treeRoot);
        else if(strcmp(comando, "rottama-auto")==0)
            rottamaAuto(treeRoot);
        else if(strcmp(comando, "demolisci-stazione")==0)
            demolisciStazione(treeRoot);
    }
    return 0;
}

void aggiungiStazione(struct nodo* root){
    int distanza, numAuto, autonomia;
    scanf("%d %d", &distanza, &numAuto);

    // controllo se la stazione è già presente
    struct nodo* z = treeSearch(root, distanza);
    if(z!=NULL){
        for(int i=0; i<numAuto; i++) // necessario?
            scanf("%d", &autonomia);
        printf("non aggiunta\n");
        return;
    }

    // z = NULL, quindi la stazione non è presente
    // creo nodo da inserire nell'albero delle stazioni
    z = (struct nodo*)malloc(sizeof(struct nodo));
    z->distanza = distanza;
    z->autonomiaMax = 0;
    z->color = RED; // necessario?
    z->p = NULL; // necessario?
    z->left = NULL; // necessario?
    z->right = NULL; // necessario?
    z->autonomiesHead = NULL; // necessario?
    insert(root, z);

    // inserisco le autonomie delle auto
    for(int i=0; i<numAuto; i++){
        scanf("%d", &autonomia);
        z->autonomiesHead = listInsert(z, autonomia);
    }

    printf("aggiunta\n");
}

void aggiungiAuto(struct nodo* root){
    int distanza, autonomia;
    scanf("%d %d", &distanza, &autonomia);
    struct nodo* z = treeSearch(root, distanza);
    if(z!=NULL){
        z->autonomiesHead = listInsert(z, autonomia);
        printf("aggiunta\n");
    }
    else
        printf("non aggiunta\n");
}

void pianificaPercorso(struct nodo* root){
    int partenza, arrivo;
    scanf("%d %d", &partenza, &arrivo);
    struct nodo* startStation = treeSearch(root, partenza);
    struct nodo* endStation = treeSearch(root, arrivo);
    if(partenza==arrivo) { // stessa stazione
        printf("%d\n", partenza);
    }
    else if(partenza<arrivo){ // da sinistra a destra, complessità fattoriale
        struct nodo3* list = NULL;
        struct nodo3* toInsert = (struct nodo3*)malloc(sizeof(struct nodo3));
        toInsert->distanza = arrivo;
        toInsert->next = NULL;
        list = toInsert;
        struct nodo* tmp = NULL;
        while(toInsert->distanza!=partenza){
            tmp = startStation;
            while(tmp!=endStation) {
                if (tmp->autonomiaMax >= arrivo - tmp->distanza) { // inserisco in testa
                    struct nodo3 *toInsert = (struct nodo3 *) malloc(sizeof(struct nodo3));
                    toInsert->distanza = tmp->distanza;
                    toInsert->next = list;
                    list = toInsert;
                    endStation = tmp;
                    arrivo = tmp->distanza;
                    break;
                }
                tmp = treeSuccessor(tmp);
            }
            if(tmp==endStation){ // non esiste percorso
                printf("nessun percorso\n");
                break;
            }
            if(toInsert->distanza==partenza){ // esiste percorso
                struct nodo3* tmp2 = list;
                while(tmp2!=NULL){
                    printf("%d ", tmp2->distanza);
                    // free(tmp2) si può fare?
                    tmp2 = tmp2->next;
                }
                printf("\n");
            }
        }
    }
    else{ // da destra a sinistra
        struct nodo3* listHead = NULL;
        struct nodo3* listTail = NULL;
        struct nodo3* toInsert = (struct nodo3*)malloc(sizeof(struct nodo3));
        toInsert->distanza = partenza;
        toInsert->next = NULL;
        listHead = toInsert;
        listTail = toInsert;
        struct nodo* tmp = NULL;
        while(toInsert->distanza!=arrivo){
            tmp = endStation;
            while(tmp!=startStation){
                if(startStation->autonomiaMax >= partenza - tmp->distanza){ // inserisco in coda
                    struct nodo3* toInsert = (struct nodo3*)malloc(sizeof(struct nodo3));
                    toInsert->distanza = tmp->distanza;
                    toInsert->next = NULL;
                    listTail->next = toInsert;
                    listTail = toInsert;
                    startStation = tmp;
                    partenza = tmp->distanza;
                    break;
                }
                tmp = treeSuccessor(tmp);
            }
            if(tmp==startStation){
                printf("nessun percorso\n");
                break;
            }
            if(toInsert->distanza==arrivo){
                struct nodo3* tmp2 = listHead;
                while(tmp2!=NULL){
                    printf("%d ", tmp2->distanza);
                    // free(tmp2) si può fare?
                    tmp2 = tmp2->next;
                }
                printf("\n");
            }
        }
    }
}

void rottamaAuto(struct nodo* root){
    int distanza, autonomia;
    scanf("%d %d", &distanza, &autonomia);
    struct nodo* z = treeSearch(root, distanza);
    if(z!=NULL){
        z->autonomiesHead = listDelete(z, autonomia);
    }
    else
        printf("non rottamata\n");
}

void demolisciStazione(struct nodo* root){
    int distanza;
    scanf("%d", &distanza);
    struct nodo* z = treeSearch(root, distanza);
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

void leftRotate(struct nodo* root, struct nodo* x){
    struct nodo* y = x->right;
    x->right = y->left;
    if(y->left!=NULL)
        y->left->p = x;
    y->p = x->p;
    if(x->p==NULL)
        root = y;
    else if(x==x->p->left)
        x->p->left = y;
    else
        x->p->right = y;
    y->left = x;
    x->p = y;
}

void rightRotate(struct nodo* root, struct nodo* x){
    struct nodo* y = x->left;
    x->left = y->right;
    if(y->right!=NULL)
        y->right->p = x;
    y->p = x->p;
    if(x->p==NULL)
        root = y;
    else if(x==x->p->right)
        x->p->right = y;
    else
        x->p->left = y;
    y->right = x;
    x->p = y;
}

void rbInsertFixup(struct nodo* root, struct nodo* z){
    struct nodo* x = NULL;
    struct nodo* y = NULL;
    if(z==root)
        root->color = BLACK;
    else{
        x = z->p;
        if(x->color==RED){
            if(x==x->p->left){
                y = x->p->right;
                if(y->color==RED){
                    x->color = BLACK;
                    y->color = BLACK;
                    x->p->color = RED;
                    rbInsertFixup(root, x->p);
                }
                else if(z==x->right){
                    z = x;
                    leftRotate(root, z);
                    x = z->p;
                }
                x->color = BLACK;
                x->p->color = RED;
                rightRotate(root, x->p);
            }
            else{
                y = x->p->left;
                if(y->color==RED){
                    x->color = BLACK;
                    y->color = BLACK;
                    x->p->color = RED;
                    rbInsertFixup(root, x->p);
                }
                else if(z==x->left){
                    z = x;
                    rightRotate(root, z);
                    x = z->p;
                }
                x->color = BLACK;
                x->p->color = RED;
                leftRotate(root, x->p);
            }
        }
    }
    // root->color = BLACK;
}

void rbDeleteFixup(struct nodo* root, struct nodo* z){
    struct nodo* x = NULL;
    struct nodo* w = NULL;
    if(x->color==RED || x->p==NULL)
        x->color = BLACK;
    else if(x==x->p->left){
        w = x->p->right;
        if(w->color==RED){
            w->color = BLACK;
            x->p->color = RED;
            leftRotate(root, x->p);
            w = x->p->right;
        }
        if(w->left->color==BLACK && w->right->color==BLACK){
            w->color = RED;
            rbDeleteFixup(root, x->p);
        }
        else if(w->right->color==BLACK){
            w->left->color = BLACK;
            w->color = RED;
            rightRotate(root, w);
            w = x->p->right;
        }
        w->color = x->p->color;
        x->p->color = BLACK;
        w->right->color = BLACK;
        leftRotate(root, x->p);
    }
    else{
        w = x->p->left;
        if(w->color==RED){
            w->color = BLACK;
            x->p->color = RED;
            rightRotate(root, x->p);
            w = x->p->left;
        }
        if(w->right->color==BLACK && w->left->color==BLACK){
            w->color = RED;
            rbDeleteFixup(root, x->p);
        }
        else if(w->left->color==BLACK){
            w->right->color = BLACK;
            w->color = RED;
            leftRotate(root, w);
            w = x->p->left;
        }
        w->color = x->p->color;
        x->p->color = BLACK;
        w->left->color = BLACK;
        rightRotate(root, x->p);
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

struct nodo2* listInsert(struct nodo* treeNode, int autonomia){
    struct nodo2* prec = NULL;
    struct nodo2* temp = treeNode->autonomiesHead;
    while(temp!=NULL && autonomia<temp->autonomia){
        prec = temp;
        temp = temp->next;
    }
    if(temp!=NULL && temp->autonomia==autonomia){
        temp->count++;
        return treeNode->autonomiesHead;
    }
    else{
        struct nodo2* newNode = (struct nodo2*)malloc(sizeof(struct nodo2));
        newNode->autonomia = autonomia;
        newNode->count = 1;
        newNode->next = temp;
        if(autonomia > treeNode->autonomiaMax)
            treeNode->autonomiaMax = autonomia;
        if(prec==NULL){
            treeNode->autonomiesHead = newNode;
            return treeNode->autonomiesHead;
        }
        else{
            prec->next = newNode;
            return treeNode->autonomiesHead;
        }
    }
}

struct nodo2* listDelete(struct nodo* treeNode, int autonomia){
    struct nodo2* prec = NULL;
    struct nodo2* temp = treeNode->autonomiesHead;
    while(temp!=NULL && autonomia<temp->autonomia){
        prec = temp;
        temp = temp->next;
    }
    if(temp!=NULL && temp->autonomia==autonomia){
        temp->count--;
        if(temp->count==0){
            if(autonomia==treeNode->autonomiaMax){
                if(temp->next!=NULL)
                    treeNode->autonomiaMax = temp->next->autonomia;
                else
                    treeNode->autonomiaMax = 0;
            }
            if(prec==NULL){
                treeNode->autonomiesHead = temp->next;
                free(temp);
                printf("rottamata\n");
                return treeNode->autonomiesHead;
            }
            else{
                prec->next = temp->next;
                free(temp);
                printf("rottamata\n");
                return treeNode->autonomiesHead;
            }
        }
        else{
            printf("rottamata\n");
            return treeNode->autonomiesHead;
        }
    }
    else{
        printf("non rottamata\n");
        return treeNode->autonomiesHead;
    }
}





