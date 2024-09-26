// console.h
// console do operador e controle de terminais
// simulador de computador
// so24b

#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdbool.h>
#include "terminal.h"

typedef struct console_t console_t;

// cria e inicializa a console
console_t *console_cria(void);

// destrói a console
void console_destroi(console_t *self);

// imprime na área geral do console
int console_printf(char *fmt, ...);

// imprime na linha de status
void console_print_status(console_t *self, char *txt);

// retorna o próximo comando externo digitado pelo operador na console.
// um comando externo é representado por um caractere, e não é executado internamente
//   na console (é executado pelo controlador).
// os comandos externos são:
//   'P': para a execução,
//   '1': executa uma instrução,
//   'C': continua a execução,
//   'F': finaliza a simulação.
// retorna '\0' caso não tenha comando externo digitado
char console_comando_externo(console_t *self);

// retorna o terminal identificado ('A', 'B', etc)
terminal_t *console_terminal(console_t *self, char id_terminal);

// esta função deve ser chamada periodicamente para que tela funcione
void console_tictac(console_t *self);

#endif // CONSOLE_H
