#ifndef METRICAS_H
#define METRICAS_H

// stores performance metrics for tree structures
typedef struct {
    // insertion count height rotations and timing
    long long nos_inseridos;
    int       altura;
    long long rotacoes;
    double    tempo_carga_s;

    // search count comparisons timing and rotations
    long long buscas;
    long long comparacoes;
    double    tempo_busca_s;
    long long rotacoes_busca;
} MetricasArvore;

// stores performance metrics for hash table
typedef struct {
    int       m; // table size
    int       n; // number of stored items
    // tracks found missed and colliding lookups
    long long hits;
    long long misses;
    long long colisoes;
} MetricasHash;

// stores loaded history entry count
typedef struct {
    int       entradas_carregadas;
} MetricasHistorico;

// stores sorting benchmark results
typedef struct {
    // tracks last sorting time and item count
    double    ultimo_tempo_geral_s;
    int       ultimo_n_geral;
} MetricasOrdenacao;

// groups all metrics into one struct
typedef struct {
    MetricasArvore    avl;
    MetricasArvore    splay;
    int               splay_ativa; // indicates if splay tree is active
    MetricasHash      hash;
    MetricasHistorico historico;
    MetricasOrdenacao ordenacao;
} Metricas;

#endif