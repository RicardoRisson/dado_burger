#include "../include/hash.h"
#include <stdlib.h>

// funcao de hash por divisao: resto da chave por m
static int hashDivisao(int chave, int m){
    return chave % m;
}

// cria a tabela com m slots vazios
void hash_inicializar(TabelaHash *t, int m){
    t->m = m;
    t->n = 0;
    t->slots = calloc((size_t) m, sizeof(NoHash*));
}

// insere o par chave/valor no comeco da lista do slot (encadeamento separado)
void hash_inserir(TabelaHash *t, int chave, int valor){
    int i = hashDivisao(chave, t->m);

    NoHash *novo = malloc(sizeof(NoHash));
    novo->chave = chave;
    novo->valor = valor;
    novo->prox = t->slots[i];
    t->slots[i] = novo;
    t->n++;
}

// procura a chave na lista do slot, retorna o valor ou -1 se nao achar
int hash_buscar(const TabelaHash *t, int chave){
    for (const NoHash *no = t->slots[hashDivisao(chave, t->m)]; no != NULL; no = no->prox){
        if (no->chave == chave){
            return no->valor;
        }
    }
    return -1;
}

// retorna 1 se o slot da chave ja esta ocupado
int hash_houve_colisao(const TabelaHash *t, int chave){
    if (t->slots[hashDivisao(chave, t->m)] != NULL){
        return 1;
    }
    return 0;
}

// libera todos os nos de todas as listas e o vetor de slots
void hash_destruir(TabelaHash *t){
    for (int i = 0; i < t->m; i++){
        NoHash *no = t->slots[i];
        while (no != NULL){
            NoHash *prox = no->prox;
            free(no);
            no = prox;
        }
    }
    free(t->slots);
    t->slots = NULL;
    t->m = 0;
    t->n = 0;
}