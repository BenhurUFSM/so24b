; programa tratador de interrupção
; contém o código a executar (em modo supervisor) quando a CPU aceitar uma interrupção
; deve ser colocado no endereço 10

; chama o SO e verifica o valor retornado: se for 0, retorna da interrupção;
;   se não, suspende a execução da CPU executando a instrução PARA

trata_int
        ; quando atende uma interrupção, a CPU coloca o código (IRQ) em A
        ; chamac chama a função C do SO passando A como argumento
        chamac
        ; o valor de retorno da função chamada é colocado em A
        ; ele representa a vontade do SO de suspender a execução ou retornar
        ;   da interrupção e executar o processo cujo estado da CPU está no
        ;   início da memória
        desvnz suspende
        reti
suspende
        para
