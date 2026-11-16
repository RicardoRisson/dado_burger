#include "heap.h"

void heapify(int a[], int n, int i){
    int menor = i;
    int esq   = 2*i + 1;
    int dir   = 2*i + 2;

    if (esq < n && a[esq] < a[menor]){
        menor = esq;
    }
    if (dir < n && a[dir] < a[menor]){
        menor = dir;
    }

    if (menor != i){
        int tmp = a[i];
        a[i] = a[menor];
        a[menor] = tmp;
        heapify(a, n, menor);
    }
}

void siftUp(int a[], int i){
    while (i > 0){
        int pai = (i - 1) / 2;
        if (a[i] < a[pai]){
            int tmp = a[i];
            a[i] = a[pai];
            a[pai] = tmp;
            i = pai;
        }else{
            break;
        }
    }
}

void fpInserir(Heap *h, int valor){
    h->dados[h->tam] = valor;
    siftUp(h->dados, h->tam);
    h->tam++;
}

int fpMinimo(Heap *h){
    return h->dados[0];
}

int fpRemoverMinimo(Heap *h){
    int min = h->dados[0];
    h->tam--;
    h->dados[0] = h->dados[h->tam];
    heapify(h->dados, h->tam, 0);
    return min;
}