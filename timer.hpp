#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>

struct Timer {
    std::chrono::steady_clock::time_point ini;
};

void timer_iniciar(Timer *t);
double timer_parar(Timer *t); 

#endif