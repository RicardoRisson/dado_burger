#include "timer.hpp"

void timer_iniciar(Timer *t) {
    t->ini = std::chrono::steady_clock::now();
}

double timer_parar(Timer *t) {
    auto fim = std::chrono::steady_clock::now();
    return std::chrono::duration<double>(fim - t->ini).count();
}