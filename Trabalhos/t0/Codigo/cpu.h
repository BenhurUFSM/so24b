// cpu.h
// executor de instruções da CPU
// simulador de computador
// so24b

#ifndef CPU_H
#define CPU_H

#include "memoria.h"
#include "es.h"
#include "err.h"

typedef struct cpu_t cpu_t; // tipo opaco

// cria uma unidade de execução com acesso à memória e ao
//   controlador de E/S fornecidos
cpu_t *cpu_cria(mem_t *mem, es_t *es);

// destrói a unidade de execução
void cpu_destroi(cpu_t *self);

// executa a instrução apontada pelo PC
//   se a CPU estiver em erro, não executa
//   se a execução causar algum erro, altera o estado da CPU
void cpu_executa_1(cpu_t *self);

// retorna o estado da CPU (ERR_OK ou erro)
err_t cpu_estado(cpu_t *self);

// concatena a descrição do estado da CPU no final de str
void cpu_concatena_descricao(cpu_t *self, char *str);

#endif // CPU_H
