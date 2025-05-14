LIST P=18F4550                          ;directiva para efinir el procesador
 #include <P18F4550.INC> ;definiciones de variables especificas del procesador

;******************************************************************************
;Bits de configuración
    CONFIG FOSC = INTOSC_XT                 ;Oscilador INT usado por el uC , XT usado por el  USB
    CONFIG BOR  = OFF  						;BROWNOUT RESET DESHABILITADO
    CONFIG PWRT = ON   						;PWR UP Timer habilitado
    CONFIG WDT  = OFF   					;Temporizador vigia apagado
    CONFIG MCLRE=OFF   						;Reset apagado
    CONFIG PBADEN=OFF
    CONFIG LVP  = OFF
;******************************************************************************
;Definiciones de variables
  CBLOCK 0x000  							;ejemplo de definición de variables en RAM de acceso
  cont 
  ENDC
;******************************************************************************
;Reset vector
  ORG 0x0000
;Inicio del programa principal
  		bcf		OSCCON,IRCF2,0
  		bsf 	OSCCON,IRCF0,0   ;Oscilador interno a125 kHz
	    movlw   0x0F
    	movwf   ADCON1,0         ;Puertos Digitales
	  	clrf 	PORTD,0
  		clrf 	TRISD,0          ;Puerto D Configurado como salida
	  	bsf 	PORTD,0,0        ;enciende LED conectado en  RD0 
correi  rlcf 	PORTD,F,0
	  	call 	retardo
 	 	bra 	correi
;******************************************************************************
retardo movlw 	0x02
        movwf 	cont,0
nada    nop
        decfsz 	cont,1,0
        bra 	nada
        return
END 