LIST P=18F4550 ;directiva para efinir el procesador
#include <P18F4550.INC> ;definiciones de variables especificas del procesador
;******************************************************************************
;Bits de configuración
CONFIG FOSC = INTOSC_XT ;Oscilador INT usado por el uC , XT usado por el USB
CONFIG BOR = OFF ;BROWNOUT RESET DESHABILITADO
CONFIG PWRT = ON ;PWR UP Timer habilitado
CONFIG WDT = OFF ;Temporizador vigia apagado
CONFIG MCLRE=OFF ;Reset apagado
CONFIG PBADEN=OFF
CONFIG LVP = OFF
;******************************************************************************
;Definiciones de variables
		CBLOCK 0x000 ;ejemplo de definición de variables en RAM de acceso
		cont
		veces
		rep
		mem
		rest
		ENDC
;******************************************************************************
;Reset vector
		ORG 0x0000
;Inicio del programa principal
		bcf 	OSCCON,IRCF2,0
		bcf 	OSCCON,IRCF1,0
		bcf 	OSCCON,IRCF0,1 ;Oscilador interno a125 kHz{7
		movlw 	0x0F
		movwf 	ADCON1,0 ;Puertos Digitales
		clrf 	PORTD,0
		clrf 	TRISD,0 ;Puerto D Configurado como salida
inicio	clrf	PORTD,0
		bsf 	PORTD,0,0 ;enciende LED conectado en RD0
		movlw	0x01
		movff	WREG,PORTD
		btfss	PORTB,0,0
		bra		inicio
		movlw	0x02
		movwf	rep
corrs	movlw	0x08
		movwf	veces
correi 	rlcf 	PORTD,F,0
		call 	retardo
		btfss	PORTB,0,0
		bra		ascenso
		decfsz	veces,f,0
		bra 	correi
		movlw	0x08
		movwf	veces
corred	rrcf	PORTD,F,0
		call 	retardo
		btfss	PORTB,0,0 
		bra		ascenso
		decfsz	veces,f,0
		bra 	corred
		decfsz	rep,f,0
		bra 	corrs
		movlw	0x02
		movwf	rep
ascenso	movlw	0x08
		movwf	veces
		movff	0x00,PORTD
inc1	movff	PORTD,WREG
		incf	PORTD,F,0
		addwf	PORTD,1,0
		call 	retardo
		btfss	PORTB,0,0 
		bra		tog
		decfsz	veces,f,0
		bra 	inc1
		movlw	0x08
		movwf	veces
		movlw 	0x01
		movwf 	mem
dec1	movff	mem,WREG
		subwf 	PORTD,F,0
		movlw	0x02
		mulwf	mem
		movff	PRODL,mem
		call 	retardo
		btfss	PORTB,0,0 
		bra		tog
		decfsz	veces,f,0
		bra 	dec1
		decfsz	rep,f,0
		bra 	ascenso
		btfss	PORTB,0,0	;prueba si es 1 y salta
		bra		inicio
tog		clrf	PORTD,0		;limpia el registro f
inve	btg		PORTD,0,0	;el primer bit es el #bit 
		call	retardo
		btg		PORTD,7,0	;el primer bit es el #bit 
		call	retardo
		btg		PORTD,1,0	;el primer bit es el #bit 
		call	retardo
		btg		PORTD,6,0	;el primer bit es el #bit 
		call	retardo
		btg		PORTD,2,0	;el primer bit es el #bit 
		call	retardo
		btg		PORTD,5,0	;el primer bit es el #bit 
		call	retardo
		btg		PORTD,3,0	;el primer bit es el #bit 
		call	retardo
		btg		PORTD,4,0	;el primer bit es el #bit 
		call	retardo
		btfsc	PORTB,0,0 
		bra		inve
		bra		inicio


;******************************************************************************
retardo movlw 	0xff
		movwf 	cont,0
nada    nop
		decfsz 	cont,1,0
		bra 	nada
		return
		END