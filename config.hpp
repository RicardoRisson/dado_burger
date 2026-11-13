#ifndef CONFIG_HPP
#define CONFIG_HPP

// tudo aqui é placeholder pode mudar dps

// valor dos clientes e paciencia deles
#define CFG_NUM_CLIENTES      2   
#define CFG_PACIENCIA_MAXIMA  15  

// tempo de validade de itens pereciveis
#define CFG_VALIDADE_CARNE    3  
#define CFG_VALIDADE_OVO      2   
#define CFG_VALIDADE_BACON    3   

// cache de pedidos que o enunciado fala que tem q ser com hash
// [Person A] m changed 101 -> 1009 (prime). See docs/adaptacoes.md, item "config.hpp".
// #ifndef lets the tests change it with -DCFG_HASH_M=101 without editing this file.
#ifndef CFG_HASH_M
#define CFG_HASH_M            1009
#endif

// [Person A] 1 = also build the Splay Tree (extra) to compare with the AVL, 0 = skip it
#ifndef CFG_COMPARAR_SPLAY
#define CFG_COMPARAR_SPLAY    1
#endif

// log obrigatorio
#define CFG_ARQUIVO_LOG       "medicoes.log"

#endif