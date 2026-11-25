#include "../include/pilha.h"
#include "../include/config.h"

#include <stdlib.h>

void pilha_inicializar(Pilha *p) {
    p->topo = NULL;
    p->tamanho = 0;
}

// empilhar
void pilha_empilhar(Pilha *p, char simbolo) {
    NoPilha *novo = malloc(sizeof(NoPilha));

    novo->simbolo = simbolo;
    novo->prox = p->topo;
    p->topo = novo;
    p->tamanho++;
}

// desempilha e retorna o simbolo do topo
char pilha_desempilhar(Pilha *p) {
    NoPilha *alvo = p->topo;
    char s = alvo->simbolo;
    p->topo = alvo->prox;
    free(alvo);
    p->tamanho--;
    return s;
}

int pilha_vazia(const Pilha *p) {
    return p->topo == NULL;
}

// ve se o pao de cima ja foi colocado
int pilha_topo_e_z(const Pilha *p) {
    return (p->topo != NULL && p->topo->simbolo == ING_PAO_CIMA);
}

// compara a pilha com a receita, topo da pilha com o fim da lista, sem desempilhar
int pilha_coincide_com_lista(const Pilha *p, const Lista *receita) {
    if (p->tamanho != receita->tamanho)
        return 0;

    NoPilha *no_p = p->topo;
    NoLista *no_l = receita->fim;

    while (no_p != NULL) {
        if (no_p->simbolo != no_l->simbolo)
            return 0;

        no_p = no_p->prox;
        no_l = no_l->ant;
    }

    return 1;
}

void pilha_destruir(Pilha *p) {
    while (p->topo != NULL) {
        NoPilha *prox = p->topo->prox;
        free(p->topo);
        p->topo = prox;
    }

    p->tamanho = 0;
}