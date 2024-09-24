## t1

Breve texto aqui, com descrição do código e do que é o t1

### Alterações no código em relação ao t0

- timer no relógio
  - relogio.[ch] - 2 novos dispositivos, timer e interrupção. timer pode ser escrito ou lido, é decrementado a cada tictac se for positivo, e se chegar a 0 liga interrupção. interrupção pode ser lido ou escrito, e indica que a CPU deve ser interrompida porque o timer espirou.
  - dispositivos.h - define os novos dispositivos
  - main.c - registra os novos dispositivos no controlador de e/s
- modo usuário e supervisor na cpu; instruções privilegiadas
  - cpu.[ch] - definição de cpu_modo_t, inicialização da CPU em modo supervisor, identificação das instruções privilegiadas, põe CPU em erro se executar instrução privilegiada em modo usuário
  - err.[ch] - novo erro, execução de instrução privilegiada
- interrupção
  - irq.[ch] - novos arquivos, com os códigos das interrupções possíveis (IRQs, *interrupt requests*) e os endereços onde a CPU salva/recupera os registradores quando aceita uma interrupção
  - cpu.[ch] - função para iniciar uma interrupção (salva registradores, passa para modo supervisor, desvia para endereço 10) e para retornar de uma interrupção
- novas instruções
  - RETI, retorna de uma interrupção
  - CHAMAS, causa uma interrupção, para chamar o SO
  - CHAMAC, executa código C (instrução configurável, será usada para executar todo o SO)
  - instrucao.[ch] - código e descrição das novas instruções
  - cpu.[ch] - implementação das novas instruções, interface para programar o que será executado por CHAMAC
- interrupção do timer
  - controle.c - no laço de execução, se o relógio pedir uma interrupção, informa a CPU
- implementação do SO
  - so.[ch] - novos arquivos, implementação parcial do SO
  - trata_int.asm - codigo asm para iniciar e terminar o tratamento de uma interrupção, desviando para o SO
  - init.asm - programa para ser o primeiro programa a ser executado pelo SO, chama outros programas
  - p?.asm - programas chamados pelo init
  - main.c - cria e inicializa o SO, não mais carrega um programa (agora é tarefa do SO)
  - demais .asm - alterados para chamar o SO
  - Makefile - inclui o que foi incluido, e uma gambiarra para montar cada asm em um endereço diferente
