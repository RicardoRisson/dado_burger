#ifndef ORDENACAO_HPP
#define ORDENACAO_HPP

#include "tipos.hpp"

// sorts the history to show it on the history screen
// algorithm: merge sort (the one from class), chosen because
// it always takes n log n time, even with sorted data or many equal values
// it is stable, so equal entries keep the file order
// two ways to sort: by name (a to z, ignoring upper and lower case) and by score (biggest first)
// ties keep the file order (older games first)
// the sort works on a copy, so the list and the file are never changed

enum CriterioOrdenacao {
    ORDENAR_POR_NOME = 0,
    ORDENAR_POR_PONTUACAO = 1
};

// makes a sorted copy of the history
// copia_out receives the new array (NULL if the history is empty)
// n_out receives how many entries the copy has
// tempo_out receives the seconds of the sort only, not of the copy (can be NULL)
// every call also updates the metrics and rewrites the log file
// returns true if ok (an empty history is ok), false only if there is no memory
// call it once when the screen opens or the order changes, not in every frame
bool ordenar_historico(CriterioOrdenacao criterio, EntradaHistorico **copia_out,
                       int *n_out, double *tempo_out);

// frees a copy made by ordenar_historico (NULL is accepted)
void liberar_copia_ordenada(EntradaHistorico *copia);

// returns the name of the algorithm, to show in the report
const char* nome_algoritmo_ordenacao();

#endif
