// console.c
// console do operador e controle de terminais
// simulador de computador
// so24b

// INCLUDES {{{1

#include "console.h"
#include "terminal.h"
#include "tela.h"

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>

// CONSTANTES {{{1

// tamanho da tela -- a janela do terminal tem que ter pelo menos esse tamanho
// altere caso queira mais linhas (ou menos)
#define N_LIN 24  // número de linhas na tela
#define N_COL 80  // número de colunas na tela

// número de linhas para cada componente da tela
#define N_TERM        4  // número de terminais, cada um ocupa 2 linhas na tela
#define N_LIN_TERM    (N_TERM * 2)
#define N_LIN_STATUS  1
#define N_LIN_ENTRADA 1
#define N_LIN_CONSOLE (N_LIN - N_LIN_TERM - N_LIN_STATUS - N_LIN_ENTRADA)

// linha onde começa cada componente
#define LINHA_TERM    0
#define LINHA_STATUS  (LINHA_TERM + N_LIN_TERM)
#define LINHA_CONSOLE (LINHA_STATUS + N_LIN_STATUS)
#define LINHA_ENTRADA (LINHA_CONSOLE + N_LIN_CONSOLE)

// números de comandos para o controlador que podem ser guardados na console
#define N_CMD_EXT 10

// DECLARAÇÃO {{{1

struct console_t {
  terminal_t *term[N_TERM];
  int cor_txt[N_TERM];
  int cor_cursor[N_TERM];
  char txt_status[N_COL+1];
  char txt_console[N_LIN_CONSOLE][N_COL+1];
  char txt_entrada[N_COL+1];
  char fila_de_comandos_externos[N_CMD_EXT];
  FILE *arquivo_de_log;
};

// CRIAÇÃO {{{1

static console_t *console_global; // gambiarra para simplificar o uso de prints na console
console_t *console_cria(void)
{
  console_t *self = malloc(sizeof(*self));
  assert(self != NULL);
  console_global = self;

  for (int t = 0; t < N_TERM; t++) {
    self->term[t] = terminal_cria(N_COL);
    if ((t % 2) == 0) {
      self->cor_txt[t] = COR_TXT_PAR;
      self->cor_cursor[t] = COR_CURSOR_PAR;
    } else {
      self->cor_txt[t] = COR_TXT_IMPAR;
      self->cor_cursor[t] = COR_CURSOR_IMPAR;
    }
  }
  for (int l = 0; l < N_LIN_CONSOLE; l++) {
    strcpy(self->txt_console[l], "");
  }
  strcpy(self->txt_entrada, "");
  self->fila_de_comandos_externos[0] = '\0';
  self->arquivo_de_log = fopen("log_da_console", "w");

  tela_init();

  return self;
}

static void console_desenha(console_t *self);

void console_destroi(console_t *self)
{
  console_desenha(self);
  if (self->arquivo_de_log != NULL) fclose(self->arquivo_de_log);
  tela_puts(COR_OCUPADO, "  digite ENTER para sair  ");
  tela_atualiza();
  while (tela_tecla() != '\n') {
    ;
  }
  tela_fim();

  for (int t = 0; t < N_TERM; t++) {
    terminal_destroi(self->term[t]);
  }
  free(self);
  return;
}

// TERMINAIS {{{1

terminal_t *console_terminal(console_t *self, char id_terminal)
{
  int num_terminal = tolower(id_terminal) - 'a';
  if (num_terminal < 0 || num_terminal >= N_TERM) return NULL;
  return self->term[num_terminal];
}

static void atualiza_terminais(console_t *self)
{
  for (int t = 0; t < N_TERM; t++) {
    terminal_tictac(self->term[t]);
  }
}

static void insere_string_no_terminal(console_t *self, char id_terminal, char *str)
{
  // insere caracteres no terminal (e espaço no final)
  terminal_t *terminal = console_terminal(self, id_terminal);
  if (terminal == NULL) {
    console_printf("Terminal '%c' inválido\n", id_terminal);
    return;
  }
  char *p = str;
  while (*p != '\0') {
    terminal_insere_char(terminal, *p);
    p++;
  }
  terminal_insere_char(terminal, ' ');
}

static void limpa_saida_do_terminal(console_t *self, char id_terminal)
{
  terminal_t *terminal = console_terminal(self, id_terminal);
  if (terminal == NULL) {
    console_printf("Terminal '%c' inválido\n", id_terminal);
    return;
  }
  terminal_limpa_saida(terminal);
}

// SAÍDA {{{1

static void insere_string_na_console(console_t *self, char *s)
{
  for(int l=0; l<N_LIN_CONSOLE-1; l++) {
    strncpy(self->txt_console[l], self->txt_console[l+1], N_COL);
    self->txt_console[l][N_COL] = '\0'; // quem definiu strncpy é estúpido!
  }
  strncpy(self->txt_console[N_LIN_CONSOLE-1], s, N_COL);
  self->txt_console[N_LIN_CONSOLE-1][N_COL] = '\0'; // grrrr
  if (self->arquivo_de_log != NULL) {
    fprintf(self->arquivo_de_log, "%s\n", s);
  }
}

static void insere_strings_na_console(console_t *self, char *s)
{
  if (*s == '\0') return;
  char *f = strchr(s, '\n');
  if (f == NULL) {
    // não tem \n, insere a string e tá pronto
    insere_string_na_console(self, s);
    return;
  }
  // tem \n, corta a string, insere e depois insere o resto recursivamente
  *f = '\0';
  insere_string_na_console(self, s);
  insere_strings_na_console(self, f + 1);
}

void console_print_status(console_t *self, char *txt)
{
  // imprime alinhado a esquerda ("-"), max N_COL chars ("*")
  sprintf(self->txt_status, "%-*s", N_COL, txt);
}

int console_printf(char *formato, ...)
{
  // esta função usa número variável de argumentos, como o printf.
  // Se não sabe como é isso, dá uma olhada em:
  // https://www.geeksforgeeks.org/variadic-functions-in-c/
  console_t *self = console_global; // gambiarra para simplificar o uso de prints na console
  char s[sizeof(self->txt_console)];
  va_list arg;
  va_start(arg, formato);
  int r = vsnprintf(s, sizeof(s), formato, arg);
  insere_strings_na_console(self, s);
  return r;
}

// ENTRADA {{{1

static void insere_comando_externo(console_t *self, char c)
{
  int n_cmd = strlen(self->fila_de_comandos_externos);
  if (n_cmd < N_CMD_EXT - 2) {
    self->fila_de_comandos_externos[n_cmd] = c;
    self->fila_de_comandos_externos[n_cmd+1] = '\0';
  }
}

static char remove_comando_externo(console_t *self)
{
  char *p = self->fila_de_comandos_externos;
  char cmd = *p;
  if (cmd != '\0') {
    memmove(p, p+1, strlen(p));
  }
  return cmd;
}

static void interpreta_linha_entrada(console_t *self)
{
  // interpreta uma linha digitada pelo operador
  // Comandos aceitos:
  // Etstr entra a string 'str' no terminal 't'  ex: eb30
  // Zt    esvazia a saída do terminal 't'  ex: za
  // Dn    altera o tempo de espera do teclado  ex: d0  -> modo turbo
  // P     para a execução
  // 1     executa uma instrução
  // C     continua a execução
  // F     fim da simulação

  char *linha = self->txt_entrada;
  console_printf("CMD: '%s'", linha);
  char cmd = toupper(linha[0]);
  int val;
  switch (cmd) {
    case 'E':
      insere_string_no_terminal(self, linha[1], &linha[2]);
      break;
    case 'Z':
      limpa_saida_do_terminal(self, linha[1]);
      break;
    case 'D':
      val = atoi(&linha[1]);
      tela_espera(val);
      break;
    case 'P':
    case '1':
    case 'C':
    case 'F':
      insere_comando_externo(self, cmd);
      break;
    default:
      console_printf("Comando '%c' não reconhecido", cmd);
  }
  strcpy(self->txt_entrada, "");
}

// lê e guarda um caractere do teclado; interpreta linha se for 'enter'
static void verifica_entrada(console_t *self)
{
  char ch = tela_tecla();

  int l = strlen(self->txt_entrada);

  if (ch == '\b' || ch == 127) {   // backspace ou del
    if (l > 0) {
      self->txt_entrada[l - 1] = '\0';
    }
  } else if (ch == '\n') {
    interpreta_linha_entrada(self);
  } else if (ch >= ' ' && ch < 127 && l < N_COL) {
    self->txt_entrada[l] = ch;
    self->txt_entrada[l+1] = '\0';
  } // senão, ignora o caractere digitado
}

char console_comando_externo(console_t *self)
{
  verifica_entrada(self);
  return remove_comando_externo(self);
}

// DESENHO {{{1

static void desenha_linha_terminal(char *txt, int linha, int cor_txt, int cor_cursor)
{
  tela_posiciona(linha, 0);
  tela_puts(cor_txt, txt);
  tela_limpa_linha();
  tela_puts(cor_cursor, " ");
}

static void desenha_terminais(console_t *self)
{
  for (int t = 0; t < N_TERM; t++) {
    terminal_t *terminal = self->term[t];
    int cor_txt = self->cor_txt[t];
    int cor_cursor = self->cor_cursor[t];
    int linha = LINHA_TERM + t * 2;
    desenha_linha_terminal(terminal_txt_entrada(terminal), linha, cor_txt, cor_cursor);
    desenha_linha_terminal(terminal_txt_saida(terminal), linha+1, cor_txt, cor_cursor);
  }
}

static void desenha_status(console_t *self)
{
  tela_posiciona(LINHA_STATUS, 0);
  tela_puts(COR_STATUS, self->txt_status);
  tela_limpa_linha();
}

static void desenha_console(console_t *self)
{
  for (int l=0; l<N_LIN_CONSOLE; l++) {
    tela_posiciona(LINHA_CONSOLE + l, 0);
    tela_puts(COR_CONSOLE, self->txt_console[l]);
    tela_limpa_linha();
  }
}

static void desenha_entrada(console_t *self)
{
  char txt_fixo[] = "P=para C=continua 1=passo F=fim  Ets=entra Zt=zera";
  tela_posiciona(LINHA_ENTRADA, 0);
  tela_puts(COR_ENTRADA, ""); // gambiarra para limpar na cor certa
  tela_limpa_linha();
  tela_posiciona(LINHA_ENTRADA, N_COL - sizeof(txt_fixo));
  tela_puts(COR_ENTRADA, txt_fixo);
  tela_posiciona(LINHA_ENTRADA, 0);
  tela_puts(COR_ENTRADA, self->txt_entrada);
}

static void console_desenha(console_t *self)
{
  desenha_terminais(self);
  desenha_status(self);
  desenha_console(self);
  desenha_entrada(self);

  // faz aparecer tudo que foi desenhado
  tela_atualiza();
}

// TICTAC {{{1
void console_tictac(console_t *self)
{
  verifica_entrada(self);
  atualiza_terminais(self);
  console_desenha(self);
}

// vim: foldmethod=marker
