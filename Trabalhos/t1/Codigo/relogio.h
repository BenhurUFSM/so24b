// relogio.h
// dispositivo de E/S para contar instruções executadas
// simulador de computador
// so24b

#ifndef RELOGIO_H
#define RELOGIO_H

// simulador do relógio
// registra a passagem do tempo

#include "err.h"

typedef struct relogio_t relogio_t;

// cria e inicializa um relógio
relogio_t *relogio_cria(void);

// destrói um relógio
// nenhuma outra operação pode ser realizada no relógio após esta chamada
void relogio_destroi(relogio_t *self);

// registra a passagem de uma unidade de tempo
// esta função é chamada pelo controlador após a execução de cada instrução
void relogio_tictac(relogio_t *self);

// retorna a hora atual do sistema, em unidades de tempo
int relogio_agora(relogio_t *self);

// Funções para acessar o relógio como dispositivo de E/S, com id:
//   '0' para ler o relógio local (contador de instruções)
//   '1' para ler o tempo de CPU consumido pelo simulador (em ms)
//   '2' para ler ou escrever em quanto tempo uma interrupção será gerada
//   '3' para ler ou escrever se uma interrupção está sendo pedida
// Devem seguir o protocolo f_leitura_t e f_escrita_t declarados em es.h
err_t relogio_leitura(void *disp, int id, int *pvalor);
err_t relogio_escrita(void *disp, int id, int pvalor);

#endif // RELOGIO_H
