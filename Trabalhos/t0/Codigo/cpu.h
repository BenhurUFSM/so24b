#ifndef CPU_H
#define CPU_H

// simulador da unidade de execução de instruções de uma CPU
// executa a instrução no PC se possível, ou retorna erro

// tem acesso a
// - memória, onde está o programa e os dados -- alterável pelas instruções
// - controlador de ES, para as instruções de ES

#include "err.h"
#include "memoria.h"
#include "es.h"

typedef struct cpu_t cpu_t; // tipo opaco

// cria uma unidade de execução com acesso à memória e ao
//   controlador de E/S fornecidos
cpu_t *cpu_cria(mem_t *mem, es_t *es);

// destrói a unidade de execução
void cpu_destroi(cpu_t *self);

// executa uma instrução
void cpu_executa_1(cpu_t *self);

err_t cpu_estado(cpu_t *self);

// concatena a descrição do estado da CPU no final de str
void cpu_concatena_descricao(cpu_t *self, char *str);

#endif // CPU_H
