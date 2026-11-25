#ifndef TIPOS_H
#define TIPOS_H

#include "config.h"

typedef struct {
    int codigo;
    char receita[TAM_MAX_RECEITA + 1];
} Pedido;

typedef struct {
    char nome[TAM_MAX_NOME + 1];
    int pontuacao;
} EntradaHistorico;

#endif