// cpu.h
// executor de instruções da CPU
// simulador de computador
// so24b

#ifndef CPU_H
#define CPU_H

#include "memoria.h"
#include "es.h"
#include "err.h"
#include "irq.h"

typedef struct cpu_t cpu_t; // tipo opaco

// os modos de execução da CPU -- normalmente seria interno a CPU.c, mas o SO vai precisar disso
typedef enum { supervisor, usuario } cpu_modo_t;

// cria uma unidade de execução com acesso à memória e ao
//   controlador de E/S fornecidos
cpu_t *cpu_cria(mem_t *mem, es_t *es);

// destrói a unidade de execução
void cpu_destroi(cpu_t *self);

// executa a instrução apontada pelo PC
//   se a CPU estiver em erro, não executa
//   se a execução causar algum erro, altera o estado da CPU
void cpu_executa_1(cpu_t *self);

// implementa uma interrupção
// passa para modo supervisor, salva o estado da CPU no início da memória,
//   altera A para identificar a requisição de interrupção, altera PC para
//   o endereço do tratador de interrupção
// retorna true se interrupção foi aceita ou false caso contrário
bool cpu_interrompe(cpu_t *self, irq_t irq);

// concatena a descrição do estado da CPU no final de str
void cpu_concatena_descricao(cpu_t *self, char *str);

#endif // CPU_H
