# t2 - implementação de memória virtual

## Descrição

No t1, foi implementado o suporte a processos, mas tem 2 limitações importantes:
- o endereço de carga de cada programa deve ser diferente, para que não tenha
  conflito de endereços entre os programas em execução. Não é possível executar o
  mesmo programa em 2 processos diferentes sem montá-lo para ser carregado em um
  endereço diferente.
- não há proteção de memória entre os processo. Não tem sequer uma forma de
  detectar se um processo fez um acesso ilegal.

O t2 visa sanar esses problemas, implementando memória virtual. Além disso, vai
ser possível executar programas mesmo que a memória necessária para sua
execução seja superior à disponível no sistema. A memória virtual será
implementada usando paginação.

Para essa implementação, vamos necessitar:
- uma MMU, para fazer a tradução entre endereços virtuais e físicos, usando uma
  tabela de páginas;
- alterar a CPU para realizar o acesso à memória através da MMU, gerando
  interrupção para o SO caso o acesso não seja possível;
- uma memória secundária, onde o conteúdo das páginas dos processos são
  mantidos quando elas não estiverem na memória principal.

E precisamos alterar o SO, para:
- manter uma tabela de páginas por processo, e alterar a configuração da MMU
  para usar a tabela certa a cada troca de processo;
- alterar a carga de programas em memória, usando a memória secundária;
- atender interrupções de falta de página, para identificar e tratar:
   - acesso ilegal à memória ou
   - troca de páginas entre a memória principal e a secundária;
- implementar um algoritmo de substituição de páginas (ou dois);
- medir o desempenho da memória virtual.

### MMU e tabela de páginas

A implementação da MMU e de tabela de páginas está sendo fornecida (arquivos mmu.[ch], tabpag.[ch]).

A tabela de páginas contém informações sobre cada página:
- o quadro correspondente a essa página;
- um bit que diz se tem tradução válida para essa página;
- um bit que diz se a página foi acessada;
- um bit que diz se a página foi alterada.

As operações sobre a tabela são:
- informar o quadro correspondente a uma página;
- invalidar as informações sobre uma página;
- obter o quadro correspondente a uma página;
- marcar/obter os bits de acesso a uma página.

A MMU tem acesso à memória principal e a uma tabela de páginas.
Ela pode realizar operações de leitura e escrita na memória, tanto com
endereços físicos quanto com endereços virtuais, que ela traduz para físicos
com o uso da tabela de páginas. Produz erros caso a tradução não seja possível
ou caso o acesso seja negado pela memória. As operações da MMU são:
- acesso de leitura ou escrita à memória -- tem interface semelhante à da
  memória, com a diferença de receber um argumento a mais, que permite que o
  acesso seja à memória virtual ou física (esse último é feito pela CPU quando em
  modo supervisor);
- definição da tabela de páginas a usar -- usada pelo SO cada vez que troca o
  processo em execução. 

### Memória secundária

A memória secundária tipicamente é implementada usando dispositivos de armazenamento (discos). Durante uma troca, o processo fica bloqueado enquanto as transferências são realizadar no disco.
No nosso caso, vamos simplificar e usar a mesma implementação da memória primária (`mem_t`), e bloquear o processo por tempo.

Vamos considerar que o tempo de transferência de uma página entre a memória principal e a secundária é sempre o mesmo (e é uma configuração do sistema).
A memória secundária mantém uma variável que diz quando o disco estará livre (ou se já está). Quando uma troca de página é necessária, se o disco estiver livre, atualiza-se esse tempo para "agora" mais o tempo de espera; se não estiver, soma-se o tempo de espera a essa variável. De qualquer forma, o valor da variável indicará a data até a qual o processo deve ser bloqueado por causa dessa troca de página.

### Uma tabela de páginas por processo

Quando um processo é criado, deve ser também criada uma tabela de páginas para o mapeamento das suas páginas virtuais nos quadros da memória física. A função `tabpag_cria` cria uma tabela vazia.

Quando um processo morre, sua tabela de páginas deve ser destruída (com `tabpag_destroi`), e as páginas de memória secundária e os quadros de memória real que o processo estiver ocupando devem ser liberados.

Uma forma simples de escolher onde colocar o processo novo é carregá-lo completamente em memória secundária quando ele é criado, sem alocar nenhum quadro da memória principal para ele. Dessa forma nenhuma página está mapeada, a tabela de páginas pode ficar vazia. Quando o processo executar, causará faltas de página, e caberá ao algoritmo de substituição de páginas decidir onde colocar as páginas do processo.

Quando o SO despacha um processo para executar, além de atualizar o estado do processador para corresponder ao do processo, deve também configurar a MMU para que utilize a tabela de páginas do processo.
Não é necessário atualizar a tabela de páginas do processo quando inicia o tratamento de uma interrupção, porque na implementação fornecida o MMU usa e altera a própria tabela do processo, e não uma cópia.

### Carga de programas

Como descrito acima, uma forma simples de gerenciar a memória secundária é alocar memória suficiente para todo o processo em memória secundária durante a carga do processo. Mais simples ainda se essa memória toda for mantida durante toda a execução. Essa simplificação tem um custo: o processo ocupa todo o seu espaço na memória secundária e mais os quadros de memória principal que estiver usando. Em geral, os SOs otimizam isso, liberando páginas da memória secundária que forem copiadas para a memória principal, e realocando quando elas voltarem para a memória secundária.

O SO não suporta alocação dinâmica, então os processos não alteram seus tamanhos durante a execução. O gerenciamento da memória secundária com essas restrições não é complicado, mais ainda se alocar a memória secundária de forma contígua.

### Atendimento à interrupção de falta de página

O sistema pode gerar dois tipos de interrupção por problemas no acesso à memória, `ERR_END_INV` e `ERR_PAG_AUSENTE`. As situações em que essas interrupções são geradas:
- quando a MMU traduzir um endereço virtual para um endereço físico mas o endereço não é reconhecido pela memória (ERR_END_INV);
- quando a página não está mapeada na tabela de páginas (ERR_PAG_AUSENTE).

O primeiro caso não deveria acontecer, e indica que o SO programou mal a MMU, apontando uma página para um quadro que está fora dos limites da memória física.

O segundo caso pode representar duas possibilidades:
- pode ser um acesso a um endereço fora do espaço de endereçamento do processo (e o SO deve matar o processo por "*segmentation fault*");
- pode ser uma falta de página: o processo está tentando acessar uma posição de memória válida, mas ela no momento não está na memória principal.

Para identificar qual é o problema, o SO deve verificar se o endereço virtual que causou o problema pertence ou não ao processo.

O endereço virtual que causou a interrupção é colocado no registrador `complemento`, que deve ter sido salvo na tabela de processos.

No caso de falta de página, o SO deve:
- encontrar um quadro livre na memória principal
- se não houver, deve liberar um quadro ocupado
  - escolher uma página que esteja em memória principal (esse é o algoritmo de substituição de página)
  - copiar a página para a memória secundária se for necessário (se ela foi alterada desde que foi copiada para a memória principal)
  - marcar essa página como não mapeada na tabela de páginas que a contém (pode ser de um processo diferente do que causou a falha de página)
  - o quadro onde a página estava agora está livre
- ler a página necessária da memória secundária para o quadro livre
- marcar o quadro como não livre
- alterar a tabela de páginas do processo, para indicar que essa página está nesse quadro
- bloquear o processo pelo tempo necessário para realizar a(s) transferência(s)

### Algoritmo de substituição de página

Implemente os algoritmos **FIFO** e **segunda chance**. Cuidado com a escolha da frequência com que o bit de acesso dos quadros de memória é zerado para o segunda chance, se for muito baixa a frequência, grande parte dos quadros estará marcada como acessada, e se for muito alta, bem poucos estarão, afetando o desempenho do algoritmo.

### Medição do sistema de memória virtual

Faça o SO contar o número de falhas de página atendidas para cada processo.
Defina a memória principal com tamanho suficiente para conter todas as páginas de todos os processos. Executando dessa forma com paginação por demanda, cada processo deve gerar um número de falhas de página igual ao número de páginas que corresponde ao seu tamanho. Talvez um pouco menos, se o processo não acessar toda a sua memória.

Altere o tamanho da memória para ter metade das páginas necessárias para conter todos os processos. Execute nessa configuração e compare o número de falhas de página e os tempos de execução dos programas em relação à configuração anterior.

Continue diminuindo o tamanho da memória pela metade e refazendo a comparação.

Qual o tamanho mínimo de memória que permite a execução dos processos?

Faça o experimento com 2 tamanhos de página, um bem pequeno (algumas palavras) e outro pelo menos 4 vezes maior. Analise as diferenças no comportamento do sistema. Faça um relatório com suas observações e análises.


## Alterações no código em relação ao t1

O código fornecido para o t2 contém algumas alterações em relação ao t1, que
implementam os novos dispositivos de hardware e seu uso;
- implementação da MMU em `mmu.[hc]`, e da tabela de páginas em `tabpag.[hc]`;
- a CPU agora faz todos os acessos à memória usando a MMU (ela é inicializada
  com a MMU em vez da memória);
- a CPU não testa mais contra acessos a endereços inferiores a 100, isso fica com a MMU;
- novo erro `ERR_PAG_AUSENTE`, para indicar falta de página;
- o Makefile foi alterado para montar os arquivos .asm para serem carregados no
  endereço 0 (exceto trata_irq) -- sem memória virtual não é possível executá-los 
  (o endereço físico 0 é usado pelo mecanismo de interrupção, e não é possível 
  colocar mais de um programa no mesmo endereço físico);
- main.c cria a MMU e passa para CPU e SO;
- o SO tem uma tabela de páginas única, isso tem que ser alterado para uma por processo;
- a carga de programa é realizada no próximo endereço livre, e a tabela de
  páginas é alterada para mapear o endereço virtual 0 para o endereço de carga --
  isso permite a execução de um programa, mas como só tem uma tabela de páginas,
  se houver a carga de um outro programa, as informações do primeiro serão
  destruídas na tabela.
- a cópia da memória para o SO (usado para copiar o nome do programa a carregar) agora
  copia da memória do processo, e não mais da memória física (precisa converter endereços
  do processo em endereços de memória, e precisa tratar a possibilidade de um endereço
  estar em memória secundária ou primária.


