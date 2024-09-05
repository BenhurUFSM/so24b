## t0 - simulador de um computador

Familiarize-se com o código anexo, que simula um pequeno computador, que será usado durante o desenvolvimento da disciplina.

Para auxiliar na familiarização com a CPU, implemente (em linguagem de montagem) um programa que imprime o número de instruções executadas e os segundos de execução, durante um certo tempo.

Para auxiliar mais na familiarização com o código, implemente um novo dispositivo de entrada para o computador, que fornece um número aleatório cada vez que é lido.
Altere o programa de adivinhação para usar esse dispositivo.

Para auxiliar mais ainda na familiarização com a CPU, implemente um programa que lê 10 valores desse novo dispositivo e imprime os 10 valores no terminal. Pode aumentar o grau de dificuldade imprimindo eles em ordem crescente.


### Descrição sucinta do código fornecido

Esta descrição é sucinta e incompleta. Olhe o código, faça perguntas. Quanto mais se familiarizar com este código, mais fácil será realizar os trabalhos. As perguntas recebidas (e suas respostas) serão colocadas no final deste texto.

O código implementa dois programas: um simulador de uma CPU bem simples e um montador, para produzir código em linguagem de máquina para essa CPU à partir de programas em linguagem de montagem (a implementação de um compilador fica como sugestão de exercício para quem estiver com um tempinho livre). São disponibilizados também alguns programas exemplo, escritos em linguagem de montagem, que podem ser montados pelo montador e executados no simulador.

#### O hardware simulado

O computador simulado é constituído de três partes:
- a memória, que contém instruções e dados,
- o subsistema de entrada e saída, que permite comunicação externa,
- a unidade central de processamento, que executa as instruções, manipula e movimenta os dados entre as demais unidades.


A memória é organizada como um vetor de `int`, endereçados a partir de 0.
A memória é utilizada para conter os programas e os dados acessáveis pela CPU.
As primeiras 100 posições de memória são reservadas para uso interno da CPU, e não devem ser usadas pelos programas. Por enquanto, a CPU não faz uso dessas posições (mas fará no t1).


O acesso aos dispositivos de E/S pela CPU é feito por meio de um controlador de E/S, que mapeia o identificação dos dispositivos reconhecidos pela CPU no código que implementa esses dispositivos. Para ser acessável pela CPU, um dispositivo deve antes ser registrado no controlador de E/S. Esse registro é realizado na inicialização do simulador.

Estão implementados dois dispositivos de E/S:
- terminal, para entrada pelo teclado e saída no vídeo,
- um relógio, que conta o número de instruções executadas e o tempo de execução da CPU.

Os terminais são controlados pela console, que é quem realmente tem o controle do teclado e do vídeo físicos. A console controla a interface com o usuário da simulação (o operador). Ela mostra na tela a entrada e saída dos vários terminais (está configurada para 4), os prints para debug do programa (feitos com console_printf) além da interface com o operador. Essa interface permite controlar os terminais e também a simulação. A console está implementada usando a biblioteca "curses". Caso tenha problemas para compilar com curses, pode alterar as funções tela_* de console.c para usar outra biblioteca.

A CPU é dividida na unidade de execução e na unidade de controle. A unidade de execução contém os registradores e sabe executar cada instrução reconhecida pela CPU. A unidade de controle contém o laço principal de execução, que ordena a execução de uma instrução por vez, a execução das funções que permitem o funcionamento das demais unidades simuladas e o controle da simulação em geral, atendendo os comandos do operador realizados na console.

...

A CPU tem três registradores principais:
- **PC**, o contador de programa, tem o endereço onde está a próxima instrução a ser executada;
- **A**, acumulador, é usado nas instruções aritméticas, e meio que pra todo o resto;
- **X**, registrador auxiliar, usado para acessos indexados à memória.

Além desses, tem um registrador de erro, para quando a CPU detecta algum problema, e um registrador complementar, para quando o registrador de erro não é suficiente para codificar o problema.
Todos os registradores podem conter um valor `int`, e são inicializados em 0.



As instruções que o processador reconhece (por enquanto) estão na tabela abaixo.

Uma intrução pode ocupar uma ou duas posições de memória.
A primeira é o código da instrução (campo `código` na tabela, o valor em `mem[PC]`), presente em todas as instruções; a segunda é o argumento da instrução (o valor em `mem[PC+1]`, chamado `A1` na tabela), presente somente em algumas instrucões.
O campo `#arg` da tabela contém 0 para instruções sem argumento (ocupam uma posição de memória) e 1 para as que contêm um argumento (essas instruções ocupam duas posições de memória).

Ao final da execução bem sucedida de uma instrução, caso não seja uma instrução de desvio que causou a alteração do PC, o PC é incrementado para apontar para a instrução seguinte (levando em consideração o número de argumentos da instrução).

| código |   nome | #arg | operação  | descrição |
| -----: | :----- | :--: | :-------- | :-------- |
|---|---|---|---| **controle** |
|      0 | NOP    | 0    | -         | não faz nada |
|      1 | PARA   | 0    | err = ERR_CPU_PARADA | para a CPU |
|      2 | CARGI  | 1    | A = A1      | carrega imediato |
|      3 | CARGM  | 1    | A = mem[A1] | carrega da memória |
|      4 | CARGX  | 1    | A = mem[A1+X] | carrega indexado |
|      5 | ARMM   | 1    | mem[A1] = A | armazena na memória |
|      6 | ARMX   | 1    | mem[A1+X] = A | armazena indexado |
|      7 | TRAX   | 0    | X ⇄ A       | troca A com X |
|      8 | CPXA   | 0    | A = X       | copia X para A |
|      9 | INCX   | 0    | X++       | incrementa X |
|     10 | SOMA   | 1    | A += mem[A1] | soma |
|     11 | SUB    | 1    | A -= mem[A1] | subtração |
|     12 | MULT   | 1    | A \*= mem[A1] | multiplicação |
|     13 | DIV    | 1    | A /= mem[A1] | quociente da divisão |
|     14 | RESTO  | 1    | A %= mem[A1] | resto da divisão |
|     15 | NEG    | 0    | A = -A       | negação |
|     16 | DESV   | 1    | PC = A1      | desvio |
|     17 | DESVZ  | 1    | se A for 0, PC = A1 | desvio condicional se zero|
|     18 | DESVNZ | 1    | se A não for 0, PC = A1 | desvio condicional se não zero |
|     19 | DESVN  | 1    | se A < 0, PC = A1 | desvio condicional se negativo |
|     20 | DESVP  | 1    | se A > 0, PC = A1 | desvio condicional se positivo |
|     21 | CHAMA  | 1    | mem[A1] = PC+2; PC = A1+1 | chama subrotina |
|     22 | RET    | 1    | PC = mem[A1] | retorna de subrotina |
|     23 | LE     | 1    | A = es[A1]   | leitura do dispositivo A1 |
|     24 | ESCR   | 1    | es[A1] = A   | escrita no dispositivo A1 |

A CPU só executa uma instrução se o registrador de erro indicar que a CPU não está em erro (valor ERR_OK).
A execução de uma instrução pode colocar a CPU em erro, por tentativa de execução de instrução ilegal, acesso a posição inválida de memória, acesso a dispositivo de E/S inexistente, etc. Os códigos de erro estão em err.h.


A implementação do simulador está dividida em vários módulos, cada um implementado em um arquivo `.h` que contém a declaração de um tipo opaco e de funções para operar sobre dados desse tipo, e em um arquivo `.c` de mesmo nome, que define o tipo e implementa as funções do `.h`.
Os módulos são:
- **controle**, o controlador da execução do hardware, contém o laço de execução das instruções
- **cpu**, o executor de instruções, contém os registradores da CPU e o código para execução de cada instrução
- **memoria**, a memória principal do processador, um vetor de inteiros e funções para acessá-lo
- **es**, o controlador de E/S, faz o meio campo entre a CPU e os dispositivos de E/S; para que a CPU possa acessar um dispositivo, ele deve antes ser registrado neste módulo
- **relogio**, conta o número de instruções executadas e disponibiliza esse número e o relógio de tempo real do sistema, em 2 dispositivos de entrada
- **console**, controla a tela e o teclado reais, implementa terminais, mostra mensagens do sistema e a console do operador (é o módulo mais complicado, e [argh] está implementado usando curses)
- **err**, define um tipo para codificar os erros
- **instrucao**, com nomes e códigos das instruções da CPU
- **programa**, carrega programas em linguagem de máquina (arquivos .maq) para a memória
- **main.c**, um programa para testar os módulos acima, inicializa o hardware, carrega um programa na memória e dispara a execução do laço principal do controlador. Como está, ele executará o programa em `ex1.maq`; tem que ser alterado para trocar o programa a executar. A função `main` está nesse arquivo

Além dos arquivos que implementam o simulador, os demais arquivos são:
- **montador.c**, um montador para transformar programas .asm (em linguagem de montagem) em .maq (em linguagem de máquina)
- **ex\*.asm**, programinhas de teste em linguagem de montagem
- **Makefile**, para facilitar a compilação da tralha toda (coloque todos esses arquivos em um diretório e execute o programa 'make' nesse diretório -- se tudo der certo, um executável 'main' será gerado, além de 'montador' e um .maq para cada .asm)

O make é meio exigente com o formato do Makefile, as linhas que não iniciam na coluna 1 têm que iniciar com um caractere tab.
Se tiver tendo problemas pra compilar, dá para compilar sem o make, com os comandos:
```
gcc -Wall -Werror main.c cpu.c es.c memoria.c relogio.c console.c instrucao.c err.c programa.c controle.c -lcurses -o main
gcc -Wall -Werror montador.c instrucao.c err.c -o montador
./montador ex1.asm > ex1.maq
./montador ex2.asm > ex2.maq
./montador ex3.asm > ex3.maq
./montador ex4.asm > ex4.maq
./montador ex5.asm > ex5.maq
./montador ex6.asm > ex6.maq
```


### Descrição menos sucinta

São dois programas, o montador e o simulador.
O montador traduz um programa escrito em linguagem de montagem em um programa equivalente em linguagem de máquina (um arquivo com os valores que devem ser colocados na memória da máquina simulada).
O simulador, tendo a memória inicializada com o programa, executa as instruções, simulando o comportamento de um computador.

#### Montador

O código do montador está no arquivo `montador.c`.

O montador lê cada linha do arquivo de entrada e traduz nos códigos equivalentes.
Por exemplo, se a linha contiver ` PARA `, ele vai gerar ` 1 ` (o código da instrução PARA, veja a tabela acima); se a linha contiver ` LE 3 ` ele vai gerar ` 23 3 `.

Além dessas conversões diretas, o montador também pode dar valores a símbolos. Tem duas formas de se fazer isso, definindo explicitamente um símbolo com a pseudo instrução `DEFINE` ou com o uso de labels.

Com `DEFINE` pode-se dar nomes a valores constantes. Por exemplo, a instrução ` LE 3 ` pode ser mais facilmente entendida se for escrita ` LE teclado `. Isso pode ser feito definindo `teclado` com o valor `3` com a pseudo instrução ` teclado DEFINE 3 `. É chamada de pseudo instrução porque não é uma instrução do processador, mas uma instrução interna para o montador.

Labels servem para dar nomes para posições de memória. Por exemplo, se quisermos colocar uma instrução que desvie para a instrução ` LE ` acima, temos que saber em que endereço essa instrução está. Com um label, o montador calcula esse endereço. O código abaixo implementa um laço, que executará até que seja lido um valor diferente de zero do dispositivo 2. O label `denovo` será definido com o endereço onde será colocada a instrução `LE`.
```
   ...
   denovo LE 2
          DESVZ denovo
   ...
```

Além de `DEFINE`, o montador reconhece as pseudo instruções `VALOR`, `STRING` e `ESPACO`. Elas são usadas para facilitar a inicialização e a reserva de espaço para variáveis do programa. `VALOR` tem um número como argumento, e coloca esse valor na próxima posição da memória. `ESPACO` também tem um número como argumento, que diz quantos zeros serão colocador nas próximas posições da memória. `STRING` define as próximas posições da memória com os valores dos caracteres entre aspas.
Por exemplo, se o código abaixo for montado no endereço 0, vai colocar o valor 23 (o código de LE) no endereço 0, 3 no endereço 1, 5 no 2 (ARMM), 5 no 3 (y vai ficar no endereço 5), 7 no 4 (VALOR), 0 em 5 e 6 (ESPACO), 65, 48 e 0 em 7, 8 e 9 (STRING).
```
   LE 3
   ARMM y
   VALOR 7
y  ESPACO 2
   STRING 'A0'
```
A saída do montador para a entrada acima é:
```
MAQ 10 0
[   0] = 23, 3, 5, 5, 7, 0, 0, 65, 48, 0,
```
A primeira linha diz que é um arquivo MAQ, que ocupa 10 posições de memória a partir do endereço 0. 
As linhas seguintes têm o endereço e o valor de até 10 posições.
A leitura desse arquivo pode ser feita por código em "programa.c".

#### Simulador

O código do simulador é formado pelos demais arquivos .c e .h.

O componente principal do simulador é o executor (em cpu.c), cuja função `cpu_executa_1` simula a execução de uma instrução.
Para isso, ela pega na memória o valor que está na posição do PC (que contém o código da próxima instrução a executar), e chama uma função correspondente a esse valor, que será responsável pela simulação dessa instrução.
Essas funções têm acesso aos registradores e à memória para realizar essa simulação.
As instruções que têm argumento (A1 na tabela de instruções) podem obtê-lo na posição PC+1 da memória.
Cada função é também responsável por atualizar o valor do PC caso a execução da instrução tenha transcorrido sem erro.

As instruções de E/S (LE e ESCR) acessam os dispositivos através do módulo controlador de E/S `es`.
Para serem acessíveis, os dispositivos devem antes ser registrados nesse módulo. 
Isso é feito na inicialização do controlador, em `controle.c`, com chamadas a `es_registra_dispositivo`, contendo como argumentos o número com que esse dispositivo vai ser identificado nas instruções de E/S, o controlador desse dispositivo, o número com que esse dispositivo é identificado pelo controlador, e as funções que devem ser usadas para ler ou escrever nesse dispositivo.
Tem dois controladores implementados, um para ler e escrever números no terminal (em `console`) e um para ler o valor do relógio (`relogio`). Esse último controla dois dispositivos, um relógio que conta as instruções executadas e outro que conta milisegundos.

O controlador de terminais (em console.c) tem suporte a vários terminais (está definido com 4).
Cada terminal tem 4 dispositivos:
- leitura do próximo caractere do teclado - fornece o próximo caractere que já foi digitado; caso não tenha caractere disponível, a CPU será colocada em erro
- leitura da disponibilidade de caracteres do teclado - fornece o valor 1 caso exista algum caractere digitado e ainda não lido, 0 caso contrário
- escrita de um caractere na tela - escreve um caractere na tela, se ela estiver disponível (ela fica indisponível enquanto está rolando), ou coloca a CPU em erro
- leitura de disponibilidade da tela - fornece o valor 1 caso a tela aceite um novo caractere, 0 caso contrário

Esses dispositivos são identificados respectivamente como 0, 1, 2 e 3 para o primeiro terminal, 4, 5, 6 e 7 para o segundo etc.
Essa é a identificação dos dispositivos no controlador de terminais (console.c). A identificação deles para a CPU depende de como eles forem registrados no controlador de E/S.

#### Console

Além de realizar a E/S dos terminais, a console mostra o estado interno da CPU, mensagens do restante do simulador, além de permitir o controle da execução.
O controle da execução é realizado pelo operador com a entrada textual de comandos na console.
Cada comando é digitado em uma linha, terminada por `enter`. A linha pode ser editada antes do `enter` com `backspace`.
Existem 3 comandos para controle dos terminais e 4 para controle da execução:
- **E**, entrada de uma linha em um terminal (exemplo: `eaxis` coloca uma linha com o valor `xis\n` na entrada do terminal `a`, o primeiro)
- **Z**, limpa a tela de um terminal (exemplo: `zb` limpa a saída (uma linha na tela) do segundo terminal, `b`)
- **D**, para alterar a espera em cada acesso a teclado, mudando a velocidade da simulação (o valor default corresponde a D5)
- **P**, para a execução, o controlador não vai mais mandar a CPU executar instruções em seu laço (a execução inicia nesse modo)
- **1**, o controlador vai executar uma instrução e depois parar
- **C**, o controlador vai continuar a execução das instruções uma após a outra
- **F**, fim da simulação
