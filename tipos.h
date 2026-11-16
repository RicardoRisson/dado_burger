#ifndef TIPOS_H
#define TIPOS_H

#define TAM_MIN_RECEITA 5
#define TAM_MAX_RECEITA 7
#define TAM_MAX_NOME    20

#define ING_PAO_CIMA      'z'
#define ING_KETCHUP       'k'
#define ING_MOSTARDA      'm'
#define ING_PICLES        'p'
#define ING_ALFACE        'a'
#define ING_QUEIJO_SUICO   's'
#define ING_QUEIJO_CHEDDAR 'q'
#define ING_CEBOLA         'c'
#define ING_TOMATE         't'
#define ING_CARNE         'n'
#define ING_OVO_FRITO     'o'
#define ING_BACON         'b'

#define CAT_INVALIDO  0
#define CAT_INFINITO  1
#define CAT_PERENE    2
#define CAT_PERECIVEL 3

typedef struct {
    int codigo;
    char receita[TAM_MAX_RECEITA + 1];
} Pedido;

typedef struct {
    char nome[TAM_MAX_NOME + 1];
    int pontuacao;
} EntradaHistorico;

#endif