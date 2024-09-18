#include "aleatorio.h"
#include <stdlib.h>  // para rand(), srand()
#include <time.h>    // para time()

// Estrutura do dispositivo de números aleatórios (se precisar de estado, como uma semente)
struct dispositivo_random_t {
    unsigned int seed;
};

// Função que cria o dispositivo de números aleatórios
dispositivo_random_t *random_cria(void) {
    dispositivo_random_t *random = (dispositivo_random_t *) malloc(sizeof(dispositivo_random_t));
    if (random != NULL) {
        random->seed = (unsigned int) time(NULL);  // usa o tempo atual como semente
        srand(random->seed);  // inicializa o gerador de números aleatórios
    }
    return random;
}

// Função que destrói o dispositivo de números aleatórios
void random_destroi(dispositivo_random_t *random) {
    if (random != NULL) {
        free(random);  // libera a memória
    }
}

// Função de leitura: gera um número aleatório entre 1 e 10
err_t random_leitura(void *disp, int id_reg, int *valor) {
    if (valor == NULL) {
        return ERR_DISP_INV;  // retorna erro se o ponteiro for nulo
    }

    // Gera um número aleatório entre 1 e 100
    *valor = (rand() % 100) + 1;  
    return ERR_OK;  // retorna OK ao completar a operação com sucesso
}
