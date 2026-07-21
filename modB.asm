INTDEF  SOMA

DELTA:      EQU     @3

RES:        DS      @1

SOMA:       LD      B, DELTA
            ADD     B
            LD      (RES), A
            RET

            END
