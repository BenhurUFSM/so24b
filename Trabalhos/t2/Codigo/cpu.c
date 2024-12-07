// cpu.c
// executor de instruções da CPU
// simulador de computador
// so24b

// INCLUDES {{{1
#include "cpu.h"
#include "err.h"
#include "instrucao.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

// DECLARAÇÃO {{{1
// uma CPU tem estado, memória, controlador de ES
struct cpu_t {
  // registradores
  int PC;
  int A;
  int X;
  // estado interno da CPU
  err_t erro;
  int complemento;
  cpu_modo_t modo;
  // acesso a dispositivos externos
  mmu_t *mmu;
  es_t *es;
  // identificação das instruções privilegiadas
  bool privilegiadas[N_OPCODE];
  // função e argumento para implementar instrução CHAMAC
  func_chamaC_t funcaoC;
  void *argC;
};

// CRIAÇÃO {{{1
cpu_t *cpu_cria(mmu_t *mmu, es_t *es)
{
  cpu_t *self;
  self = malloc(sizeof(*self));
  assert(self != NULL);

  self->mmu = mmu;
  self->es = es;
  // inicializa registradores
  self->PC = 0;
  self->A = 0;
  self->X = 0;
  self->erro = ERR_OK;
  self->complemento = 0;
  self->modo = usuario;
  self->funcaoC = NULL;
  // inicializa instruções privilegiadas
  memset(self->privilegiadas, 0, sizeof(self->privilegiadas));
  self->privilegiadas[PARA] = true;
  self->privilegiadas[LE] = true;
  self->privilegiadas[ESCR] = true;
  self->privilegiadas[RETI] = true;
  self->privilegiadas[CHAMAC] = true;
  // gera uma interrupção de reset, para o SO poder executar
  cpu_interrompe(self, IRQ_RESET);

  return self;
}

void cpu_destroi(cpu_t *self)
{
  // eu nao criei MMU nem es; quem criou que destrua!
  free(self);
}

void cpu_define_chamaC(cpu_t *self, func_chamaC_t funcaoC, void *argC)
{
  self->funcaoC = funcaoC;
  self->argC = argC;
}

// IMPRESSÃO {{{1
static void imprime_registradores(cpu_t *self, char *str)
{
  sprintf(str, "%s PC=%04d A=%06d X=%06d",
                self->modo == supervisor ? "SUP " : "usu ",
                self->PC, self->A, self->X);
}

static void imprime_instrucao(cpu_t *self, char *str)
{
  int opcode;
  if (mmu_le(self->mmu, self->PC, &opcode, self->modo) != ERR_OK) {
    strcpy(str, " PC inválido");
    return;
  }
  if (instrucao_num_args(opcode) == 0) {
    sprintf(str, " %02d %s", opcode, instrucao_nome(opcode));
    // imprime argumento da instrução, se houver
  } else {
    int A1;
    mmu_le(self->mmu, self->PC + 1, &A1, self->modo);
    sprintf(str, " %02d %s %d", opcode, instrucao_nome(opcode), A1);
  }
}

static void imprime_erro(cpu_t *self, char *str)
{
  if (self->erro != ERR_OK) {
    sprintf(str, " E=%d(%d) %s",
                 self->erro, self->complemento, err_nome(self->erro));
  } else {
    strcpy(str, "");
  }
}

void cpu_concatena_descricao(cpu_t *self, char *str)
{
  char aux[40];

  imprime_registradores(self, aux);
  strcat(str, aux);

  imprime_instrucao(self, aux);
  strcat(str, aux);

  imprime_erro(self, aux);
  strcat(str, aux);
}

// ACESSO À MEMÓRIA E E/S {{{1

// ---------------------------------------------------------------------
// funções auxiliares para usar durante a execução das instruções
// alteram o estado da CPU caso ocorra erro

// lê um valor da memória
static bool pega_mem(cpu_t *self, int endereco, int *pval)
{
  self->erro = mmu_le(self->mmu, endereco, pval, self->modo);
  if (self->erro == ERR_OK) return true;
  self->complemento = endereco;
  return false;
}

// lê o opcode da instrução no PC
// retorna true se ele pode ser executado, ou põe em erro o motivo de não poder
static bool pega_opcode(cpu_t *self, int *popc)
{
  // não tem que testar endereços, é tarefa da mmu
  // não pode executar se houver erro na leitura da memória
  if (!pega_mem(self, self->PC, popc)) return false;
  // pode executar se tiver privilégio para isso
  if (self->modo == supervisor || !self->privilegiadas[*popc]) return true;
  // não pode executar instrução privilegiada em modo usuário
  self->erro = ERR_INSTR_PRIV;
  return false;
}

// lê o argumento 1 da instrução no PC
static bool pega_A1(cpu_t *self, int *pA1)
{
  return pega_mem(self, self->PC + 1, pA1);
}

// escreve um valor na memória
static bool poe_mem(cpu_t *self, int endereco, int val)
{
  self->erro = mmu_escreve(self->mmu, endereco, val, self->modo);
  if (self->erro == ERR_OK) return true;
  self->complemento = endereco;
  return false;
}

// lê um valor da E/S
static bool pega_es(cpu_t *self, int dispositivo, int *pval)
{
  self->erro = es_le(self->es, dispositivo, pval);
  if (self->erro == ERR_OK) return true;
  self->complemento = dispositivo;
  return false;
}

// escreve um valor na E/S
static bool poe_es(cpu_t *self, int dispositivo, int val)
{
  self->erro = es_escreve(self->es, dispositivo, val);
  if (self->erro == ERR_OK) return true;
  self->complemento = dispositivo;
  return false;
}

// INSTRUÇÕES {{{1
// ---------------------------------------------------------------------
// funções auxiliares para implementação de cada instrução

static void op_NOP(cpu_t *self) // não faz nada
{
  self->PC += 1;
}

static void op_PARA(cpu_t *self) // para a CPU
{
  self->erro = ERR_CPU_PARADA;
}

static void op_CARGI(cpu_t *self) // carrega imediato
{
  int A1;
  if (pega_A1(self, &A1)) {
    self->A = A1;
    self->PC += 2;
  }
}

static void op_CARGM(cpu_t *self) // carrega da memória
{
  int A1, mA1;
  if (pega_A1(self, &A1) && pega_mem(self, A1, &mA1)) {
    self->A = mA1;
    self->PC += 2;
  }
}

static void op_CARGX(cpu_t *self) // carrega indexado
{
  int A1, mA1mX;
  int X = self->X;
  if (pega_A1(self, &A1) && pega_mem(self, A1 + X, &mA1mX)) {
    self->A = mA1mX;
    self->PC += 2;
  }
}

static void op_ARMM(cpu_t *self) // armazena na memória
{
  int A1;
  if (pega_A1(self, &A1) && poe_mem(self, A1, self->A)) {
    self->PC += 2;
  }
}

static void op_ARMX(cpu_t *self) // armazena indexado
{
  int A1;
  int X = self->X;
  if (pega_A1(self, &A1) && poe_mem(self, A1 + X, self->A)) {
    self->PC += 2;
  }
}

static void op_TRAX(cpu_t *self) // troca A com X
{
  int A = self->A;
  int X = self->X;
  self->X = A;
  self->A = X;
  self->PC += 1;
}

static void op_CPXA(cpu_t *self) // copia X para A
{
  self->A = self->X;
  self->PC += 1;
}

static void op_INCX(cpu_t *self) // incrementa X
{
  self->X += 1;
  self->PC += 1;
}

static void op_SOMA(cpu_t *self) // soma
{
  int A1, mA1;
  if (pega_A1(self, &A1) && pega_mem(self, A1, &mA1)) {
    self->A += mA1;
    self->PC += 2;
  }
}

static void op_SUB(cpu_t *self) // subtração
{
  int A1, mA1;
  if (pega_A1(self, &A1) && pega_mem(self, A1, &mA1)) {
    self->A -= mA1;
    self->PC += 2;
  }
}

static void op_MULT(cpu_t *self) // multiplicação
{
  int A1, mA1;
  if (pega_A1(self, &A1) && pega_mem(self, A1, &mA1)) {
    self->A *= mA1;
    self->PC += 2;
  }
}

static void op_DIV(cpu_t *self) // divisão
{
  int A1, mA1;
  if (pega_A1(self, &A1) && pega_mem(self, A1, &mA1)) {
    self->A /= mA1;
    self->PC += 2;
  }
}

static void op_RESTO(cpu_t *self) // resto
{
  int A1, mA1;
  if (pega_A1(self, &A1) && pega_mem(self, A1, &mA1)) {
    self->A %= mA1;
    self->PC += 2;
  }
}

static void op_NEG(cpu_t *self) // inverte sinal
{
  self->A = -self->A;
  self->PC += 1;
}

static void op_DESV(cpu_t *self) // desvio incondicional
{
  int A1;
  if (pega_A1(self, &A1)) {
    self->PC = A1;
  }
}

static void op_DESVZ(cpu_t *self) // desvio condicional
{
  if (self->A == 0) {
    op_DESV(self);
  } else {
    self->PC += 2;
  }
}

static void op_DESVNZ(cpu_t *self) // desvio condicional
{
  if (self->A != 0) {
    op_DESV(self);
  } else {
    self->PC += 2;
  }
}

static void op_DESVN(cpu_t *self) // desvio condicional
{
  if (self->A < 0) {
    op_DESV(self);
  } else {
    self->PC += 2;
  }
}

static void op_DESVP(cpu_t *self) // desvio condicional
{
  if (self->A > 0) {
    op_DESV(self);
  } else {
    self->PC += 2;
  }
}

static void op_CHAMA(cpu_t *self) // chamada de subrotina
{
  int A1;
  if (pega_A1(self, &A1) && poe_mem(self, A1, self->PC + 2)) {
    self->PC = A1 + 1;
  }
}

static void op_RET(cpu_t *self) // retorno de subrotina
{
  int A1, mA1;
  if (pega_A1(self, &A1) && pega_mem(self, A1, &mA1)) {
    self->PC = mA1;
  }
}

static void op_LE(cpu_t *self) // leitura de E/S
{
  int A1, dado;
  if (pega_A1(self, &A1) && pega_es(self, A1, &dado)) {
    self->A = dado;
    self->PC += 2;
  }
}

static void op_ESCR(cpu_t *self) // escrita de E/S
{
  int A1;
  if (pega_A1(self, &A1) && poe_es(self, A1, self->A)) {
    self->PC += 2;
  }
}

// declara uma função auxiliar (só para a interrupção e o retorno ficarem perto)
static void cpu_desinterrompe(cpu_t *self);

static void op_RETI(cpu_t *self) // retorno de interrupção
{
  cpu_desinterrompe(self);
}

static void op_CHAMAC(cpu_t *self) // chama função em C
{
  if (self->funcaoC == NULL) {
    self->erro = ERR_OP_INV;
    return;
  }
  self->A = self->funcaoC(self->argC, self->A);
  self->PC += 1;
}

static void op_CHAMAS(cpu_t *self) // chamada de sistema
{
  self->PC += 1;
  // causa uma interrupção, para forçar a execução do SO
  cpu_interrompe(self, IRQ_SISTEMA);

}

// EXECUTA UMA INSTRUÇÃO {{{1

static void executa_a_instrucao(cpu_t *self, int opcode)
{
  switch (opcode) {
    case NOP:    op_NOP(self);    break;
    case PARA:   op_PARA(self);   break;
    case CARGI:  op_CARGI(self);  break;
    case CARGM:  op_CARGM(self);  break;
    case CARGX:  op_CARGX(self);  break;
    case ARMM:   op_ARMM(self);   break;
    case ARMX:   op_ARMX(self);   break;
    case TRAX:   op_TRAX(self);   break;
    case CPXA:   op_CPXA(self);   break;
    case INCX:   op_INCX(self);   break;
    case SOMA:   op_SOMA(self);   break;
    case SUB:    op_SUB(self);    break;
    case MULT:   op_MULT(self);   break;
    case DIV:    op_DIV(self);    break;
    case RESTO:  op_RESTO(self);  break;
    case NEG:    op_NEG(self);    break;
    case DESV:   op_DESV(self);   break;
    case DESVZ:  op_DESVZ(self);  break;
    case DESVNZ: op_DESVNZ(self); break;
    case DESVN:  op_DESVN(self);  break;
    case DESVP:  op_DESVP(self);  break;
    case CHAMA:  op_CHAMA(self);  break;
    case RET:    op_RET(self);    break;
    case LE:     op_LE(self);     break;
    case ESCR:   op_ESCR(self);   break;
    case RETI:   op_RETI(self);   break;
    case CHAMAC: op_CHAMAC(self); break;
    case CHAMAS: op_CHAMAS(self); break;
    default:     self->erro = ERR_INSTR_INV;
  }
}

void cpu_executa_1(cpu_t *self)
{
  // não executa se CPU já estiver em erro
  if (self->erro != ERR_OK) return;

  int opcode;
  if (pega_opcode(self, &opcode)) {
    executa_a_instrucao(self, opcode);
  }

  // se a CPU entrou em erro, causa uma interrupção
  // a menos que a CPU tenha parado, porque a única forma de a CPU entrar nesse
  //   estado é pela execução da instrução PARA em modo supervisor, e é a forma de
  //   o SO dizer que não tem mais nada para fazer, e deve-se deixar a CPU dormindo
  //   até que venha uma interrupção de E/S
  if (self->erro != ERR_OK && self->erro != ERR_CPU_PARADA) {
    // se a interrupção não é aceita nesse ponto, temos um problema grave...
    assert(cpu_interrompe(self, IRQ_ERR_CPU));
  }
}

// INTERRUPÇÃO {{{1

bool cpu_interrompe(cpu_t *self, irq_t irq)
{
  // só aceita interrupção em modo usuário ou quando a CPU está dormindo
  if (self->modo != usuario && self->erro != ERR_CPU_PARADA) return false;

  // A interrupção será atendida em modo supervisor. Já troca o modo aqui
  //   para garantir que o estado do processador será salvo em endereços
  //   físicos e não lógicos, e que se tem permissão para realizar esse
  //   acesso (para quando existir proteção de memória)
  self->modo = supervisor;

  // esta é uma CPU boazinha, salva todo o estado interno da CPU no início da memória
  // self->erro é alterado por poe_mem, copia antes!
  int erro = self->erro;
  int complemento = self->complemento;
  poe_mem(self, IRQ_END_PC,          self->PC);
  poe_mem(self, IRQ_END_A,           self->A);
  poe_mem(self, IRQ_END_X,           self->X);
  poe_mem(self, IRQ_END_erro,        erro);
  poe_mem(self, IRQ_END_complemento, complemento);
  poe_mem(self, IRQ_END_modo,        usuario);

  // altera o estado da CPU para ela poder executar o tratador de interrupção
  // vai iniciar o tratamento da interrupção no endereço IRQ_END_TRATADOR,
  //   com o A contendo o valor da requisição de interrupção e sem erro
  // se o tratador da interrupção precisar do estado da CPU de antes da
  //   interrupção, deve acessar o início da memória, onde esse estado foi salvo
  self->PC = IRQ_END_TRATADOR;
  self->A = irq;
  self->erro = ERR_OK;

  return true;
}

static void cpu_desinterrompe(cpu_t *self)
{
  // a interrupção retornou
  // recupera o estado da CPU, para que volte a executar o que foi interrompido
  //   quando a interrupção foi atendida
  
  // tem que estar em modo supervisor para ler nesses endereços
  self->modo = supervisor;
  pega_mem(self, IRQ_END_PC,          &self->PC);
  pega_mem(self, IRQ_END_A,           &self->A);
  pega_mem(self, IRQ_END_X,           &self->X);
  // não dá para pegar o erro nem o modo diretamente porque eles não são int
  int erro, modo;
  pega_mem(self, IRQ_END_erro,        &erro);
  pega_mem(self, IRQ_END_complemento, &self->complemento);
  pega_mem(self, IRQ_END_modo,        &modo);
  self->modo = modo;
  // coloca o erro por último, porque pode ser alterado por pega_mem
  self->erro = erro;
}

// vim: foldmethod=marker
