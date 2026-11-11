
#ifndef FILA_HPP
#define FILA_HPP

#include "tipos.hpp"

// dados do cliente
typedef struct {
    int codigo;       // codigo do pedido
    char receita[8];  // texto da receita
    int paciencia;    // turnos restante
} Cliente;

// no da fila encadeada
typedef struct NoFila NoFila;

struct NoFila {
    NoFila *prox;
    Cliente dado;
};

// descritor da fila
typedef struct {
    NoFila *frente;
    NoFila *final;
    int tamanho;
} Fila;

void fila_inicializar(Fila *f);

int fila_vazia(const Fila *f);

int fila_entrar(Fila *f, const Cliente *c);

int fila_sair(Fila *f);

Cliente* fila_frente(Fila *f);

int fila_tamanho(const Fila *f);

void fila_destruir(Fila *f);

#endif