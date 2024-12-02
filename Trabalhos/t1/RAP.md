## RAP - Respostas a perguntas

### Executa?

Da forma como o código foi fornecido, executando o programa ele vai executar o
programa init, até que ele pede para criar um processo, então ele passa a
executar o programa p1. Mas não vai voltar a executar o init, porque não tem
processos. Aparece na tela uma mensagem de carga de trata_int, e logo que a
simulação começa, tem uma mensagem de carga do init, e as mensagens que o SO
imprime a cada chamada.

### Como funciona a Inicialização

- em main.c, é criada a CPU e o SO (entre outros).
- quando a CPU é inicializada, ela se programa para atender uma interrupção de
  reset (passa para o modo supervisor, e inicializa o PC com o endereço 10 e o
  A com o valor da interrupção de reset).
- quando o SO executa so_cria, carrega o programa trata_int no endereço 10 e
  configura a CPU para chamar so_trata_int quando executar a instrução CHAMAC.
- depois de tudo inicializado, o main chama o laço do controle, que vai chamar
  a CPU para executar instruções.
- a CPU vai começar a executar as instruções no endereço 10, que é o programa
  trata_int.asm. A primeira instrução nesse programa é CHAMAC.
- a CPU vai então chamar a função que o SO registrou na CPU, so_trata_int,
  passando como argumentos um ponteiro para a estrutura do SO (que o SO
  registrou junto com a função) e o valor de A (que é a identificação do IRQ de
  reset).
- o SO vai então executar a função que trata do reset, que vai carregar o
  programa "init" e colocar o endereço inicial do init no local onde a CPU vai
  pegar para colocar no PC quando retornar da interrupção.
- quando a função so_trata_int retornar, o valor retornado pelo SO é colocado
  no reg A pela CPU, e termina a execução da instrução CHAMAC.
- a próxima intrução executada (a segunda do trata_int) testa o valor de A (que
  é o valor de retorno do SO), para decidir se para a CPU ou se retorna da
  interrupção
- a instrução seguinte é RETI, retorno de interrupção. Essa instrução carrega
  os valores o início da memória para os registradores (um desses valores é o
  valor de início do init, que o SO colocou no endereço correspondente ao PC).
  A próxima instrução a ser executada é a primeira do init.

### E a execução normal

- as instruções são então executadas como antes, sem interferência do SO (que
  nem executa)
- vai mudar quando o init executar a intrução CHAMAS, para chamar o SO. Ele vai
  fazer isso para imprimir um caractere, executando CHAMAS depois de colocar o
  caractere a imprimir em X e a identificação da chamada de sistema (SO_ESCR) em
  A. A execução dessa instrução causa uma interrupção. Para realizar uma
  interrupção, a CPU:
   - passa para o modo supervisor
   - salva todos os registradores no início da memória
   - inicializa o PC com 10 (o endereço de tratamento de interrupção)
   - inicializa o A com o código da interrupção (que no caso corresponde a
     chamada de sistema)
- a próxima instrução executada pela CPU é a que está no endereço 10, que é
  CHAMAC, então a CPU vai chamar so_trata_int, passando como argumento o valor em
  A (que identifica interrupção de chamada de sistema)
- so_trata_int chama a função que trata chamada de sistema, que vai pegar o
  valor do registrador A da memória (onde foi salvo o valor que tinha antes da
  interrupção, que é o valor que o init colocou lá, que identifica a chamada de
  sistema pretendida (SO_ESCR)).
- a função que trata da chamada de escrita vai obter o valor do registrador X,
  que contém o caractere a escrever, e vai escrever ele na tela.
- a função também coloca o valor que quer passar para o init como retorno da
  chamada de sistema na posição da memória de onde a CPU vai recuperar o
  registrador A quando retornar da interrupção.
- o SO então retorna, o que termina a execução de CHAMAC, depois é executado
  RETI, que recupera os registradores da CPU (só que com um valor diferente de A,
  que foi alterado pelo SO)
- a próxima instrução executada pela CPU é a que segue CHAMAS no programa init.
- o SO só vai executar de novo na próxima interrupção.

### Acesso aos registradores da CPU

A única forma de execução do SO após a inicialização é em atendimento a uma
interrupção. A interrupção interrompe a execução de um processo, que na maior
parte das vezes deve ser retomada mais tarde. Para que isso aconteça, os
registradores da CPU quando o tratamento da interrupção inicia e quando ele se
encerra (do ponto de vista desse processo) devem conter os mesmos valores.

A interrupção também é usada para permitir a comunicação entre um processo e o
SO, no que se chama "chamada de sistema". A troca de informações entre o
processo e o SO nesse caso é realizada por meio dos registradores: o SO deve
ter acesso ao valor dos registradores do processo para receber informação e
deve poder alterar esses registradores para enviar informação ao processo.

A forma usual de se implementar isso é com o SO mantendo na tabela de processos
o valor de todos os registradores disponíveis a cada processo. Quando um
processo é escolhido para executar, os valores dos registradores
correspondentes à esse processo são transferidos para a CPU, e quando a
execução de um processo é interrompida, os valores dos registradores são
transferidos da CPU para a tabela.

Essa operação de transferência deve ser auxiliada pelo hardware, porque para
que o processador possa iniciar o tratamento de uma interrupção, é necessário
alterar o valor de alguns registradores (no mínimo o PC). Para que o valor
original seja recuperável, tem que ser salvo pela CPU antes de alterá-lo.

No caso do nosso processador simulado, o hardware salva todos os registradores
em memória quando atende uma interrupção, e recupera todos quando retorna de
uma interrupção. Isso simplifica a tarefa do SO, diminuindo a quantidade de
código que tem que ser escrito em assembly para implementar salvamento e
recuperação do estado interno da CPU.

Os registradores são colocados na memória, e podem ser recuperados pelo SO com
operações de acesso á memória (mem_le e mem_escr). Os endereços estão definidos
em irq.h (IRQ_END_A para o registrador A, por exemplo). Na implementação do SO
que foi fornecida, como não tem a tabela de processos, em todo lugar onde o
valor de um registrador precisa ser acessado ou alterado é realizado um acesso
direto a uma dessas posições de memória. Com a implementação de processos,
passa-se a ter os valores de registradores associados aos processos, e o SO não
mais acessa os registradores físicos da CPU na maioria das vezes. Limite esse
acesso a somente dois locais: quando os valores dos registradores físicos são
salvos na tabela de processos e quando esses valores são recuperados da tabela
do processos. Todos os demais acessos a registradores devem ser feitos na
entrada da tabela correspondente ao processo sendo manipulado pelo SO.

### Sobre a prioridade

Tinha um erro em `so.c` no cálculo da prioridade, era para ser a média, mas tava faltando dividir por 2.

O cálculo da prioridade resulta em um número menor para o processo que deve ser prioritário. Então, o processo a ser escolhido é aquele que tem o menor valor no campo prioridade.

### Como assim relatório?

Google é seu amiguinho:
- https://engenharia360.com/elaborar-um-relatorio-experimental/
- https://edisciplinas.usp.br/pluginfile.php/3072406/mod_resource/content/4/Como%20fazer%20um%20bom%20relatorio.pdf
- https://docente.ifrn.edu.br/jonathanpereira/disciplinas/organizacao-e-manutencao-de-computadores/aulas-de-laboratorio/modelo-para-relatorios-de-aulas-de-laboratorio/view
- https://edisciplinas.usp.br/pluginfile.php/1034718/mod_resource/content/1/Referencias.pdf

Não precisa ser algo excessivamente formal, mas espera-se que um relatório descreva o experimento, apresente resultados obtidos e uma discussão a respeito desses resultados (pra se ver até que ponto o assunto foi entendido...).
