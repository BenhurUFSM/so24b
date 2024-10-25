// tabpag.h
// tabela de páginas para a MMU
// simulador de computador
// so24b

#ifndef TABPAG_H
#define TABPAG_H

// estrutura auxiliar para a implementação de paginação na MMU
// realiza a tradução de números de páginas do espaço de endereçamento
//   de um processo em números de quadros da memória principal onde essas
//   páginas estão mapeadas
// mantém para cada página mapeada um bit de acesso e um bit de alteração

#include "err.h"
#include <stdbool.h>

// tipo opaco que representa a tabela de páginas
typedef struct tabpag_t tabpag_t;

// cria uma tabela de páginas
// retorna um ponteiro para um descritor, que deverá ser usado em todas
//   as operações nessa tabela
// mata o programa em caso de erro (malloc)
tabpag_t *tabpag_cria(void);

// destrói uma tabela de páginas
// libera a memória ocupara pela tabela
// nenhuma outra operação pode ser realizada na tabela após esta chamada
void tabpag_destroi(tabpag_t *self);

// define que a tradução da página 'pagina' deve resultar no quadro 'quadro'
// essa página é marcada como válida, e os bits de acesso e alteração para essa
//   página são zerados
// páginas sem quadro definido são consideradas inválidas
void tabpag_define_quadro(tabpag_t *self, int pagina, int quadro);

// marca a página 'pagina' como inválida.
// as informações sobre essa página são perdidas.
void tabpag_invalida_pagina(tabpag_t *self, int pagina);

// marca o bit de acesso à página; se alteracao for true, marca também o
//   bit de alteração
// não faz nada se a página for inválida
void tabpag_marca_bit_acesso(tabpag_t *self, int pagina, bool alteracao);

// zera o bit de acesso à página; não afeta o bit de alteração
// não faz nada se a página for inválida
void tabpag_zera_bit_acesso(tabpag_t *self, int pagina);

// retorna o valor do bit de acesso à página
// retorna false se a página for inválida
bool tabpag_bit_acesso(tabpag_t *self, int pagina);

// retorna o valor do bit de alteração da página
// retorna false se a página for inválida
bool tabpag_bit_alteracao(tabpag_t *self, int pagina);

// traduz a página 'pagina'; coloca o quadro correspondente na posição apontada
//   por 'pquadro'
// retorna ERR_PAG_AUSENTE (e não altera '*pquadro') se a página for inválida
err_t tabpag_traduz(tabpag_t *self, int pagina, int *pquadro);

#endif // TABPAG_H
