;******************************************************************************

	LIST P=18F4550	                            ;directiva para definir el procesador
	#include <P18F4550.INC>	;definiciones de variables especificas del procesador

;******************************************************************************
;Bits de configuraci�n
    ;CONFIG FOSC = INTOSC_XT                   ;Oscilador INT usado por el uC , XT usado por el  USB
    CONFIG BOR  = OFF			 ;BROWNOUT RESET DESHABILITADO
    CONFIG PWRT = ON 			 ;PWR UP Timer habilitado
    CONFIG WDT  = OFF			 ;Temporizador vigia apagado
    CONFIG MCLRE=OFF			 ;Reset apagado
    CONFIG PBADEN=OFF
    CONFIG LVP  = OFF
;******************************************************************************
;Definiciones de variables
		CBLOCK	0x000		   ;ejemplo de definici�n de variables en RAM de acceso
		cont
		ciclo
		index
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
ini		clrf	index,0			;inicia conteo en cero
cods	movf	index,W,0
		call	tabla
		movwf	PORTD,0
		call	repite
		incf	index,F,0
		movlw	0x09
		cpfsgt	index
		bra		cods
		bra		ini
;******************************************************************************
tabla	rlcf	index,W,0		;multiplica �ndice por 2
		addwf   PCL,F,0			;ajusta el PCL de acuerdo al valor del �ndice
		retlw	0xC0			;c�digo del cero
		retlw	0xf9			;c�digo del uno
		retlw	0xA4			;c�digo del dos
		retlw	0xb0			;c�digo del tres
		retlw	0x99			;c�digo del cuatro
		retlw	0x92			;c�digo del cinco
		retlw	0x82			;c�digo del seis
		retlw	0xb8			;c�digo del siete
		retlw	0x80			;c�digo del ocho
		retlw	0x98			;c�digo del nueve
;****************************************************************
retardo movlw	0xff
	    movwf	cont,0
nada	nop
        decfsz	cont,1,0
        bra		nada
		btg		PORTB,2,0	;monitor duracion retardo
	    return
;******************************************************************************
repite	movlw	d'30'		;llama 25 veces a la rutina retardo
	    movwf	ciclo
llama	call	retardo
        decfsz  ciclo,F,0
        bra     llama
		return
	    END

