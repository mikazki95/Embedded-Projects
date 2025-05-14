module COUNTER
symbol count8
family spartan
symboltemplate counter0
attributes
   BUS_WIDTH = 4
   STYLE = MAX_SPEED
   OPTYPE = UP
   ENCODING = BINARY
   COUNT_TO = 7
pins
   D_IN(3:0)
   LOAD
   CLOCK
   Q_OUT(3:0)
   TERM_CNT
