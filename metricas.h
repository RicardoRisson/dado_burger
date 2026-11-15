#ifndef METRICAS_H
#define METRICAS_H


typedef struct {
    long long nos_inseridos;
    int       altura;
    long long rotacoes;
    double    tempo_insercao_s;
    double    tempo_carga_s;

    long long buscas;
    long long comparacoes;
    double    tempo_busca_s;
    long long rotacoes_busca;
} MetricasArvore;

typedef struct {
    int       m;
    int       n;
    long long hits;
    long long misses;
    long long colisoes;
} MetricasHash;

typedef struct {
    double    tempo_leitura_s;
} MetricasCarga;

typedef struct {
    int       entradas_carregadas;
} MetricasHistorico;

typedef struct {
    int       medicoes[2];
    double    ultimo_tempo_s[2];
    double    melhor_tempo_s[2];
    int       n_entradas[2];
    double    ultimo_tempo_geral_s;
    int       ultimo_n_geral;
} MetricasOrdenacao;

typedef struct {
    MetricasArvore    avl;
    MetricasArvore    splay;
    int               splay_ativa;
    MetricasHash      hash;
    MetricasCarga     carga;
    MetricasHistorico historico;
    MetricasOrdenacao ordenacao;
    int               partidas_encerradas;
} Metricas;

extern Metricas g_metricas;

#endif