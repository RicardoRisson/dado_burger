#ifndef CONFIG_H
#define CONFIG_H

// partida
#define CFG_NUM_CLIENTES      6
#define CFG_PACIENCIA_MAXIMA  15

// validade (em turnos) de cada perecivel
#define CFG_VALIDADE_CARNE    3
#define CFG_VALIDADE_OVO      2
#define CFG_VALIDADE_BACON    3

// 1 item novo por turno e validade maxima de 3 turnos, entao no maximo 3 vivos no heap
#define CFG_HEAP_CAP          3

// primo logo acima dos 6 clientes de uma partida, a hash e recriada a cada partida
#define CFG_HASH_M            7

#define CFG_COMPARAR_SPLAY    1

#define CFG_ARQUIVO_LOG       "medicoes.log"

// receita e nome do historico
#define TAM_MAX_RECEITA 7
#define TAM_MAX_NOME    20

// simbolo de cada ingrediente
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

// ingredientes de cada categoria, a posicao na string e o indice no estoque
#define PERENES    "sqct"
#define PERECIVEIS "nob"
#define INFINITOS  "zkmpa"

#define CAT_INVALIDO  0
#define CAT_INFINITO  1
#define CAT_PERENE    2
#define CAT_PERECIVEL 3

#endif