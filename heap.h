#ifndef HEAP_H
#define HEAP_H

typedef struct {
    int *dados;
    int tamanho;
    int capacidade;
} Heap;

void heap_inicializar(Heap *h, int capacidade_inicial);

void heap_inserir(Heap *h, int turno_validade);

int heap_minimo(const Heap *h);

void heap_remover_minimo(Heap *h);

int heap_tamanho(const Heap *h);
int heap_vazio(const Heap *h);

void heap_destruir(Heap *h);

#endif
