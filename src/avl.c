#include "../include/avl.h"
#include "../include/timer.h"
#include <stdlib.h>

// returns the larger of two numbers
static int maximo(int a, int b) {
    if (a > b)
        return a;
    return b;
}

// gets the height of a node (leaf = 0, NULL = -1, as in class)
static int altura_no(const NoAVL *n) {
    if (n == NULL)
        return -1;
    return n->altura;
}

// calculates height difference between left and right sides
static int fator_balanceamento(const NoAVL *n) {
    return altura_no(n->esq) - altura_no(n->dir);
}

// updates node height based on taller child
static void atualizar_altura(NoAVL *n) {
    n->altura = 1 + maximo(altura_no(n->esq), altura_no(n->dir));
}

// allocates and sets up a new node
static NoAVL* criar_no(const Pedido *p) {
    NoAVL *n = malloc(sizeof(NoAVL));
    n->pedido = *p;
    n->altura = 0; // a new node is a leaf
    n->esq = NULL;
    n->dir = NULL;
    return n;
}

// rotates tree branch right to fix balance
static NoAVL* rotacao_direita(NoAVL *y, MetricasArvore *met) {
    NoAVL *x = y->esq;
    // moves inner branch between nodes
    y->esq = x->dir;
    x->dir = y;
    atualizar_altura(y);
    atualizar_altura(x);
    met->rotacoes++; // counts the rotation
    return x;
}

// rotates tree branch left to fix balance
static NoAVL* rotacao_esquerda(NoAVL *x, MetricasArvore *met) {
    NoAVL *y = x->dir;
    // moves inner branch between nodes
    x->dir = y->esq;
    y->esq = x;
    atualizar_altura(x);
    atualizar_altura(y);
    met->rotacoes++; // counts the rotation
    return y;
}

// keeps the node balanced after changes
static NoAVL* reequilibrar(NoAVL *no, MetricasArvore *met) {
    atualizar_altura(no);
    int fb = fator_balanceamento(no);

    // fixes imbalance when left side is taller
    if (fb > 1) {
        // aligns inner branch before rotating right
        if (fator_balanceamento(no->esq) < 0)
            no->esq = rotacao_esquerda(no->esq, met);
        return rotacao_direita(no, met);
    }

    // fixes imbalance when right side is taller
    if (fb < -1) {
        // aligns inner branch before rotating left
        if (fator_balanceamento(no->dir) > 0)
            no->dir = rotacao_direita(no->dir, met);
        return rotacao_esquerda(no, met);
    }

    return no;
}

// inserts a new order into the tree
static NoAVL* inserir_rec(NoAVL *no, const Pedido *p, MetricasArvore *met, int *resultado) {
    // creates new node when empty spot is found
    if (no == NULL) {
        *resultado = 1;
        return criar_no(p);
    }

    // moves left or right based on order code
    if (p->codigo < no->pedido.codigo)
        no->esq = inserir_rec(no->esq, p, met, resultado);
    else if (p->codigo > no->pedido.codigo)
        no->dir = inserir_rec(no->dir, p, met, resultado);
    else {
        // ignores duplicate order codes
        *resultado = 0;
        return no;
    }

    if (*resultado != 1)
        return no;
    return reequilibrar(no, met);
}

// sets up an empty tree and its metrics
void avl_inicializar(ArvoreAVL *a, MetricasArvore *met) {
    a->raiz = NULL;
    a->met = met;
}

// adds an order and updates tree stats
int avl_inserir(ArvoreAVL *a, const Pedido *p) {
    int resultado = 0;
    a->raiz = inserir_rec(a->raiz, p, a->met, &resultado);

    // records new node count and tree height
    if (resultado == 1) {
        a->met->nos_inseridos++;
        a->met->altura = altura_no(a->raiz) + 1; // reported in levels (empty = 0, leaf = 1)
    }
    return resultado;
}

// searches for an order by its code
const Pedido* avl_buscar(ArvoreAVL *a, int codigo) {
    Timer t;
    timer_iniciar(&t); // starts search timer

    const NoAVL *no = a->raiz;
    const Pedido *achou = NULL;
    long long comparacoes = 0;

    // walks down the tree until code is found
    while (no != NULL) {
        comparacoes++;
        if (codigo == no->pedido.codigo) {
            achou = &no->pedido;
            break;
        }
        // chooses left or right branch
        if (codigo < no->pedido.codigo)
            no = no->esq;
        else
            no = no->dir;
    }

    // saves search metrics and elapsed time
    a->met->buscas++;
    a->met->comparacoes += comparacoes;
    a->met->tempo_busca_s += timer_parar(&t);
    return achou;
}

// returns the smallest order code (leftmost node)
int avl_codigo_menor(const ArvoreAVL *a) {
    const NoAVL *no = a->raiz;
    if (no == NULL)
        return 0;
    while (no->esq != NULL)
        no = no->esq;
    return no->pedido.codigo;
}

// returns the biggest order code (rightmost node)
int avl_codigo_maior(const ArvoreAVL *a) {
    const NoAVL *no = a->raiz;
    if (no == NULL)
        return 0;
    while (no->dir != NULL)
        no = no->dir;
    return no->pedido.codigo;
}

// returns total height of the tree in levels (empty = 0, only root = 1)
int avl_altura(const ArvoreAVL *a) {
    return altura_no(a->raiz) + 1;
}

// frees all nodes from bottom to top
static void destruir_rec(NoAVL *no) {
    if (no == NULL)
        return;
    destruir_rec(no->esq);
    destruir_rec(no->dir);
    free(no);
}

// clears the entire tree and resets root
void avl_destruir(ArvoreAVL *a) {
    destruir_rec(a->raiz);
    a->raiz = NULL;
}