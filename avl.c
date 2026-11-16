#include "avl.h"
#include "timer.h"
#include <stdlib.h>

static int maximo(int a, int b) {
    if (a > b)
        return a;
    return b;
}

static int altura_no(const NoAVL *n) {
    if (n == NULL)
        return 0;
    return n->altura;
}

static int fator_balanceamento(const NoAVL *n) {
    return altura_no(n->esq) - altura_no(n->dir);
}

static void atualizar_altura(NoAVL *n) {
    n->altura = 1 + maximo(altura_no(n->esq), altura_no(n->dir));
}

static NoAVL* criar_no(const Pedido *p) {
    NoAVL *n = malloc(sizeof(NoAVL));
    n->pedido = *p;
    n->altura = 1;
    n->esq = NULL;
    n->dir = NULL;
    return n;
}

static NoAVL* rotacao_direita(NoAVL *y, MetricasArvore *met) {
    NoAVL *x = y->esq;
    y->esq = x->dir;
    x->dir = y;
    atualizar_altura(y);
    atualizar_altura(x);
    met->rotacoes++;
    return x;
}

static NoAVL* rotacao_esquerda(NoAVL *x, MetricasArvore *met) {
    NoAVL *y = x->dir;
    x->dir = y->esq;
    y->esq = x;
    atualizar_altura(x);
    atualizar_altura(y);
    met->rotacoes++;
    return y;
}

static NoAVL* reequilibrar(NoAVL *no, MetricasArvore *met) {
    atualizar_altura(no);
    int fb = fator_balanceamento(no);

    if (fb > 1) {
        if (fator_balanceamento(no->esq) < 0)
            no->esq = rotacao_esquerda(no->esq, met);
        return rotacao_direita(no, met);
    }

    if (fb < -1) {
        if (fator_balanceamento(no->dir) > 0)
            no->dir = rotacao_direita(no->dir, met);
        return rotacao_esquerda(no, met);
    }

    return no;
}

static NoAVL* inserir_rec(NoAVL *no, const Pedido *p, MetricasArvore *met, int *resultado) {
    if (no == NULL) {
        *resultado = 1;
        return criar_no(p);
    }

    if (p->codigo < no->pedido.codigo)
        no->esq = inserir_rec(no->esq, p, met, resultado);
    else if (p->codigo > no->pedido.codigo)
        no->dir = inserir_rec(no->dir, p, met, resultado);
    else {
        *resultado = 0;
        return no;
    }

    if (*resultado != 1)
        return no;
    return reequilibrar(no, met);
}

void avl_inicializar(ArvoreAVL *a, MetricasArvore *met) {
    a->raiz = NULL;
    a->met = met;
}

int avl_inserir(ArvoreAVL *a, const Pedido *p) {
    int resultado = 0;
    a->raiz = inserir_rec(a->raiz, p, a->met, &resultado);

    if (resultado == 1) {
        a->met->nos_inseridos++;
        a->met->altura = altura_no(a->raiz);
    }
    return resultado;
}

const Pedido* avl_buscar(ArvoreAVL *a, int codigo) {
    Timer t;
    timer_iniciar(&t);

    const NoAVL *no = a->raiz;
    const Pedido *achou = NULL;
    long long comparacoes = 0;

    while (no != NULL) {
        comparacoes++;
        if (codigo == no->pedido.codigo) {
            achou = &no->pedido;
            break;
        }
        if (codigo < no->pedido.codigo)
            no = no->esq;
        else
            no = no->dir;
    }

    a->met->buscas++;
    a->met->comparacoes += comparacoes;
    a->met->tempo_busca_s += timer_parar(&t);
    return achou;
}

int avl_altura(const ArvoreAVL *a) {
    return altura_no(a->raiz);
}

static void destruir_rec(NoAVL *no) {
    if (no == NULL)
        return;
    destruir_rec(no->esq);
    destruir_rec(no->dir);
    free(no);
}

void avl_destruir(ArvoreAVL *a) {
    destruir_rec(a->raiz);
    a->raiz = NULL;
}