#include "../include/splay.h"
#include "../include/timer.h"
#include <stdlib.h>

// rotates tree branch right x goes up, y goes down
static NoSplay* splayRotDir(NoSplay *y, long long *rotacoes) {
    NoSplay *x = y->esq;
    y->esq = x->dir;
    x->dir = y;
    (*rotacoes)++; // counts the rotation
    return x;
}

// rotates tree branch left y goes up, x goes down
static NoSplay* splayRotEsq(NoSplay *x, long long *rotacoes) {
    NoSplay *y = x->dir;
    x->dir = y->esq;
    y->esq = x;
    (*rotacoes)++; // counts the rotation
    return y;
}

// splay bottom-up, as in class
static NoSplay* splay(NoSplay *raiz, int codigo, long long *comparacoes, long long *rotacoes) {
    if (raiz == NULL)
        return raiz;

    // goes down from the root saving the visited nodes
    size_t capacidade = 64;
    size_t n = 0;
    NoSplay **caminho = malloc(capacidade * sizeof(NoSplay*));
    NoSplay *no = raiz;
    while (1) {
        if (n == capacidade) {
            capacidade *= 2;
            caminho = realloc(caminho, capacidade * sizeof(NoSplay*));
        }
        caminho[n++] = no;
        (*comparacoes)++; // compares the code with the current node
        if (codigo == no->pedido.codigo)
            break;
        NoSplay *proximo;
        if (codigo < no->pedido.codigo)
            proximo = no->esq;
        else
            proximo = no->dir;
        if (proximo == NULL)
            break; // does not exist
        no = proximo;
    }

    // goes back up moving the last node two levels at a time
    NoSplay *x = caminho[n - 1];

    // if odd depth the single zig is done first, at the bottom, like in class
    if ((n - 1) % 2 == 1) {
        NoSplay *p = caminho[n - 2];
        if (p->esq == x)
            x = splayRotDir(p, rotacoes);
        else
            x = splayRotEsq(p, rotacoes);
        if (n >= 3) {
            NoSplay *g = caminho[n - 3];
            if (g->esq == p)
                g->esq = x;
            else
                g->dir = x;
        }
        caminho[n - 2] = x;
        n--;
    }

    while (n > 1) {
        NoSplay *p = caminho[n - 2];

        NoSplay *g = caminho[n - 3];
        NoSplay *sub;
        if (g->esq == p && p->esq == x) {
            // zig-zig left
            sub = splayRotDir(splayRotDir(g, rotacoes), rotacoes);
        } else if (g->dir == p && p->dir == x) {
            // zig-zig right
            sub = splayRotEsq(splayRotEsq(g, rotacoes), rotacoes);
        } else if (g->esq == p) {
            // zig-zag left-right
            g->esq = splayRotEsq(p, rotacoes);
            sub = splayRotDir(g, rotacoes);
        } else {
            // zig-zag right-left
            g->dir = splayRotDir(p, rotacoes);
            sub = splayRotEsq(g, rotacoes);
        }

        // hangs the new subtree where the grandparent was
        if (n >= 4) {
            NoSplay *gg = caminho[n - 4];
            if (gg->esq == g)
                gg->esq = sub;
            else
                gg->dir = sub;
        }
        caminho[n - 3] = sub;
        x = sub;
        n -= 2;
    }

    NoSplay *novaRaiz = caminho[0];
    free(caminho);
    return novaRaiz;
}

// sets up an empty splay tree
void splay_inicializar(ArvoreSplay *a, MetricasArvore *met) {
    a->raiz = NULL;
    a->met = met;
}

// inserts a new order at the root, returns 1 = inserted, 0 = code already exists
int splay_inserir(ArvoreSplay *a, const Pedido *p) {
    long long comparacoes_ignoradas = 0;

    // creates and fills the new node
    NoSplay *novo = malloc(sizeof(NoSplay));
    novo->pedido = *p;
    novo->esq = NULL;
    novo->dir = NULL;

    if (a->raiz == NULL) {
        a->raiz = novo;
        a->met->nos_inseridos++;
        return 1;
    }

    // brings the closest node to the root and checks duplicates
    a->raiz = splay(a->raiz, p->codigo, &comparacoes_ignoradas, &a->met->rotacoes);
    if (a->raiz->pedido.codigo == p->codigo) {
        free(novo);
        return 0;
    }

    // splits the old tree around the new root
    if (p->codigo < a->raiz->pedido.codigo) {
        novo->dir = a->raiz;
        novo->esq = a->raiz->esq;
        a->raiz->esq = NULL;
    } else {
        novo->esq = a->raiz;
        novo->dir = a->raiz->dir;
        a->raiz->dir = NULL;
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
    a->raiz = splay(a->raiz, codigo, &comparacoes, &rotacoes);
    if (a->raiz != NULL && a->raiz->pedido.codigo == codigo)
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

// calculates and returns the tree height in levels (empty = 0, only root = 1, same as the avl)
int splay_calcular_altura(ArvoreSplay *a) {
    int altura = -1;
    if (a->raiz != NULL) {
        NoENivel *pilha = malloc((size_t) (a->met->nos_inseridos + 1) * sizeof(NoENivel));
        long long topo = 0;
        pilha[topo].no = a->raiz;
        pilha[topo].nivel = 0;
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
    }
    a->met->altura = altura + 1; // levels, the loop above counts edges
    return altura + 1;
}

// frees all nodes without recursion
void splay_destruir(ArvoreSplay *a) {
    long long rotacoes_ignoradas = 0;
    NoSplay *no = a->raiz;
    while (no != NULL) {
        if (no->esq != NULL)
            no = splayRotDir(no, &rotacoes_ignoradas);
        else {
            NoSplay *proximo = no->dir;
            free(no);
            no = proximo;
        }
    }
    a->raiz = NULL;
}