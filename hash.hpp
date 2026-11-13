#ifndef HASH_HPP
#define HASH_HPP

#include "tipos.hpp"
#include "metricas.hpp"

// hash table used as a cache of orders, the key is the order code
// first we ask the cache, if the order is there (hit) we do not search the avl
// if it is not there (miss) we search the avl and save the order here
// function: index = code mod m (m is a prime number)
// collisions: separate chaining (each slot has a linked list)

// one entry of a chain, it keeps a copy of the order
struct NoHash {
    Pedido  pedido;
    NoHash *prox;      // next entry in the same slot
};

struct TabelaHash {
    NoHash       **slots;   // array with m list heads (NULL = empty slot)
    int            m;       // number of slots
    MetricasHash  *met;     // where the counters are written
};

// starts an empty table with m slots and zeros the counters
// returns 1 if ok, 0 if m is not valid or there is no memory
int hash_inicializar(TabelaHash *t, int m, MetricasHash *met);

// looks for an order in the cache
// returns 1 and copies the order to pedido_out if found (counts a hit)
// returns 0 if not found (counts a miss)
int hash_buscar(TabelaHash *t, int codigo, Pedido *pedido_out);

// saves a copy of the order in the cache
// if the slot already had entries it counts one collision
// returns 1 if saved, 0 if the code was already there, -1 if there is no memory
int hash_inserir(TabelaHash *t, const Pedido *p);

// returns the load factor (entries divided by slots)
double hash_fator_de_carga(const TabelaHash *t);

// frees all the entries and the array of slots
void hash_destruir(TabelaHash *t);

#endif
