
SOMA:       INTUSE

INTDEF  MAIN

LIMITE:     EQU     @10

CTR:        DC      @0


; MACROS
ZERO_A:     MCDEFN
            XOR     A
            MCEND

LOAD:       MCDEFN  &REG &VAL
            LD      &REG, &VAL
            MCEND

INIT:       MCDEFN
LOAD_BC:    MCDEFN
            LD      C, @0
            LD      E, @0
            MCEND
            LOAD_BC
            MCEND

RETRY:      MCDEFN  &DEST
TRY.SER:    JP      NZ, &DEST
            MCEND

; CODIGO
MAIN:       ZERO_A


            LD      B, LIMITE

            INIT

            LOAD      A, (CTR)

            INC     A

            LD      (CTR), A


            CP      B

            RETRY   MAIN
            RETRY   MAIN

            CALL    SOMA

            HALT
            END     MAIN
