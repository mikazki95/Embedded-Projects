module COUNTER
symbol count256
family spartan
symboltemplate counter1
attributes
   BUS_WIDTH = 32
   STYLE = MAX_SPEED
   OPTYPE = UP
   ENCODING = BINARY
   COUNT_TO = 256
pins
   D_IN(31:0)
   LOAD
   CLOCK
   TERM_CNT
