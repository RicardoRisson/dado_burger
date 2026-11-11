#include "heap.hpp"
#include <stdlib.h>

static void heap_swap(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

static void siftUp(Heap *h, int i) { // "sobe"
    while (i > 0) {
        int pai = (i - 1) / 2;
        if (h->dados[pai] <= h->dados[i]) break;
        heap_swap(&h->dados[pai], &h->dados[i]);
        i = pai;
    }
}

static void heapify(Heap *h, int i) { // "desce"
    int menor = i;
    int esq = 2 * i + 1;
    int dir = 2 * i + 2;

    if (esq < h->tamanho && h->dados[esq] < h->dados[menor])
        menor = esq;

    if (dir < h->tamanho && h->dados[dir] < h->dados[menor])
        menor = dir;

    if (menor != i) {
        heap_swap(&h->dados[i], &h->dados[menor]);
        heapify(h, menor);
    }
}

void heap_inicializar(Heap *h, int capacidade_inicial) {
    h->dados = (int*) malloc(capacidade_inicial * sizeof(int));
    h->tamanho = 0;
    h->capacidade = (h->dados != NULL) ? capacidade_inicial : 0;
}

int heap_inserir(Heap *h, int turno_validade) {
    if (h->tamanho == h->capacidade) {
        int nova_cap = (h->capacidade == 0) ? 4 : h->capacidade * 2;
        int *novo = (int*) realloc(h->dados, nova_cap * sizeof(int));
        if (novo == NULL) return 0;
        h->dados = novo;
        h->capacidade = nova_cap;
    }

    h->dados[h->tamanho] = turno_validade;
    siftUp(h, h->tamanho);
    h->tamanho++;
    return 1;
}

int heap_minimo(const Heap *h) {
    return h->dados[0];
}

void heap_remover_minimo(Heap *h) {
    if (h->tamanho == 0) return;

    h->dados[0] = h->dados[h->tamanho - 1];
    h->tamanho--;

    if (h->tamanho > 0)
        heapify(h, 0);
}

int heap_tamanho(const Heap *h) {
    return h->tamanho;
}

int heap_vazio(const Heap *h) {
    return h->tamanho == 0;
}

void heap_destruir(Heap *h) {
    free(h->dados);
    h->dados = NULL;
    h->tamanho = 0;
    h->capacidade = 0;
}