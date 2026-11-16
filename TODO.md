# TODO — Dado Burger (só o que realmente falta)

## 1. Obrigatório

- [ ] Relatório em PDF (máx. 8 páginas, incluindo capa, template da aula), com as seções nesta ordem:
  - [ ] Introdução: grupo, visão geral, requisitos implementados e não implementados (com justificativa)
  - [ ] O jogo: como funciona a partida, ações, extras (splay tree) e justificativa dos parâmetros:
    - Paciência: 15
    - Clientes: 2
    - Validade: carne 3 / ovo 2 / bacon 3
    - Algoritmo de ordenação: Merge Sort
  - [ ] Estruturas de dados: uma subseção por estrutura (AVL, lista contígua dinâmica, hash, lista encadeada, pilha, fila, heap), com as adaptações feitas (converter o adaptacoes.txt) e:
    - Hash: função de divisão (chave mod m), m = 1009, encadeamento separado, com justificativa
    - Heap: min-heap, armazenando direto turno de preparo + validade
    - Histórico: chamar de "lista contígua dinâmica"
  - [ ] Medições e análise: tabelas comparando empírico x teoria:
    - AVL: 837.253 nós, altura 20 (limite teórico 1,44·log2 n ≈ 28), 837.233 rotações (pedidos.txt está ordenado, ~1 rotação por inserção)
    - Buscas na AVL: média de comparações por busca (~log2 n)
    - Hash: hits, misses, taxa de acerto, colisões, λ = n/m (explicar que com poucos clientes quase não há hit)
    - Merge Sort: tempo de ordenação do histórico (n ≈ 100 mil)
    - Splay x AVL, se for contar como extra
  - [ ] Conclusão: dificuldades, divisão de tarefas, aprendizados
  - [ ] Declaração de uso de IA: obrigatória (enunciado §9.6 e §10). Informar ferramenta, finalidade e artefatos 
- [ ] Gerar o medicoes.log final:
  1. Restaurar o historico.txt original (partidas de teste acrescentam linhas)
  2. Rodar `./dado_burger pedidos.txt historico.txt`
  3. Jogar uma partida até o fim
  4. Abrir o Histórico (opção 3) para registrar o tempo de ordenação
  5. Sair pelo 0, sem rodar de novo (cada execução sobrescreve o log)
  6. Conferir que não aparece "Ainda nao medido"
- [ ] Montar o tp-grupoX.zip só com os .c, .h, medicoes.log e o PDF

## 2. Recomendado

- [ ] Compilar do zero na máquina da apresentação: `gcc -o dado_burger *.c -lm -Wall -std=c99` (já compila sem warnings aqui)
- [ ] Revisar para a arguição:
  - Min-heap: heapify/siftUp, descarte dos vencidos no início do turno
  - Splay tree top-down
  - Cálculo das métricas da hash (colisão por inserção em posição ocupada, λ = n/m)
- [ ] Mencionar no relatório que CFG_HEAP_CAP = 3 basta porque só se prepara 1 unidade por turno e a validade máxima é 3 (se aumentar a validade, aumentar a capacidade)

## Já feito / não precisa

- [x] Heap trocado para min-heap
- [x] enum CategoriaIngrediente trocado por #define
- [x] adaptacoes.txt renomeado, sem rascunhos
- [x] Teste de borda: 3 pedidos sem historico.txt (passou)
- [x] Teste de borda: `p b` repetido com CFG_HEAP_CAP = 3 (passou, sem erros no AddressSanitizer)
- [x] Compilação limpa com -Wall -std=c99
- [x] Binário dado_burger não está no Git (nada a remover)
- [x] Remover `#include <string.h>` não usado em ordenacao.c

## Opcional (cosmético, só se sobrar tempo)

- [ ] Uniformizar o estilo das chaves em avl.c, splay.c, fila.c etc.
- [ ] Makefile e .gitignore (a entrega é um zip, então não contam)