#ifndef CONFIG_H
#define CONFIG_H

#define CFG_HEAP_CAP 3
#define CFG_NUM_CLIENTES      2   
#define CFG_PACIENCIA_MAXIMA  15  

#define CFG_VALIDADE_CARNE    3  
#define CFG_VALIDADE_OVO      2   
#define CFG_VALIDADE_BACON    3   

#ifndef CFG_HASH_M
#define CFG_HASH_M            1009
#endif

#ifndef CFG_COMPARAR_SPLAY
#define CFG_COMPARAR_SPLAY    1
#endif

#define CFG_ARQUIVO_LOG       "medicoes.log"

#endif