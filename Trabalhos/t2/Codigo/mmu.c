// mmu.c
// unidade de gerenciamento de memória
// simulador de computador
// so24b

#include "mmu.h"
#include <stdlib.h>
#include <assert.h>

// tipo de dados opaco para representar uma MMU
struct mmu_t {
  // memória física
  mem_t *mem;
  // tabela de páginas
  tabpag_t *tabpag;
};

mmu_t *mmu_cria(mem_t *mem)
{
  mmu_t *self;
  self = malloc(sizeof(*self));
  assert(self != NULL);
  self->mem = mem;
  self->tabpag = NULL;
  return self;
}

void mmu_destroi(mmu_t *self)
{
  if (self != NULL) {
    // nem a tabela de páginas nem a memória pertencem à MMU, não são liberadas aqui
    free(self);
  }
}

void mmu_define_tabpag(mmu_t *self, tabpag_t *tabpag)
{
  self->tabpag = tabpag;
}

// tradur o endereço virtual 'endvirt', colocando o endereço físico
//   correspondente em 'pendfis'.
// retorna ERR_OK ou um erro se a tradução não for possível
static err_t mmu__traduz(mmu_t *self, int endvirt, int *pendfis)
{
  int pagina = endvirt / TAM_PAGINA;
  int deslocamento = endvirt % TAM_PAGINA;
  int quadro;
  err_t err = tabpag_traduz(self->tabpag, pagina, &quadro);
  if (err == ERR_OK) {
    *pendfis = quadro * TAM_PAGINA + deslocamento;
  }
  return err;
}

err_t mmu_le(mmu_t *self, int endvirt, int *pvalor, cpu_modo_t modo)
{
  // em modo supervisor ou se não tiver tabela de páginas,
  //   não faz tradução de endereços, nem marca o acesso
  if (modo == supervisor || self->tabpag == NULL) {
    return mem_le(self->mem, endvirt, pvalor);
  }
  int endfis;
  err_t err = mmu__traduz(self, endvirt, &endfis);
  if (err == ERR_OK) {
    err = mem_le(self->mem, endfis, pvalor);
    if (err == ERR_OK) {
      tabpag_marca_bit_acesso(self->tabpag, endvirt / TAM_PAGINA, false);
    }
  }
  return err;
}

err_t mmu_escreve(mmu_t *self, int endvirt, int valor, cpu_modo_t modo)
{
  // em modo supervisor ou se não tiver tabela de páginas,
  //   não faz tradução de endereços, nem marca o acesso
  if (modo == supervisor || self->tabpag == NULL) {
    return mem_escreve(self->mem, endvirt, valor);
  }
  int endfis;
  err_t err = mmu__traduz(self, endvirt, &endfis);
  if (err == ERR_OK) {
    err = mem_escreve(self->mem, endfis, valor);
    if (err == ERR_OK) {
      tabpag_marca_bit_acesso(self->tabpag, endvirt / TAM_PAGINA, true);
    }
  }
  return err;
}
