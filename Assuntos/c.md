## C

Os trabalhos da disciplina são desenvolvidos na linguagem C.
Alguns aspectos da linguagem e da forma como ela é usada no trabalho estão abaixo.
Caso tenha algum tópico que gostaria que fosse colocado aqui, me fala.
Os tópicos abaixo não estão organizados em nenhuma ordem muito lógica.

### Organização dos módulos no t0 (e nos demais trabalhos)

O programa é separado em módulos, com a tentativa de se ter uma boa separação e uma interface pequena e clara entre os módulos.
A maior parte dos módulos está organizado como um tipo abstrato de dados, definindo um tipo de dados e as operações realizáveis em dados desse tipo.
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
