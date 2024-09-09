// relogio.c
// dispositivo de E/S para contar instruções executadas
// simulador de computador
// so24b

#include "relogio.h"

#include <stdlib.h>
#include <time.h>
#include <assert.h>

struct relogio_t {
  // que horas são (em tics)
  int agora;
};

relogio_t *relogio_cria(void)
{
  relogio_t *self;
  self = malloc(sizeof(relogio_t));
  assert(self != NULL);

  self->agora = 0;

  return self;
}

void relogio_destroi(relogio_t *self)
{
  free(self);
}

void relogio_tictac(relogio_t *self)
{
  self->agora++;
}

int relogio_agora(relogio_t *self)
{
  return self->agora;
}

err_t relogio_leitura(void *disp, int id, int *pvalor)
{
  relogio_t *self = disp;
  err_t err = ERR_OK;
  switch (id) {
    case 0:
      *pvalor = self->agora;
      break;
    case 1:
      *pvalor = clock()/(CLOCKS_PER_SEC/1000);
      break;
    default: 
      err = ERR_END_INV;
  }
  return err;
}
