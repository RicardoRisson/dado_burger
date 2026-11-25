#ifndef HASH_H
#define HASH_H

// struct que guarda a chave, o valor e o prox
typedef struct NoHash {
    int chave;
    int valor;
    struct NoHash *prox;
} NoHash;

// struct da tabela hash, cada slot guarda uma lista encadeada
typedef struct {
    NoHash **slots;
    int      m;   
    int      n;   
} TabelaHash;

void hash_inicializar(TabelaHash *t, int m);

void hash_inserir(TabelaHash *t, int chave, int valor);

int hash_buscar(const TabelaHash *t, int chave);

int hash_houve_colisao(const TabelaHash *t, int chave);

void hash_destruir(TabelaHash *t);

#endif