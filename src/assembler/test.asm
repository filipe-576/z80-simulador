; símbolos
DATA_SEG    EQU     &100
CODE_SEG    EQU     &0
LIMIT       EQU     5

; dados
            ORG     DATA_SEG
COUNTER     DS      1
RESULT      DS      1

; codigo
            ORG     CODE_SEG
START       LD      A, 0
            LD      (COUNTER), A

LOOP        LD      A, (COUNTER)
            INC     A
            LD      (COUNTER), A
            CP      LIMIT
            JP      NZ, LOOP

            LD      (RESULT), A
            JP      ENDCODE

MID         LD      A, $FF
ENDCODE     HALT

            END     START