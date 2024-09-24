## t1

### Alterações no código em relação ao t0

- timer no relógio
  - relogio.[ch] - 2 novos dispositivos, timer e interrupção. timer pode ser escrito ou lido, é decrementado a cada tictac se for positivo, e se chegar a 0 liga interrupção. interrupção pode ser lido ou escrito, e indica que a CPU deve ser interrompida porque o timer espirou.
  - dispositivos.h - define os novos dispositivos
  - main.c - registra os novos dispositivos no controlador de e/s
- modo usuário e supervisor na cpu; instruções privilegiadas
  - cpu.[ch] - definição de cpu_modo_t, inicialização da CPU em modo supervisor, identificação das instruções privilegiadas, põe CPU em erro se executar instrução privilegiada em modo usuário
  - err.[ch] - novo erro, execução de instrução privilegiada

