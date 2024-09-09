// terminal.h
// entrada e saída em um terminal
// simulador de computador
// so24b

#ifndef TERMINAL_H
#define TERMINAL_H

// simulação de um terminal
//
// mantém o conteúdo da linha de saída de um terminal (o que aparece na tela) e
// da linha de entrada (o que foi digitado e ainda não foi lido pela CPU)
//
// implementa 4 dispositivos associados a um terminal:
// - leitura do próximo caractere de entrada
// - leitura do estado da entrada (se tem caractere disponível ou não)
// - escrita de um caractere na saída
// - leitura do estado da saída (se um caractere pode ser escrito ou não)
//
// a leitura não é possível quando não existir caractere na entrada
// existe um limite para caracteres digitados e não lidos; caracteres adicionais
//   são ignorados
// o número de caracteres na saída é limitado ao tamanho da linha. um caractere
//   adicional causa a "rolagem", que remove o primeiro caractere da linha para
//   gerar espaço para o novo. a impressão de um \n causa a "limpeza" da linha.
// a escrita não é possível se a saída estiver rolando ou sendo limpa, o que é
//   feito um caractere por vez (a cada chamada a tictac).
//
// a E/S efetiva é realizada pela console. ela obtém acesso às linhas de entrada e
//   saída chamando terminal_txt_entrada ou terminal_txt_saida. a console insere
//   caracteres digitados no terminal chamando terminal_insere_char, e limpa a
//   linha de saída com terminal_limpa_saida.

#include <stdbool.h>
#include "es.h"

typedef struct terminal_t terminal_t;

// aloca e inicializa um novo terminal
terminal_t *terminal_cria(int tam_linha);
// libera a memória ocupada por um terminal
void terminal_destroi(terminal_t *self);

// retorna a linha de entrada do terminal (para uso pela console)
char *terminal_txt_entrada(terminal_t *self);

// retorna a linha de saida do terminal (para uso pela console)
char *terminal_txt_saida(terminal_t *self);

// insere um novo caractere na entrada do terminal
// (para uso pela console, para simular um caractere digitado no teclado)
void terminal_insere_char(terminal_t *self, char ch);

// limpa a linha de saída (para uso pela console)
void terminal_limpa_saida(terminal_t *self);

// esta função deve ser chamada periodicamente
void terminal_tictac(terminal_t *self);

// Funções para implementar o protocolo de acesso a um dispositivo pelo
//   controlador de E/S
// Devem seguir o protocolo f_leitura_t e f_escrita_t declarados em es.h
err_t terminal_leitura(void *disp, int id, int *pvalor);
err_t terminal_escrita(void *disp, int id, int valor);

#endif // TERMINAL_H
