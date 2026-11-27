#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "funcoes.h"

/* Letra e nome de cada categoria (mesma ordem das constantes) */
static const char LETRAS[NUM_CATEGORIAS] = {'U', 'P', 'N'};
static const char *NOMES[NUM_CATEGORIAS] = {"Urgente", "Preferencial", "Normal"};

/* ---------- Funções de entrada (com validação) ---------- */

/* Lê uma linha do teclado sem o '\n'. Se a entrada acabar, encerra. */
static void lerLinha(char *texto, int tamanho)
{
    int c;

    if (fgets(texto, tamanho, stdin) == NULL) {
        printf("\nEntrada encerrada.\n");
        exit(0);
    }
    if (strchr(texto, '\n') != NULL) {
        texto[strcspn(texto, "\n")] = '\0';
    } else {
        while ((c = getchar()) != '\n' && c != EOF) { }   /* descarta o excesso */
    }
}

/* Pede um número inteiro entre minimo e maximo, até o usuário acertar. */
int lerInteiro(const char *mensagem, int minimo, int maximo)
{
    char linha[50];
    char *fim;
    long valor;

    while (1) {
        printf("%s", mensagem);
        lerLinha(linha, sizeof(linha));
        valor = strtol(linha, &fim, 10);
        if (fim != linha && *fim == '\0' && valor >= minimo && valor <= maximo) {
            return (int)valor;
        }
        printf("Entrada inválida! Digite um número de %d a %d.\n", minimo, maximo);
    }
}

/* Pergunta S/N. Retorna 1 para sim e 0 para não. */
static int lerSimNao(const char *pergunta)
{
    char linha[50];

    while (1) {
        printf("%s (S/N): ", pergunta);
        lerLinha(linha, sizeof(linha));
        if (strlen(linha) == 1 && toupper(linha[0]) == 'S') return 1;
        if (strlen(linha) == 1 && toupper(linha[0]) == 'N') return 0;
        printf("Entrada inválida! Digite S ou N.\n");
    }
}

/* Lê o nome: não pode ser vazio nem ter números ou símbolos. */
static void lerNome(char *nome)
{
    char linha[TAM_NOME];
    int i, valido;

    while (1) {
        printf("Nome do paciente: ");
        lerLinha(linha, sizeof(linha));
        valido = (strlen(linha) > 0);
        for (i = 0; linha[i] != '\0'; i++) {
            unsigned char c = (unsigned char)linha[i];
            if (!isalpha(c) && c != ' ' && c < 128) valido = 0;   /* c >= 128: letra acentuada */
        }
        if (valido && strspn(linha, " ") != strlen(linha)) {
            strcpy(nome, linha);
            return;
        }
        printf("Nome inválido! Use apenas letras e espaços.\n");
    }
}

/* Lê o gênero (M ou F). */
static char lerGenero(void)
{
    char linha[50];

    while (1) {
        printf("Gênero (M/F): ");
        lerLinha(linha, sizeof(linha));
        if (strlen(linha) == 1 && (toupper(linha[0]) == 'M' || toupper(linha[0]) == 'F')) {
            return (char)toupper(linha[0]);
        }
        printf("Entrada inválida! Digite M ou F.\n");
    }
}

/* ---------- Funções do sistema ---------- */

void iniciarSistema(Sistema *sistema)
{
    int i;

    for (i = 0; i < NUM_CATEGORIAS; i++) {
        sistema->filas[i].inicio = 0;
        sistema->filas[i].fim = 0;
    }
    sistema->emAtendimento = 0;
}

void exibirMenu(void)
{
    printf("\n===== CLÍNICA - SENHAS DE ATENDIMENTO =====\n");
    printf("1 - Cadastrar paciente\n");
    printf("2 - Consultar próxima senha\n");
    printf("3 - Chamar próximo paciente\n");
    printf("4 - Finalizar atendimento\n");
    printf("5 - Exibir painel de atendimento\n");
    printf("0 - Sair do sistema\n");
    printf("===========================================\n");
}

/* Define a categoria. A ordem dos testes garante a prioridade. */
int classificarPaciente(const Paciente *paciente)
{
    if (paciente->idade > IDADE_URGENTE) {
        return URGENTE;
    }
    if (paciente->gestante || paciente->lactante ||
        paciente->necessidadeEspecial || paciente->obeso) {
        return PREFERENCIAL;
    }
    if (paciente->genero == 'F' && paciente->idade >= IDADE_PREF_MULHER) {
        return PREFERENCIAL;
    }
    if (paciente->genero == 'M' && paciente->idade >= IDADE_PREF_HOMEM) {
        return PREFERENCIAL;
    }
    return NORMAL;
}

/* Monta a senha: letra da categoria + número com 3 dígitos (ex.: P002). */
void gerarSenha(Paciente *paciente, int numero)
{
    sprintf(paciente->senha, "%c%03d", LETRAS[paciente->categoria], numero);
}

/* Coloca o paciente no fim da fila. */
void adicionarFila(Fila *fila, const Paciente *paciente)
{
    fila->pacientes[fila->fim] = *paciente;
    fila->fim++;
}

void cadastrarPaciente(Sistema *sistema)
{
    Paciente paciente;
    Fila *fila;

    printf("\n--- CADASTRO DE PACIENTE ---\n");
    paciente.gestante = 0;
    paciente.lactante = 0;

    lerNome(paciente.nome);
    paciente.genero = lerGenero();
    paciente.idade = lerInteiro("Idade: ", 0, IDADE_MAXIMA);
    if (paciente.genero == 'F') {       /* só se aplica a mulheres */
        paciente.gestante = lerSimNao("Está gestante?");
        paciente.lactante = lerSimNao("É lactante?");
    }
    paciente.necessidadeEspecial = lerSimNao("Possui necessidade especial?");
    paciente.obeso = lerSimNao("Possui condição de obesidade?");

    paciente.categoria = classificarPaciente(&paciente);
    fila = &sistema->filas[paciente.categoria];

    if (fila->fim == MAX_FILA) {
        printf("A fila %s está cheia. Cadastro não realizado.\n", NOMES[paciente.categoria]);
        return;
    }

    /* o número da senha é a posição na fila + 1 (cada fila conta sozinha) */
    gerarSenha(&paciente, fila->fim + 1);
    adicionarFila(fila, &paciente);

    printf("\nPaciente cadastrado!\n");
    printf("Categoria: %s\n", NOMES[paciente.categoria]);
    printf("Senha: %s\n", paciente.senha);
}

/* Procura a fila que será chamada: Urgente, depois Preferencial, depois Normal.
   Retorna o número da categoria ou -1 se todas estiverem vazias. */
static int buscarProximaFila(const Sistema *sistema)
{
    int i;

    for (i = 0; i < NUM_CATEGORIAS; i++) {
        if (sistema->filas[i].inicio < sistema->filas[i].fim) {
            return i;
        }
    }
    return -1;
}

void consultarProximaSenha(const Sistema *sistema)
{
    int categoria = buscarProximaFila(sistema);

    printf("\n--- PRÓXIMA SENHA ---\n");
    if (categoria == -1) {
        printf("Não há pacientes aguardando.\n");
    } else {
        const Paciente *p = &sistema->filas[categoria].pacientes[sistema->filas[categoria].inicio];
        printf("Próxima senha: %s (%s) - %s\n", p->senha, NOMES[categoria], p->nome);
    }
}

void chamarPaciente(Sistema *sistema)
{
    int categoria;
    Fila *fila;

    printf("\n--- CHAMAR PRÓXIMO PACIENTE ---\n");

    if (sistema->emAtendimento) {
        printf("Já existe um atendimento em andamento (senha %s).\n", sistema->atual.senha);
        printf("Finalize-o antes de chamar outro paciente.\n");
        return;
    }

    categoria = buscarProximaFila(sistema);
    if (categoria == -1) {
        printf("Não há pacientes aguardando.\n");
        return;
    }

    fila = &sistema->filas[categoria];
    sistema->atual = fila->pacientes[fila->inicio];
    fila->inicio++;                      /* o paciente sai da fila e não volta */
    sistema->emAtendimento = 1;

    printf("Chamando: %s - %s (%s)\n", sistema->atual.senha,
           sistema->atual.nome, NOMES[categoria]);
}

void finalizarAtendimento(Sistema *sistema)
{
    printf("\n--- FINALIZAR ATENDIMENTO ---\n");

    if (!sistema->emAtendimento) {
        printf("Não há nenhum paciente em atendimento.\n");
        return;
    }

    printf("Atendimento da senha %s (%s) finalizado.\n",
           sistema->atual.senha, sistema->atual.nome);
    sistema->emAtendimento = 0;
}

void exibirPainel(const Sistema *sistema)
{
    int i, j;
    int mostradas = 0;

    printf("\n========== PAINEL DE ATENDIMENTO ==========\n");

    if (sistema->emAtendimento) {
        printf("Em atendimento: %s\n", sistema->atual.senha);
        printf("Paciente: %s\n", sistema->atual.nome);
        printf("Categoria: %s\n", NOMES[sistema->atual.categoria]);
    } else {
        printf("Em atendimento: nenhum\n");
    }

    printf("\nPróximas senhas:\n");
    for (i = 0; i < NUM_CATEGORIAS; i++) {
        for (j = sistema->filas[i].inicio; j < sistema->filas[i].fim; j++) {
            if (mostradas < LIMITE_PAINEL) {
                printf("  %d. %s - %s\n", mostradas + 1,
                       sistema->filas[i].pacientes[j].senha,
                       sistema->filas[i].pacientes[j].nome);
                mostradas++;
            }
        }
    }
    if (mostradas == 0) {
        printf("  (nenhuma)\n");
    }

    printf("\nAguardando:\n");
    for (i = 0; i < NUM_CATEGORIAS; i++) {
        printf("  %s: %d\n", NOMES[i], sistema->filas[i].fim - sistema->filas[i].inicio);
    }
    printf("===========================================\n");
}
