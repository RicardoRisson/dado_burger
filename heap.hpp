#ifndef HEAP_HPP
#define HEAP_HPP

// min-heap de turnos
typedef struct {
    int *dados;
    int tamanho;
    int capacidade;
} Heap;

void heap_inicializar(Heap *h, int capacidade_inicial);

// insere um turno de validade
int heap_inserir(Heap *h, int turno_validade);

// consulta o menor sem remover
int heap_minimo(const Heap *h);

// remove o menor
void heap_remover_minimo(Heap *h);

// saber o tamanho e se ta vazio
int heap_tamanho(const Heap *h);
int heap_vazio(const Heap *h);

// libera o vetor interno
void heap_destruir(Heap *h);

#endif