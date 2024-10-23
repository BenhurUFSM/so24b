// tela_curses.c
// entrada e saída no terminal físico, usando curses
// simulador de computador
// so24b

// se quiser se livrar do curses, é aqui que tem que mexer

#include "tela.h"

#include <curses.h>
#include <locale.h>

void tela_init(void)
{
  setlocale(LC_ALL, "");  // para ter suporte a UTF8
  initscr();     // inicializa o curses
  cbreak();      // lê cada char, não espera enter
  noecho();      // não mostra o que é digitado
  timeout(5);    // não espera digitar, retorna ERR se nada foi digitado
  // inicializa algumas cores
  start_color();
  init_pair(COR_TXT_PAR,      COLOR_GREEN,  COLOR_BLACK );
  init_pair(COR_CURSOR_PAR,   COLOR_BLACK,  COLOR_GREEN );
  init_pair(COR_TXT_IMPAR,    COLOR_YELLOW, COLOR_BLACK );
  init_pair(COR_CURSOR_IMPAR, COLOR_BLACK,  COLOR_YELLOW);
  init_pair(COR_CONSOLE,      COLOR_BLUE,   COLOR_BLACK );
  init_pair(COR_ENTRADA,      COLOR_GREEN,  COLOR_BLACK );
  init_pair(COR_STATUS,       COLOR_BLACK,  COLOR_BLUE  );
  init_pair(COR_OCUPADO,      COLOR_BLACK,  COLOR_RED   );
}

void tela_fim()
{
  // acaba com o curses
  endwin();
}

void tela_espera(int ms)
{
  timeout(ms);
}

void tela_posiciona(int lin, int col)
{
  move(lin, col);
}

void tela_puts(int cor, char *str)
{
  attron(COLOR_PAIR(cor));
  addstr(str);
}

void tela_limpa_linha()
{
  clrtoeol();
}

char tela_tecla(void)
{
  int ch = getch();
  if (ch == ERR) return 0;
  return ch;
}

void tela_atualiza()
{
  refresh();
}
