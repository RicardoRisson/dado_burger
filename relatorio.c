#include "relatorio.h"
#include "metricas.h"
#include "ordenacao.h"
#include <stdio.h>

// column widths for aligning report labels
#define COL_A 32
#define COL_H 29

// calculates average avoiding division by zero
static double media(long long total, long long quantidade){
    if (quantidade > 0){
        return (double) total / (double) quantidade;
    }
    return 0.0;
}

// formats all performance metrics into text buffer
int gerar_texto_relatorio(char *buffer, int tamanho){
    int p = 0; // tracks write position in buffer
    const Metricas *m = &g_metricas;
    (void) tamanho; // suppresses unused parameter warning

    p += sprintf(buffer + p, "=== RELATORIO DE DESEMPENHO ===\n\n");

    // writes avl loading metrics
    p += sprintf(buffer + p, "-- AVL: carregamento --\n");
    p += sprintf(buffer + p, "%-*s%lld\n", COL_A, "Nos inseridos:", m->avl.nos_inseridos);
    p += sprintf(buffer + p, "%-*s%d\n", COL_A, "Altura resultante:", m->avl.altura);
    p += sprintf(buffer + p, "%-*s%lld\n", COL_A, "Rotacoes (insercao):", m->avl.rotacoes);
    p += sprintf(buffer + p, "%-*s%.6f s\n\n", COL_A, "Tempo de carregamento:", m->avl.tempo_carga_s);

    // writes history loading metrics
    p += sprintf(buffer + p, "-- Historico: carregamento --\n");
    p += sprintf(buffer + p, "%-*s%d\n\n", COL_A, "Entradas carregadas:", m->historico.entradas_carregadas);

    // writes avl search metrics
    p += sprintf(buffer + p, "-- AVL: buscas durante a partida --\n");
    p += sprintf(buffer + p, "%-*s%lld\n", COL_A, "Total de buscas:", m->avl.buscas);
    p += sprintf(buffer + p, "%-*s%lld\n", COL_A, "Total de comparacoes:", m->avl.comparacoes);
    p += sprintf(buffer + p, "%-*s%.2f\n", COL_A, "Media de comparacoes por busca:", media(m->avl.comparacoes, m->avl.buscas));
    p += sprintf(buffer + p, "%-*s%.6f s\n\n", COL_A, "Tempo total acumulado:", m->avl.tempo_busca_s);

    // calculates hash hit rate and load factor
    long long consultas = m->hash.hits + m->hash.misses;
    double taxa = 0.0;
    if (consultas > 0){
        taxa = 100.0 * (double) m->hash.hits / (double) consultas;
    }
    double lambda = 0.0;
    if (m->hash.m > 0){
        lambda = (double) m->hash.n / (double) m->hash.m;
    }

    // writes hash table metrics
    p += sprintf(buffer + p, "-- Tabela hash --\n");
    p += sprintf(buffer + p, "%-*s%d\n", COL_H, "Tamanho (m):", m->hash.m);
    p += sprintf(buffer + p, "%-*s%d\n", COL_H, "Entradas ocupadas (n):", m->hash.n);
    p += sprintf(buffer + p, "%-*s%.2f\n", COL_H, "Fator de carga (lambda):", lambda);
    p += sprintf(buffer + p, "%-*s%lld\n", COL_H, "Total de hits:", m->hash.hits);
    p += sprintf(buffer + p, "%-*s%lld\n", COL_H, "Total de misses:", m->hash.misses);
    p += sprintf(buffer + p, "%-*s%.1f%%\n", COL_H, "Taxa de acerto:", taxa);
    p += sprintf(buffer + p, "%-*s%lld\n\n", COL_H, "Total de colisoes:", m->hash.colisoes);

    // writes history sorting metrics if measured
    p += sprintf(buffer + p, "-- Ordenacao do historico (ao acessar Historico) --\n");
    if (m->ordenacao.ultimo_n_geral == 0){
        p += sprintf(buffer + p, "Ainda nao medido (abra a tela Historico).\n");
    }else{
        p += sprintf(buffer + p, "Tamanho: %d entradas\n", m->ordenacao.ultimo_n_geral);
        p += sprintf(buffer + p, "%s:  %.6f s\n", nome_algoritmo_ordenacao(), m->ordenacao.ultimo_tempo_geral_s);
    }

    p += sprintf(buffer + p, "\n================================\n");
    return p;
}

// builds report and saves to log file
void gravar_relatorio_log(const char *caminho_log){
    char texto[TAM_BUFFER_RELATORIO];
    gerar_texto_relatorio(texto, TAM_BUFFER_RELATORIO);

    FILE *f = fopen(caminho_log, "w");
    fputs(texto, f);
    fclose(f);
}

// writes initial loading report to log file
void registrar_log_carregamento(const char *caminho_log){
    gravar_relatorio_log(caminho_log);
}

// prints final report and saves to log
void exibir_e_gravar_relatorio_final(const char *caminho_log){
    g_metricas.partidas_encerradas++; // counts finished match

    char texto[TAM_BUFFER_RELATORIO];
    gerar_texto_relatorio(texto, TAM_BUFFER_RELATORIO);
    fputs(texto, stdout); // prints report to standard output

    gravar_relatorio_log(caminho_log);
}

// updates sorting stats and refreshes log file
void atualizar_log_ordenacao(const char *caminho_log, double tempo_s, int n_entradas){
    g_metricas.ordenacao.ultimo_tempo_geral_s = tempo_s;
    g_metricas.ordenacao.ultimo_n_geral = n_entradas;
    gravar_relatorio_log(caminho_log);
}