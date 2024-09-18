// main.c
// inicializa e encerra a simulação
// simulador de computador
// so24b

#include "controle.h"
#include "programa.h"
#include "memoria.h"
#include "cpu.h"
#include "relogio.h"
#include "console.h"
#include "terminal.h"
#include "es.h"
#include "dispositivos.h"
#include "aleatorio.h"

#include <stdio.h>
#include <stdlib.h>

// constantes
#define MEM_TAM 2000        // tamanho da memória principal

// estrutura com os componentes do computador simulado
typedef struct {
  mem_t *mem;
  cpu_t *cpu;
  relogio_t *relogio;
  console_t *console;
  es_t *es;
  controle_t *controle;
  dispositivo_random_t *random;
} hardware_t;

static void cria_hardware(hardware_t *hw)
{
  // cria a memória
  hw->mem = mem_cria(MEM_TAM);

  // cria dispositivos de E/S
  hw->console = console_cria();
  hw->relogio = relogio_cria();

  // cria o controlador de E/S e registra os dispositivos
  //   por exemplo, o dispositivo 8 do controlador de E/S (e da CPU) será o
  //   dispositivo 0 do relógio (que é o contador de instruções)
  hw->es = es_cria();
  // lê teclado, testa teclado, escreve tela, testa tela do terminal A
  terminal_t *terminal;
  terminal = console_terminal(hw->console, 'A');
  es_registra_dispositivo(hw->es, D_TERM_A_TECLADO    , terminal, 0, terminal_leitura, NULL);
  es_registra_dispositivo(hw->es, D_TERM_A_TECLADO_OK , terminal, 1, terminal_leitura, NULL);
  es_registra_dispositivo(hw->es, D_TERM_A_TELA       , terminal, 2, NULL, terminal_escrita);
  es_registra_dispositivo(hw->es, D_TERM_A_TELA_OK    , terminal, 3, terminal_leitura, NULL);
  // lê teclado, testa teclado, escreve tela, testa tela do terminal B
  terminal = console_terminal(hw->console, 'B');
  es_registra_dispositivo(hw->es, D_TERM_B_TECLADO    , terminal, 0, terminal_leitura, NULL);
  es_registra_dispositivo(hw->es, D_TERM_B_TECLADO_OK , terminal, 1, terminal_leitura, NULL);
  es_registra_dispositivo(hw->es, D_TERM_B_TELA       , terminal, 2, NULL, terminal_escrita);
  es_registra_dispositivo(hw->es, D_TERM_B_TELA_OK    , terminal, 3, terminal_leitura, NULL);
  // lê relógio virtual, relógio real
  es_registra_dispositivo(hw->es, D_RELOGIO_INSTRUCOES, hw->relogio, 0, relogio_leitura, NULL);
  es_registra_dispositivo(hw->es, D_RELOGIO_REAL      , hw->relogio, 1, relogio_leitura, NULL);

   hw->random = random_cria();  // Cria o dispositivo aleatório
  es_registra_dispositivo(hw->es, D_ALEATORIO, hw->random, 0, random_leitura, NULL);  // Registra o dispositivo aleatório
  


  // cria a unidade de execução e inicializa com a memória e o controlador de E/S
  hw->cpu = cpu_cria(hw->mem, hw->es);

  // cria o controlador da CPU e inicializa com a unidade de execução, a console e
  //   o relógio
  hw->controle = controle_cria(hw->cpu, hw->console, hw->relogio);
}

static void destroi_hardware(hardware_t *hw)
{
  controle_destroi(hw->controle);
  cpu_destroi(hw->cpu);
  es_destroi(hw->es);
  relogio_destroi(hw->relogio);
  console_destroi(hw->console);
  mem_destroi(hw->mem);
  random_destroi(hw->random);  // Destroi o dispositivo aleatório
}

// inicializa a memória com o conteúdo do programa
static void init_mem(mem_t *mem, char *nome_do_executavel)
{
  // programa para executar na nossa CPU
  programa_t *prog = prog_cria(nome_do_executavel);
  if (prog == NULL) {
    fprintf(stderr, "Erro na leitura do programa '%s'\n", nome_do_executavel);
    exit(1);
  }

  int end_ini = prog_end_carga(prog);
  int end_fim = end_ini + prog_tamanho(prog);

  for (int end = end_ini; end < end_fim; end++) {
    if (mem_escreve(mem, end, prog_dado(prog, end)) != ERR_OK) {
      printf("Erro na carga da memória, endereco %d\n", end);
      exit(1);
    }
  }
  prog_destroi(prog);
}

int main(int argc, char *argv[])
{
  hardware_t hw;
  char *nome_do_programa = "ex1.maq";
  if (argc > 1) nome_do_programa = argv[1];

  // cria o hardware
  cria_hardware(&hw);
  // coloca um programa na memória
  init_mem(hw.mem, nome_do_programa);

  // executa o laço principal do controlador
  controle_laco(hw.controle);

  // destroi tudo
  destroi_hardware(&hw);
}
