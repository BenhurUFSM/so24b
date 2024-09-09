// terminal.c
// entrada e saída em um terminal
// simulador de computador
// so24b

#include "terminal.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

// TERMINAL

// dados para cada terminal
struct terminal_t {
  // número de caracteres que cabem em uma linha
  int tam_linha;
  // texto já digitado no terminal, esperando para ser lido
  char *entrada;
  // texto sendo mostrado na saída do terminal
  char *saida;
  // normal: aceitando novos caracteres na saída
  // rolando: removendo um caractere no início para gerar espaço.
  //   move um caractere por vez para a esquerda, até chegar no final
  //   da linha.
  //   entra neste estado quando recebe um caractere na última posição.
  //   não aceita novos caracteres
  // limpando: removendo um caractere no início da linha por vez, até
  //   ficar com a linha vazia.
  //   entra nesse estado quando recebe um '\n'.
  //   não aceita novos caracteres
  enum { normal, rolando, limpando } estado_saida;
  // posicao do caractere que está sendo movido durante uma rolagem
  int pos_rolagem;
};


terminal_t *terminal_cria(int tam_linha)
{
  terminal_t *self = malloc(sizeof(*self));
  assert(self != NULL);

  self->saida = malloc(tam_linha + 1);
  self->entrada = malloc(tam_linha + 1);
  assert(self->saida != NULL && self->entrada != NULL);

  self->tam_linha = tam_linha;
  strcpy(self->entrada, "");
  strcpy(self->saida, "");
  self->estado_saida = normal;

  return self;
}

void terminal_destroi(terminal_t *self)
{
  free(self->entrada);
  free(self->saida);
  free(self);
}

static bool terminal_entrada_vazia(terminal_t *self)
{
  return self->entrada[0] == '\0';
}

static char terminal_le_char(terminal_t *self)
{
  char *p = self->entrada;
  char ch = p[0];
  if (ch != '\0') {
    memmove(&p[0], &p[1], strlen(p));
  }
  return ch;
}

void terminal_insere_char(terminal_t *self, char ch)
{
  char *p = self->entrada;
  int tam = strlen(p);
  // se não cabe, ignora silenciosamente
  if (tam >= self->tam_linha-2) return;
  p[tam] = ch;
  p[tam+1] = '\0';
}

static bool terminal_pode_imprimir(terminal_t *self)
{
  return self->estado_saida == normal;
}

static void terminal_imprime(terminal_t *self, char ch)
{
  if (terminal_pode_imprimir(self)) {
    if (ch == '\n') {
      self->estado_saida = limpando;
      return;
    }
    int tam = strlen(self->saida);
    self->saida[tam] = ch;
    tam++;
    self->saida[tam] = '\0';
    if (tam >= self->tam_linha - 1) {
      self->estado_saida = rolando;
      self->pos_rolagem = 0;
    }
  }
}

void terminal_limpa_saida(terminal_t *self)
{
  self->saida[0] = '\0';
  self->estado_saida = normal;
}

static void terminal_atualiza_rolagem(terminal_t *self)
{
  // remove o caractere na posição de rolagem e avança
  // se chegou no final da string, terminou a rolagem
  char *p = self->saida;
  p[self->pos_rolagem] = p[self->pos_rolagem + 1];
  if (p[self->pos_rolagem] != '\0') {
    self->pos_rolagem++;
    p[self->pos_rolagem] = ' ';
  } else {
    self->estado_saida = normal;
  }
}

static void terminal_atualiza_limpeza(terminal_t *self)
{
  // remove um caractere do início da string; volta ao estado normal se era o último
  char *p = self->saida;
  int tam = strlen(p);
  memmove(p, p+1, tam);
  if (tam <= 1) {
    self->estado_saida = normal;
  }
}

// altera a string de saída em 1 caractere, se estiver rolando ou limpando
void terminal_tictac(terminal_t *self)
{
  switch (self->estado_saida) {
    case normal: 
      break;
    case rolando:
      terminal_atualiza_rolagem(self);
      break;
    case limpando:
      terminal_atualiza_limpeza(self);
      break;
  }
}

char *terminal_txt_entrada(terminal_t *self)
{
  return self->entrada;
}

char *terminal_txt_saida(terminal_t *self)
{
  return self->saida;
}

// Operações de leitura e escrita no terminal, chamadas pelo controlador de E/S
// Para o controlador, cada terminal é composto por 4 dispositivos:
//   leitura, estado da leitura, escrita, estado da escrita
err_t terminal_leitura(void *disp, int id, int *pvalor)
{
  terminal_t *self = disp;

  switch (id % 4) {
    case 0: // leitura do teclado
      if (terminal_entrada_vazia(self)) return ERR_OCUP;
      *pvalor = terminal_le_char(self);
      break;
    case 1: // estado do teclado
      if (terminal_entrada_vazia(self)) {
        *pvalor = 0;
      } else {
        *pvalor = 1;
      }
      break;
    case 2: // escrita na tela
      return ERR_OP_INV;
    case 3: // estado da tela
      if (terminal_pode_imprimir(self)) {
        *pvalor = 1;
      } else {
        *pvalor = 0;
      }
      break;
    default:
      return ERR_DISP_INV;
  }
  return ERR_OK;
}

err_t terminal_escrita(void *disp, int id, int valor)
{
  terminal_t *self = disp;
  switch (id % 4) {
    case 0: // leitura do teclado
      return ERR_OP_INV;
    case 1: // estado do teclado
      return ERR_OP_INV;
    case 2: // escrita na tela
      if (!terminal_pode_imprimir(self)) return ERR_OCUP;
      terminal_imprime(self, valor);
      break;
    case 3: // estado da tela
      return ERR_OP_INV;
    default:
      return ERR_DISP_INV;
  }
  return ERR_OK;
}
