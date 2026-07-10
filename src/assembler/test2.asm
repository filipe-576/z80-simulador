            INTDEF  CONTA ; Global, Relativo
            INTDEF  TOTAL

LIMITE      EQU     @100 ; Local, Absoluto

TOTAL       DS      @1
DADO        DC      @7

EXTERNO     INTUSE  ; Externo, Relativo

CONTA       LD      A,(DADO)
            LD      B,LIMITE
            ADD     A,B
            LD      (TOTAL),A
            CALL    EXTERNO
            LD      A,(EXTERNO)
            RET
            END     CONTA