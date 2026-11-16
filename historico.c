#include "historico.h"
#include "metricas.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CAPACIDADE_INICIAL 1024
#define TAM_LINHA          256
#define TAM_CAMINHO        1024

static EntradaHistorico *g_entradas   = NULL;
static int               g_total      = 0;
static int               g_capacidade = 0;

static char g_caminho[TAM_CAMINHO] = "";

static void garantir_espaco(void){
    if (g_total == g_capacidade){
        g_capacidade = g_capacidade * 2;
        g_entradas = realloc(g_entradas, g_capacidade * sizeof(EntradaHistorico));
    }
}

static int ler_linha(FILE *f, char *buf, int tam){
    if (fgets(buf, tam, f) == NULL){
        return 0;
    }
    buf[strcspn(buf, "\r\n")] = '\0';
    return 1;
}

static void copiar_nome(char *dest, const char *src){
    int i = 0;
    while (src[i] != '\0' && i < TAM_MAX_NOME){
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

static void anexar_ao_arquivo(const char *caminho, const EntradaHistorico *e){
    FILE *f = fopen(caminho, "a");
    fprintf(f, "%s\n%d\n", e->nome, e->pontuacao);
    fclose(f);
}

void carregar_historico(const char *caminho_arquivo){
    liberar_historico();

    g_metricas.historico.entradas_carregadas = 0;

    strcpy(g_caminho, caminho_arquivo);

    g_capacidade = CAPACIDADE_INICIAL;
    g_entradas = malloc(g_capacidade * sizeof(EntradaHistorico));

    FILE *f = fopen(caminho_arquivo, "r");
    if (f == NULL){
        return;
    }

    char nome[TAM_LINHA];
    char pontos[TAM_LINHA];

    while (ler_linha(f, nome, TAM_LINHA)){
        pontos[0] = '\0';
        ler_linha(f, pontos, TAM_LINHA);

        garantir_espaco();
        copiar_nome(g_entradas[g_total].nome, nome);
        g_entradas[g_total].pontuacao = atoi(pontos);
        g_total++;
    }
    fclose(f);

    g_metricas.historico.entradas_carregadas = g_total;
}

void adicionar_historico(const char *nome, int pontuacao){
    garantir_espaco();

    EntradaHistorico *nova = &g_entradas[g_total];
    copiar_nome(nova->nome, nome);
    nova->pontuacao = pontuacao;
    g_total++;

    anexar_ao_arquivo(g_caminho, nova);
}

const EntradaHistorico* obter_entradas_historico(int *total_out){
    *total_out = g_total;
    return g_entradas;
}

void liberar_historico(void){
    free(g_entradas);
    g_entradas = NULL;
    g_total = 0;
    g_capacidade = 0;
    g_caminho[0] = '\0';
}