#include "es.h"
#include <stdio.h>
#include <stdlib.h>

// estrutura para definir um dispositivo
typedef struct {
   f_leitura_t f_leitura; // função para ler um inteiro do dispositivo
   f_escrita_t f_escrita; // função para escrever um inteiro no dispositivo
   void *controladora;    // descritor do dispositivo (arg das f acima)
   int id;                // identificador do (sub)dispositivo (arg das f acima)
} dispositivo_t;

#define N_DISPOSITIVOS 100 // número máximo de dispositivos suportados

// define a estrutura opaca
struct es_t {
  dispositivo_t dispositivos[N_DISPOSITIVOS];
};

es_t *es_cria(void)
{
  es_t *self = calloc(1, sizeof(*self)); // com calloc já zera toda a struct
  return self;
}

void es_destroi(es_t *self)
{
  free(self);
}

bool es_registra_dispositivo(es_t *self, int dispositivo,
                             void *controladora, int id,
                             f_leitura_t f_leitura, f_escrita_t f_escrita)
{
  if (dispositivo < 0 || dispositivo >= N_DISPOSITIVOS) return false;
  self->dispositivos[dispositivo].controladora = controladora;
  self->dispositivos[dispositivo].id = id;
  self->dispositivos[dispositivo].f_leitura = f_leitura;
  self->dispositivos[dispositivo].f_escrita = f_escrita;
  return true;
}

err_t es_le(es_t *self, int dispositivo, int *pvalor)
{
  if (dispositivo < 0 || dispositivo >= N_DISPOSITIVOS) return ERR_DISP_INV;
  if (self->dispositivos[dispositivo].f_leitura == NULL) return ERR_OP_INV;
  void *controladora = self->dispositivos[dispositivo].controladora;
  int id = self->dispositivos[dispositivo].id;
  return self->dispositivos[dispositivo].f_leitura(controladora, id, pvalor);
}

err_t es_escreve(es_t *self, int dispositivo, int valor)
{
  if (dispositivo < 0 || dispositivo >= N_DISPOSITIVOS) return ERR_DISP_INV;
  if (self->dispositivos[dispositivo].f_escrita == NULL) return ERR_OP_INV;
  void *controladora = self->dispositivos[dispositivo].controladora;
  int id = self->dispositivos[dispositivo].id;
  return self->dispositivos[dispositivo].f_escrita(controladora, id, valor);
}
