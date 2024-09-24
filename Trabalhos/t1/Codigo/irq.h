#ifndef IRQ_H
#define IRQ_H

// os motivos para uma requisição de interrupção (irq) ser gerada
typedef enum {
  // interrupções geradas internamente na CPU
  IRQ_RESET,         // inicialização da CPU
  IRQ_ERR_CPU,       // erro interno na CPU (ver registrador de erro)
  IRQ_SISTEMA,       // chamada de sistema
  // interrupções geradas por dispositivos de E/S
  IRQ_RELOGIO,       // interrupção causada pelo relógio
  // interrupções de E/S ainda não implementadas
  IRQ_TECLADO,       // interrupção causada pelo teclado
  IRQ_TELA,          // interrupção causada pela tela
  N_IRQ              // número de interrupções
} irq_t;

char *irq_nome(irq_t irq);

// endereços na memória onde a CPU salva os valores dos registradores
//   quando aceita uma interrupção, e de onde recupera esses valores
//   quando retorna de uma interrupção
#define IRQ_END_PC          0
#define IRQ_END_A           1
#define IRQ_END_X           2
#define IRQ_END_erro        3
#define IRQ_END_complemento 4
#define IRQ_END_modo        5

// endereço para onde desviar quando aceita uma interrupção
#define IRQ_END_TRATADOR   10

#endif // IRQ_H
