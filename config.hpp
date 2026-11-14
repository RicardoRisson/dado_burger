#ifndef CONFIG_HPP
#define CONFIG_HPP

// [Pessoa B] valores do jogo (definidos pela equipe, ver docs/adaptacoes.md)
// Uma receita de 7 itens pode custar ate ~13 turnos (preparar + adicionar),
// por isso a paciencia tem folga para 1 ou 2 erros.

// quantidade de clientes na fila e paciencia de cada um (turnos)
#define CFG_NUM_CLIENTES      3
#define CFG_PACIENCIA_MAXIMA  25

// validade dos pereciveis, em turnos (turno do preparo + validade = expira)
#define CFG_VALIDADE_CARNE    6
#define CFG_VALIDADE_OVO      5
#define CFG_VALIDADE_BACON    6

// cache de pedidos que o enunciado fala que tem q ser com hash
// [Person A] m changed 101 -> 1009 (prime). See docs/adaptacoes.md, item "config.hpp".
#define CFG_HASH_M            1009

// [Person A] 1 = also build the Splay Tree (extra) to compare with the AVL, 0 = skip it
#ifndef CFG_COMPARAR_SPLAY
#define CFG_COMPARAR_SPLAY    1
#endif

// log obrigatorio
#define CFG_ARQUIVO_LOG       "medicoes.log"

#endif