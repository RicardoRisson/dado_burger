#include "hash.h"
#include <stdlib.h>

// funcao de hash por divisao: resto da chave por m (o + m garante que nunca da negativo)
static int hashDivisao(int chave, int m){
    return ((chave % m) + m) % m;
}

// slot onde o pedido com esse codigo deve ficar
static int indice_do_codigo(const TabelaHash *t, int codigo){
    return hashDivisao(codigo, t->m);
}

// cria a tabela com m slots vazios e zera as metricas
void hash_inicializar(TabelaHash *t, int m, MetricasHash *met){
    t->m = m;
    t->met = met;
    t->slots = calloc((size_t) m, sizeof(NoHash*));

    met->m = m;
    met->n = 0;
    met->hits = 0;
    met->misses = 0;
    met->colisoes = 0;
}

// procura o codigo na lista do slot, retorna 1 (hit) e devolve o pedido, ou 0 (miss)
int hash_buscar(TabelaHash *t, int codigo, Pedido *pedido_out){
    for (const NoHash *no = t->slots[indice_do_codigo(t, codigo)]; no != NULL; no = no->prox){
        if (no->pedido.codigo == codigo){
            *pedido_out = no->pedido;
            t->met->hits++;
            return 1;
        }
    }
    t->met->misses++;
    return 0;
}

// insere o pedido no comeco da lista do slot (encadeamento separado)
void hash_inserir(TabelaHash *t, const Pedido *p){
    int i = indice_do_codigo(t, p->codigo);

    NoHash *novo = malloc(sizeof(NoHash));
    novo->pedido = *p;

    // slot ja ocupado = 1 colisao, nao importa o tamanho da lista
    if (t->slots[i] != NULL){
        t->met->colisoes++;
    }

    novo->prox = t->slots[i];
    t->slots[i] = novo;
    t->met->n++;
}

// libera todos os nos de todas as listas e o vetor de slots, usado quando o programa termina
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
}