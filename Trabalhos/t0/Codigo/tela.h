// tela.h
// entrada e saída no terminal físico
// simulador de computador
// so24b

#ifndef TELA_H
#define TELA_H

// identificação da cor para cada parte
// essa definição deveria ser da console, mas tou com preguiça de refatorar isso
#define COR_TXT_PAR      1
#define COR_CURSOR_PAR   2
#define COR_TXT_IMPAR    3
#define COR_CURSOR_IMPAR 4
#define COR_CONSOLE      5
#define COR_ENTRADA      6
#define COR_STATUS       7
#define COR_OCUPADO      8

// inicializa o uso da tela
void tela_init(void);

// finaliza o uso da tela
void tela_fim();

// programa número de milisegundos a esperar a cada leitura do teclado
void tela_espera(int ms);

// posiciona o cursor
void tela_posiciona(int lin, int col);

// escreve o texto "str" no par de cores "cor"
void tela_puts(int cor, char *str);

// limpa a linha do cursor até o final
void tela_limpa_linha();

// retorna a próxima tecla digitada, ou 0 se não houver
char tela_tecla(void);

// envia para a tela o que foi escrito
void tela_atualiza();

#endif // TELA_H
