; símbolos
LIMIT       EQU     @5

; dados

COUNTER     DC      @1
RESULT      DS      @1

RESETREG    MCDEFN
            LD A, @0
            LD B, @0
            LD C, @0
            LD D, @0
            LD E, @0
            LD H, @0
            LD L, @0
            LD IX, @0
            LD IY, @0
            MCEND

; codigo

START       RESETREG
            LD      (COUNTER), A
            LD      B, LIMIT

LOOP        LD      A, (COUNTER)
            INC     A
            LD      (COUNTER), A
            CP      B
            JP      NZ, LOOP

            LD      (RESULT), A
            JP      ENDCODE

MID         LD      A, $FF
ENDCODE     HALT

            END     START