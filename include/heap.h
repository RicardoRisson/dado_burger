#ifndef HEAP_H
#define HEAP_H

#include "config.h"

// min-heap em vetor, dados guarda o turno de validade e tam quantos tem
typedef struct {
    int dados[CFG_HEAP_CAP];
    int tam;
} Heap;

void heapify(int a[], int n, int i);
void siftUp(int a[], int i);

void fpInserir(Heap *h, int valor);
int  fpMinimo(Heap *h);
int  fpRemoverMinimo(Heap *h);

#endif