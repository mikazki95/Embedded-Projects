;******************************************************************************

	LIST P=18F4550	                            ;directiva para definir el procesador
	#include <P18F4550.INC>	;definiciones de variables especificas del procesador

;******************************************************************************
;Bits de configuración
    CONFIG FOSC = INTOSC_XT                   ;Oscilador INT usado por el uC , XT usado por el  USB
    CONFIG BOR  = OFF			 ;BROWNOUT RESET DESHABILITADO
    CONFIG PWRT = ON 			 ;PWR UP Timer habilitado
    CONFIG WDT  = OFF			 ;Temporizador vigia apagado
    CONFIG MCLRE=OFF			 ;Reset apagado
    CONFIG PBADEN=OFF
    CONFIG LVP  = OFF
;******************************************************************************
;Definiciones de variables
		CBLOCK	0x000		   ;ejemplo de definición de variables en RAM de acceso
		cont
		ciclo
		ENDC			   ;fin del bloque de constantes
;******************************************************************************
;Reset vector
		ORG	0x0000
;Inicio del programa principal
        bcf 	OSCCON,IRCF2,0
	    bsf		OSCCON,IRCF0,0  ;Oscilador interno a125 kHz
        movlw   0x0F
        movwf   ADCON1,0            ;Puertos Digitales
		clrf	PORTD,0
		clrf	TRISD,0	                 ;Puerto D Configurado como salida
		bcf		TRISB,2,0		;RB2 salida
cero	movlw	0xC0		   ;código del cero
		movwf	PORTD,0
		call	repite
		btfss	PORTB,3,0
		bra		nueve
uno		movlw	0xF9		   ;código del uno
		movwf	PORTD,0
		call	repite	
		btfss	PORTB,3,0
		bra		cero
dos		movlw	0xA4		   ;código del dos
		movwf	PORTD,0
		call	repite	
		btfss	PORTB,3,0
		bra		uno	
tres	movlw	0xB0		 ;código del tres
		movwf	PORTD,0
		call	repite
		btfss	PORTB,3,0
		bra		dos
cuatro  movlw	0x99		;código del cuatro
		movwf	PORTD,0
		call	repite
        btfss	PORTB,3,0
		bra		tres
cinco	movlw	0x92		;código del cinco
		movwf	PORTD,0
		call	repite
		btfss	PORTB,3,0
		bra		cuatro
seis	movlw	0x82		;código del seis
		movwf	PORTD,0
		call	repite
		btfss	PORTB,3,0
		bra		cinco
siete	movlw	0xB8		;código del siete
		movwf	PORTD,0
        call    repite
        btfss	PORTB,3,0
		bra		seis
ocho 	movlw	0x80		;código del ocho
		movwf	PORTD,0
		call	repite
		btfss	PORTB,3,0
		bra		siete
nueve	movlw	0x98		;código del nueve
		movwf	PORTD,0
		call	repite
		btfss	PORTB,3,0
		bra		ocho
		bra		cero
;******************************************************************************
retardo movlw	0xff
	    movwf	cont,0
nada	nop
        decfsz	cont,1,0
        bra		nada
		btg		PORTB,2,0	;monitor duracion retardo
	    return
;******************************************************************************
repite	movlw	d'25'		;llama 25 veces a la rutina retardo
	    movwf	ciclo
llama	call	retardo
        decfsz  ciclo,F,0
        bra     llama
		return
	    END

