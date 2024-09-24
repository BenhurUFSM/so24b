// dispositivos.h
// identificação dos dispositivos de entrada e saída
// simulador de computador
// so24b

#ifndef DISPOSITIVOS_H
#define DISPOSITIVOS_H

typedef enum {
  D_TERM_A_TECLADO        =  0,
  D_TERM_A_TECLADO_OK     =  1,
  D_TERM_A_TELA           =  2,
  D_TERM_A_TELA_OK        =  3,
  D_TERM_B_TECLADO        =  4,
  D_TERM_B_TECLADO_OK     =  5,
  D_TERM_B_TELA           =  6,
  D_TERM_B_TELA_OK        =  7,
  D_TERM_C_TECLADO        =  8,
  D_TERM_C_TECLADO_OK     =  9,
  D_TERM_C_TELA           = 10,
  D_TERM_C_TELA_OK        = 11,
  D_TERM_D_TECLADO        = 12,
  D_TERM_D_TECLADO_OK     = 13,
  D_TERM_D_TELA           = 14,
  D_TERM_D_TELA_OK        = 15,
  D_RELOGIO_INSTRUCOES    = 16,
  D_RELOGIO_REAL          = 17,
  D_RELOGIO_TIMER         = 18,
  D_RELOGIO_INTERRUPCAO   = 19,
  N_DISPOSITIVOS
} dispositivo_id_t;

#endif // DISPOSITIVOS_H

