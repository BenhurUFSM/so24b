# t0 - simulador de um computador

Familiarize-se com o código anexo, que simula um pequeno computador, que será usado durante o desenvolvimento da disciplina.

Esta descrição é incompleta.
Leia o código, faça perguntas.
As perguntas recebidas (e suas respostas) serão colocadas no final deste texto.
Quanto mais se familiarizar com este código, mais fácil será realizar os trabalhos.
Quanto antes isso for feito, maiores as chances de sucesso na disciplina.

O código implementa dois programas: um simulador de uma CPU bem simples e um montador, para produzir código em linguagem de máquina para essa CPU à partir de programas em linguagem de montagem (a implementação de um compilador fica como sugestão de exercício para quem estiver com um tempinho livre). São disponibilizados também alguns programas exemplo, escritos em linguagem de montagem, que podem ser montados pelo montador e executados no simulador.

## O hardware simulado

O computador simulado é constituído de três partes:
- a memória, que contém instruções e dados,
- o subsistema de entrada e saída, que permite comunicação externa,
- a unidade central de processamento, que executa as instruções, manipula e movimenta os dados entre as demais unidades.

### Memória

A **memória** é organizada como um vetor de `int`, endereçados a partir de 0.
A memória é utilizada para conter os programas e os dados acessáveis pela CPU.
As primeiras 100 posições de memória são reservadas para uso interno da CPU, e não devem ser usadas pelos programas. Por enquanto, a CPU não faz uso dessas posições (mas fará no t1).

### Entrada e saída

O acesso aos dispositivos de **E/S** pela CPU é feito por meio de um controlador de E/S, que mapeia a identificação dos dispositivos reconhecidos pela CPU no código que implementa esses dispositivos. Para ser acessável pela CPU, um dispositivo deve antes ser registrado no controlador de E/S. Esse registro é realizado na inicialização do simulador.

Estão implementados dois dispositivos de E/S:
- **terminal**, para entrada pelo teclado e saída no vídeo,
- **relógio**, que conta o número de instruções executadas e o tempo de execução da CPU.

Os terminais são controlados pela **console**, que é quem realmente tem o controle do teclado e do vídeo físicos.
A console controla também a interface com o usuário da simulação (o operador).
Ela mostra na tela a entrada e saída dos vários terminais (está configurada para 4), os prints para debug do programa (feitos com a função console_printf), além da interface com o operador.
Essa interface permite realizar entrada e saída nos terminais e também controlar a simulação.

As funções que a console usa para acesso ao teclado e à tela estão em tela_curses.c, implementadas usando a biblioteca "curses".
Caso tenha problemas para compilar com curses, pode reimplementar as funções de tela.h em outro arquivo, usando outra biblioteca.

### CPU

A CPU é dividida em unidade de execução e unidade de controle. A **unidade de execução** contém os registradores e sabe executar cada instrução reconhecida pela CPU. A **unidade de controle** contém o laço principal de execução, que ordena a execução de uma instrução por vez, a execução das funções que permitem o funcionamento das demais unidades simuladas e o controle da simulação em geral, atendendo os comandos do operador realizados na console.

A CPU tem três registradores principais:
- **PC**, o contador de programa, tem o endereço onde está a próxima instrução a ser executada;
- **A**, acumulador, é usado nas instruções aritméticas, e meio que pra todo o resto;
- **X**, registrador auxiliar, usado para acessos indexados à memória.

Além desses, tem um registrador de **erro**, para quando a CPU detecta algum problema, e um registrador **complementar**, para quando o registrador de erro não é suficiente para codificar o problema.
Todos os registradores contêm um valor `int`, e são inicializados em 0, exceto o PC, que é inicializado em 100.

#### Instruções

As instruções que o processador reconhece (por enquanto) estão na tabela abaixo.

Uma intrução pode ocupar uma ou duas posições de memória.
A primeira é o código da instrução (campo `código` na tabela, o valor em `mem[PC]`), presente em todas as instruções; a segunda é o argumento da instrução (o valor em `mem[PC+1]`, chamado `A1` na tabela), presente somente em algumas instrucões.
O campo `args` da tabela contém `0` para instruções sem argumento (ocupam uma posição de memória e não tem `A1`) e `1` para as que contêm um argumento (essas instruções ocupam duas posições de memória).

Ao final da execução bem sucedida de uma instrução, caso não seja uma instrução de desvio que causou a alteração do PC, o PC é incrementado para apontar para a instrução seguinte (levando em consideração o número de argumentos da instrução).

| código | nome   | args | operação                  | descrição |
| -----: | :----- | :--: | :--------                 | :-------- |
|    --- | ------ | ---  | ------------------------- | **controle** |
|      0 | NOP    | 0    | -                         | não faz nada |
|      1 | PARA   | 0    | erro = ERR_CPU_PARADA     | para a CPU |
|    --- | ------ | ---  | ------------------------- | **acesso à memória** |
|      2 | CARGI  | 1    | A = A1                    | carrega imediato |
|      3 | CARGM  | 1    | A = mem[A1]               | carrega da memória |
|      4 | CARGX  | 1    | A = mem[A1+X]             | carrega indexado |
|      5 | ARMM   | 1    | mem[A1] = A               | armazena na memória |
|      6 | ARMX   | 1    | mem[A1+X] = A             | armazena indexado |
|    --- | ------ | ---  | ------------------------- | **acesso a registradores** |
|      7 | TRAX   | 0    | X ⇄ A                     | troca A com X |
|      8 | CPXA   | 0    | A = X                     | copia X para A |
|    --- | ------ | ---  | ------------------------- | **aritmética** |
|      9 | INCX   | 0    | X++                       | incrementa X |
|     10 | SOMA   | 1    | A += mem[A1]              | soma |
|     11 | SUB    | 1    | A -= mem[A1]              | subtração |
|     12 | MULT   | 1    | A \*= mem[A1]             | multiplicação |
|     13 | DIV    | 1    | A /= mem[A1]              | quociente da divisão |
|     14 | RESTO  | 1    | A %= mem[A1]              | resto da divisão |
|     15 | NEG    | 0    | A = -A                    | negação |
|    --- | ------ | ---  | ------------------------- | **desvios** |
|     16 | DESV   | 1    | PC = A1                   | desvio |
|     17 | DESVZ  | 1    | se A for 0, PC = A1       | desvio se zero|
|     18 | DESVNZ | 1    | se A não for 0, PC = A1   | desvio se não zero |
|     19 | DESVN  | 1    | se A < 0, PC = A1         | desvio se negativo |
|     20 | DESVP  | 1    | se A > 0, PC = A1         | desvio se positivo |
|    --- | ------ | ---  | ------------------------- | **chamada de subrotina** |
|     21 | CHAMA  | 1    | mem[A1] = PC+2; PC = A1+1 | chamada de subrotina |
|     22 | RET    | 1    | PC = mem[A1]              | retorno de subrotina |
|    --- | ------ | ---  | ------------------------- | **entrada e saída** |
|     23 | LE     | 1    | A = es[A1]                | leitura do dispositivo A1 |
|     24 | ESCR   | 1    | es[A1] = A                | escrita no dispositivo A1 |

A CPU só executa uma instrução se o registrador de erro indicar que a CPU não está em erro (valor `ERR_OK`).
A execução de uma instrução pode colocar a CPU em erro, por tentativa de execução de instrução ilegal, acesso a posição inválida de memória, acesso a dispositivo de E/S inexistente, etc.
Os códigos de erro estão em `err.h`.
Caso isso aconteça, o valor do PC não é alterado e o código do erro é colocado no registrador de erro.
Para alguns erros, um valor adicional é colocado no registrador de complemento de erro (por exemplo, em caso de erro de acesso à memória, é colocado no complemento o endereço que causou erro de acesso).

O processador não tem uma pilha de execução, que a maior parte dos processadores reais usa para implementar chamadas de subrotinas.
A chamada de subrotinas é implementada de uma forma mais simples e limitada: deve-se reservar uma posição de memória antes da primeira instrução de uma subrotina.
A instrução CHAMA tem como argumento o endereço dessa posição, e coloca aí o endereço para onde a subrotina deve retornar, antes de desviar para o endereço seguinte. A instrução RET tem esse mesmo endereço como argumento, e desvia para esse endereço que está nesse local.

## Compilação e execução do simulador

A compilação é realizada pelo programa `make`, que por sua vez é controlado pelo arquivo `Makefile`. Basta copiar os arquivos do github para um diretório e executar
```sh
make
```
Se tudo der certo, um executável 'main' será gerado, além de 'montador' e um .maq para cada .asm)

Se tiver tendo problemas pra compilar com o make, dá para compilar manualmente (ou com um script) com os comandos:
```
gcc -Wall -Werror main.c controle.c cpu.c instrucao.c err.c memoria.c es.c relogio.c terminal.c console.c tela_curses.c programa.c -lcurses -o main
gcc -Wall -Werror montador.c instrucao.c err.c -o montador
./montador ex1.asm > ex1.maq
./montador ex2.asm > ex2.maq
./montador ex3.asm > ex3.maq
./montador ex4.asm > ex4.maq
./montador ex5.asm > ex5.maq
./montador ex6.asm > ex6.maq
```

Para executar o simulador, execute
```sh
./main
```
ou, caso queira executar um programa diferente de ex1.maq,
```sh
./main ex5.maq
```
Durante a execução, é apresentada a console do simulador.
Ela é definida com um tamanho fixo de 24 linhas de 80 colunas.
Para que a apresentação seja bem sucedida, é necessário que o emulador de terminal tenha pelo menos 24 linhas de 80 colunas.
A tela do simulador é dividida em 4 partes:
- no topo, duas linhas para cada terminal, uma com a saída do terminal (o que for escrito nele pelo programa) e outra com a entrada (o que for digitado pelo operador e ainda não lido pelo programa).
- no meio, uma linha com o estado do simulador (a primeira palavra, que deve ser `PARADO`, já que a simulação inicia nesse estado) e o estado da CPU, com:
   - o valor dos registradores PC, A e X,
   - a próxima instrução a ser executada (a que está no endereço do PC),
   - o erro da CPU, se for o caso.
- embaixo, linha de entrada de comandos pelo operador
- no espaço entre a linha de estado e a de entrada aparecerão mensagens da console, se houverem,

O controle da execução é realizado pelo operador com a entrada textual de comandos na console.
Cada comando é digitado em uma linha, terminada por `enter`.
A linha pode ser editada antes do `enter` com `backspace`.
Existem 3 comandos para controle dos terminais e 4 para controle da execução:
- **E**, faz a entrada de texto em um terminal (exemplo: `eaxis` coloca uma linha com o valor `xis ` na entrada do terminal `a`, o primeiro)
- **Z**, limpa a tela de um terminal (exemplo: `zb` limpa a saída (uma linha na tela) do segundo terminal, `b`)
- **D**, altera o tempo de espera em cada acesso ao teclado, mudando a velocidade da simulação (o valor default corresponde a D5)
- **P**, para a execução, o controlador não vai mais mandar a CPU executar instruções em seu laço (a execução inicia nesse modo)
- **1**, o controlador vai executar uma instrução e depois parar
- **C**, o controlador vai continuar a execução das instruções uma após a outra
- **F**, fim da simulação.

## Linguagem de montagem

A entrada do montador é um arquivo contendo um programa em linguagem de montagem.
Esse programa é um texto em ASCII, dividido em linhas.
O caractere `;` e todos que o seguem em uma linha são ignorados pelo montador.
Linhas em branco também são ignoradas.

Uma linha não ignorada é constituída de 3 partes, separadas por espaços:
- label
- instrução ou pseudo-instrução
- argumento da instrução

O label pode ser qualquer palavra, que não seja uma instrução válida e que ainda não tenha sido utilizada como label.
O label, se presente, deve iniciar na primeira coluna da linha.

A instrução pode ser o nome de uma instrução (coluna `nome` na tabela de instruções) ou uma das pseudo-instruções reconhecidas pelo montador (`DEFINE`, `VALOR`, `ESPACO` ou `STRING`).
A instrução, se presente, não pode iniciar na primeira coluna da linha.

O argumento da instrução deve sempre seguir a instrução, e os argumentos válidos dependem da instrução.

O montador tem internamente um valor que corresponde ao endereço de montagem da próxima instrução. É o endereço na memória do processador onde será colocada a próxima instrução montada, quando o programa for executado. Esse valor é inicializado em 100 pelo montador, ou pode ser mudado para outro valor passando a opção `-e valor` para o montador.

O montador lê cada linha do arquivo de entrada e traduz nos códigos equivalentes. Por exemplo, se a linha contiver ` PARA`, ele vai gerar o valor `1` (o código da instrução PARA) no endereço de montagem; se a linha contiver ` LE 3` ele vai gerar `23 3` nos próximos dois endereços.

Um label define um valor para um símbolo. Esse valor será o endereço de montagem da próxima instrução ou um outro valor, se o label for imediatamente seguido da pseudo instrução `DEFINE`. Nesse caso, o valor do label será definido como o valor do argumento.
Por exemplo,
`xis define 12` definirá o valor 12 para o símbolo `xis`.

Usando-se símbolos internos, os programas podem ficar mais legíveis.
Por exemplo, a instrução `LE 3` pode ser mais facilmente entendida se for escrita `LE teclado`, supondo que e teclado corresponda ao dispositivo 3. Isso pode ser feito definindo teclado com o valor 3 com a pseudo instrução `teclado DEFINE 3`.

Labels também servem para dar nomes para posições de memória. Por exemplo, se quisermos colocar uma instrução que desvie para a instrução ` LE ` acima, temos que saber em que endereço essa instrução está. Com um label na instrução, o montador atribui esse endereço para o símbolo. O código abaixo implementa um laço, que executará até que seja lido um valor diferente de zero do dispositivo 2. O label `denovo` será definido com o endereço onde será colocada a instrução `LE`.

```
   ...
   denovo LE 2          ; lê o valor do dispositivo 2 e coloca no reg. A
          DESVZ denovo  ; se o reg. A for 0, desvia para a instrução em denovo
   ...
```

Além de `DEFINE`, o montador reconhece as pseudo instruções `VALOR`, `STRING` e `ESPACO`. Elas são usadas para facilitar a inicialização e a reserva de espaço para variáveis do programa.
- `VALOR` tem um número como argumento, e coloca esse valor na próxima posição da memória.
- `ESPACO` também tem um número como argumento, que diz quantos zeros serão colocados nas próximas posições da memória (é como se fossem vários `VALOR 0`).
- `STRING` define as próximas posições da memória com os valores ASCII dos caracteres entre aspas, seguido de um valor 0.

Por exemplo, se o código abaixo (que não faz nada de útil, é só para exemplificar o que o montador vai fazer) for montado no endereço 100, cada linha vai gerar:
- linha 1: nada, só define o símbolo tecl,
- linha 2: o valor 23 (o código de LE) no endereço 100, 3 (o valor definido para tecl) no endereço 101,
- linha 3: 5 no 102 (ARMM), 105 no 103 (y vai ficar no endereço 105),
- linha 4: 7 no 104 (VALOR),
- linha 5: 0 em 105 e 106 (ESPACO),
- linha 6: 65, 48 e 0 em 107, 108 e 109 (STRING).
```
tecl DEFINE 3
     LE tecl
     ARMM y
     VALOR 7
y    ESPACO 2
     STRING 'A0'
```
A saída do montador para a entrada acima é:
```
MAQ 10 100
[ 100] = 23, 3, 5, 105, 7, 0, 0, 65, 48, 0,
```
A primeira linha diz que é um arquivo MAQ, que ocupa 10 posições de memória a partir do endereço 100.
As linhas seguintes têm o endereço e o valor de até 10 posições.
A leitura desse arquivo pode ser feita por código em "programa.c".


## Implementação

A implementação do simulador está dividida em vários módulos, cada um implementado em um arquivo `.h` que contém a declaração de um tipo opaco e de funções para operar sobre dados desse tipo, e em um arquivo `.c` de mesmo nome, que define o tipo e implementa as funções do `.h` e mais funções auxiliares ao módulo.
Os módulos são:
- **controle**, o controlador da execução do hardware, contém o laço de execução das instruções
- **cpu**, o executor de instruções, contém os registradores da CPU e o código para execução de cada instrução
- **memoria**, a memória principal do processador, um vetor de inteiros e funções para acessá-lo
- **es**, o controlador de E/S, faz o meio campo entre a CPU e os dispositivos de E/S; para que a CPU possa acessar um dispositivo, ele deve antes ser registrado neste módulo
- **dispositivos.h**, contém a identificação dos dispositivos conhecidos (são registrados em main.c)
- **relogio**, conta o número de instruções executadas e disponibiliza esse número e o relógio de tempo real do sistema, em 2 dispositivos de entrada
- **terminal**, simula um terminal de vídeo mínimo, com uma linha de entrada e uma de saída
- **console**, controla a tela e o teclado reais, gerencia os terminais, mostra mensagens do sistema e a console do operador (é o módulo mais complicado)
- **tela_curses**, funções para acesso à tela e ao teclado, implementadas usando curses [argh]
- **err**, define um tipo para codificar os erros
- **instrucao**, com nomes e códigos das instruções da CPU
- **programa**, carrega programas em linguagem de máquina (arquivos .maq) para a memória
- **main.c**, um programa para conectar os módulos acima, inicializa o hardware, carrega um programa na memória e dispara a execução do laço principal do controlador. Se não receber argumentos, carregará o programa em `ex1.maq`. Para executar outro programa, execute com o nome do arquivo como argumento (`./main ex2.maq`). A função `main` está nesse arquivo.

Além dos arquivos que implementam o simulador, os demais arquivos são:
- **montador.c**, um montador para transformar programas .asm (em linguagem de montagem) em .maq (em linguagem de máquina)
- **ex\*.asm**, programinhas de teste em linguagem de montagem
- **Makefile**, para facilitar a compilação da tralha toda.


### Simulador

O componente principal do simulador é o executor (em cpu.c), cuja função `cpu_executa_1` simula a execução de uma instrução.
Para isso, ela pega na memória o valor que está na posição do PC (que contém o código da próxima instrução a executar), e chama uma função correspondente a esse valor, que será responsável pela simulação dessa instrução.
Essas funções têm acesso aos registradores e à memória para realizar essa simulação.
As instruções que têm argumento (A1 na tabela de instruções) podem obtê-lo na posição PC+1 da memória.
Cada função é também responsável por atualizar o valor do PC caso a execução da instrução tenha transcorrido sem erro.

As instruções de E/S (LE e ESCR) acessam os dispositivos através do módulo controlador de E/S `es`.
Para serem acessíveis, os dispositivos devem antes ser registrados nesse módulo. 
Isso é feito na inicialização do simulador, em `main.c`, com chamadas a `es_registra_dispositivo`, contendo como argumentos o número com que esse dispositivo vai ser identificado nas instruções de E/S, o controlador desse dispositivo, o número com que esse dispositivo é identificado pelo controlador, e as funções que devem ser usadas para ler ou escrever nesse dispositivo.
Tem dois controladores de dispositivos implementados, um para ler e escrever em um terminal (em `terminal.c`) e um para ler o valor do relógio (`relogio.c`).

O relógio tem dois dispositivos:
- 0: contém o número de instruções executadas
- 1: contém os milisegundos (de tempo real) que se passaram desde o início da simulação.

Cada terminal tem 4 dispositivos:
- 0: leitura do próximo caractere do teclado - fornece o próximo caractere que já foi digitado; caso não tenha caractere disponível, a CPU será colocada em erro
- 1: leitura da disponibilidade de caracteres do teclado - fornece o valor 1 caso exista algum caractere digitado e ainda não lido, 0 caso contrário
- 2: escrita de um caractere na tela - escreve um caractere na tela, se ela estiver disponível (ela fica indisponível enquanto está rolando), ou coloca a CPU em erro
- 3: leitura de disponibilidade da tela - fornece o valor 1 caso a tela aceite um novo caractere, 0 caso contrário

O controlador de console (em `console.c`), além de controlar a console do operador, realiza o suporte a vários terminais (está definido com 4).
Os terminais são inicializados pela console quando esta é inicializada.
Na forma como está o código, apesar da console controlar 4 terminais, só os dois primeiros estão sendo registrados no controlador de E/S na inicialização do hardware (em `main.c`): dispositivos 0, 1, 2 e 3 para o primeiro terminal e 4, 5, 6 e 7 para o segundo. Os dispositivos 8 e 9 estão sendo registrados como os dispositivos 0 e 1 do relógio.

A sequência de execução do simulador é:
- em main.c, cria os módulos de hardware, interliga-os, inicializa-os, carrega um programa na memória, e chama o laço de simulação
- em controle.c, o laço de simulação repete os passos abaixo até que o operador encerre a simulação:
   - executa uma instrução
   - para a simulação se a CPU ficar em erro
   - chama a função tictac do relógio, para ele se atualizar
   - chama a função tictac da console, para ela se atualizar, ler o teclado, atualizar os terminais, desenhar a tela
   - executa eventual comando do operador vindo da console
   - atualiza a linha de status da console

### Montador

Olha o código, se tiver curiosidade.

## Sugestões do que fazer com isso

Compile, execute os programas .asm exemplo, tente entender os .asm.

Para auxiliar na familiarização com a CPU, implemente (em linguagem de montagem) um programa que imprime o número de instruções executadas e os segundos de execução, durante um certo tempo.

Para auxiliar na familiarização com o código, implemente um novo dispositivo de entrada para o computador, que fornece um número aleatório cada vez que é lido.
Altere o programa de adivinhação para usar esse dispositivo.

Para auxiliar mais ainda na familiarização com a CPU, implemente um programa .asm que lê 10 (ou "n") valores desse novo dispositivo e imprime os valores no terminal. Pode aumentar o grau de dificuldade imprimindo eles em ordem crescente.

Como a console toma conta da tela do terminal, não use printf no seu programa. Caso necessite que o programa imprima (para depuração por exemplo), use a função console_printf, que imprime na tela da console. Tudo que é impresso dessa forma é salvo em um arquivo de log.

### Alterações ao código após a publicação

Depois de publicado (em 10set), o código do t0 foi alterado no github:
- 13set:
   - comentários no Makefile sobre a necessidade dos caracteres "tab";
   - inclusão de dispositivos.h, com nomes para a identificação dos dispositivos, e alterações nos arquivos afetados (principalmente main.c)
- 16set:
   - não para o relógio quando a CPU tem erro (no t0 não faz diferença, mas no t1 sim)
   - remove cpu_estado(), que só servia para isso
