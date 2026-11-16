#include "splay.h"
#include "timer.h"
#include <stdlib.h>

static NoSplay* rotacao_direita(NoSplay *y) {
    NoSplay *x = y->esq;
    y->esq = x->dir;
    x->dir = y;
    return x;
}

static NoSplay* rotacao_esquerda(NoSplay *x) {
    NoSplay *y = x->dir;
    x->dir = y->esq;
    y->esq = x;
    return y;
}

static NoSplay* splay_topdown(NoSplay *raiz, int codigo,
                              long long *comparacoes, long long *rotacoes) {
    NoSplay sentinela;
    sentinela.esq = NULL;
    sentinela.dir = NULL;
    NoSplay *esq_max = &sentinela;
    NoSplay *dir_min = &sentinela;

    while (1) {
        (*comparacoes)++;

        if (codigo < raiz->pedido.codigo) {
            if (raiz->esq == NULL)
                break;
            if (codigo < raiz->esq->pedido.codigo) {
                (*comparacoes)++;
                raiz = rotacao_direita(raiz);
                (*rotacoes)++;
                if (raiz->esq == NULL)
                    break;
            }
            dir_min->esq = raiz;
            dir_min = raiz;
            raiz = raiz->esq;
            (*rotacoes)++;
        } else if (codigo > raiz->pedido.codigo) {
            if (raiz->dir == NULL)
                break;
            if (codigo > raiz->dir->pedido.codigo) {
                (*comparacoes)++;
                raiz = rotacao_esquerda(raiz);
                (*rotacoes)++;
                if (raiz->dir == NULL)
                    break;
            }
            esq_max->dir = raiz;
            esq_max = raiz;
            raiz = raiz->dir;
            (*rotacoes)++;
        } else
            break;
    }

    esq_max->dir = raiz->esq;
    dir_min->esq = raiz->dir;
    raiz->esq = sentinela.dir;
    raiz->dir = sentinela.esq;
    return raiz;
}

void splay_inicializar(ArvoreSplay *a, MetricasArvore *met) {
    a->raiz = NULL;
    a->met = met;
}

int splay_inserir(ArvoreSplay *a, const Pedido *p) {
    long long comparacoes_ignoradas = 0;

    if (a->raiz != NULL) {
        a->raiz = splay_topdown(a->raiz, p->codigo, &comparacoes_ignoradas, &a->met->rotacoes);
        if (a->raiz->pedido.codigo == p->codigo)
            return 0;
    }

    NoSplay *novo = malloc(sizeof(NoSplay));
    novo->pedido = *p;
    novo->esq = NULL;
    novo->dir = NULL;

    if (a->raiz != NULL) {
        if (p->codigo < a->raiz->pedido.codigo) {
            novo->esq = a->raiz->esq;
            novo->dir = a->raiz;
            a->raiz->esq = NULL;
        } else {
            novo->dir = a->raiz->dir;
            novo->esq = a->raiz;
            a->raiz->dir = NULL;
        }
    }
    a->raiz = novo;
    a->met->nos_inseridos++;
    return 1;
}

const Pedido* splay_buscar(ArvoreSplay *a, int codigo) {
    Timer t;
    timer_iniciar(&t);

    long long comparacoes = 0;
    long long rotacoes = 0;
    const Pedido *achou = NULL;

    a->raiz = splay_topdown(a->raiz, codigo, &comparacoes, &rotacoes);
    if (a->raiz->pedido.codigo == codigo)
        achou = &a->raiz->pedido;

    a->met->buscas++;
    a->met->comparacoes += comparacoes;
    a->met->rotacoes_busca += rotacoes;
    a->met->tempo_busca_s += timer_parar(&t);
    return achou;
}

typedef struct {
    const NoSplay *no;
    int nivel;
} NoENivel;

int splay_calcular_altura(ArvoreSplay *a) {
    NoENivel *pilha = malloc((size_t) (a->met->nos_inseridos + 1) * sizeof(NoENivel));

    int altura = 0;
    long long topo = 0;
    pilha[topo].no = a->raiz;
    pilha[topo].nivel = 1;
    topo++;

    while (topo > 0) {
        topo--;
        const NoSplay *no = pilha[topo].no;
        int nivel = pilha[topo].nivel;
        if (nivel > altura)
            altura = nivel;
        if (no->esq != NULL) {
            pilha[topo].no = no->esq;
            pilha[topo].nivel = nivel + 1;
            topo++;
        }
        if (no->dir != NULL) {
            pilha[topo].no = no->dir;
            pilha[topo].nivel = nivel + 1;
            topo++;
        }
    }

    free(pilha);
    a->met->altura = altura;
    return altura;
}

void splay_destruir(ArvoreSplay *a) {
    NoSplay *no = a->raiz;
    while (no != NULL) {
        if (no->esq != NULL)
            no = rotacao_direita(no);
        else {
            NoSplay *proximo = no->dir;
            free(no);
            no = proximo;
        }
    }
    a->raiz = NULL;
}