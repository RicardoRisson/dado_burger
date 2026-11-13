#include "splay.hpp"
#include "timer.hpp"
#include <stdlib.h>

// right rotation, the left child goes up
static NoSplay* rotacao_direita(NoSplay *y) {
    NoSplay *x = y->esq;
    y->esq = x->dir;
    x->dir = y;
    return x;
}

// left rotation, the right child goes up
static NoSplay* rotacao_esquerda(NoSplay *x) {
    NoSplay *y = x->dir;
    x->dir = y->esq;
    y->esq = x;
    return y;
}

// moves the node with this code to the root (top-down version)
// if the code does not exist, the last node visited goes to the root
// we go down from the root, cutting nodes into a left tree (smaller codes)
// and a right tree (bigger codes), then we glue both trees to the node we reached
// comparacoes counts one for each node on the path
// rotacoes counts how many levels the node went up (zig-zig counts 2, a simple step counts 1)
static NoSplay* splay_topdown(NoSplay *raiz, int codigo,
                              long long *comparacoes, long long *rotacoes) {
    if (raiz == NULL) {
        return NULL;
    }

    NoSplay sentinela;               // fake node that holds the start of both trees
    sentinela.esq = NULL;
    sentinela.dir = NULL;
    NoSplay *esq_max = &sentinela;   // last node of the left tree
    NoSplay *dir_min = &sentinela;   // last node of the right tree

    while (true) {
        (*comparacoes)++;                            // looks at the current node

        if (codigo < raiz->pedido.codigo) {
            if (raiz->esq == NULL) {
                break;                               // nowhere to go, stops
            }
            if (codigo < raiz->esq->pedido.codigo) { // zig-zig to the left
                (*comparacoes)++;                    // the child was looked at too
                raiz = rotacao_direita(raiz);
                (*rotacoes)++;
                if (raiz->esq == NULL) {
                    break;
                }
            }
            dir_min->esq = raiz;                     // cuts raiz and puts it in the right tree
            dir_min = raiz;
            raiz = raiz->esq;
            (*rotacoes)++;
        } else if (codigo > raiz->pedido.codigo) {
            if (raiz->dir == NULL) {
                break;
            }
            if (codigo > raiz->dir->pedido.codigo) { // zig-zig to the right
                (*comparacoes)++;
                raiz = rotacao_esquerda(raiz);
                (*rotacoes)++;
                if (raiz->dir == NULL) {
                    break;
                }
            }
            esq_max->dir = raiz;                     // cuts raiz and puts it in the left tree
            esq_max = raiz;
            raiz = raiz->dir;
            (*rotacoes)++;
        } else {
            break;                                   // found it
        }
    }

    // glues everything: the node gets the left tree and the right tree as children
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
    long long comparacoes_ignoradas = 0;             // we do not report comparisons of insertion

    if (a->raiz != NULL) {
        a->raiz = splay_topdown(a->raiz, p->codigo, &comparacoes_ignoradas, &a->met->rotacoes);
        if (a->raiz->pedido.codigo == p->codigo) {
            return 0;                                // code already exists
        }
    }

    NoSplay *novo = (NoSplay*) malloc(sizeof(NoSplay));
    if (novo == NULL) {
        return -1;                                   // no memory
    }
    novo->pedido = *p;                               // stores the order in the new node
    novo->esq = NULL;
    novo->dir = NULL;

    if (a->raiz != NULL) {
        // the root is now the closest code to the new one
        // the new node becomes the root and the old root goes to one side
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
    timer_iniciar(&t);                               // the time includes the moving to the root

    long long comparacoes = 0;
    long long rotacoes = 0;
    const Pedido *achou = NULL;

    a->raiz = splay_topdown(a->raiz, codigo, &comparacoes, &rotacoes);
    if (a->raiz != NULL && a->raiz->pedido.codigo == codigo) {
        achou = &a->raiz->pedido;                    // the root is the node we wanted
    }

    a->met->buscas++;                                // adds this search to the totals
    a->met->comparacoes += comparacoes;
    a->met->rotacoes_busca += rotacoes;
    a->met->tempo_busca_s += timer_parar(&t);
    return achou;
}

// a node and its level, used by the height loop
struct NoENivel {
    const NoSplay *no;
    int nivel;
};

int splay_calcular_altura(ArvoreSplay *a) {
    if (a->raiz == NULL) {
        a->met->altura = 0;
        return 0;
    }

    // we keep our own stack in memory instead of using recursion
    long long capacidade = a->met->nos_inseridos + 1;
    NoENivel *pilha = (NoENivel*) malloc((size_t) capacidade * sizeof(NoENivel));
    if (pilha == NULL) {
        return -1;
    }

    int altura = 0;
    long long topo = 0;
    pilha[topo].no = a->raiz;                        // starts with the root at level 1
    pilha[topo].nivel = 1;
    topo++;

    while (topo > 0) {
        topo--;                                      // takes one node from the stack
        const NoSplay *no = pilha[topo].no;
        int nivel = pilha[topo].nivel;
        if (nivel > altura) {
            altura = nivel;                          // deepest level seen so far
        }
        if (no->esq != NULL && topo < capacidade) {  // puts the children in the stack
            pilha[topo].no = no->esq;
            pilha[topo].nivel = nivel + 1;
            topo++;
        }
        if (no->dir != NULL && topo < capacidade) {
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
    // frees without recursion: while the root has a left child, rotate it to the right
    // when there is no left child, free the root and the right child becomes the root
    NoSplay *no = a->raiz;
    while (no != NULL) {
        if (no->esq != NULL) {
            no = rotacao_direita(no);
        } else {
            NoSplay *proximo = no->dir;
            free(no);
            no = proximo;
        }
    }
    a->raiz = NULL;
}
