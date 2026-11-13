#ifndef PEDIDOS_HPP
#define PEDIDOS_HPP

#include "tipos.hpp"
#include "lista.hpp"

// this is the only door to the orders
// the game does not know about the avl, the splay tree or the hash table
// it only asks for a random order code and for the recipe of a code
// inside, this module keeps
//   a plain array with all the orders (only to draw a code that really exists)
//   the avl tree with all the orders (main storage and search)
//   the hash table (cache of the orders already searched)
//   the splay tree (extra, only to compare with the avl in the report)
// file format: 2 lines for each order (the code, then the recipe with 5 to 7 letters)

// reads the orders file and builds the structures, call it once before the menu
// it measures the load time and fills the counters
// wrong records are ignored and counted (bad code, bad recipe, repeated code)
// returns false if the file cannot be opened, there is no memory, or there is no valid order
bool carregar_pedidos(const char *caminho_arquivo);

// finds the order with this code
// first asks the hash cache (hit), if it is not there (miss) searches the avl
// and saves the result in the cache
// returns true and fills pedido_out if the order exists, false if it does not
bool get_pedido(int codigo, Pedido *pedido_out);

// draws a random code that exists in the avl, returns -1 if there are no orders
// we draw from the real codes because the codes can have holes
int sortear_codigo_pedido();

// frees everything (array, avl, splay and hash)
void liberar_pedidos();

// call it at the start of each game
// it zeros the counters of the searches, so the report shows only the last game
// the hash cache is not cleared, it lives during the whole session
void pedidos_iniciar_partida();

// returns how many different orders are loaded
int total_pedidos();

// turns a recipe text like "nkmpz" into a linked list (bottom to top)
// the list must be new or empty, the caller must call lista_destruir on it
// returns false if there is no memory
bool receita_para_lista(const char *receita, Lista *lista_out);

// sets the seed of the random generator (only for tests that must repeat)
void pedidos_definir_semente(unsigned int semente);

#endif