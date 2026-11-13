#include "avl.hpp"
#include "timer.hpp"
#include <stdlib.h>

// returns the bigger of two numbers
static int maximo(int a, int b) {
    return a > b ? a : b;
}

// returns the height of a node (an empty place has height 0)
static int altura_no(const NoAVL *n) {
    return n ? n->altura : 0;
}

// balance factor = height of the left side minus height of the right side
// the tree is balanced when it is -1, 0 or 1
static int fator_balanceamento(const NoAVL *n) {
    return n ? altura_no(n->esq) - altura_no(n->dir) : 0;
}

// calculates the height of a node again from its two children
static void atualizar_altura(NoAVL *n) {
    if (n) {
        n->altura = 1 + maximo(altura_no(n->esq), altura_no(n->dir));
    }
}

// creates a new leaf with a copy of the order
static NoAVL* criar_no(const Pedido *p) {
    NoAVL *n = (NoAVL*) malloc(sizeof(NoAVL));
    if (n == NULL) {
        return NULL;                 // no memory
    }
    n->pedido = *p;                  // stores the order in the node
    n->altura = 1;                   // a new leaf has height 1
    n->esq = NULL;
    n->dir = NULL;
    return n;
}

// right rotation, used when the left side is too heavy
// the left child goes up and the node goes down to the right
static NoAVL* rotacao_direita(NoAVL *y, MetricasArvore *met) {
    NoAVL *x = y->esq;
    y->esq = x->dir;                 // the right child of x goes to the left of y
    x->dir = y;                      // y goes down to the right of x
    atualizar_altura(y);             // y first, because it is lower now
    atualizar_altura(x);
    met->rotacoes++;                 // counts one rotation
    return x;                        // x is the new top
}

// left rotation, used when the right side is too heavy
// it is the mirror of the right rotation
static NoAVL* rotacao_esquerda(NoAVL *x, MetricasArvore *met) {
    NoAVL *y = x->dir;
    x->dir = y->esq;
    y->esq = x;
    atualizar_altura(x);
    atualizar_altura(y);
    met->rotacoes++;
    return y;
}

// checks a node after an insertion and fixes it if it is not balanced
// the 4 cases are LL, LR, RR and RL
// lr and rl need two rotations, so they count 2
static NoAVL* reequilibrar(NoAVL *no, MetricasArvore *met) {
    atualizar_altura(no);
    int fb = fator_balanceamento(no);

    if (fb > 1) {                                   // left side too heavy
        if (fator_balanceamento(no->esq) < 0) {     // case lr
            no->esq = rotacao_esquerda(no->esq, met);
        }
        return rotacao_direita(no, met);            // case ll
    }

    if (fb < -1) {                                  // right side too heavy
        if (fator_balanceamento(no->dir) > 0) {     // case rl
            no->dir = rotacao_direita(no->dir, met);
        }
        return rotacao_esquerda(no, met);           // case rr
    }

    return no;                                      // already balanced
}

// inserts going down like a normal search tree, then fixes the nodes on the way back
// resultado receives 1 (inserted), 0 (repeated code) or -1 (no memory)
static NoAVL* inserir_rec(NoAVL *no, const Pedido *p, MetricasArvore *met, int *resultado) {
    if (no == NULL) {                               // found the empty place
        NoAVL *novo = criar_no(p);
        *resultado = (novo != NULL) ? 1 : -1;
        return novo;
    }

    if (p->codigo < no->pedido.codigo) {
        no->esq = inserir_rec(no->esq, p, met, resultado);   // smaller goes left
    } else if (p->codigo > no->pedido.codigo) {
        no->dir = inserir_rec(no->dir, p, met, resultado);   // bigger goes right
    } else {
        *resultado = 0;                             // same code, does nothing
        return no;
    }

    if (*resultado != 1) {
        return no;                                  // nothing was added, nothing to fix
    }
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
        a->met->nos_inseridos++;                    // counts the new node
        a->met->altura = altura_no(a->raiz);        // keeps the height updated
    }
    return resultado;
}

const Pedido* avl_buscar(ArvoreAVL *a, int codigo) {
    Timer t;
    timer_iniciar(&t);                              // starts the clock

    const NoAVL *no = a->raiz;
    const Pedido *achou = NULL;
    long long comparacoes = 0;

    while (no != NULL) {
        comparacoes++;                              // one visited node = one comparison
        if (codigo == no->pedido.codigo) {
            achou = &no->pedido;                    // found it
            break;
        }
        no = (codigo < no->pedido.codigo) ? no->esq : no->dir;   // goes left or right
    }

    a->met->buscas++;                               // adds this search to the totals
    a->met->comparacoes += comparacoes;
    a->met->tempo_busca_s += timer_parar(&t);       // stops the clock and adds the time
    return achou;
}

int avl_altura(const ArvoreAVL *a) {
    return altura_no(a->raiz);
}

// frees a part of the tree (recursion is safe because the avl is always low)
static void destruir_rec(NoAVL *no) {
    if (no == NULL) {
        return;
    }
    destruir_rec(no->esq);
    destruir_rec(no->dir);
    free(no);
}

void avl_destruir(ArvoreAVL *a) {
    destruir_rec(a->raiz);
    a->raiz = NULL;
}
