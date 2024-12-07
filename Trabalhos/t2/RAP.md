## RAP - Respostas a perguntas

Neste arquivo são colocadas respostar a perguntas recebidas por email que forem relacionadas ao t2 e consideradas suficientemente não pessoais.

- Fiquei com dúvidas sobre como trabalhar com a função so_carrega_programa_na_memoria_virtual() e como usar a mmu nela.

   Minha sugestão é ter a memória secundária grande o suficiente para conter todos os programas. Dessa forma, quando um programa é carregado, tem lugar pra ele na memória secundária, e a posição de uma página de um processo na memória secundária nunca precisa mudar depois da carga.

   Fazendo assim, a carga de um programa é feita sempre na memória secundária, sem preocupação com a memória primária. As páginas do processo vão ser copiadas para a memória principal sob demanda, quando acontecerem as falhas de página durante a execução.

- Na hora de carregar um programa para a memória, como é que se deve invalidar as páginas? 

   É só criar uma tabela de páginas vazia para o processo. As páginas do processo são carregadas todas para a memória secundária, e a tabela tá vazia.
   Quanto o processo for executar, a primeira instrução vai causar uma falha de página, e a primeira página dele vai ter que ser trazida da secundária pra primária, pelo mecanismo normal de tratamento de falhas de página (que deve ser implementado, é a parte principal do t2).

- Quando a função tabpag_invalida_pagina é aplicada na última página, ela apaga toda a tabpag. É para invalidar todas as páginas exceto a última? Na descrição do trabalho, o senhor diz que a tabela de páginas pode ficar vazia. Mas nesse caso, como o processo saberá onde está a memória dele?

   A tabela de páginas contém o número do quadro onde está cada página do processo. Para as páginas que não estão na memória principal, a única informação que precisa é saber que não estão mapeadas (marcadas como inválidas na tabela). Na forma como está implementada, se a página não está na tabela, é como se tivesse, mas marcada como inválida. Então, se todas as páginas estão inválidas (o processo não tem nenhuma página na memória principal), a tabela pode estar vazia.

   O processo não sabe onde estão suas páginas. É por isso que se chama "virtual", o processo funciona com menos memória sem precisar ser alterado.

- Para fazer o carregamento de um novo programa para a memória, é necessário alterar a tabpag que a mmu usa com a função mmu_define_tabpag. Ao final da função de carregamento, é necessário restaurar a tabpag anterior (a do processo que chamou SO_CRIA_PROC)?

   A MMU vai fazer traduções de endereço usando a tabpag que for definida para ela. Quando a CPU vai acessar a memória, pede esse acesso à MMU. Então, quando a CPU vai executar uma instrução de um processo, a MMU deve estar com a tabela que contém as informações de tradução para esse processo. Quando o SO despacha um processo, deve instruir a MMU a usar a tabpag desse processo. Esse é o uso normal da MMU, e o único local em que realmente é necessário definir a tabpag da MMU.

   Se quiser usar a MMU para ajudar a traduzir endereços dentro do SO, deve programar a MMU para usar a tabpag do processo cujos endereços se quer traduzir. Mas não é complicado de fazer essa tradução dentro do SO sem usar a MMU (ainda mais se isolar em uma função).

- Outra dúvida, em cima da declação do quadro_livre, o senhor disse que o controle de memória livre e ocupada deve ser mais completo. O senhor pode dar um norte sobre como fazer isso?

   O controle da memória principal precisa ser mais completo. No código fornecido, a memória vai sendo ocupada de forma contígua, sem nunca ser liberada. Cada novo programa é colocado na próxima posição livre, como se a memória fosse infinita.

   O controle vai ser realizado pelo mecanismo de substituição de páginas. Quando se necessita de um novo quadro para colocar uma página que falta, tem que saber quais quadros estão livres. Quando não se tem um quadro livre e um deve ser escolhido para ser liberado, tem que saber que páginas de que processo estão em quais quadros, para saber se a página tem que ser copiada para memória secundária (e para onde nessa memória) e qual tabpag vai ter que ser alterada para informar que essa página não é mais acessível.

   O controle da memória secundária, por sua vez, pode ser feito como se fosse infinita: cada programa é colocado na próxima posição livre (tomando o cuidado de iniciar em um múltiplo de tamanho de página para ficar mais fácil de achar), e quando encher não dá mais para criar processo, a memória nunca é liberada.

- Devemos criar uma tabela que mapeia as páginas do processo que não estão na memória principal para a memória secundária?

   Se o programa é carregado de forma contígua na memória secundária, só precisa guardar onde está a primeira página dele na memória secundária e quantas páginas são. Com isso, consegue saber onde está cada página na memória secundária.

