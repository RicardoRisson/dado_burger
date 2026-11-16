#ifndef TIMER_H
#define TIMER_H
#include <time.h>

typedef struct { clock_t inicio; } Timer;
void   timer_iniciar(Timer *t);
double timer_parar(Timer *t);
#endif