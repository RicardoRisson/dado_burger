#include <cstdio>
#include "pedidos.hpp"
#include "historico.hpp"
#include "config.hpp"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::fprintf(stderr, "Uso: %s <arquivo_pedidos> <arquivo_historico>\n",
                     argv[0]);
        return 1;
    }

    const char *arq_pedidos   = argv[1];
    const char *arq_historico = argv[2];

    if (!carregar_pedidos(arq_pedidos)) {
        std::fprintf(stderr, "Erro: nao foi possivel carregar '%s'\n",
                     arq_pedidos);
        return 1;
    }

    // historico ausente = historico vazio (a carregar_historico decide isso)
    if (!carregar_historico(arq_historico)) {
        std::fprintf(stderr, "Aviso: falha ao ler '%s', iniciando vazio\n",
                     arq_historico);
    }

    // aqui fazer menu loop com o raylib (Novo jogo / Como jogar / Historico / Sair)

    salvar_historico(arq_historico);
    liberar_historico();
    liberar_pedidos();
    return 0;
}