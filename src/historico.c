#include "../include/historico.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CAPACIDADE_INICIAL 1024
#define TAM_LINHA          256

// doubles array size when it is full
static void garantir_espaco(Historico *h){
    if (h->total == h->capacidade){
        h->capacidade = h->capacidade * 2;
        h->entradas = realloc(h->entradas, h->capacidade * sizeof(EntradaHistorico));
    }
}

// reads one line from file and strips newlines
static int ler_linha(FILE *f, char *buf, int tam){
    if (fgets(buf, tam, f) == NULL){
        return 0;
    }
    // removes trailing newline characters
    buf[strcspn(buf, "\r\n")] = '\0';
    return 1;
}

// copies name up to max allowed length
static void copiar_nome(char *dest, const char *src){
    int i = 0;
    while (src[i] != '\0' && i < TAM_MAX_NOME){
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0'; // ends string with null character
}

// appends one history entry to the file
static void anexar_ao_arquivo(const char *caminho, const EntradaHistorico *e){
    FILE *f = fopen(caminho, "a");
    fprintf(f, "%s\n%d\n", e->nome, e->pontuacao);
    fclose(f);
}

// loads saved history entries from a file
void carregar_historico(const char *caminho_arquivo, Historico *h, Metricas *metricas){
    liberar_historico(h); // clears any previous history in memory

    metricas->historico.entradas_carregadas = 0;

    strcpy(h->caminho, caminho_arquivo); // saves file path for later writes

    // allocates initial memory for history entries
    h->capacidade = CAPACIDADE_INICIAL;
    h->entradas = malloc(h->capacidade * sizeof(EntradaHistorico));

    FILE *f = fopen(caminho_arquivo, "r");
    if (f == NULL){
        return;
    }

    char nome[TAM_LINHA];
    char pontos[TAM_LINHA];

    // reads pairs of name and score lines
    while (ler_linha(f, nome, TAM_LINHA)){
        pontos[0] = '\0';
        ler_linha(f, pontos, TAM_LINHA);

        garantir_espaco(h);
        copiar_nome(h->entradas[h->total].nome, nome);
        h->entradas[h->total].pontuacao = atoi(pontos); // converts score text to integer
        h->total++;
    }
    fclose(f);

    metricas->historico.entradas_carregadas = h->total; // updates loaded entries metric
}

// adds entry to memory and file
void adicionar_historico(Historico *h, const char *nome, int pontuacao){
    garantir_espaco(h);

    // fills next available slot in the array
    EntradaHistorico *nova = &h->entradas[h->total];
    copiar_nome(nova->nome, nome);
    nova->pontuacao = pontuacao;
    h->total++;

    anexar_ao_arquivo(h->caminho, nova);
}

// frees memory and resets history state
void liberar_historico(Historico *h){
    free(h->entradas);
    h->entradas = NULL;
    h->total = 0;
    h->capacidade = 0;
    h->caminho[0] = '\0';
}