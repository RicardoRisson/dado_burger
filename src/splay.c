#include "../include/splay.h"
#include "../include/timer.h"
#include <stdlib.h>

// rotates tree branch right
static NoSplay* rotacao_direita(NoSplay *y) {
    NoSplay *x = y->esq;
    // moves inner branch between nodes
    y->esq = x->dir;
    x->dir = y;
    return x;
}

// rotates tree branch left
static NoSplay* rotacao_esquerda(NoSplay *x) {
    NoSplay *y = x->dir;
    // moves inner branch between nodes
    x->dir = y->esq;
    y->esq = x;
    return y;
}

// moves matching node closer to the root
static NoSplay* splay_topdown(NoSplay *raiz, int codigo,
                              long long *comparacoes, long long *rotacoes) {
    // sets up helper pointers for splitting tree
    NoSplay sentinela;
    sentinela.esq = NULL;
    sentinela.dir = NULL;
    NoSplay *esq_max = &sentinela;
    NoSplay *dir_min = &sentinela;

    while (1) {
        (*comparacoes)++;

        // searches left side and rotates if needed
        if (codigo < raiz->pedido.codigo) {
            if (raiz->esq == NULL)
                break;
            // rotates right when target is further left
            if (codigo < raiz->esq->pedido.codigo) {
                (*comparacoes)++;
                raiz = rotacao_direita(raiz);
                (*rotacoes)++;
                if (raiz->esq == NULL)
                    break;
            }
            // links current node to right subtree
            dir_min->esq = raiz;
            dir_min = raiz;
            raiz = raiz->esq;
            (*rotacoes)++;
        // searches right side and rotates if needed
        } else if (codigo > raiz->pedido.codigo) {
            if (raiz->dir == NULL)
                break;
            // rotates left when target is further right
            if (codigo > raiz->dir->pedido.codigo) {
                (*comparacoes)++;
                raiz = rotacao_esquerda(raiz);
                (*rotacoes)++;
                if (raiz->dir == NULL)
                    break;
            }
            // links current node to left subtree
            esq_max->dir = raiz;
            esq_max = raiz;
            raiz = raiz->dir;
            (*rotacoes)++;
        } else
            break;
    }

    // reassembles left and right subtrees around root
    esq_max->dir = raiz->esq;
    dir_min->esq = raiz->dir;
    raiz->esq = sentinela.dir;
    raiz->dir = sentinela.esq;
    return raiz;
}

// sets up an empty splay tree
void splay_inicializar(ArvoreSplay *a, MetricasArvore *met) {
    a->raiz = NULL;
    a->met = met;
}

// inserts a new order at the root
int splay_inserir(ArvoreSplay *a, const Pedido *p) {
    long long comparacoes_ignoradas = 0;

    // moves closest node to root and checks duplicates
    if (a->raiz != NULL) {
        a->raiz = splay_topdown(a->raiz, p->codigo, &comparacoes_ignoradas, &a->met->rotacoes);
        if (a->raiz->pedido.codigo == p->codigo)
            return 0;
    }

    // creates and fills the new node
    NoSplay *novo = malloc(sizeof(NoSplay));
    novo->pedido = *p;
    novo->esq = NULL;
    novo->dir = NULL;

    // attaches old tree parts under new root
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

// searches for an order by its code
const Pedido* splay_buscar(ArvoreSplay *a, int codigo) {
    Timer t;
    timer_iniciar(&t); // starts search timer

    long long comparacoes = 0;
    long long rotacoes = 0;
    const Pedido *achou = NULL;

    // brings searched node to the root
    a->raiz = splay_topdown(a->raiz, codigo, &comparacoes, &rotacoes);
    if (a->raiz->pedido.codigo == codigo)
        achou = &a->raiz->pedido;

    // records search metrics and elapsed time
    a->met->buscas++;
    a->met->comparacoes += comparacoes;
    a->met->rotacoes_busca += rotacoes;
    a->met->tempo_busca_s += timer_parar(&t);
    return achou;
}

// stores a node and its depth level
typedef struct {
    const NoSplay *no;
    int nivel;
} NoENivel;

// finds maximum height using a stack
int splay_calcular_altura(ArvoreSplay *a) {
    // allocates stack to walk through all nodes
    NoENivel *pilha = malloc((size_t) (a->met->nos_inseridos + 1) * sizeof(NoENivel));

    // pushes root node onto the stack
    int altura = 0;
    long long topo = 0;
    pilha[topo].no = a->raiz;
    pilha[topo].nivel = 1;
    topo++;

    // pops each node and checks child depths
    while (topo > 0) {
        topo--;
        const NoSplay *no = pilha[topo].no;
        int nivel = pilha[topo].nivel;
        // updates max height if current level is deeper
        if (nivel > altura)
            altura = nivel;
        // pushes left child onto stack
        if (no->esq != NULL) {
            pilha[topo].no = no->esq;
            pilha[topo].nivel = nivel + 1;
            topo++;
        }
        // pushes right child onto stack
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

// frees all nodes without using recursion
void splay_destruir(ArvoreSplay *a) {
    NoSplay *no = a->raiz;
    // flattens left branches before freeing each node
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