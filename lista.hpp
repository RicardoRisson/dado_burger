#ifndef LISTA_HPP
#define LISTA_HPP

// no da lista duplamente encadeada
typedef struct NoLista {
    char simbolo;
    struct NoLista *prox;
    struct NoLista *ant;
} NoLista;

// estrutura da lista
typedef struct {
    NoLista *ini;
    NoLista *fim;
    int tamanho;
} Lista;

void lista_inicializar(Lista *l);

// inserir
int inserir_fim(Lista *l, char simbolo);
int inserir_inicio(Lista *l, char simbolo);

// consultar
int lista_buscar(const Lista *l, char simbolo);
int lista_tamanho(const Lista *l);

// remover
int lista_remover(Lista *l, char simbolo);

// destrui
void lista_destruir(Lista *l);

#endif