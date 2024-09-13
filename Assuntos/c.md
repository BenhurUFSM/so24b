## C

Os trabalhos da disciplina são desenvolvidos na linguagem C.
Alguns aspectos da linguagem e da forma como ela é usada no trabalho estão abaixo.
Caso tenha algum tópico que gostaria que fosse colocado aqui, me fala.
Os tópicos abaixo não estão organizados em nenhuma ordem muito lógica.

### Organização dos módulos no t0 (e nos demais trabalhos)

O programa é dividido em módulos, com a tentativa de se ter uma boa separação e uma interface pequena e clara entre os módulos.
Cada módulo (mas nem todos...) está organizado como um tipo abstrato de dados (TAD), definindo um tipo de dados e as operações realizáveis em dados desse tipo.
Para isso, cada módulo é implementado em 2 arquivos:
- um `.h` com a interface que esse módulo apresenta aos demais. Se tentou que essa interface seja a menor possível, declarando o tipo como um registro, sem informar os campos desse registro. Isso impede que outros módulos acessem esses campos diretamente. Toda a comunicação com um dado desse tipo deve ser realizada usando funções. A declaração dessas funções está também nesse `.h`.
- um `.c` com a declaração completa do registro, a implementação das funções da interface declaradas no `.h`, e o que mais for necessário para permitir essa implementação. Esses "a mais" devem ser internos ao módulo e não usados fora dele.

Cada módulo tem um nome, o nome do tipo de dados e um prefixo para as funções que implementam as operações que manipulam o dado. Em geral, esses 3 nomes são o mesmo, ou parecidos. Por exemplo, no arquivo `relogio.h` é declarado o tipo `relogio_t`, e todas as funções declaradas ali são prefixadas com `relogio_`.

A linguagem permite declarar um ponteiro para um registro não completamente definido, mas não permite declarar uma variável como sendo um tal registro. Por isso, a interface com as funções é feita toda através de ponteiros.
Todas as funções que implementam as operações sobre um TAD recebem como primeiro argumento um ponteiro para o dado (do tipo do TAD) que será operado. A excessão é a função de alocação e inicialização de um dado desse tipo, que retorna um tal ponteiro.

Todos os TAD têm uma função `_cria` para alocação e inicialização de um dado, e `_destroi` para liberação da memória alocada.

A declaração do tipo no `.h` é sempre da forma:
```c
  typedef struct nome_t nome_t;
```
que declara o tipo `nome_t` como sendo um sinônimo para `struct nome_t`.
O `.c` contém a declaração de `struct nome_t`.

Funções auxiliares são declaradas e implementadas somente no `.c`, de preferência declaradas como `static`, para que seus nomes não sejam exportados pelo compilador. Tipos de dados auxiliares também são restritos ao `.c` onde são necessários.

### Ponteiros para funções

O controlador de E/S é um módulo que centraliza as operações de E/S.
O objetivo é simplificar o acesso aos diferentes dispositivos, uniformizando a forma como esses dispositivos são acessados e permitindo a inclusão de novos dispositivos sem a necessidade de se alterar todo código que pode acessar algum dispositivo para que tenha acesso ao novo.

Por exemplo, a CPU tem somente uma instrução para leitura em dispositivos de E/S, com um argumento que identifica o dispositivo. Para a implementação dessa instrução, é necessário efetuar a leitura em um ou outro dispositivo, dependendo dessa identificação. No t0, tem 2 tipos de dispositivo, terminal e relógio, cada um com uma função diferente para realizar a operação de leitura (`terminal_leitura` ou `relogio_leitura`). Uma forma de se implementar a intrução de leitura seria verificar a identificação do dispositivo e realizar um teste, chamando uma ou outra função para realizar a operação. Cada vez que se cria um dispositivo novo, teria que alterar essa função para incluir uma chamada a nova função que permite o acesso ao novo dispositivo (assim como todos os demais locais do código onde se tem acesso a algum dispositivo).

Em vez disso, se fez o controlador de E/S: todo acesso a algum dispositivo deve ser direcionado a ele, e é ele que mapeia a identificação do dispositivo para as funções que realizam as operações de E/S em cada dispositivo.
Para dar mais liberdade de implementação, o controlador não tem conhecimento prévio de quais tipos de dispositivos existem, os dispositivos devem ser registrados. Para que isso seja possível, deve-se definir uma interface única de acesso a um dispositivo, que deve ser implementada por um controlador de dispositivo para que possa ser registrado e controlado pela controladora de E/S.

No computador simulado, só existem duas operações de E/S, leitura de um dado ou escrita de um dado, que podem ser implementadas como uma função para cada. A interface dessas funções foi definida como um primeiro argumento recebendo um ponteiro para a controladora do dispositivo (para manter a nossa regra dos TAD, em que todas as funções recebem um ponteiro para o dado como primeiro argumento), a identificação do dispositivo da controladora como segundo argumento (para permitir que uma controladora controle mais de um dispositivo - temos 4 terminais no t0, por exemplo), e retorne um código de erro (para que se tenha uma forma simples de comunicar erros de E/S). Além disso, a operação de escrita recebe o dado a escrever (um int), e a operação de leitura recebe um ponteiro para onde colocar o dado lido (um ponteiro para int).

Para não precisar ter na controladora de E/S uma coleção de `if` para chamar essas funções de uma ou outra controladora, dependendo da identificação do dispositivo que se está acessando, no registro de uma controladora de dispositivo se registra, além do ponteiro para a controladora e da identificação do dispositivo, as funções que devem ser chamadas quando houver uma operação de leitura ou de escrita nesse dispositivo.

A forma de se fazer isso em C é com o uso de ponteiros para funções.
Se uma função tem um ponteiro para uma outra função, pode realizar a chamada a essa função, e a função que será efetivamente chamada dependerá do valor desse ponteiro (que é um dado). Portanto, é possível ter código que, mesmo sem ser alterado, pode chamar ora uma ora outra função, dependendo do valor de uma variável, que é um ponteiro para uma função.

Em C, o nome de uma função sem os parênteses representa um ponteiro para essa função, que pode ser colocada em uma variável do tipo ponteiro.
Se uma variável contém um ponteiro para uma função, é possível chamar essa função colocando parênteses depois do nome dessa variável, como se ela fosse um nome de função.
Por exemplo, se a variável `f` no código abaixo for um ponteiro para função, o código chamará primeiro a função `quadrado`, e depois a função `raiz`.
```c
  f = quadrado;
  y = f(x);    // equivale a y = quadrado(x);
  f = raiz;
  y = f(x);    // equivale a y = raiz(x);
```

Assim como ponteiros para dados são tipados (um ponteiro para int não pode apontar para um float), ponteiros para funções também são. O tipo de uma função é um pouco mais complexo, porque envolve o tipo de retorno da função e o número e tipo de cada argumento. Um "ponteiro para função que recebe um float e retorna um int" não é compatível com "ponteiro para função que recebe dois float e retorna void".

A sintaxe para a declaração de um ponteiro para função em C não é especialmente óbvia... No trecho abaixo, `f` e `g` são declarados como ponteiros para funções, dos tipos comentados no parágrafo acima.
```c
  int (*f)(float);
  void (*g)(float, float);
```
É comum de declarar um tipo de ponteiro para função com typedef, e depois se declarar variáveis desse tipo. O código abaixo declara `f` com o mesmo tipo acima:
```
  typedef int (*ponteiro_para_funcao)(float);
  ponteiro_para_funcao f;
```

No caso do t0, os tipos de ponteiros para funções que devem ser implementadas por uma controladora de dispositivo para que ela possa ser usada na controladora de E/S está em `es.h`.
O registro de controladoras de dispositivo junto à controladora de E/S está em `main.c`.
