# Sistema de Gerenciamento de Senhas de Atendimento Clínico

Programa em C, para terminal, que controla as filas de atendimento de uma clínica. Ele cadastra pacientes, classifica cada um por prioridade, gera a senha e controla a chamada e a finalização dos atendimentos.

> **Aviso:** as categorias e faixas etárias são regras didáticas definidas para esta atividade. Não reproduzem a legislação de atendimento prioritário nem a classificação médica de risco, e a classificação por idade não substitui avaliação clínica.

## Estrutura dos arquivos

```text
sistema-clinica/
├── main.c              # só a função main(): inicia o sistema e controla o menu
├── funcoes/
│   ├── funcoes.h       # constantes, structs e declaração das funções
│   └── funcoes.c       # implementação das funções
├── README.md
└── .gitignore
```

## Funcionalidades

1. **Cadastrar paciente:** pede nome, gênero, idade, gestante, lactante, necessidade especial e obesidade; classifica e gera a senha.
2. **Consultar próxima senha:** mostra quem será chamado, sem chamar.
3. **Chamar próximo paciente:** coloca o próximo da fila em atendimento.
4. **Finalizar atendimento:** encerra o atendimento atual.
5. **Exibir painel:** mostra a senha em atendimento, o nome e a categoria do paciente, as próximas senhas e quantos aguardam em cada categoria.
0. **Sair.**

## Regras de prioridade

| Categoria | Letra | Quem é |
|---|---|---|
| Urgente | U | Idade **superior a 80 anos** (vale mesmo que haja outras condições) |
| Preferencial | P | Gestante, lactante, necessidade especial, obeso, mulher com 60 anos ou mais, homem com 65 anos ou mais |
| Normal | N | Todos os demais |

- A ordem de chamada é: **Urgente → Preferencial → Normal**.
- Dentro de cada categoria vale a ordem de chegada (FIFO).
- Cada categoria tem sua própria numeração: `N001, N002...`, `P001, P002...`, `U001, U002...`.
- Quem já foi chamado não volta para a fila.
- Não é possível chamar outro paciente com um atendimento em andamento.
- Não é possível finalizar sem haver um paciente em atendimento.
- Todas as entradas são validadas (menu, nome, gênero, idade e S/N).
- Cada fila aceita até 100 cadastros (`MAX_FILA`).

## Funções (em `funcoes/funcoes.c`)

| Função | O que faz |
|---|---|
| `iniciarSistema` | Zera as filas e o estado de atendimento |
| `exibirMenu` | Mostra o menu |
| `lerInteiro` | Lê um número validado dentro de um intervalo |
| `cadastrarPaciente` | Coleta os dados e coloca o paciente na fila |
| `classificarPaciente` | Retorna a categoria (U, P ou N) |
| `gerarSenha` | Monta a senha, por exemplo `P002` |
| `adicionarFila` | Insere o paciente no fim da fila |
| `consultarProximaSenha` | Mostra a próxima senha |
| `chamarPaciente` | Retira o próximo da fila e inicia o atendimento |
| `finalizarAtendimento` | Encerra o atendimento atual |
| `exibirPainel` | Mostra o painel |

Funções auxiliares internas (`static`): `lerLinha`, `lerSimNao`, `lerNome`, `lerGenero` e `buscarProximaFila`.

## Como funciona a fila

Cada fila é um vetor com dois índices: `inicio` (próximo a ser chamado) e `fim` (próxima posição livre). Cadastrar aumenta `fim`; chamar aumenta `inicio`. A fila está vazia quando `inicio == fim`.

## Compilação e execução

```bash
gcc main.c funcoes/funcoes.c -o sistema
./sistema
```

No Windows: `sistema.exe`. Se os acentos aparecerem errados, rode `chcp 65001` antes.

## Exemplo de uso

Cadastros: Joao (30 anos, M) → `N001`; Maria (28 anos, F, gestante) → `P001`; Jose (85 anos, M) → `U001`.

```text
--- CHAMAR PRÓXIMO PACIENTE ---
Chamando: U001 - Jose (Urgente)

========== PAINEL DE ATENDIMENTO ==========
Em atendimento: U001
Paciente: Jose
Categoria: Urgente

Próximas senhas:
  1. P001 - Maria
  2. N001 - Joao

Aguardando:
  Urgente: 0
  Preferencial: 1
  Normal: 1
===========================================
```

## Exemplo demonstrativo: geração e chamada de senhas

Cadastros feitos nesta ordem:

| Paciente | Dados | Categoria | Senha |
|---|---|---|---|
| Joao Silva | M, 30 anos | Normal | N001 |
| Maria Souza | F, 28 anos, gestante | Preferencial | P001 |
| Ana Lima | F, 25 anos | Normal | N002 |
| Jose Antunes | M, 85 anos | Urgente | U001 |
| Carlos Dias | M, 66 anos | Preferencial | P002 |
| Helena Costa | F, 90 anos | Urgente | U002 |

Ordem em que as senhas são chamadas (sempre pela opção 3, seguida da opção 4):

```text
1º U001 - Jose Antunes   (Urgente)
2º U002 - Helena Costa   (Urgente)
3º P001 - Maria Souza    (Preferencial)
4º P002 - Carlos Dias    (Preferencial)
5º N001 - Joao Silva     (Normal)
6º N002 - Ana Lima       (Normal)
```

Mesmo tendo chegado antes, Joao (N001) só é chamado depois de todos os urgentes e preferenciais, e dentro de cada categoria a ordem de emissão é mantida.

## Testes realizados

| Situação testada | Resultado esperado | Resultado |
|---|---|---|
| Cadastro nas três categorias | N, P e U classificados corretamente | OK |
| Numeração independente | N001, P001, U001 em contadores separados | OK |
| Idade 85 e 90 (mesmo com gestação ou obesidade) | Urgente | OK |
| Idade exatamente 80 | Preferencial (a regra exige idade superior a 80) | OK |
| Mulher com 60 / 59 anos | Preferencial / Normal | OK |
| Homem com 65 / 64 anos | Preferencial / Normal | OK |
| Prioridade na chamada | U, depois P, depois N | OK |
| Ordem FIFO dentro da categoria | Ordem de emissão respeitada | OK |
| Chamar com atendimento em andamento | Bloqueado, com aviso | OK |
| Finalizar sem atendimento ativo | Recusado, com mensagem | OK |
| Chamar com as filas vazias | Recusado, com mensagem | OK |
| Entradas inválidas (menu, nome, gênero, idade, S/N) | Pergunta de novo | OK |
| 101º cadastro na mesma fila | Recusado (limite de 100) | OK |
