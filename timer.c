#include "timer.h"
void timer_iniciar(Timer *t) { t->inicio = clock(); }
double timer_parar(Timer *t) {
    return (double)(clock() - t->inicio) / CLOCKS_PER_SEC;
}