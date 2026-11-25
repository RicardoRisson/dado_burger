#include "../include/pedidos.h"
#include "../include/timer.h"
#include "../include/config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_LINHA          256

// reads a line and removes newlines
static int ler_linha(FILE *f, char *buf, int tam) {
    if (fgets(buf, tam, f) == NULL)
        return 0;
    // strips trailing newline characters
    buf[strcspn(buf, "\r\n")] = '\0';
    return 1;
}

// loads orders from file into structures
int carregar_pedidos(const char *caminho_arquivo, ArvoreAVL *avl, ArvoreSplay *splay, Metricas *metricas) {
    // opens orders file for reading
    FILE *f = fopen(caminho_arquivo, "r");
    if (f == NULL)
        return 0;

    char linha_codigo[TAM_LINHA];
    char linha_receita[TAM_LINHA];
    Pedido pedido;

    avl_inicializar(avl, &metricas->avl); // sets up empty avl tree

    Timer t_avl;
    timer_iniciar(&t_avl);
    // reads pairs of code and recipe lines and inserts them into avl tree
    while (ler_linha(f, linha_codigo, TAM_LINHA) && ler_linha(f, linha_receita, TAM_LINHA)) {
        pedido.codigo = atoi(linha_codigo);
        strcpy(pedido.receita, linha_receita);
        avl_inserir(avl, &pedido);
    }
    metricas->avl.tempo_carga_s = timer_parar(&t_avl);
    metricas->avl.altura = avl_altura(avl);

    metricas->splay_ativa = 0;
    splay_inicializar(splay, &metricas->splay);
    // builds splay tree if comparison is active
    if (CFG_COMPARAR_SPLAY) {
        rewind(f);
        Timer t_splay;
        timer_iniciar(&t_splay);
        // reads the file again and inserts orders into splay tree
        while (ler_linha(f, linha_codigo, TAM_LINHA) && ler_linha(f, linha_receita, TAM_LINHA)) {
            pedido.codigo = atoi(linha_codigo);
            strcpy(pedido.receita, linha_receita);
            splay_inserir(splay, &pedido);
        }
        metricas->splay.tempo_carga_s = timer_parar(&t_splay);

        splay_calcular_altura(splay);
        metricas->splay_ativa = 1;
    }
    fclose(f);
    return 1;
}

// finds an order by its code
int get_pedido(int codigo, Pedido *pedido_out, ArvoreAVL *avl, ArvoreSplay *splay, const Metricas *metricas) {
    // searches order in avl tree
    const Pedido *achou = avl_buscar(avl, codigo);

    // runs search on splay tree too
    if (metricas->splay_ativa)
        splay_buscar(splay, codigo);

    if (achou == NULL)
        return 0;

    *pedido_out = *achou;
    return 1;
}

// draws quantidade different order codes between inf and sup, using the hash to detect repeated ones
void sortear_codigos_pedidos(int *vetor, int quantidade, int inf, int sup, TabelaHash *hash, Metricas *metricas) {
    metricas->hash.hits = 0;
    metricas->hash.misses = 0;
    metricas->hash.colisoes = 0;
    int codigo = 0;
    int i = 0;
    while (i < quantidade) {
        codigo = rand() % (sup - inf + 1) + inf;
        // if there are less orders than clients, repeated codes are allowed
        if (quantidade > metricas->avl.nos_inseridos) {
            vetor[i] = codigo;
            i++;
        } else {
            // code was already drawn
            if (hash_buscar(hash, codigo) != -1) {
                metricas->hash.hits++;
            } else {
                metricas->hash.misses++;
                // slot already taken = 1 collision
                if (hash_houve_colisao(hash, codigo) == 1) {
                    metricas->hash.colisoes++;
                }
                vetor[i] = codigo;
                hash_inserir(hash, codigo, codigo);
                i++;
            }
        }
    }
    metricas->hash.n = hash->n;
}

// resets search metrics for new match
void pedidos_iniciar_partida(TabelaHash *hash, Metricas *metricas) {
    metricas->avl.buscas = 0;
    metricas->avl.comparacoes = 0;
    metricas->avl.tempo_busca_s = 0.0;
    metricas->avl.rotacoes_busca = 0;

    metricas->splay.buscas = 0;
    metricas->splay.comparacoes = 0;
    metricas->splay.tempo_busca_s = 0.0;
    metricas->splay.rotacoes_busca = 0;

    // recreates the hash for the new match (it only holds the codes drawn in one match)
    hash_destruir(hash);
    hash_inicializar(hash, CFG_HASH_M);
    metricas->hash.m = hash->m;
    metricas->hash.n = hash->n;
}

// converts recipe text into a list
void receita_para_lista(const char *receita, Lista *lista_out) {
    inicializar(lista_out);
    for (int i = 0; receita[i] != '\0'; i++) {
        inserirFim(lista_out, receita[i]);
    }
}

// frees all memory
void liberar_pedidos(ArvoreAVL *avl, ArvoreSplay *splay) {
    splay_destruir(splay);
    avl_destruir(avl);
}