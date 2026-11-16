1. URGENTE (sem isso a entrega fica incompleta)

Relatório em PDF (máx. 8 páginas, template da aula), com as seções na ordem: Introdução, O jogo, Estruturas de dados, Medições e análise, Conclusão e Declaração de uso de IA.
Seção de medições em tabelas, comparando com a teoria. Exemplos:
A altura 20 da AVL com 837 mil nós, abaixo do limite de 1,44·log₂ n.
As 837233 rotações altas porque o pedidos.txt está ordenado.
A hash com hits, misses, colisões e λ.
O tempo do Merge Sort.
Seção de estruturas de dados (o adaptações.txt vira isto). Atualize com o texto final e inclua a função hash, o valor de m (1009) e a política de colisão (encadeamento separado), todos com justificativa.
Justificar os parâmetros do jogo na seção “O jogo”: paciência 15, 2 clientes, validade 3/2/3.
medicoes.log gerado com pedidos.txt e historico.txt e incluído no zip.
Declaração de uso de IA, se a sua turma exigir.
Zip no formato certo (tp-grupoX.zip) com os .c/.h, o log e o PDF.

2. FORTEMENTE SUGERIDO (pode dar problema)

Heap: descrever no relatório que é max-heap com valor negativo, ou voltar ao min-heap. O enunciado pede “heap mínimo” e é pergunta provável na apresentação.
Histórico: chamar de “lista contígua dinâmica” no relatório.
Testar com um pedidos pequeno (como o do apêndice, 3 pedidos) e um historico.txt que não existe, que é um caso que o enunciado exige tratar.
Testar p b vários turnos para confirmar que o heap com CFG_HEAP_CAP 3 não estoura.
Compilar do zero com gcc -std=c99 -Wall -lm *.c na máquina que vai apresentar.
Tirar o dado_burger do git e usar .gitignore.
Saber explicar o código para a apresentação: valor negativo no heap, splay top-down, métricas da hash.

3. LUXO (pequenos ajustes)

Trocar o enum CategoriaIngrediente por #define.
Renomear adaptações.txt para adaptacoes.txt e apagar os rascunhos (adaptacoes.md, FAZER_PRIORIDADE.txt).
Alinhar a coluna da seção hash com as outras no relatório.
Remover o #include <string.h> sem uso no ordenacao.c.
Criar um Makefile.
Padronizar as chaves nos arquivos que ainda estão sem (avl.c, splay.c, fila.c, etc.).