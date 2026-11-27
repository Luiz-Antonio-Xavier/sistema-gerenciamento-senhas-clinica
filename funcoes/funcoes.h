#ifndef FUNCOES_H
#define FUNCOES_H

/* ---------- Constantes ---------- */
#define TAM_NOME 50
#define TAM_SENHA 8
#define MAX_FILA 100          /* máximo de pacientes cadastrados por fila */
#define IDADE_MAXIMA 130
#define IDADE_URGENTE 80      /* urgente: idade SUPERIOR a 80 anos */
#define IDADE_PREF_MULHER 60
#define IDADE_PREF_HOMEM 65
#define LIMITE_PAINEL 5       /* quantas próximas senhas o painel mostra */

/* Categorias (a ordem do número é a ordem de prioridade) */
#define URGENTE 0
#define PREFERENCIAL 1
#define NORMAL 2
#define NUM_CATEGORIAS 3

/* ---------- Estruturas ---------- */
typedef struct {
    char nome[TAM_NOME];
    char genero;              /* 'M' ou 'F' */
    int idade;
    int gestante;             /* 1 = sim, 0 = não */
    int lactante;
    int necessidadeEspecial;
    int obeso;
    int categoria;            /* URGENTE, PREFERENCIAL ou NORMAL */
    char senha[TAM_SENHA];    /* exemplo: "N001" */
} Paciente;

/* Fila simples com vetor: entra no fim, sai pelo início (FIFO).
   Os pacientes atendidos ficam para trás e nunca voltam. */
typedef struct {
    Paciente pacientes[MAX_FILA];
    int inicio;               /* posição do próximo a ser chamado */
    int fim;                  /* posição livre para o próximo cadastro */
} Fila;

typedef struct {
    Fila filas[NUM_CATEGORIAS];
    Paciente atual;           /* paciente em atendimento */
    int emAtendimento;        /* 1 = há atendimento em andamento */
} Sistema;

/* ---------- Funções ---------- */
void iniciarSistema(Sistema *sistema);
void exibirMenu(void);
int lerInteiro(const char *mensagem, int minimo, int maximo);
void cadastrarPaciente(Sistema *sistema);
int classificarPaciente(const Paciente *paciente);
void gerarSenha(Paciente *paciente, int numero);
void adicionarFila(Fila *fila, const Paciente *paciente);
void consultarProximaSenha(const Sistema *sistema);
void chamarPaciente(Sistema *sistema);
void finalizarAtendimento(Sistema *sistema);
void exibirPainel(const Sistema *sistema);

#endif
