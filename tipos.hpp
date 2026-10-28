#ifndef TIPOS_HPP
#define TIPOS_HPP

/* Limites definidos na especificacao (Secao 4.1 e Secao 6) */
#define TAM_MIN_RECEITA 5
#define TAM_MAX_RECEITA 7
#define TAM_MAX_NOME    20

/* Simbolos dos ingredientes (Tabela 1) */
/* Sem preparo, quantidade infinita */
#define ING_PAO_CIMA      'z'
#define ING_KETCHUP       'k'
#define ING_MOSTARDA      'm'
#define ING_PICLES        'p'
#define ING_ALFACE        'a'

/* Com preparo, perenes */
#define ING_QUEIJO_SUICO   's'
#define ING_QUEIJO_CHEDDAR 'q'
#define ING_CEBOLA         'c'
#define ING_TOMATE         't'

/* Com preparo, pereciveis (heaps) */
#define ING_CARNE         'n'
#define ING_OVO_FRITO     'o'
#define ING_BACON         'b'

/* Categorias de ingredientes (Secao 4.7) */
enum CategoriaIngrediente {
    CAT_INVALIDO = 0,
    CAT_INFINITO,   /* z, k, m, p, a */
    CAT_PERENE,     /* s, q, c, t    */
    CAT_PERECIVEL   /* n, o, b       */
};

/* Registo de pedido (Secao 4.1 e Secao 6) */
struct Pedido {
    int codigo;
    char receita[TAM_MAX_RECEITA + 1];
};

/* Registo de historico de partidas (Secao 4.2 e Secao 6) */
struct EntradaHistorico {
    char nome[TAM_MAX_NOME + 1];
    int pontuacao;
};

#endif