#ifndef PILHA_H
#define PILHA_H

#include "lista.h"

typedef struct NoPilha {
    char simbolo;
    struct NoPilha *prox;
} NoPilha;

typedef struct {
    NoPilha *topo;
    int tamanho;
} Pilha;

void pilha_inicializar(Pilha *p);


void pilha_empilhar(Pilha *p, char simbolo);
char pilha_desempilhar(Pilha *p);

int pilha_tamanho(const Pilha *p);
int pilha_vazia(const Pilha *p);

int pilha_topo_e_z(const Pilha *p);

int pilha_coincide_com_lista(const Pilha *p, const Lista *receita);

void pilha_destruir(Pilha *p);

#endif