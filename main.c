#include <stdio.h>
#include <stdlib.h>
#include "pedidos.h"
#include "historico.h"
#include "ordenacao.h"
#include "relatorio.h"
#include "jogo.h"
#include "config.h"

#define TAM_ENTRADA   256
#define HIST_MOSTRAR  20

// tela estatica com as regras
static void tela_como_jogar(void){
    printf("\n=== COMO JOGAR ===\n");
    printf("Objetivo: atender todos os clientes do Dado Burger montando o hamburguer\n");
    printf("exatamente como o pedido, antes que a paciencia do cliente da frente acabe.\n\n");
    printf("Ingredientes (simbolo):\n");
    printf("  Sem preparo, infinitos: pao de cima (z), ketchup (k), mostarda (m), picles (p), alface (a)\n");
    printf("  Com preparo, duram para sempre: queijo suico (s), cheddar (q), cebola (c), tomate (t)\n");
    printf("  Com preparo, perecem: carne (n), ovo frito (o), bacon (b)\n\n");
    printf("Acoes (cada uma consome 1 turno):\n");
    printf("  p <simbolo>  preparar um ingrediente (ex: p n)\n");
    printf("  a <simbolo>  adicionar um ingrediente ao hamburguer (ex: a k)\n");
    printf("  r            remover o ingrediente do topo (ele e descartado)\n");
    printf("  s            sair da partida (nao consome turno)\n\n");
    printf("O pao de baixo ja esta no prato. Feche o hamburguer com o pao de cima (z).\n");
    printf("Pereciveis vencem apos alguns turnos e sao descartados do estoque.\n");
    printf("Vitoria: todos os clientes atendidos. Derrota: a paciencia do cliente da frente chega a zero.\n");
    printf("\nPressione Enter para voltar ao menu...");
    char lixo[TAM_ENTRADA];
    fgets(lixo, TAM_ENTRADA, stdin);
}

// ordena uma copia do historico e mostra so os primeiros
static void tela_historico(void){
    char linha[TAM_ENTRADA];
    int escolha = 0;
    printf("\n=== HISTORICO ===\n1) Ordem alfabetica\n2) Pontuacao decrescente\n> ");
    if (fgets(linha, TAM_ENTRADA, stdin) == NULL){
        return;
    }
    sscanf(linha, "%d", &escolha);

    CriterioOrdenacao criterio;
    criterio.tipo = ORDENAR_POR_NOME;
    if (escolha == 2){
        criterio.tipo = ORDENAR_POR_PONTUACAO;
    }

    EntradaHistorico *copia;
    int n;
    double tempo;
    ordenar_historico(criterio, &copia, &n, &tempo);

    // n pode ser maior que o limite da tela
    int mostrados = n;
    if (n > HIST_MOSTRAR){
        mostrados = HIST_MOSTRAR;
    }

    for (int i = 0; i < mostrados; i++){
        printf("%3d. %-20s %d\n", i + 1, copia[i].nome, copia[i].pontuacao);
    }
    printf("(%d de %d entradas) %s: %.6f s\n", mostrados, n,
           nome_algoritmo_ordenacao(), tempo);
    liberar_copia_ordenada(copia);
}

// carrega pedidos e historico dos argumentos e abre o menu
int main(int argc, char *argv[]){
    if (argc < 3){
        fprintf(stderr, "Uso: %s <arquivo_pedidos> <arquivo_historico>\n",
                argv[0]);
        return 1;
    }

    const char *arq_pedidos   = argv[1];
    const char *arq_historico = argv[2];

    if (!carregar_pedidos(arq_pedidos)){
        fprintf(stderr, "Erro: nao foi possivel carregar '%s'\n",
                arq_pedidos);
        return 1;
    }

    carregar_historico(arq_historico);

    // grava no log as metricas do carregamento
    registrar_log_carregamento(CFG_ARQUIVO_LOG);

    char linha[TAM_ENTRADA];
    int opcao = -1;
    while (opcao != 0){
        printf("\n=== DADO BURGER ===\n1) Novo jogo\n2) Como jogar\n3) Historico\n0) Sair\n> ");
        if (fgets(linha, TAM_ENTRADA, stdin) == NULL){
            break;
        }
        opcao = -1;
        sscanf(linha, "%d", &opcao);

        if (opcao == 1){
            jogo_executar();
        }else if (opcao == 2){
            tela_como_jogar();
        }else if (opcao == 3){
            tela_historico();
        }
    }

    liberar_historico();
    liberar_pedidos();
    return 0;
}