LIMIT:  EQU     @5

CTR:    DC      @0
INTDEF  COUNT
EXTERNO INTUSE

ZERO_A: MCDEFN
        XOR     A; A = 0
        MCEND

LOAD:   MCDEFN  &REG &VAL
        LD      &REG, &VAL
        MCEND

SKIP:   MCDEFN  &DEST
LOOP.SER: JP    NZ, &DEST
        MCEND

INIT:   MCDEFN
        LOAD    B @0
        LOAD    C @0
        MCEND


START:  ZERO_A
        LD      A, @0
        LD      B, LIMIT
        INC     A

        LOAD    A @1

        INIT

COUNT:  LD      A, (CTR)

        CP      B

        SKIP    COUNT
        SKIP    COUNT

        JP      _END

        LD     A, EXTERNO

_END:   HALT
        END     START
