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
    // int autonomiaMax;
    char color; // RED=0, BLACK=1
    struct nodo* p;
    struct nodo* left;
    struct nodo* right;
    int* autonomies; // array dinamico delle autonomie
    unsigned short* multiplicities; // array dinamico delle molteplicità di ogni autonomia
};

// LISTA DELLE AUTONOMIE: ordinata in ordine decrescente di autonomia
//struct nodo2{
//    int autonomia;
//    int count;
//    struct nodo2* next;
//};

// LISTA DELLE TAPPE DEL PERCORSO
// struct nodo3{
//    int distanza;
//    struct nodo3* next;
//};

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
// struct nodo2* listInsert(struct nodo* treeNode, int autonomia);
// struct nodo2* listDelete(struct nodo* treeNode, int autonomia);

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
    int distanza=0, numAuto=0, autonomia=0, dimArray=0, numEmpty=0;
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
        z->color = 0; // necessario?
        z->p = NULL; // necessario?
        z->left = NULL; // necessario?
        z->right = NULL; // necessario?
        if(numAuto==512)
            dimArray = 512;
        else
            dimArray = (numAuto/8+1)*8;
        z->autonomies = (int*)calloc(dimArray, sizeof(int));
        z->multiplicities = (unsigned short*)calloc(dimArray, sizeof(unsigned short));
        numEmpty = dimArray;
        insert(root, z);

        // inserisco le autonomie delle auto
        for(int i=0; i<numAuto; i++){
            helper = scanf("%d", &autonomia);
            for(int i=0; i<dimArray; i++){
                if(autonomia>(z->autonomies[i])){
                    if(z->autonomies[i]==0){ // array è vuoto da qui in avanti. Inserisco auto
                        z->autonomies[i] = autonomia;
                        z->multiplicities[i] = 1;
                    }
                    else{
                        int tmpA1=autonomia, tmpA2=0, j=0;
                        unsigned short tmpM1=1, tmpM2=0;
                        while(i+j<dimArray && z->autonomies[i+j]!=0){
                            tmpA2 = z->autonomies[i+j];
                            tmpM2 = z->multiplicities[i+j];
                            z->autonomies[i+j] = tmpA1;
                            z->multiplicities[i+j] = tmpM1;
                            tmpA1 = tmpA2;
                            tmpM1 = tmpM2;
                            j++;
                        }
                        z->autonomies[i+j] = tmpA1;
                        z->multiplicities[i+j] = tmpM1;
                        numEmpty--;
                        break;
                    }

                }
                else if(autonomia==(z->autonomies[i])){
                    z->multiplicities[i]++;
                    break;
                }
            }
        }
        if(numEmpty>8){ // ci sono troppi spazi vuoti
            dimArray = ((dimArray - numEmpty)/8 + 1)*8;
            if(dimArray>512) // non dovrebbe verificarsi mai
                dimArray = 512;
            z->autonomies = realloc(z->autonomies, dimArray*sizeof(int));
            z->multiplicities = realloc(z->multiplicities, dimArray*sizeof(unsigned short));
        }

        printf("aggiunta\n");
    }
}

void aggiungiAuto(struct nodo** root){
    int distanza=0, autonomia=0;
    helper = scanf("%d %d", &distanza, &autonomia);
    struct nodo* z = treeSearch(*root, distanza);
    int dimArray = sizeof(z->autonomies);
    if(z!=NULL){
        // controllo se riallocare gli arrays
        if(z->autonomies[dimArray-2]!=0 && dimArray!=512){ // solo 1 spazio libero!! array vanno riallocati
            dimArray = dimArray + 8;
            if(dimArray>512)
                dimArray = 512;
            z->autonomies = realloc(z->autonomies, dimArray*sizeof(int));
            z->multiplicities = realloc(z->multiplicities, dimArray*sizeof(unsigned short));
        }

        // aggiungo l'auto
        for(int i=0; i<dimArray; i++){
            if(autonomia>(z->autonomies[i])){
                if(z->autonomies[i]==0){ // array è vuoto da qui in avanti. Inserisco auto
                    z->autonomies[i] = autonomia;
                    z->multiplicities[i] = 1;
                }
                else{
                    int tmpA1=autonomia, tmpA2=0, j=0;
                    unsigned short tmpM1=1, tmpM2=0;
                    while(i+j<dimArray && z->autonomies[i+j]!=0){
                        tmpA2 = z->autonomies[i+j];
                        tmpM2 = z->multiplicities[i+j];
                        z->autonomies[i+j] = tmpA1;
                        z->multiplicities[i+j] = tmpM1;
                        tmpA1 = tmpA2;
                        tmpM1 = tmpM2;
                        j++;
                    }
                    z->autonomies[i+j] = tmpA1;
                    z->multiplicities[i+j] = tmpM1;
                    break;
                }
            }
            else if(autonomia==(z->autonomies[i])){
                z->multiplicities[i]++;
                break;
            }
        }
        printf("aggiunta\n");
    }
    else
        printf("non aggiunta\n");
}

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
        /* struct nodo3* list = NULL;
        struct nodo3* toInsert = (struct nodo3*)malloc(sizeof(struct nodo3));
        toInsert->distanza = arrivo;
        toInsert->next = NULL;
        list = toInsert;
         */
        int* percorso = (int*)calloc(4, sizeof(int));
        percorso[0] = arrivo;
        int lastOcc = 0; // indice dell'ultima posizione occupata dell'array
        struct nodo* tmp = NULL;
        while(percorso[lastOcc]!=partenza){
            tmp = startStation;
            flag=0;
            while(tmp!=endStation) {
                if (tmp->autonomies[0] >= arrivo - tmp->distanza) { // inserisco in coda nell'array
                    /*toInsert = (struct nodo3 *) malloc(sizeof(struct nodo3));
                    toInsert->distanza = tmp->distanza;
                    toInsert->next = list;
                    list = toInsert;
                     */
                    if(sizeof(percorso)==lastOcc+2){ // array ha solo ultima posizione vuota
                        percorso = realloc(percorso, (sizeof(percorso)+2)*sizeof(int));
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
                free(percorso);
                break;
            }
            if(percorso[lastOcc]==partenza){ // esiste percorso
                /*struct nodo3* tmp2 = list;
                while(tmp2!=NULL){
                    printf("%d ", tmp2->distanza);
                    // free(tmp2) si può fare?
                    tmp2 = tmp2->next;
                }*/
                for(int i=lastOcc; i>=0; i--){
                    printf("%d ", percorso[i]);
                }
                printf("\n");
                free(percorso);
                break; // non necessario
            }
        }
    }
    else{ // da destra a sinistra
        /*struct nodo3* listHead = NULL;
        struct nodo3* listTail = NULL;
        struct nodo3* toInsert = (struct nodo3*)malloc(sizeof(struct nodo3));
        toInsert->distanza = partenza;
        toInsert->next = NULL;
        listHead = toInsert;
        listTail = toInsert;
         */
        int* percorso = (int*)calloc(4, sizeof(int));
        percorso[0] = partenza;
        int lastOcc = 0; // indice dell'ultima posizione occupata dell'array
        struct nodo* tmp = NULL;
        while(percorso[lastOcc]!=arrivo){
            tmp = endStation;
            flag=0;
            while(tmp!=startStation){
                if(startStation->autonomies[0] >= partenza - tmp->distanza){ // inserisco in coda
                    /*toInsert = (struct nodo3*)malloc(sizeof(struct nodo3));
                    toInsert->distanza = tmp->distanza;
                    toInsert->next = NULL;
                    listTail->next = toInsert;
                    listTail = toInsert;
                     */
                    if(sizeof(percorso)==lastOcc+2){ // array ha solo ultima posizione vuota
                        percorso = realloc(percorso, (sizeof(percorso)+2)*sizeof(int));
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
                free(percorso);
                break;
            }
            if(percorso[lastOcc]==arrivo){
                /*
                struct nodo3* tmp2 = listHead;
                while(tmp2!=NULL){
                    printf("%d ", tmp2->distanza);
                    // free(tmp2) si può fare?
                    tmp2 = tmp2->next;
                }*/
                for(int i=0; i<=lastOcc; i++){
                    printf("%d ", percorso[i]);
                }
                printf("\n");
                free(percorso);
                break; // non necessario
            }
        }
    }
}

void rottamaAuto(struct nodo** root){
    int distanza=0, autonomia=0;
    helper = scanf("%d %d", &distanza, &autonomia);
    struct nodo* z = treeSearch(*root, distanza);
    if(z!=NULL){
        for(int i=0; i<sizeof(z->autonomies); i++){
            if(autonomia>(z->autonomies[i])){
                printf("non rottamata\n");
                break;
            }
            else if(autonomia==(z->autonomies[i])){
                z->multiplicities[i]--;
                if(z->multiplicities[i]==0){
                    int j=0;
                    if(z->autonomies[i+j+1]==0) // array è vuoto da qui in avanti
                        z->autonomies[i+j] = 0;
                    while(i+j<sizeof(z->autonomies) && z->autonomies[i+j+1]!=0){
                        z->autonomies[i+j] = z->autonomies[i+j+1];
                        z->multiplicities[i+j] = z->multiplicities[i+j+1];
                        j++;
                    }
                }
                printf("rottamata\n");
                break;
            }
        }
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
        /*
        if(z->autonomiesHead!=NULL){ // necessario?
            struct nodo2* tmp = z->autonomiesHead->next;
            while(tmp!=NULL){
                free(z->autonomiesHead);
                z->autonomiesHead = tmp;
                tmp = tmp->next;
            }
            free(z->autonomiesHead);
        }
         */
        free(z->autonomies); // necessario?
        free(z->multiplicities); // necessario?
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
        if(y->p==z)
            x->p=y;
        else{
            rbTransplant(root, y, y->right);
            y->right = z->right;
            y->right->p = y;
        }
        rbTransplant(root, z, y);
        y->left = z->left;
        y->left->p = y;
        y->color = z->color;
    }
    if(yOriginalColor==1)
        rbDeleteFixup(root, x);
    return y;
    /*
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
     */
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

void rightRotate(struct nodo** root, struct nodo* x){
    struct nodo* y = x->left;
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
    if(z!=NULL)
        z->color = 1;
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

/*
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
*/



