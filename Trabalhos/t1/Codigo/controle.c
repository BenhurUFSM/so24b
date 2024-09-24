// controle.c
// unidade de controle da CPU
// simulador de computador
// so24b

#include "controle.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

struct controle_t {
  cpu_t *cpu;
  relogio_t *relogio;
  console_t *console;
  enum { executando, passo, parado, fim } estado;
};

// funções auxiliares
static void controle_processa_comandos_da_console(controle_t *self);
static void controle_atualiza_estado_na_console(controle_t *self);


controle_t *controle_cria(cpu_t *cpu, console_t *console, relogio_t *relogio)
{
  controle_t *self = malloc(sizeof(*self));
  assert(self != NULL);

  self->cpu = cpu;
  self->console = console;
  self->relogio = relogio;
  self->estado = parado;

  return self;
}

void controle_destroi(controle_t *self)
{
  free(self);
}

void controle_laco(controle_t *self)
{
  // executa uma instrução por vez até a console dizer que chega
  do {
    if (self->estado == passo || self->estado == executando) {
      cpu_executa_1(self->cpu);
      relogio_tictac(self->relogio);

      if (self->estado == passo) self->estado = parado;

      // enquanto não tem controlador de interrupção, fala direto com o relógio
      // o dispositivo 3 do relógio contém 1 se o timer expirou
      int tem_int;
      relogio_leitura(self->relogio, 3, &tem_int);
      if (tem_int != 0) {
        cpu_interrompe(self->cpu, IRQ_RELOGIO);
      }
    }
    console_tictac(self->console);

    controle_processa_comandos_da_console(self);
    controle_atualiza_estado_na_console(self);
  } while (self->estado != fim);

  console_printf("Fim da execução.");
  console_printf("relógio: %d\n", relogio_agora(self->relogio));
}
 

static void controle_processa_comandos_da_console(controle_t *self)
{
  char cmd = console_comando_externo(self->console);
  switch (cmd) {
    case 'F':
      self->estado = fim;
      break;
    case 'P':
      self->estado = parado;
      break;
    case '1':
      if (self->estado == parado) self->estado = passo;
      break;
    case 'C':
      self->estado = executando;
      break;
  }
}

static void controle_atualiza_estado_na_console(controle_t *self)
{
  char status[100];
  switch (self->estado) {
    case fim:        strcpy(status, "FIM    | "); break;
    case parado:     strcpy(status, "PARADO | "); break;
    case executando: strcpy(status, "EXEC   | "); break;
    case passo:      strcpy(status, "PASSO  | "); break;
  }
  cpu_concatena_descricao(self->cpu, status);
  console_print_status(self->console, status);
}
