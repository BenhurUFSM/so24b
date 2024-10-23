// controle.h
// unidade de controle da CPU
// simulador de computador
// so24b

#ifndef CONTROLE_H
#define CONTROLE_H

typedef struct controle_t controle_t;

#include "cpu.h"
#include "console.h"
#include "relogio.h"

controle_t *controle_cria(cpu_t *cpu, console_t *console, relogio_t *relogio);
void controle_destroi(controle_t *self);

// o laço principal da simulação
void controle_laco(controle_t *self);

#endif // CONTROLE_H
