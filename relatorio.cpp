#include "relatorio.hpp"
#include "metricas.hpp"
#include "ordenacao.hpp"
#include <stdio.h>
#include <stdarg.h>

// the text has no accents on purpose, so it looks the same everywhere

// adds text (printf style) at the end of the buffer
// pos is how many characters are already in the buffer
// if the text does not fit it is cut, returns the new pos
static int anexar(char *buf, int tam, int pos, const char *formato, ...) {
    if (pos >= tam - 1) {
        return pos;
    }
    va_list args;
    va_start(args, formato);
    int escritos = vsnprintf(buf + pos, (size_t) (tam - pos), formato, args);
    va_end(args);

    if (escritos < 0) {
        return pos;
    }
    if (escritos >= tam - pos) {
        return tam - 1;                         // it was cut, the buffer is full
    }
    return pos + escritos;
}

// returns total divided by quantidade, or 0 if quantidade is 0
static double media(long long total, long long quantidade) {
    return quantidade > 0 ? (double) total / (double) quantidade : 0.0;
}

int gerar_texto_relatorio(char *buffer, int tamanho) {
    if (buffer == NULL || tamanho <= 0) {
        return 0;
    }
    buffer[0] = '\0';
    int p = 0;
    const Metricas &m = g_metricas;             // short name for reading

    p = anexar(buffer, tamanho, p, "=== RELATORIO DE DESEMPENHO ===\n");

    // part 1: the numbers that the assignment asks for

    p = anexar(buffer, tamanho, p, "-- AVL: carregamento --\n");
    p = anexar(buffer, tamanho, p, "Nos inseridos: %lld\n", m.avl.nos_inseridos);
    p = anexar(buffer, tamanho, p, "Altura resultante: %d\n", m.avl.altura);
    p = anexar(buffer, tamanho, p, "Rotacoes (insercao): %lld\n", m.avl.rotacoes);
    p = anexar(buffer, tamanho, p, "Tempo de carregamento: %.6f s\n", m.avl.tempo_carga_s);

    p = anexar(buffer, tamanho, p, "-- Historico: carregamento --\n");
    p = anexar(buffer, tamanho, p, "Entradas carregadas: %d\n", m.historico.entradas_carregadas);

    p = anexar(buffer, tamanho, p, "-- AVL: buscas durante a partida --\n");
    p = anexar(buffer, tamanho, p, "Total de buscas: %lld\n", m.avl.buscas);
    p = anexar(buffer, tamanho, p, "Total de comparacoes: %lld\n", m.avl.comparacoes);
    p = anexar(buffer, tamanho, p, "Media de comparacoes por busca: %.2f\n", media(m.avl.comparacoes, m.avl.buscas));
    p = anexar(buffer, tamanho, p, "Tempo total acumulado: %.6f s\n", m.avl.tempo_busca_s);

    // hit rate in percent and load factor (n divided by m)
    long long consultas = m.hash.hits + m.hash.misses;
    double taxa = consultas > 0 ? 100.0 * (double) m.hash.hits / (double) consultas : 0.0;
    double lambda = m.hash.m > 0 ? (double) m.hash.n / (double) m.hash.m : 0.0;

    p = anexar(buffer, tamanho, p, "-- Tabela hash --\n");
    p = anexar(buffer, tamanho, p, "Tamanho (m): %d\n", m.hash.m);
    p = anexar(buffer, tamanho, p, "Entradas ocupadas (n): %d\n", m.hash.n);
    p = anexar(buffer, tamanho, p, "Fator de carga (lambda): %.2f\n", lambda);
    p = anexar(buffer, tamanho, p, "Total de hits: %lld\n", m.hash.hits);
    p = anexar(buffer, tamanho, p, "Total de misses: %lld\n", m.hash.misses);
    p = anexar(buffer, tamanho, p, "Taxa de acerto: %.1f%%\n", taxa);
    p = anexar(buffer, tamanho, p, "Total de colisoes: %lld\n", m.hash.colisoes);

    p = anexar(buffer, tamanho, p, "-- Ordenacao do historico (ao acessar Historico) --\n");
    const MetricasOrdenacao &o = m.ordenacao;
    if (o.medicoes[0] == 0 && o.medicoes[1] == 0) {
        p = anexar(buffer, tamanho, p, "Ainda nao medido (abra a tela Historico).\n");   // no sort yet
    } else {
        int n = (o.medicoes[1] > 0 && o.medicoes[0] == 0) ? o.n_entradas[1] : o.n_entradas[0];
        p = anexar(buffer, tamanho, p, "Tamanho: %d entradas\n", n);
        if (o.medicoes[0] > 0) {
            p = anexar(buffer, tamanho, p, "%s (por nome): %.6f s  [medicoes: %d, melhor: %.6f s]\n",
                       nome_algoritmo_ordenacao(), o.ultimo_tempo_s[0], o.medicoes[0], o.melhor_tempo_s[0]);
        }
        if (o.medicoes[1] > 0) {
            p = anexar(buffer, tamanho, p, "%s (por pontuacao): %.6f s  [medicoes: %d, melhor: %.6f s]\n",
                       nome_algoritmo_ordenacao(), o.ultimo_tempo_s[1], o.medicoes[1], o.melhor_tempo_s[1]);
        }
    }

    // part 2: extra information, useful for the report pdf

    p = anexar(buffer, tamanho, p, "-- Extra: leitura dos arquivos --\n");
    p = anexar(buffer, tamanho, p, "Tempo de leitura dos pedidos: %.6f s\n", m.carga.tempo_leitura_s);
    p = anexar(buffer, tamanho, p, "Tempo so de insercao na AVL: %.6f s\n", m.avl.tempo_insercao_s);
    p = anexar(buffer, tamanho, p, "Pedidos ignorados (invalidos): %lld\n", m.carga.linhas_invalidas);
    p = anexar(buffer, tamanho, p, "Pedidos ignorados (codigo repetido): %lld\n", m.carga.codigos_repetidos);
    p = anexar(buffer, tamanho, p, "Entradas do historico ignoradas (invalidas): %lld\n", m.historico.linhas_invalidas);
    p = anexar(buffer, tamanho, p, "Partidas encerradas nesta execucao: %d\n", m.partidas_encerradas);

    p = anexar(buffer, tamanho, p, "-- Extra: Splay Tree comparada com AVL --\n");
    if (!m.splay_ativa) {
        p = anexar(buffer, tamanho, p, "Splay Tree nao construida (CFG_COMPARAR_SPLAY = 0)\n");
    } else {
        p = anexar(buffer, tamanho, p, "                         AVL            Splay\n");
        p = anexar(buffer, tamanho, p, "Nos inseridos:           %-14lld %lld\n", m.avl.nos_inseridos, m.splay.nos_inseridos);
        p = anexar(buffer, tamanho, p, "Altura:                  %-14d %d\n", m.avl.altura, m.splay.altura);
        p = anexar(buffer, tamanho, p, "Rotacoes (insercao):     %-14lld %lld\n", m.avl.rotacoes, m.splay.rotacoes);
        p = anexar(buffer, tamanho, p, "Tempo de insercao (s):   %-14.6f %.6f\n", m.avl.tempo_insercao_s, m.splay.tempo_insercao_s);
        p = anexar(buffer, tamanho, p, "Tempo de carga (s):      %-14.6f %.6f\n", m.avl.tempo_carga_s, m.splay.tempo_carga_s);
        p = anexar(buffer, tamanho, p, "Buscas na partida:       %-14lld %lld\n", m.avl.buscas, m.splay.buscas);
        p = anexar(buffer, tamanho, p, "Comparacoes (total):     %-14lld %lld\n", m.avl.comparacoes, m.splay.comparacoes);
        p = anexar(buffer, tamanho, p, "Comparacoes por busca:   %-14.2f %.2f\n",
                   media(m.avl.comparacoes, m.avl.buscas), media(m.splay.comparacoes, m.splay.buscas));
        p = anexar(buffer, tamanho, p, "Tempo de buscas (s):     %-14.6f %.6f\n", m.avl.tempo_busca_s, m.splay.tempo_busca_s);
        p = anexar(buffer, tamanho, p, "Rotacoes nas buscas:     %-14lld %lld\n", m.avl.rotacoes_busca, m.splay.rotacoes_busca);
    }

    p = anexar(buffer, tamanho, p, "================================\n");
    return p;
}

bool gravar_relatorio_log(const char *caminho_log) {
    char texto[TAM_BUFFER_RELATORIO];
    gerar_texto_relatorio(texto, TAM_BUFFER_RELATORIO);     // builds the text first

    FILE *f = fopen(caminho_log, "w");          // "w" creates the file or overwrites it
    if (f == NULL) {
        fprintf(stderr, "Aviso: nao foi possivel gravar o log '%s'\n", caminho_log);
        return false;
    }
    fputs(texto, f);
    return fclose(f) == 0;
}

bool registrar_log_carregamento(const char *caminho_log) {
    return gravar_relatorio_log(caminho_log);
}

bool exibir_e_gravar_relatorio_final(const char *caminho_log) {
    g_metricas.partidas_encerradas++;           // counts this game

    char texto[TAM_BUFFER_RELATORIO];
    gerar_texto_relatorio(texto, TAM_BUFFER_RELATORIO);
    fputs(texto, stdout);                       // shows it in the terminal

    return gravar_relatorio_log(caminho_log);
}

bool atualizar_log_ordenacao(const char *caminho_log, double tempo_s, int n_entradas) {
    g_metricas.ordenacao.ultimo_tempo_geral_s = tempo_s;    // saves the last sort numbers
    g_metricas.ordenacao.ultimo_n_geral = n_entradas;
    return gravar_relatorio_log(caminho_log);
}
