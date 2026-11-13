#ifndef METRICAS_HPP
#define METRICAS_HPP

// this file keeps all the numbers of the report in one place
// each structure gets a pointer to its own part of g_metricas and updates it
// relatorio.cpp only reads these numbers

// counters of one tree (avl and splay use the same struct)
struct MetricasArvore {
    // filled while the orders are loaded
    long long nos_inseridos;       // how many nodes were added
    int       altura;              // height of the tree (empty = 0, only root = 1)
    long long rotacoes;            // rotations while inserting (double rotation = 2)
    double    tempo_insercao_s;    // seconds spent only inserting nodes
    double    tempo_carga_s;       // seconds of the whole load (read file + insert)

    // filled during a game (reset when a new game starts)
    long long buscas;              // how many searches were made
    long long comparacoes;         // one comparison for each node visited
    double    tempo_busca_s;       // total seconds spent searching
    long long rotacoes_busca;      // rotations made by searches (only splay)
};

// counters of the hash table
struct MetricasHash {
    int       m;                   // number of slots
    int       n;                   // number of entries stored
    long long hits;                // times the order was found in the cache
    long long misses;              // times the order was not in the cache
    long long colisoes;            // insertions that arrived at a used slot
};

// info about reading the orders file
struct MetricasCarga {
    double    tempo_leitura_s;     // seconds to read the file
    long long linhas_invalidas;    // orders ignored because they were wrong
    long long codigos_repetidos;   // orders ignored because the code already existed
};

// counters of the history
struct MetricasHistorico {
    int       entradas_carregadas; // entries read from the file at the start
    long long linhas_invalidas;    // entries ignored because they were wrong
};

// counters of the sorting (index 0 = by name, index 1 = by score)
struct MetricasOrdenacao {
    int       medicoes[2];         // how many times each sort was measured
    double    ultimo_tempo_s[2];   // time of the last measurement
    double    melhor_tempo_s[2];   // fastest time so far
    int       n_entradas[2];       // size of the list in the last measurement
    double    ultimo_tempo_geral_s;// last time given to atualizar_log_ordenacao
    int       ultimo_n_geral;      // last size given to atualizar_log_ordenacao
};

// all the counters together
struct Metricas {
    MetricasArvore    avl;
    MetricasArvore    splay;       // extra tree, only for comparing with the avl
    int               splay_ativa; // 1 if the splay tree was built
    MetricasHash      hash;
    MetricasCarga     carga;
    MetricasHistorico historico;
    MetricasOrdenacao ordenacao;
    int               partidas_encerradas; // games that already showed the final report
};

// the only copy, every file sees the same one (starts with zeros)
inline Metricas g_metricas = {};

#endif
