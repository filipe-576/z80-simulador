; símbolos
LIMIT       EQU     @5

; dados

COUNTER     DC      @1
RESULT      DS      @1

; codigo

START       LD      A, @0
            LD      (COUNTER), A

LOOP        LD      A, (COUNTER)
            INC     A
            LD      (COUNTER), A
            LD      B, LIMIT
            CP      B
            JP      NZ, LOOP

            LD      (RESULT), A
            JP      ENDCODE

MID         LD      A, $FF
ENDCODE     HALT

            END     START