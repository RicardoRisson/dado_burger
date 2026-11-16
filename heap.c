#include "heap.h"

void heapify(int a[], int n, int i) {
    int maior = i;
    int esq   = 2*i + 1;
    int dir   = 2*i + 2;

    if (esq < n && a[esq] > a[maior]) maior = esq;
    if (dir < n && a[dir] > a[maior]) maior = dir;

    if (maior != i) {
        int tmp = a[i]; a[i] = a[maior]; a[maior] = tmp;
        heapify(a, n, maior);
    }
}

void siftUp(int a[], int i) {
    while (i > 0) {
        int pai = (i - 1) / 2;
        if (a[i] > a[pai]) {
            int tmp = a[i]; a[i] = a[pai]; a[pai] = tmp;
            i = pai;
        } else {
            break;
        }
    }
}

void fpInserir(Heap *h, int valor) {
    h->dados[h->tam] = valor;
    siftUp(h->dados, h->tam);
    h->tam++;
}

int fpMaximo(Heap *h) {
    return h->dados[0];
}

int fpRemoverMaximo(Heap *h) {
    int max = h->dados[0];
    h->tam--;
    h->dados[0] = h->dados[h->tam];
    heapify(h->dados, h->tam, 0);
    return max;
}