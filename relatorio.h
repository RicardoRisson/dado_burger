#ifndef RELATORIO_H
#define RELATORIO_H


// max character size for report text
#define TAM_BUFFER_RELATORIO 6144

// saves initial loading report to log file
void registrar_log_carregamento(const char *caminho_log);

// prints final report and saves to log
void exibir_e_gravar_relatorio_final(const char *caminho_log);

// updates sorting stats and refreshes log file
void atualizar_log_ordenacao(const char *caminho_log, double tempo_s, int n_entradas);

// formats performance metrics into a text buffer
int gerar_texto_relatorio(char *buffer, int tamanho);

// builds report and saves to log file
void gravar_relatorio_log(const char *caminho_log);

#endif