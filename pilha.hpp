#ifndef PILHA_HPP
#define PILHA_HPP

#define PILHA_CAPACIDADE 16   // ajustar conforme config

struct Pilha {
    char itens[PILHA_CAPACIDADE];
    int topo;
};

void pilha_iniciar(Pilha *p);
bool pilha_vazia(const Pilha *p);
bool pilha_empilhar(Pilha *p, char ing);
bool pilha_desempilhar(Pilha *p, char *ing_out);
bool pilha_topo(const Pilha *p, char *ing_out);
int  pilha_tamanho(const Pilha *p);
char pilha_elemento(const Pilha *p, int i);  // consulta sem destruir
void pilha_limpar(Pilha *p);

#endif