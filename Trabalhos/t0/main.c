#include "controle.h"
#include "programa.h"
#include "memoria.h"
#include "cpu.h"
#include "relogio.h"
#include "console.h"
#include "terminal.h"

#include <stdio.h>
#include <stdlib.h>

// constantes
#define MEM_TAM 2000        // tamanho da memória principal

// funções auxiliares
static void init_mem(mem_t *mem, char *nome_do_executavel);
static void cria_hardware();
static void destroi_hardware();

// estrutura global que contém os componentes do computador simulado
static struct {
  mem_t *mem;
  cpu_t *cpu;
  relogio_t *relogio;
  console_t *console;
  es_t *es;
  controle_t *controle;
} hardware;

static void cria_hardware()
{
  // cria a memória
  hardware.mem = mem_cria(MEM_TAM);

  // cria dispositivos de E/S
  hardware.console = console_cria();
  hardware.relogio = relogio_cria();

  // cria o controlador de E/S e registra os dispositivos
  //   por exemplo, o dispositivo 8 do controlador de E/S (e da CPU) será o
  //   dispositivo 0 do relógio (que é o contador de instruções)
  hardware.es = es_cria();
  // lê teclado, testa teclado, escreve tela, testa tela do terminal A
  terminal_t *terminal;
  terminal = console_terminal(hardware.console, 'A');
  es_registra_dispositivo(hardware.es, 0, terminal, 0, terminal_leitura, NULL);
  es_registra_dispositivo(hardware.es, 1, terminal, 1, terminal_leitura, NULL);
  es_registra_dispositivo(hardware.es, 2, terminal, 2, NULL, terminal_escrita);
  es_registra_dispositivo(hardware.es, 3, terminal, 3, terminal_leitura, NULL);
  // lê teclado, testa teclado, escreve tela, testa tela do terminal B
  terminal = console_terminal(hardware.console, 'B');
  es_registra_dispositivo(hardware.es, 4, terminal, 0, terminal_leitura, NULL);
  es_registra_dispositivo(hardware.es, 5, terminal, 1, terminal_leitura, NULL);
  es_registra_dispositivo(hardware.es, 6, terminal, 2, NULL, terminal_escrita);
  es_registra_dispositivo(hardware.es, 7, terminal, 3, terminal_leitura, NULL);
  // lê relógio virtual, relógio real
  es_registra_dispositivo(hardware.es, 8, hardware.relogio, 0, relogio_leitura, NULL);
  es_registra_dispositivo(hardware.es, 9, hardware.relogio, 1, relogio_leitura, NULL);

  // cria a unidade de execução e inicializa com a memória e o controlador de E/S
  hardware.cpu = cpu_cria(hardware.mem, hardware.es);

  // cria o controlador da CPU e inicializa com a unidade de execução, a console e
  //   o relógio
  hardware.controle = controle_cria(hardware.cpu, hardware.console, hardware.relogio);
}

static void destroi_hardware()
{
  controle_destroi(hardware.controle);
  cpu_destroi(hardware.cpu);
  es_destroi(hardware.es);
  relogio_destroi(hardware.relogio);
  console_destroi(hardware.console);
  mem_destroi(hardware.mem);
}

int main(int argc, char *argv[])
{
  char *nome_do_programa = "ex1.maq";
  if (argc > 1) nome_do_programa = argv[1];
  // cria o hardware
  cria_hardware();
  // coloca um programa na memória
  init_mem(hardware.mem, nome_do_programa);
  // executa o laço principal do controlador
  controle_laco(hardware.controle);
  // destrói tudo
  destroi_hardware();
}


// cria memória e inicializa com o conteúdo do programa
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
