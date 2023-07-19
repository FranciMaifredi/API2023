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
void leftRotate(struct nodo* root, struct nodo* x);
void rightRotate(struct nodo* root, struct nodo* x);
void rbInsertFixup(struct nodo* root, struct nodo* z);
void rbDeleteFixup(struct nodo* root, struct nodo* z);
struct nodo* treeSuccessor(struct nodo* x);
struct nodo* treeMinimum(struct nodo* x);

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
        root.color = BLACK;
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
                    xx->color = BLACK;
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





