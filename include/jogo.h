#ifndef JOGO_H
#define JOGO_H

#include "avl.h"
#include "splay.h"
#include "hash.h"
#include "historico.h"
#include "metricas.h"

// executa jogo
void jogo_executar(ArvoreAVL *avl, ArvoreSplay *splay, TabelaHash *hash, Historico *historico, Metricas *metricas);
#endif