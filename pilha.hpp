#ifndef PILHA_HPP
#define PILHA_HPP

#include "lista.hpp"

// No da pilha encadeada
typedef struct NoPilha {
    char simbolo;
    struct NoPilha *prox;
} NoPilha;

// topo e tananho
typedef struct {
    NoPilha *topo;
    int tamanho;
} Pilha;

void pilha_inicializar(Pilha *p);


int pilha_empilhar(Pilha *p, char simbolo);
char pilha_desempilhar(Pilha *p);

// consulta o topo
char pilha_topo(const Pilha *p);

//saber tamanho
int pilha_tamanho(const Pilha *p);
int pilha_vazia(const Pilha *p);

// nao empilha dps do z
int pilha_topo_e_z(const Pilha *p);

// retorna 1 se coincide e 0 se nao
int pilha_coincide_com_lista(const Pilha *p, const Lista *receita);

void pilha_destruir(Pilha *p);

#endif