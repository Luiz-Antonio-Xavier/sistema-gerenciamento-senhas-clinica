#include <stdio.h>
#include "funcoes/funcoes.c"

int main(void)
{
    Sistema sistema;
    int opcao;

    iniciarSistema(&sistema);

    do {
        exibirMenu();
        opcao = lerInteiro("Escolha uma opção: ", 0, 5);

        switch (opcao) {
            case 1: cadastrarPaciente(&sistema);     break;
            case 2: consultarProximaSenha(&sistema); break;
            case 3: chamarPaciente(&sistema);        break;
            case 4: finalizarAtendimento(&sistema);  break;
            case 5: exibirPainel(&sistema);          break;
            case 0: printf("\nSistema encerrado.\n"); break;
        }
    } while (opcao != 0);

    return 0;
}
