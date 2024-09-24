// so.c
// sistema operacional
// simulador de computador
// so24b

// INCLUDES {{{1
#include "so.h"
#include "dispositivos.h"
#include "irq.h"
#include "programa.h"
#include "instrucao.h"

#include <stdlib.h>
#include <stdbool.h>

// CONSTANTES E TIPOS {{{1
// intervalo entre interrupções do relógio
#define INTERVALO_INTERRUPCAO 50   // em instruções executadas

struct so_t {
  cpu_t *cpu;
  mem_t *mem;
  es_t *es;
  console_t *console;
  bool erro_interno;
};


// função de tratamento de interrupção (entrada no SO)
static int so_trata_interrupcao(void *argC, int reg_A);

// funções auxiliares
static int so_carrega_programa(so_t *self, char *nome_do_executavel);
static bool copia_str_da_mem(int tam, char str[tam], mem_t *mem, int ender);

// CRIAÇÃO {{{1

so_t *so_cria(cpu_t *cpu, mem_t *mem, es_t *es, console_t *console)
{
  so_t *self = malloc(sizeof(*self));
  if (self == NULL) return NULL;

  self->cpu = cpu;
  self->mem = mem;
  self->es = es;
  self->console = console;
  self->erro_interno = false;

  // quando a CPU executar uma instrução CHAMAC, deve chamar a função
  //   so_trata_interrupcao
  cpu_define_chamaC(self->cpu, so_trata_interrupcao, self);

  // coloca o tratador de interrupção na memória
  // quando a CPU aceita uma interrupção, passa para modo supervisor, 
  //   salva seu estado à partir do endereço 0, e desvia para o endereço 10
  // colocamos no endereço 10 o programa de tratamento de interrupção
  //   em asm, que deve conter a instrução CHAMAC, que vai chamar 
  //   so_trata_interrupcao (conforme foi definido acima)
  int ender = so_carrega_programa(self, "trata_int.maq");
  if (ender != IRQ_END_INTERRUPCAO) {
    console_printf("SO: problema na carga do programa inicial");
    self->erro_interno = true;
  }

  // programa o relógio para gerar uma interrupção após INTERVALO_INTERRUPCAO
  es_escreve(self->es, D_RELOGIO_TIMER, INTERVALO_INTERRUPCAO);
  // TODO: testar erro

  return self;
}

void so_destroi(so_t *self)
{
  cpu_define_chamaC(self->cpu, NULL, NULL);
  free(self);
}


// TRATAMENTO DE INTERRUPÇÃO {{{1

// funções auxiliares para tratar cada tipo de interrupção
static void so_trata_irq(so_t *self, int irq);
static void so_trata_irq_reset(so_t *self);
static void so_trata_irq_chamada_sistema(so_t *self);
static void so_trata_irq_err_cpu(so_t *self);
static void so_trata_irq_relogio(so_t *self);
static void so_trata_irq_desconhecida(so_t *self, int irq);

// funções auxiliares para o tratamento de interrupção
static void so_salva_estado_da_cpu(so_t *self);
static void so_trata_pendencias(so_t *self);
static void so_escalona(so_t *self);
static int so_despacha(so_t *self);

// função a ser chamada pela CPU quando executa a instrução CHAMAC
// essa instrução só deve ser executada quando for tratar uma interrupção
// o primeiro argumento é um ponteiro para o SO, o segundo é a identificação
//   da interrupção
// na inicialização do SO é colocada no endereço 10 uma rotina que executa
//   CHAMAC; quando recebe uma interrupção, a CPU salva os registradores
//   no endereço 0, e desvia para o endereço 10
// o valor retornado por esta função é colocado no registrador A, e pode ser
//   testado pelo código que está após o CHAMAC. Esse valor é usado para decidir
//   se a CPU deve retornar da interrupção (e executa o código de usuário) ou
//   executar PARA e ficar suspensa até receber outra interrupção
static int so_trata_interrupcao(void *argC, int reg_A)
{
  so_t *self = argC;
  irq_t irq = reg_A;
  console_printf("SO: recebi IRQ %d (%s)", irq, irq_nome(irq));
  // salva o estado da cpu no descritor do processo que foi interrompido
  so_salva_estado_da_cpu(self);
  // faz o atendimento da interrupção
  so_trata_irq(self, irq);
  // faz o processamento independente da interrupção
  so_trata_pendencias(self);
  // escolhe o próximo processo a executar
  so_escalona(self);
  // recupera o estado do processo escolhido
  return so_despacha(self);
}

static void so_salva_estado_da_cpu(so_t *self)
{
  // se não houver processo corrente, não faz nada
  // salva os registradores que compõem o estado da cpu no descritor do
  //   processo corrente
  // mem_le(self->mem, IRQ_END_A, endereco onde vai o A no descritor);
  // mem_le(self->mem, IRQ_END_X, endereco onde vai o X no descritor);
  // etc
}
static void so_trata_pendencias(so_t *self)
{
  // realiza ações que não são diretamente ligadar com a interrupção que
  //   está sendo atendida:
  // - E/S pendente
  // - desbloqueio de processos
  // - contabilidades
}
static void so_escalona(so_t *self)
{
  // escolhe o próximo processo a executar, que passa a ser o processo
  //   corrente; pode continuar sendo o mesmo de antes ou não
}
static int so_despacha(so_t *self)
{
  // se houver processo corrente, coloca todo o estado desse processo em
  //   IRQ_END_* e retorna 0, senão retorna 1
  // o valor retornado será o valor de retorno de CHAMAC
  if (self->erro_interno) return 1;
  else return 0;
}


static void so_trata_irq(so_t *self, int irq)
{
  // verifica o tipo de interrupção que está acontecendo, e
  //   atende de acordo
  switch (irq) {
    case IRQ_RESET:
      so_trata_irq_reset(self);
      break;
    case IRQ_SISTEMA:
      so_trata_irq_chamada_sistema(self);
      break;
    case IRQ_ERR_CPU:
      so_trata_irq_err_cpu(self);
      break;
    case IRQ_RELOGIO:
      so_trata_irq_relogio(self);
      break;
    default:
      so_trata_irq_desconhecida(self, irq);
  }
}

static void so_trata_irq_reset(so_t *self)
{
  // coloca um programa na memória
  int ender = so_carrega_programa(self, "init.maq");
  if (ender != 100) {
    console_printf("SO: problema na carga do programa inicial");
    self->erro_interno = true;
    return;
  }

  // t1: deveria criar um processo para o init, e inicializar o estado do
  //   processador para esse processo com os registradores zerados, exceto
  //   o PC e o modo.
  // como não tem suporte a processos, está carregando os valores dos
  //   registradores diretamente para a memória, de onde a CPU vai carregar
  //   para os seus registradores quando executar a instrução RETI

  // altera o PC para o endereço de carga (deve ter sido 100)
  mem_escreve(self->mem, IRQ_END_PC, ender);
  // passa o processador para modo usuário
  mem_escreve(self->mem, IRQ_END_modo, usuario);
}

static void so_trata_irq_err_cpu(so_t *self)
{
  // Ocorreu um erro interno na CPU
  // O erro está codificado em IRQ_END_erro
  // Em geral, causa a morte do processo que causou o erro
  // Ainda não temos processos, causa a parada da CPU
  int err_int;
  // com suporte a processos, deveria pegar o valor do registrador erro
  //   no descritor do processo corrente, e reagir de acordo com esse erro
  //   (em geral, matando o processo)
  mem_le(self->mem, IRQ_END_erro, &err_int);
  err_t err = err_int;
  console_printf("SO: IRQ não tratada -- erro na CPU: %s", err_nome(err));
  self->erro_interno = true;
}

static void so_trata_irq_relogio(so_t *self)
{
  // ocorreu uma interrupção do relógio
  // rearma o interruptor do relógio e reinicializa o timer para a próxima interrupção
  es_escreve(self->es, 11, 0); // desliga o sinalizador de interrupção
  es_escreve(self->es, 10, INTERVALO_INTERRUPCAO);
  // trata a interrupção
  // por exemplo, decrementa o quantum do processo corrente, quando se tem
  // um escalonador com quantum
  console_printf("SO: interrupção do relógio (não tratada)");
}

static void so_trata_irq_desconhecida(so_t *self, int irq)
{
  console_printf("SO: não sei tratar IRQ %d (%s)", irq, irq_nome(irq));
}

// CHAMADAS DE SISTEMA {{{1

static void so_chamada_le(so_t *self);
static void so_chamada_escr(so_t *self);
static void so_chamada_cria_proc(so_t *self);
static void so_chamada_mata_proc(so_t *self);
static void so_chamada_espera_proc(so_t *self);

static void so_trata_irq_chamada_sistema(so_t *self)
{
  // com processos, a identificação da chamada está no reg A no descritor
  //   do processo
  int id_chamada;
  mem_le(self->mem, IRQ_END_A, &id_chamada);
  console_printf("SO: chamada de sistema %d", id_chamada);
  switch (id_chamada) {
    case SO_LE:
      so_chamada_le(self);
      break;
    case SO_ESCR:
      so_chamada_escr(self);
      break;
    case SO_CRIA_PROC:
      so_chamada_cria_proc(self);
      break;
    case SO_MATA_PROC:
      so_chamada_mata_proc(self);
      break;
    case SO_ESPERA_PROC:
      so_chamada_espera_proc(self);
      break;
    default:
      console_printf("SO: chamada de sistema desconhecida (%d)", id_chamada);
      // deveria matar o processo...
  }
}

// implementação da chamada se sistema SO_LE
// faz a leitura de um dado da entrada corrente do processo, retorna o dado em A
static void so_chamada_le(so_t *self)
{
  // implementação com espera ocupada
  //   T1: deveria realizar a leitura somente se a entrada estiver disponível,
  //   senão, deveria bloquear o processo.
  //   de qualquer forma, não pode ficar esperando dentro do sistema operacional.
  //   no caso de bloqueio do processo, a leitura (e desbloqueio) deverá
  //   ser feita mais tarde, em tratamentos pendentes em outra interrupção,
  //   ou diretamente em uma interrupção específica do dispositivo, se for
  //   o caso
  // implementação lendo direto do terminal A
  //   T1: deveria usar dispositivo de entrada corrente do processo
  for (;;) {
    int estado;
    es_le(self->es, 1, &estado);
    if (estado != 0) break;
    // como não está saindo do SO, a unidade de controle não está executando seu laço.
    // esta gambiarra faz pelo menos a console ser atualizada
    // como não está saindo do SO, o laço do processador não está sendo executado.
    // com isso, a console não será atualizada
    // esta gambiarra faz o console andar
    // T1: com a implementação de bloqueio de processo, esta gambiarra não
    //   deve mais existir.
    console_tictac(self->console);
  }
  int dado;
  es_le(self->es, 0, &dado);
  // escreve no reg A do processador
  // (na verdade, na posição onde o processador vai pegar o A quando retornar da int)
  // T1: se houvesse processo, deveria escrever no reg A do processo
  // T1: o acesso só deve ser feito nesse momento se for possível; se não, o processo
  //   é bloqueado, e o acesso só deve ser feito mais tarde (e o processo desbloqueado)
  mem_escreve(self->mem, IRQ_END_A, dado);
}

// implementação da chamada se sistema SO_ESCR
// escreve o valor do reg X na saída corrente do processo
static void so_chamada_escr(so_t *self)
{
  // implementação com espera ocupada
  //   T1: deveria bloquear o processo se dispositivo ocupado
  // implementação escrevendo direto do terminal A
  //   T1: deveria usar o dispositivo de saída corrente do processo
  for (;;) {
    int estado;
    es_le(self->es, 3, &estado);
    if (estado != 0) break;
    // como não está saindo do SO, a unidade de controle não está executando seu laço.
    // esta gambiarra faz pelo menos a console ser atualizada
    // T1: não deve mais existir quando houver suporte a processos, porque o SO não poderá
    //   executar por muito tempo, permitindo a execução do laço da unidade de controle
    console_tictac(self->console);
  }
  int dado;
  // está lendo o valor de X e escrevendo o de A direto onde o processador colocou/vai pegar
  // T1: deveria usar os registradores do processo que está realizando a E/S
  // T1: caso o processo tenha sido bloqueado, esse acesso deve ser realizado em outra execução
  //   do SO, quando ele verificar que esse acesso já pode ser feito.
  mem_le(self->mem, IRQ_END_X, &dado);
  es_escreve(self->es, 2, dado);
  mem_escreve(self->mem, IRQ_END_A, 0);
}

// implementação da chamada se sistema SO_CRIA_PROC
// cria um processo
// T1: deveria criar um novo processo
static void so_chamada_cria_proc(so_t *self)
{
  // ainda sem suporte a processos, carrega programa e passa a executar ele
  // quem chamou o sistema não vai mais ser executado, coitado!

  // em X está o endereço onde está o nome do arquivo
  int ender_proc;
  // deveria ler o X do descritor do processo criador
  if (mem_le(self->mem, IRQ_END_X, &ender_proc) == ERR_OK) {
    char nome[100];
    if (copia_str_da_mem(100, nome, self->mem, ender_proc)) {
      int ender_carga = so_carrega_programa(self, nome);
      if (ender_carga > 0) {
        // deveria escrever no PC do descritor do processo criado
        mem_escreve(self->mem, IRQ_END_PC, ender_carga);
        return;
      }
    }
  }
  // deveria escrever -1 (se erro) ou o PID do processo criado (se OK) no reg A
  //   do processo que pediu a criação
  mem_escreve(self->mem, IRQ_END_A, -1);
}

// implementação da chamada se sistema SO_MATA_PROC
// mata o processo com pid X (ou o processo corrente se X é 0)
// T1: deveria matar um processo
static void so_chamada_mata_proc(so_t *self)
{
  // ainda sem suporte a processos, retorna erro -1
  console_printf("SO: SO_MATA_PROC não implementada");
  mem_escreve(self->mem, IRQ_END_A, -1);
}

// implementação da chamada se sistema SO_ESPERA_PROC
// espera o fim do processo com pid X
// T1: deveria bloquear o processo se for o caso (e desbloquear na morte do erperado)
static void so_chamada_espera_proc(so_t *self)
{
  // ainda sem suporte a processos, retorna erro -1
  console_printf("SO: SO_ESPERA_PROC não implementada");
  mem_escreve(self->mem, IRQ_END_A, -1);
}

// CARGA DE PROGRAMA {{{1

// carrega o programa na memória
// retorna o endereço de carga ou -1
static int so_carrega_programa(so_t *self, char *nome_do_executavel)
{
  // programa para executar na nossa CPU
  programa_t *prog = prog_cria(nome_do_executavel);
  if (prog == NULL) {
    console_printf("Erro na leitura do programa '%s'\n", nome_do_executavel);
    return -1;
  }

  int end_ini = prog_end_carga(prog);
  int end_fim = end_ini + prog_tamanho(prog);

  for (int end = end_ini; end < end_fim; end++) {
    if (mem_escreve(self->mem, end, prog_dado(prog, end)) != ERR_OK) {
      console_printf("Erro na carga da memória, endereco %d\n", end);
      return -1;
    }
  }
  prog_destroi(prog);
  console_printf("SO: carga de '%s' em %d-%d", nome_do_executavel, end_ini, end_fim);
  return end_ini;
}

// ACESSO À MEMÓRIA DOS PROCESSOS {{{1

// copia uma string da memória do simulador para o vetor str.
// retorna false se erro (string maior que vetor, valor não char na memória,
//   erro de acesso à memória)
// T1: deveria verificar se a memória pertence ao processo
static bool copia_str_da_mem(int tam, char str[tam], mem_t *mem, int ender)
{
  for (int indice_str = 0; indice_str < tam; indice_str++) {
    int caractere;
    if (mem_le(mem, ender + indice_str, &caractere) != ERR_OK) {
      return false;
    }
    if (caractere < 0 || caractere > 255) {
      return false;
    }
    str[indice_str] = caractere;
    if (caractere == 0) {
      return true;
    }
  }
  // estourou o tamanho de str
  return false;
}

// vim: foldmethod=marker
