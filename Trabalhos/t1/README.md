## t1 - implementação de processos

No t0, o simulador era dedicado a executar um só programa, colocado na memória na inicialização do simulador, e não podia ser alterado sem interromper e reiniciar a simulação.
Agora teremos um mini sistema operacional, para permitir a execução de mais de um programa.
Nesse SO, será implementado:
- suporte a processos com multiprogramação,
- um escalonador (ou três),
- chamadas de sistema,
- bloqueio de processos por entrada e saída e para esperar a morte de outro, para melhorar o uso da CPU,
- preempção de processos, para melhorar a distribuição da CPU.

Os processos não podem mais executar as intruções de E/S ou de parada, devendo chamar o SO para isso.
Cada processo terá uma entrada e uma saída, que serão atribuídos ao processo na sua criação.

Nosso SO tem 5 chamadas de sistema:
- SO_LE: lê um inteiro da entrada do processo;
- SO_ESCR: escreve um inteiro na saída do processo;
- SO_CRIA_PROC: cria um novo processo;
- SO_MATA_PROC: mata um processo (podendo ser o próprio que fez a chamada);
- SO_ESPERA_PROC: espera a morte de outro processo.

Para possilitar esses recursos, necessitamos algumas alterações no hardware.

A CPU passa a ter dois modos de execução, **usuário** e **supervisor**.
Algumas instruções sensíveis só são executadas no modo supervisor.
Os programas serão executados no modo usuário, não sendo possível para eles executar essas instruções.

A passagem da CPU para o modo supervisor só pode acontecer de uma forma controlada, em que, além de mudar o modo, muda também o endereço de execução.
Além disso, a execução nesse novo endereço só é permitida em modo supervisor.
Com isso, dá para organizar a execução de forma que o programa de usuário não consiga executar instruções de E/S, nem desviar diretamente para código que só deve ser executado como supervisor.
Esse código será a porta de entrada para o SO, que será o único com direito a executar no modo supervisor.

Ao passar ao modo supervisor, a CPU vai alterar alguns de seus registradores (no mínimo o PC), mas para que a execução possa continuar no mesmo ponto mais tarde, é necessário que o valor anterior desses registradores seja recuperável.
Para isso, a CPU salva o valor de todos os registradores quando passa para o modo supervisor, e recupera novamente quando volta ao modo usuário. Esse salvamento é realizado nos endereços iniciais da memória.

A troca de modo de execução é realizada por duas novas instruções, CHAMAS (chamada ao sistema) e RETI (retorno de interrupção).
Essa operação toda é chamada de **interrupção**, e pode acontecer em algumas situações:
- quando o programa de usuário executa a instrução CHAMAS,
- quando a CPU encontra algum erro (instrução inválida, acesso ilegal à memória, etc),
- quando algum dispositivo de E/S pede atenção.

Em qualquer desses casos, ao atender a interrupção, a CPU passa a executar em um endereço pré-definido, em modo supervisor.
Para facilitar nossa vida, a CPU não aceita interrupções em modo supervisor.

Ao atender a interrupção, a CPU, depois de mudar o modo para supervisor e salvar os registradores, altera o PC para o endereço pré-definido, onde deve ter sido colocado o código do SO. 
No nosso simulador, não vamos implementar o SO em linguagem de montagem, e não temos um compilador para traduzir código de outra linguagem para essa linguagem.

Temos uma nova intrução que, quando for encontrada pelo simulador causa a execução do uma função C.
Usaremos essa instrução para executar o SO, como se todo o sistema operacional fosse uma única instrução do processador.
Essa instrução é "CHAMAC", e deve-se configurar a CPU para dizer qual a função a chamar para executar essa instrução.

Para garantir que o SO tenha controle da máquina, o relógio passa a ter um timer, que pode ser programado para gerar interrupções, e causar a execução periódica do SO.

Essas inicializações são realizadas na inicialização do SO.

Além disso, para que o SO tenha a chance de ser a primeira coisa a executar, a CPU, antes de executar sua primeira instrução, causa uma interrupção de reset.


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
  - Makefile - inclui o que foi incluído, e uma gambiarra para montar cada asm em um endereço diferente


### Parte I

Entenda as mudanças no código e o funcionamento do SO.

Faça uma implementação inicial de processos:
- crie um tipo de dados que é uma estrutura que contém as informações a respeito de um processo
- crie uma tabela de processos, que é um vetor dessas estruturas
- inicialize a primeira entrada nessa tabela (o primeiro processo) na criação do init
- crie uma variável que designe o processo em execução. Faça de tal forma que tenha suporte a não ter nenhum processo em execução
- faça funções para salvar o estado do processador na tabela de processos (na entrada correspondente ao processo em execução) e para recuperar o estado do processador a partir da tabela; use essas funções para implementar so_salva_estado_da_cpu e so_despacha.
- implemente a função do escalonador (so_escalona). Ela escolhe o próximo processo a executar (altera a variável que designa o processo em execução). Pode ser bem simples: se o processo que estava em execução estiver pronto continua sendo executado e se não, escolhe o primeiro que encontrar na tabela que esteja pronto.
- implemente as chamadas de criação e morte de processos
- altere as chamadas de E/S, para usar um terminal diferente dependendo do pid do processo
- o pid do processo não é a mesma coisa que sua entrada na tabela: quando um processo termina sua entrada na tabela pode ser reutilizada por outro processo, o pid não, é uma espécie de número de série do processo.

### Parte II

Na parte I, um processo não bloqueia, se ele não está morto, ele pode executar.
Nesta parte, vamos implementar o bloqueio de processos e eliminar a espera ocupada na E/S.
- nas chamadas de E/S, se o dispositivo não estiver pronto, o SO deve bloquear o processo e não realizar a E/S; se o dispositivo estiver pronto, ele realiza a E/S e não bloqueia, como na parte I.
- na função que trata de pendências, o SO deve verificar o estado dos dispositivos que causaram bloqueio e realizar operações pendentes e desbloquear processos se for o caso
- implemente a chamada de sistema SO_ESPERA_PROC, que bloqueia o processo chamador até que o processo que ele identifica na chamada tenha terminado. Se o processo esperado não existe ou se for o próprio processo chamador, retorna um erro para o processo, não bloqueia ele esperando algo que não vai acontecer. Quando tratar a morte de um processo, o SO deve verificar se há alguém esperando por esse acontecimento.

### Parte III

Implemente um escalonador preemptivo *round-robin* (circular):
- os processos prontos são colocados em uma fila
- o escalonador sempre escolhe o primeiro da fila
- quando um processo fica pronto (é criado ou desbloqueia), vai para o final da fila
- se terminar o *quantum* de um processo, ele é colocado no final da fila (preempção)

O *quantum* é definido como um múltiplo do intervalo de interrupção do relógio (em outras palavras, o *quantum* equivale a tantas interrupções). Quando um processo é selecionado para executar, tem o direito de executar durante o tempo de um quantum. Uma forma simples de implementar isso é ter uma variável do SO, controlada pelo escalonador, que é inicializada com o valor do quantum (em interrupções) quando um processo diferente do que foi interrompido é selecionado. Cada vez que recebe uma interrupção do relógio, decrementa essa variável. Quando essa variável chega a zero, o processo corrente é movido para o final da fila.

Implemente um segundo escalonador, semelhante ao circular: os processos têm um quantum, e sofrem preempção quando esse quantum é excedido. Os processos têm também uma prioridade, e o escalonador escolhe o processo com maior prioridade entre os prontos.
A prioridade de um processo é calculada da seguinte forma:
- quando um processo é criado, recebe prioridade 0,5
- quando um processo perde o processador (porque bloqueou ou porque acabou seu quantum), a prioridade do processo é calculada como `prio = (prio + t_exec/t_quantum)`, onde `t_exec` é o tempo desde que ele foi escolhido para executar e `t_quantum` é o tempo do quantum. O `t_exec` é o quantum menos o valor da variável que o escalonador decrementa a cada interrupção.

O SO deve manter algumas métricas, que devem ser apresentadas no final da execução (quando o init morrer):
- número de processos criados
- tempo total de execução
- tempo total em que o sistema ficou ocioso (todos os processos bloqueados)
- número de interrupções recebidas de cada tipo
- número de preempções
- tempo de retorno de cada processo (diferença entre data do término e da criação)
- número de preempções de cada processo
- número de vezes que cada processo entrou em cada estado (pronto, bloqueado, executando)
- tempo total de cada processo em cada estado (pronto, bloqueado, executando)
- tempo médio de resposta de cada processo (tempo médio em estado pronto)

Gere um relatório de execuções do sistema em diferentes configurações.

