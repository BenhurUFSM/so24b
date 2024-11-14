# so24b

Página da disciplina de Sistemas Operacionais, segundo semestre de 2024

ELC1080, DLSC/CT/UFSM
turmas CC e SI

professor: Benhur Stein ([benhur+so24b@inf.ufsm.br](mailto:benhur%2bso24b@inf.ufsm.br))\
terças e quintas, 10h30, sala 1309

## Desenvolvimento da disciplina

Os canais de comunicação entre o professor e os alunos são 
- os encontros em sala de aula,
- esta página (<https://github.com/BenhurUFSM/so24b>),
- e-mail para [benhur+so24b@inf.ufsm.br](mailto:benhur%2bso24b@inf.ufsm.br),
- encontros agendados com o professor (por e-mail).

O e-mail pode ser usado para o envio de perguntas, exercícios, reclamações, sugestões, críticas e o que mais for, durante todo o período em que durar a disciplina.

Esta página concentrará os assuntos vistos em aula, a descrição de exercícios e trabalhos, avaliações, etc.
Ela será aumentada/atualizada durante todo o semestre.
Ela é o canal mais oficial de informações sobre a disciplina, e deve ser consultada frequentemente pelo aluno (sugiro habilitar notificações de alteração do github).

A disciplina vai seguir de forma bastante próxima o assunto como ele é visto no livro
```
   Sistemas Operacionais Modernos
   Andrew S Tanenbaum
```

### Ementa

Ver [aqui](https://www.ufsm.br/ementario/disciplinas/ELC1080/).


## Aulas 

|    N |   data | assunto
| ---: | :----: | :--------
|    1 | 10set  | introdução
|    2 | 12set  | apresentação do código do t0 (em [C](Assuntos/c.md))
|    3 | 17set  | origens de SO -- programa monitor, processamento em lote, multiprogramação
|    4 | 19set  | processos -- modo supervisor, estado do processador, interrupção, estados de um processo, criação e morte
|    5 | 24set  | processos -- tabela de processos, funcionamento básico de um SO
|    6 | 26set  | apresentação do t1
|      |  1out  | [vade retro vīrus](https://www.ufsm.br/2024/09/30/nota-atividades-administrativas-e-academicas-presenciais-suspensas-ate-01-de-outubro)
|    7 |  3out  | mais detalhes do t1; escalonamento
|    8 |  8out  | algoritmos de escalonamento
|    9 | 10out  | *threads*
|   10 | 15out  | [programa para teste de threads](Testes/thr.c), comunicação entre processos
|   11 | 17out  | comunicação entre processos (TSL, semáforo, mutex, monitor, variável de condição; produtor-consumidor)
|   12 | 22out  | comunicação entre processos (barreira; jantar dos filósofos, leitores+escritores)
|   13 | 24out  | gerenciamento de memória
|   14 | 29out  | gerenciamento de memória (memória virtual, segmentação, paginação)
|   15 | 31out  | gerenciamento de memória (TLB, tabela de páginas invertida)
|   16 |  5nov  | gerenciamento de memória
|   17 |  7nov  | apresentação do t2
|   18 | 12nov  | 
|   19 | 14nov  | 
|   20 | 19nov  | 
|   21 | 21nov  | 
|   22 | 26nov  | JAI
|   23 | 28nov  | JAI
|   24 |  3dez  | 
|   25 |  5dez  | 
|   26 | 10dez  | 
|   27 | 12dez  | 
|      | 17dez  | sexta-feira
|      | 19dez  | sábado

<!---->
<!--|    2 | 10ago  | introdução (até seção 1.2 do livro)-->
<!--|    3 | 15ago  | introdução (até seção 1.3 do livro)-->
<!--|    4 | 17ago  | código do t0-->
<!--|    5 | 22ago  | introdução-->
<!--|    6 | 24ago  | introdução-->
<!--|    7 | 29ago  | processos-->
<!--|    8 |  31ago | código do t1-->
<!--|    9 |   5set | threads-->
<!--|   10 |  12set | escalonamento (processos em lote e interativos)-->
<!--|   11 |  14set | escalonamento (processos de tempo real)-->
<!--|   12 |  19set | comunicação entre processos-->
<!--|   13 |  21set | comunicação entre processos-->
<!--|   14 |  26set | comunicação entre processos-->
<!--|   15 |  28set | gerência de memória-->
<!--|   16 |   3out | gerência de memória-->
<!--|   17 |   5out | gerência de memória-->
<!--|   18 |  10out | gerência de memória ([algoritmo wsclock](Assuntos/wsclock.md))-->
<!--|   19 |  17out | gerência de memória-->
<!--|   20 |  19out | sistemas de arquivos-->
<!--|   21 |  31out | sistemas de arquivos-->
<!--|   22 |   7nov | sistemas de arquivos-->
<!--|   23 |   9nov | entrada e saída-->
<!--|   24 |  14nov | entrada e saída-->
<!--|   25 |  16nov | entrada e saída-->
<!--|   26 |  21nov | entrada e saída-->
<!--|   27 |  24nov | deadlocks-->
<!--|   28 |  27nov | deadlocks **Atenção, esta segunda tem aulas de quinta**-->
<!--|   -- |  28nov | **Atenção, esta terça tem aulas de sexta**-->
<!--|   29 |  30nov | -->
<!--|   -- |   5dez | **Atenção, esta terça tem aulas de sexta**-->
<!--|   30 |   7dez | apresentações/conversas-->
<!--|   -- |  12dez | apresentações/conversas **Atenção, esta terça tem aulas de sábado**-->
<!--|      |  14dez | -->
<!--|      |  19dez | exame-->

## Trabalhos, provas, exercícios

|    id | prazo | assunto
| ----: | :---- | :-----------
| t0    | 24set | [simulador de computador](/Trabalhos/t0) \| entrega = dispositivo rand, programa de adivinhar; forma de entrega: mail com arquivos ou link para git (faz um branch)
| t1    | 18nov | [implementação de processos](/Trabalhos/t1)
| t2    |       | [gerenciamento de memória](/Trabalhos/t2)


