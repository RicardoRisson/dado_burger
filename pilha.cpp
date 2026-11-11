
#include "pilha.hpp"
#include <stdlib.h>

void pilha_inicializar(Pilha *p) {
    p->topo = NULL;
    p->tamanho = 0;
}

int pilha_empilhar(Pilha *p, char simbolo) {
    NoPilha *novo = (NoPilha*) malloc(sizeof(NoPilha));
    if (novo == NULL) {
        return 0;
    }

    novo->simbolo = simbolo;
    novo->prox = p->topo;
    p->topo = novo;
    p->tamanho++;
    return 1;
}

char pilha_desempilhar(Pilha *p) {
    if (p->topo == NULL) {
        return '\0';
    }

    NoPilha *alvo = p->topo;
    char s = alvo->simbolo;
    p->topo = alvo->prox;
    free(alvo);
    p->tamanho--;
    return s;
}

char pilha_topo(const Pilha *p) {
    if (p->topo == NULL) {
        return '\0';
    }

    return p->topo->simbolo;
}

int pilha_tamanho(const Pilha *p) {
    return p->tamanho;
}

int pilha_vazia(const Pilha *p) {
    return p->topo == NULL;
}

int pilha_topo_e_z(const Pilha *p) {
    return (p->topo != NULL && p->topo->simbolo == 'z');
}

int pilha_coincide_com_lista(const Pilha *p, const Lista *receita) {
    if (p->tamanho != receita->tamanho) {
        return 0;
    }

    NoPilha *no_p = p->topo;
    NoLista *no_l = receita->fim;  // fim da lista = topo da receita

    while (no_p != NULL && no_l != NULL) {
        if (no_p->simbolo != no_l->simbolo) {
            return 0;
        }

        no_p = no_p->prox;
        no_l = no_l->ant;
    }

    return (no_p == NULL && no_l == NULL);
}

void pilha_destruir(Pilha *p) {
    while (p->topo != NULL) {
        NoPilha *prox = p->topo->prox;
        free(p->topo);
        p->topo = prox;
    }

    p->tamanho = 0;
}