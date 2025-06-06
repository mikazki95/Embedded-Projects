;******************************************************************************

LIST P=18F4550                             ;directiva para definir el procesador
#include <P18F4550.INC> ;definiciones de variables especificas del procesador

;******************************************************************************
;Bits de configuraci�n
    CONFIG FOSC = INTOSC_XT                   ;Oscilador INT usado por el uC , XT usado por el  USB
    CONFIG BOR  = OFF    ;BROWNOUT RESET DESHABILITADO
    CONFIG PWRT = ON     ;PWR UP Timer habilitado
    CONFIG WDT  = OFF    ;Temporizador vigia apagado
    CONFIG MCLRE=OFF    ;Reset apagado
    CONFIG PBADEN=OFF
    CONFIG LVP  = OFF
;******************************************************************************
;Definiciones de variables
	CBLOCK 0x000     ;ejemplo de definici�n de variables en RAM de acceso
		flags
		index
  		ciclo
	ENDC      ;fin del bloque de constantes
;******************************************************************************
;Reset vector 
	ORG 0x0000;Inicio del programa principal
		goto	inicio2 
		org     0x08 
		bra		RST0
inicio2	bcf 	OSCCON,IRCF2,0
		bsf 	OSCCON,IRCF1,0
		bcf 	OSCCON,IRCF0,0  ;Oscilador interno a125 kHz
		movlw   0x0F
		movwf   ADCON1,0            ;Puertos Digitales
		clrf 	PORTD,0
		clrf 	TRISD,0                  ;Puerto D Configurado como salida
		clrf 	PORTA,0
		clrf 	TRISA,0
		movlw   0xC7
		movwf 	T0CON,0
		movlw	0xA0
		movwf	INTCON,0
		;movlw	0xc2
		movlw   0x0B
		movwf 	TMR0L
inicio	clrf	index,0
cod		movf	index,W,0
		call 	tabla
		movwf	PORTD,0
		btg		PORTA,0,0
		call	espera
		btfss	PORTB,0,0
		decf	index,1
		btfsc	PORTB,0,0
		incf	index,1
		movlw	0x0f
		cpfsgt	index
		bra		cero
		bra		inicio
cero	btfsc	PORTB,0,0
		bra		cod
		movlw	0x00
		cpfseq	index
		bra		cod	
		movlw	0x3F
		movwf	PORTD,0
		call	espera	
		movlw	0x0f
		movwf	index,0
		bra		cod





;******************************************************************************
tabla   rlcf 	index,W,0  ;multiplica �ndice por 2
		addwf   PCL,F,0   ;ajusta el PCL de acuerdo al valor del �ndice
		retlw 	0x3F   ;c�digo del cero
		retlw 	0x06   ;c�digo del uno
		retlw 	0x5B   ;c�digo del dos 
		retlw 	0x4F   ;c�digo del tres
		retlw 	0x66   ;c�digo del cuatro
		retlw 	0x6D   ;c�digo del cinco
		retlw 	0x7D   ;c�digo del seis
		retlw 	0x07   ;c�digo del siete
		retlw 	0x7F   ;c�digo del ocho
		retlw 	0x67   ;c�digo del nueve
		retlw 	0x77   ;c�digo del A
		retlw 	0x7C   ;c�digo del B
		retlw 	0x39   ;c�digo del C
		retlw 	0x5E   ;c�digo del D
		retlw 	0x79   ;c�digo del E
		retlw 	0x71   ;c�digo del F

;*******************************************************
espera 	btfss	flags,0,0
		bra		espera
		bcf		flags,0,0
	    return
;****************************************************************
RST0	bcf		INTCON,TMR0IF,0
		;movlw 	d'194'
		movlw   0x0B
		movwf	TMR0L
		bsf 	flags,0,0 ;monitor de un 1s
		retfie
	    END