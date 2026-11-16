#ifndef RELATORIO_H
#define RELATORIO_H


#define TAM_BUFFER_RELATORIO 6144

void registrar_log_carregamento(const char *caminho_log);

void exibir_e_gravar_relatorio_final(const char *caminho_log);

void atualizar_log_ordenacao(const char *caminho_log, double tempo_s, int n_entradas);

int gerar_texto_relatorio(char *buffer, int tamanho);

void gravar_relatorio_log(const char *caminho_log);

#endif