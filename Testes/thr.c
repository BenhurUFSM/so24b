// programa de teste de threads
// faz um cálculo simples de soma de um valor em um acumulador,
// usando um número configurável de threads

// exemplifica o problema de acesso a variável compartilhada sem proteção
// mostra também alguns problemas de cache em multiprocessadores

// para medir o tempo de execução do programa, dá para executar com time:
//   time ./thr

#include <stdio.h>
#include <pthread.h>

// número de vezes que o cálculo será feito
//   altere para que o tempo de execução do programa seja de alguns segundos com
//   uma thread
#define N 1200000000
// número de threads que será usado -- altere para mudar o nível de paralelismo
#define NT 1

typedef struct {
  // variável que vai acumular as somas
  double soma;
  // variável para simular outros dados -- consegue explicar o que acontece se
  //   comentar essa linha?
  double y[20];
} dado_t;

// uma estrutura para cada thread
dado_t dados[NT];

// variável para guardar o somatório geral
double total;

// função que realiza o cálculo de cada valor a ser somado
//   vai ser chamada muitas vezes
double calc() {
  return 0.25;
}

// função que será executada por cada thread
// recebe em ptr um ponteiro para um dado_t exclusivo da thread
void *f(void *ptr)
{
  dado_t *dp = ptr;
  dp->soma = 0;
  for (int i = 0; i < N/NT; i++) {
    // escolhe a segunda linha para fazer a soma direto na variável compartilhada
    dp->soma += calc();
    //total += calc();
  }
  return NULL;
}

int main()
{
  pthread_t thr[NT];
  total = 0;

  // cria cada thread, para executar a função f, passando um ptr para dado_t
  for (int t = 0; t < NT; t++) {
    pthread_create(&thr[t], NULL, f, &dados[t]);
  }
  // espera as threads terminarem, e totaliza as somas
  for (int t = 0; t < NT; t++) {
    pthread_join(thr[t], NULL);
    total += dados[t].soma;
  }
  // imprime o resultado final
  printf("%f\n", total);
}
