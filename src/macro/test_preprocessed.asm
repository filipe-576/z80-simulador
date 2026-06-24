    ORG $0000 ; Define a origem do programa
MINHA_MACRO MCDEFN &REG
    LD &REG, 10 ; Carrega 10 no registrador
    INC &REG
MCEND
    LD A, 5
    MINHA_MACRO B
    HALT ; Fim do programa
