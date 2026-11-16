#ifndef FILA_H
#define FILA_H

#include "tipos.h"

typedef struct {
    int codigo;
    char receita[8];
    int paciencia;
} Cliente;


typedef struct NoFila NoFila;

struct NoFila {
    NoFila *prox;
    Cliente dado;
};

typedef struct {
    NoFila *frente;
    NoFila *final;
    int tamanho;
} Fila;

void fila_inicializar(Fila *f);

int fila_vazia(const Fila *f);

void fila_entrar(Fila *f, const Cliente *c);

void fila_sair(Fila *f);

Cliente* fila_frente(Fila *f);

void fila_destruir(Fila *f);

#endif