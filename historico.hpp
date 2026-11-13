#ifndef HISTORICO_HPP
#define HISTORICO_HPP

#include "tipos.hpp"

// history of the games, kept in a contiguous list (a dynamic array in memory)
// each entry has a player name (up to 20 characters) and a score
// names and scores are not unique
// file format: 2 lines for each entry (the name, then the score)
// if the file does not exist, the history starts empty
// the size of the file is never assumed, the list grows when needed
// a new game is added to the end of the list and to the end of the file
// sorting never changes this list or the file (see ordenacao.hpp)

// reads the file into the list, call it once before the menu
// returns true if the history is ready (a missing file is fine, it means empty history)
// returns false if the file exists but cannot be read, or if there is no memory
bool carregar_historico(const char *caminho_arquivo);

// adds one finished game to the end of the list and to the end of the file
// the name is cleaned: control characters become spaces, spaces at the ends are removed,
// it is cut to 20 characters, and an empty name becomes "Anonimo"
// returns true if it is in memory and in the file
// returns false if something failed (if only the file failed, the entry stays in memory)
bool adicionar_historico(const char *nome, int pontuacao);

// writes the whole list to a file
// if it is the loaded file and everything is already saved, it does nothing
// returns true if the file is correct at the end
bool salvar_historico(const char *caminho_arquivo);

// gives read-only access to the list, in file order (not sorted)
// total_out receives how many entries there are
// the pointer can be NULL if the list is empty
// it is only valid until the next adicionar_historico or liberar_historico
const EntradaHistorico* obter_entradas_historico(int *total_out);

// frees the list
void liberar_historico();

#endif