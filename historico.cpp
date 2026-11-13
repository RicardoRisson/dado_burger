#include "historico.hpp"
#include "metricas.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// the list is a dynamic array
// g_entradas points to the memory, g_capacidade is how many entries fit
// and g_total is how many are really used
// when it is full, the memory is made twice bigger

#define CAPACIDADE_INICIAL 1024
#define TAM_LINHA          256      // biggest line we read (the rest is thrown away)
#define TAM_CAMINHO        1024

// data of the module, only this file can see it
static EntradaHistorico *g_entradas   = NULL;   // the list
static int               g_total      = 0;      // entries in use
static int               g_capacidade = 0;      // entries that fit in the array

static char g_caminho[TAM_CAMINHO] = "";        // file given to carregar_historico
static bool g_tem_caminho = false;
static bool g_pendente = false;                 // true if some entry is not in the file yet
static bool g_cauda_incompleta = false;         // true if the file ended with a name without score

// makes room for one more entry, returns false if there is no memory
static bool garantir_espaco() {
    if (g_total < g_capacidade) {
        return true;                            // there is room
    }
    int nova = (g_capacidade == 0) ? CAPACIDADE_INICIAL : g_capacidade * 2;
    EntradaHistorico *novo = (EntradaHistorico*) realloc(g_entradas, (size_t) nova * sizeof(EntradaHistorico));
    if (novo == NULL) {
        return false;                           // the old array is still valid
    }
    g_entradas = novo;
    g_capacidade = nova;
    return true;
}

// reads one line of the file into buf and removes the end of line (\n or \r\n)
// if the line is too long, the extra part is thrown away
// returns false at the end of the file
static bool ler_linha(FILE *f, char *buf, int tam) {
    if (fgets(buf, tam, f) == NULL) {
        return false;
    }
    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] != '\n' && len == (size_t) tam - 1) {
        int c;
        while ((c = fgetc(f)) != EOF && c != '\n') {
            // throws away the rest of the long line
        }
    }
    while (len > 0 && (buf[len - 1] == '\n' || buf[len - 1] == '\r')) {
        buf[--len] = '\0';                      // removes the end of line
    }
    return true;
}

// copies a name keeping at most 20 bytes
// if the cut would break a letter with more than one byte, the whole letter is dropped
static void copiar_nome(char *dest, const char *src) {
    size_t n = strlen(src);
    if (n > TAM_MAX_NOME) {
        n = TAM_MAX_NOME;
        while (n > 0 && (((unsigned char) src[n]) & 0xC0) == 0x80) {
            n--;                                // this byte is the middle of a letter
        }
    }
    memcpy(dest, src, n);
    dest[n] = '\0';
}

// turns the name typed by the player into a safe name for the file
static void limpar_nome(char *dest, const char *src) {
    char tmp[TAM_LINHA];
    size_t n = 0;

    // control characters would break the 2 lines per entry format, so they become spaces
    for (size_t i = 0; src[i] != '\0' && n < TAM_LINHA - 1; i++) {
        unsigned char c = (unsigned char) src[i];
        tmp[n++] = (c < 32 || c == 127) ? ' ' : (char) c;
    }
    tmp[n] = '\0';

    // removes spaces at the start and at the end
    size_t ini = 0;
    while (tmp[ini] == ' ') {
        ini++;
    }
    n = strlen(tmp);
    while (n > ini && tmp[n - 1] == ' ') {
        tmp[--n] = '\0';
    }

    copiar_nome(dest, tmp + ini);               // cuts to 20 bytes

    // the cut can leave a space at the end
    n = strlen(dest);
    while (n > 0 && dest[n - 1] == ' ') {
        dest[--n] = '\0';
    }
    if (dest[0] == '\0') {
        strcpy(dest, "Anonimo");                // empty name
    }
}

// returns true if the file has text and its last character is not a line break
// in this case we must write a line break before adding an entry
static bool falta_quebra_de_linha(const char *caminho) {
    FILE *f = fopen(caminho, "rb");
    if (f == NULL) {
        return false;                           // the file does not exist yet
    }
    bool falta = false;
    if (fseek(f, 0, SEEK_END) == 0 && ftell(f) > 0 && fseek(f, -1, SEEK_END) == 0) {
        falta = (fgetc(f) != '\n');             // looks at the last character
    }
    fclose(f);
    return falta;
}

// writes one entry at the end of the file (the file is created if needed)
static bool anexar_ao_arquivo(const char *caminho, const EntradaHistorico *e) {
    bool quebra = falta_quebra_de_linha(caminho);
    FILE *f = fopen(caminho, "a");              // "a" means add at the end
    if (f == NULL) {
        return false;
    }
    if (quebra) {
        fputc('\n', f);
    }
    fprintf(f, "%s\n%d\n", e->nome, e->pontuacao);
    return fclose(f) == 0;                      // fclose can fail if the disk is full
}

// writes all the entries of the list to a file (the old content is replaced)
static bool escrever_tudo(const char *caminho) {
    FILE *f = fopen(caminho, "w");
    if (f == NULL) {
        return false;
    }
    for (int i = 0; i < g_total; i++) {
        fprintf(f, "%s\n%d\n", g_entradas[i].nome, g_entradas[i].pontuacao);
    }
    return fclose(f) == 0;
}

bool carregar_historico(const char *caminho_arquivo) {
    liberar_historico();                        // starts clean, even if called twice

    g_metricas.historico.entradas_carregadas = 0;
    g_metricas.historico.linhas_invalidas = 0;

    strncpy(g_caminho, caminho_arquivo, TAM_CAMINHO - 1);   // remembers the file name
    g_caminho[TAM_CAMINHO - 1] = '\0';
    g_tem_caminho = true;

    FILE *f = fopen(caminho_arquivo, "r");
    if (f == NULL) {
        // no file means empty history, any other problem is an error
        return errno == ENOENT;
    }

    char nome[TAM_LINHA];
    char pontos[TAM_LINHA];
    bool primeira = true;
    bool ok = true;

    while (ler_linha(f, nome, TAM_LINHA)) {     // reads the name line
        if (primeira) {
            primeira = false;
            // some editors put 3 invisible bytes (bom) at the start of the file, removes them
            if ((unsigned char) nome[0] == 0xEF && (unsigned char) nome[1] == 0xBB &&
                (unsigned char) nome[2] == 0xBF) {
                memmove(nome, nome + 3, strlen(nome + 3) + 1);
            }
        }
        if (!ler_linha(f, pontos, TAM_LINHA)) {
            g_cauda_incompleta = true;          // a name without score at the end, ignores it
            break;
        }

        // the score line must be a whole number
        char *fim = NULL;
        errno = 0;
        long valor = strtol(pontos, &fim, 10);
        while (fim != NULL && (*fim == ' ' || *fim == '\t')) {
            fim++;
        }
        if (fim == pontos || *fim != '\0' || errno == ERANGE || valor > 2147483647L || valor < -2147483647L) {
            g_metricas.historico.linhas_invalidas++;    // bad score, ignores this entry
            continue;
        }

        if (!garantir_espaco()) {
            ok = false;                         // no memory, keeps what we have
            break;
        }
        copiar_nome(g_entradas[g_total].nome, nome);    // stores the entry in the list
        g_entradas[g_total].pontuacao = (int) valor;
        g_total++;
    }
    fclose(f);

    g_metricas.historico.entradas_carregadas = g_total;
    return ok;
}

bool adicionar_historico(const char *nome, int pontuacao) {
    if (!garantir_espaco()) {
        return false;
    }

    EntradaHistorico *nova = &g_entradas[g_total];
    limpar_nome(nova->nome, nome);              // stores the clean name
    nova->pontuacao = pontuacao;
    g_total++;                                  // now it is part of the list

    if (g_tem_caminho) {
        if (g_cauda_incompleta) {
            // the file ends with a name without score
            // if we only added at the end, the new name would be read as that score
            // so we write the whole list again (all valid entries are kept)
            if (escrever_tudo(g_caminho)) {
                g_cauda_incompleta = false;
                return true;
            }
        } else if (anexar_ao_arquivo(g_caminho, nova)) {
            return true;
        }
    }
    g_pendente = true;                          // salvar_historico will try again
    return false;
}

bool salvar_historico(const char *caminho_arquivo) {
    bool mesmo_arquivo = g_tem_caminho && strcmp(caminho_arquivo, g_caminho) == 0;

    // everything is already in the file, does not touch it
    if (mesmo_arquivo && !g_pendente) {
        return true;
    }

    bool ok = escrever_tudo(caminho_arquivo);

    if (ok && mesmo_arquivo) {
        g_pendente = false;
        g_cauda_incompleta = false;
    }
    return ok;
}

const EntradaHistorico* obter_entradas_historico(int *total_out) {
    if (total_out != NULL) {
        *total_out = g_total;
    }
    return g_entradas;
}

void liberar_historico() {
    free(g_entradas);
    g_entradas = NULL;
    g_total = 0;
    g_capacidade = 0;
    g_caminho[0] = '\0';
    g_tem_caminho = false;
    g_pendente = false;
    g_cauda_incompleta = false;
}
