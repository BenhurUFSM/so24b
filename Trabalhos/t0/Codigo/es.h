// es.h
// controlador de dispositivos de entrada e saída
// simulador de computador
// so24b

#ifndef ES_H
#define ES_H

#include "err.h"
#include "dispositivos.h"

#include <stdbool.h>

typedef struct es_t es_t; // declara o tipo como sendo uma estrutura opaca

// tipos de ponteiros de funções que um dispositivo deve implementar para
//   poder ser controlado pelo controlador de E/S
// as funções recebem um ponteiro que identifica o
//   controlador do dispositivo (o equivalente à controladora de hw)
//   e um inteiro que identifica o dispositivo entre os possivelmente vários
//   desse controlador. Esses valores são fornecidos quando o dispositivo é
//   registrado.
// a função de leitura recebe um ponteiro onde colocar o dado lido.
// a função de escrita recebe o valor a ser escrito.
typedef err_t (*f_leitura_t)(void *controladora, int id, int *endereco);
typedef err_t (*f_escrita_t)(void *controladora, int id, int valor);

// aloca e inicializa um controlador de E/S
// retorna NULL em caso de erro
es_t *es_cria(void);

// libera os recursos ocupados pelo controlador
void es_destroi(es_t *self);

// registra um dispositivo, identificado com o valor 'dispositivo'.
// esse dispositivo é controlado pela controladora apontada por 'controladora',
//   que o identifica por 'id'.
// para acessar esse dispositivo, devem ser usadas as funções apontadas pelos
//   ponteiros 'f_leitura' e 'f_escrita'.
// se 'f_leitura' ou 'f_escrita' for NULL, considera-se que a operação
//   correspondente é inválida.
// retorna false se não foi possível registrar
bool es_registra_dispositivo(es_t *self, dispositivo_id_t dispositivo,
                             void *controladora, int id,
                             f_leitura_t f_leitura, f_escrita_t f_escrita);

// lê um inteiro de um dispositivo
// retorna ERR_OK se bem sucedido, ou
//   ERR_DISP_INV se dispositivo desconhecido
//   ERR_OP_INV se operação inválida
err_t es_le(es_t *self, dispositivo_id_t dispositivo, int *pvalor);

// escreve um inteiro em um dispositivo
// retorna ERR_OK se bem sucedido, ou
//   ERR_DISP_INV se dispositivo desconhecido
//   ERR_OP_INV se operação inválida
err_t es_escreve(es_t *self, dispositivo_id_t dispositivo, int valor);

#endif // ES_H
