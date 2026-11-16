#include "jogo.h"
#include "tipos.h"
#include "config.h"
#include "pedidos.h"
#include "historico.h"
#include "relatorio.h"
#include "fila.h"
#include "pilha.h"
#include "lista.h"
#include "heap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_ENTRADA 256

static const char PERENES[]    = "sqct";
static const char PERECIVEIS[] = "nob";
static const char INFINITOS[]  = "zkmpa";
static const int  VALIDADE[3]  = { CFG_VALIDADE_CARNE, CFG_VALIDADE_OVO, CFG_VALIDADE_BACON };

typedef struct {
    int  perenes[4];
    Heap pereciveis[3];
} Estoque;

static const char* nome_ingrediente(char s){
    switch (s){
        case ING_PAO_CIMA:       
            return "pao de cima";
        case ING_KETCHUP:        
            return "ketchup";
        case ING_MOSTARDA:       
            return "mostarda";
        case ING_PICLES:         
            return "picles";
        case ING_ALFACE:         
            return "alface";
        case ING_QUEIJO_SUICO:   
            return "queijo suico";
        case ING_QUEIJO_CHEDDAR: 
            return "queijo cheddar";
        case ING_CEBOLA:         
            return "cebola";
        case ING_TOMATE:         
            return "tomate";
        case ING_CARNE:          
            return "carne";
        case ING_OVO_FRITO:      
            return "ovo frito";
        case ING_BACON:          
            return "bacon";
    }
    return "?";
}

static int posicao(const char *vet, char s){
    for (int i = 0; vet[i] != '\0'; i++){
        if (vet[i] == s){
            return i;
        }
    }
    return -1;
}

static int categoria(char s){
    if (posicao(INFINITOS, s) >= 0){
        return CAT_INFINITO;
    }
    if (posicao(PERENES, s) >= 0){
        return CAT_PERENE;
    }
    if (posicao(PERECIVEIS, s) >= 0){
        return CAT_PERECIVEL;
    }
    return CAT_INVALIDO;
}

static int ler_linha(char *buf, int tam){
    if (fgets(buf, tam, stdin) == NULL){
        return 0;
    }
    buf[strcspn(buf, "\r\n")] = '\0';
    return 1;
}

static void descartar_vencidos(Estoque *e, int turno){
    for (int i = 0; i < 3; i++){
        while (e->pereciveis[i].tam > 0 && fpMinimo(&e->pereciveis[i]) <= turno){
            fpRemoverMinimo(&e->pereciveis[i]);
        }
    }
}

static void mostrar_tela(const Cliente *c, int numero, Pilha *prato, const Estoque *e, int turno){
    printf("\n=========== Cliente %d de %d | turno %d ===========\n", numero, CFG_NUM_CLIENTES, turno);
    printf("Turnos restantes (paciencia): %d\n", c->paciencia);
    printf("Pedido (baixo -> cima): ");

    for (int i = 0; c->receita[i] != '\0'; i++){
        printf("%c(%s) ", c->receita[i], nome_ingrediente(c->receita[i]));
    }

    printf("\n");
    Pilha aux;
    pilha_inicializar(&aux);

    while (!pilha_vazia(prato)){
        pilha_empilhar(&aux, pilha_desempilhar(prato));
    }
    printf("Hamburguer (baixo -> cima): [pao de baixo]");

    while (!pilha_vazia(&aux)){
        char s = pilha_desempilhar(&aux);
        printf(" %c", s);
        pilha_empilhar(prato, s);
    }
    
    printf("\n");
    pilha_destruir(&aux);

    printf("Estoque: ");
    for (int i = 0; i < 4; i++){
        printf("%c=%d ", PERENES[i], e->perenes[i]);
    }
    for (int i = 0; i < 3; i++){
        printf("%c=%d ", PERECIVEIS[i], e->pereciveis[i].tam);
    }
    printf("| infinitos: z k m p a\n");
    printf("Comandos: p <ing> preparar | a <ing> adicionar | r remover topo | s sair\n");
}

static int acao_preparar(Estoque *e, char s, int turno){
    int i = posicao(PERENES, s);
    if (i >= 0){
        e->perenes[i]++;
        return 1;
    }
    i = posicao(PERECIVEIS, s);
    if (i >= 0){
        fpInserir(&e->pereciveis[i], turno + VALIDADE[i]);
        return 1;
    }
    printf("Esse ingrediente nao existe ou nao requer preparo.\n");
    return 0;
}

static int acao_adicionar(Estoque *e, Pilha *prato, char s){
    if (pilha_topo_e_z(prato)){
        printf("O pao de cima ja foi colocado.\n");
        return 0;
    }
    int cat = categoria(s);
    if (cat == CAT_INFINITO){
        pilha_empilhar(prato, s);
        return 1;
    }
    if (cat == CAT_PERENE){
        int i = posicao(PERENES, s);
        if (e->perenes[i] == 0){
            printf("Sem %s no estoque.\n", nome_ingrediente(s));
            return 0;
        }
        e->perenes[i]--;
        pilha_empilhar(prato, s);
        return 1;
    }
    if (cat == CAT_PERECIVEL){
        int i = posicao(PERECIVEIS, s);
        if (e->pereciveis[i].tam == 0){
            printf("Sem %s no estoque.\n", nome_ingrediente(s));
            return 0;
        }
        fpRemoverMinimo(&e->pereciveis[i]);
        pilha_empilhar(prato, s);
        return 1;
    }
    printf("Ingrediente invalido.\n");
    return 0;
}

static int acao_remover(Pilha *prato){
    if (pilha_vazia(prato)){
        printf("O hamburguer esta vazio.\n");
        return 0;
    }
    pilha_desempilhar(prato);
    return 1;
}

void jogo_executar(void){
    pedidos_iniciar_partida();

    Fila fila;
    fila_inicializar(&fila);
    for (int i = 0; i < CFG_NUM_CLIENTES; i++){
        Cliente c;
        c.codigo = 0;
        c.receita[0] = '\0';
        c.paciencia = CFG_PACIENCIA_MAXIMA;
        fila_entrar(&fila, &c);
    }

    Estoque estoque;
    memset(estoque.perenes, 0, sizeof(estoque.perenes));
    for (int i = 0; i < 3; i++){
        estoque.pereciveis[i].tam = 0;
    }

    int turno = 1;
    int atendidos = 0;
    int derrota = 0;
    int saiu = 0;
    char linha[TAM_ENTRADA];

    while (!fila_vazia(&fila) && !derrota && !saiu){
        Cliente *cliente = fila_frente(&fila);

        Pedido ped;
        get_pedido(sortear_codigo_pedido(), &ped);
        cliente->codigo = ped.codigo;
        strcpy(cliente->receita, ped.receita);

        Lista receita;
        receita_para_lista(cliente->receita, &receita);
        Pilha prato;
        pilha_inicializar(&prato);

        int concluido = 0;
        while (!concluido && !derrota && !saiu){
            descartar_vencidos(&estoque, turno);

            int consumiu = 0;
            while (!consumiu && !saiu){
                mostrar_tela(cliente, atendidos + 1, &prato, &estoque, turno);
                printf("> ");
                if (!ler_linha(linha, TAM_ENTRADA)){
                    saiu = 1;
                    break;
                }
                char cmd = '\0', ing = '\0';
                sscanf(linha, " %c %c", &cmd, &ing);

                if (cmd == 'p'){
                    consumiu = acao_preparar(&estoque, ing, turno);
                }else if (cmd == 'a'){
                    consumiu = acao_adicionar(&estoque, &prato, ing);
                }else if (cmd == 'r'){
                    consumiu = acao_remover(&prato);
                }else if (cmd == 's'){
                    saiu = 1;
                }else{
                    printf("Comando invalido.\n");
                }
            }
            if (saiu){
                break;
            }

            turno++;
            if (pilha_topo_e_z(&prato) && pilha_coincide_com_lista(&prato, &receita)){
                concluido = 1;
            }else{
                cliente->paciencia--;
                if (cliente->paciencia == 0){
                    derrota = 1;
                }
            }
        }

        pilha_destruir(&prato);
        destruir(&receita);
        if (concluido){
            atendidos++;
            fila_sair(&fila);
        }
    }

    if (derrota){
        printf("\nDERROTA: um cliente perdeu a paciencia.\n");
    }else if (saiu){
        printf("\nVoce saiu da partida.\n");
    }else{
        printf("\nVITORIA: todos os clientes foram atendidos!\n");
    }
    printf("Pontuacao: %d\n", atendidos);

    fila_destruir(&fila);

    printf("Nome do jogador: ");
    if (!ler_linha(linha, TAM_ENTRADA)){
        linha[0] = '\0';
    }
    adicionar_historico(linha, atendidos);
    exibir_e_gravar_relatorio_final(CFG_ARQUIVO_LOG);
}