#ifndef ALEATORIO_H
#define ALEATORIO_H

#include "err.h"

// Estrutura que representa o dispositivo de números aleatórios
typedef struct {
  unsigned int seed;
} dispositivo_random_t;

// Função que cria o dispositivo de números aleatórios
dispositivo_random_t *random_cria(void);

// Função que destrói o dispositivo de números aleatórios
void random_destroi(dispositivo_random_t *random);

// Função de leitura do dispositivo de números aleatórios
err_t random_leitura(void *disp, int id_reg, int *valor);

#endif // ALEATORIO_H
