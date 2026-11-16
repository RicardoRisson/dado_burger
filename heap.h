#ifndef HEAP_H
#define HEAP_H

#include "config.h"

typedef struct {
    int dados[CFG_HEAP_CAP];
    int tam;
} Heap;

void heapify(int a[], int n, int i);
void siftUp(int a[], int i);

void fpInserir(Heap *h, int valor);
int  fpMaximo(Heap *h);
int  fpRemoverMaximo(Heap *h);

#endif