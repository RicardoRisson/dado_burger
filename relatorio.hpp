#ifndef RELATORIO_HPP
#define RELATORIO_HPP

// tem q ajustar quando metricas.hpp 
void registrar_log_carregamento(const char *caminho_log);
void exibir_e_gravar_relatorio_final(const char *caminho_log);
void atualizar_log_ordenacao(const char *caminho_log, double tempo_s, int n_entradas);

#endif