#include "../include/heap.h"

// desce o no i ate o menor filho ser maior que ele
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

// sobe o no i ate o pai ser menor que ele
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

// poe no final e sobe
void fpInserir(Heap *h, int valor){
    h->dados[h->tam] = valor;
    siftUp(h->dados, h->tam);
    h->tam++;
}

// o menor fica sempre na raiz
int fpMinimo(Heap *h){
    return h->dados[0];
}

// tira a raiz, poe o ultimo no lugar e desce
int fpRemoverMinimo(Heap *h){
    int min = h->dados[0];
    h->tam--;
    h->dados[0] = h->dados[h->tam];
    heapify(h->dados, h->tam, 0);
    return min;
}