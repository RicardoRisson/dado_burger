#ifndef RELATORIO_HPP
#define RELATORIO_HPP

// makes the performance report and writes the log file (medicoes.log)
// it only reads the numbers from g_metricas
// the report is first built as text in a buffer, and the same text is used for
//   the log file, the terminal and the raylib window
// the log is always written complete (every section) and it is overwritten, never added to

// size of a buffer that is big enough for the whole report
#define TAM_BUFFER_RELATORIO 6144

// call it right after carregar_pedidos and carregar_historico (before the menu)
// creates (or overwrites) the log with the loading numbers
bool registrar_log_carregamento(const char *caminho_log);

// call it when a game ends (victory, defeat or the player leaves)
// prints the full report in the terminal and writes the log
// it also counts the finished game
bool exibir_e_gravar_relatorio_final(const char *caminho_log);

// rewrites the log after a sorting measurement
// ordenar_historico already calls it by itself
bool atualizar_log_ordenacao(const char *caminho_log, double tempo_s, int n_entradas);

// writes the full report text in buffer (at most tamanho bytes, always ends with '\0')
// nothing is printed and no file is touched, so it is safe to call in every frame
// lines are separated by '\n', returns how many characters were written
int gerar_texto_relatorio(char *buffer, int tamanho);

// writes only the log file (does not print in the terminal)
bool gravar_relatorio_log(const char *caminho_log);

#endif