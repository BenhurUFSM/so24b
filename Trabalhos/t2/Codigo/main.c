// main.c
// inicializa e encerra a simulação
// simulador de computador
// so24b

#include "controle.h"
#include "memoria.h"
#include "mmu.h"
#include "cpu.h"
#include "relogio.h"
#include "console.h"
#include "terminal.h"
#include "es.h"
#include "dispositivos.h"
#include "so.h"

#include <stdio.h>
#include <stdlib.h>

// constantes
#define MEM_TAM 10000        // tamanho da memória principal

// estrutura com os componentes do computador simulado
typedef struct {
  mem_t *mem;
  mmu_t *mmu;
  cpu_t *cpu;
  relogio_t *relogio;
  console_t *console;
  es_t *es;
  controle_t *controle;
} hardware_t;

static void cria_hardware(hardware_t *hw)
{
  // cria a memória e a MMU
  hw->mem = mem_cria(MEM_TAM);
  hw->mmu = mmu_cria(hw->mem);

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
  // lê teclado, testa teclado, escreve tela, testa tela do terminal C
  terminal = console_terminal(hw->console, 'C');
  es_registra_dispositivo(hw->es, D_TERM_C_TECLADO    , terminal, 0, terminal_leitura, NULL);
  es_registra_dispositivo(hw->es, D_TERM_C_TECLADO_OK , terminal, 1, terminal_leitura, NULL);
  es_registra_dispositivo(hw->es, D_TERM_C_TELA       , terminal, 2, NULL, terminal_escrita);
  es_registra_dispositivo(hw->es, D_TERM_C_TELA_OK    , terminal, 3, terminal_leitura, NULL);
  // lê teclado, testa teclado, escreve tela, testa tela do terminal D
  terminal = console_terminal(hw->console, 'D');
  es_registra_dispositivo(hw->es, D_TERM_D_TECLADO    , terminal, 0, terminal_leitura, NULL);
  es_registra_dispositivo(hw->es, D_TERM_D_TECLADO_OK , terminal, 1, terminal_leitura, NULL);
  es_registra_dispositivo(hw->es, D_TERM_D_TELA       , terminal, 2, NULL, terminal_escrita);
  es_registra_dispositivo(hw->es, D_TERM_D_TELA_OK    , terminal, 3, terminal_leitura, NULL);
  // lê relógio virtual, relógio real
  es_registra_dispositivo(hw->es, D_RELOGIO_INSTRUCOES, hw->relogio, 0, relogio_leitura, NULL);
  es_registra_dispositivo(hw->es, D_RELOGIO_REAL      , hw->relogio, 1, relogio_leitura, NULL);
  es_registra_dispositivo(hw->es, D_RELOGIO_TIMER     , hw->relogio, 2, relogio_leitura, relogio_escrita);
  es_registra_dispositivo(hw->es, D_RELOGIO_INTERRUPCAO,hw->relogio, 3, relogio_leitura, relogio_escrita);

  // cria a unidade de execução e inicializa com a MMU e E/S
  hw->cpu = cpu_cria(hw->mmu, hw->es);

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
  mmu_destroi(hw->mmu);
  mem_destroi(hw->mem);
}

int main()
{
  hardware_t hw;
  so_t *so;

  // cria o hardware
  cria_hardware(&hw);
  // cria o sistema operacional
  so = so_cria(hw.cpu, hw.mem, hw.mmu, hw.es, hw.console);
  
  // executa o laço principal do controlador
  controle_laco(hw.controle);

  // destroi tudo
  so_destroi(so);
  destroi_hardware(&hw);
}

