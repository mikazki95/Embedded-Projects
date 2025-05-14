;******************************************************************************

LIST P=18F4550                             ;directiva para definir el procesador
#include <P18F4550.INC> ;definiciones de variables especificas del procesador

;******************************************************************************
;Bits de configuración
    CONFIG FOSC = INTOSC_XT                   ;Oscilador INT usado por el uC , XT usado por el  USB
    CONFIG BOR  = OFF    ;BROWNOUT RESET DESHABILITADO
    CONFIG PWRT = ON     ;PWR UP Timer habilitado
    CONFIG WDT  = OFF    ;Temporizador vigia apagado
    CONFIG MCLRE=OFF    ;Reset apagado
    CONFIG PBADEN=OFF
    CONFIG LVP  = OFF
;******************************************************************************
;Definiciones de variables
	CBLOCK 0x000     ;ejemplo de definición de variables en RAM de acceso
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
tabla   rlcf 	index,W,0  ;multiplica índice por 2
		addwf   PCL,F,0   ;ajusta el PCL de acuerdo al valor del índice
		retlw 	0x3F   ;código del cero
		retlw 	0x06   ;código del uno
		retlw 	0x5B   ;código del dos 
		retlw 	0x4F   ;código del tres
		retlw 	0x66   ;código del cuatro
		retlw 	0x6D   ;código del cinco
		retlw 	0x7D   ;código del seis
		retlw 	0x07   ;código del siete
		retlw 	0x7F   ;código del ocho
		retlw 	0x67   ;código del nueve
		retlw 	0x77   ;código del A
		retlw 	0x7C   ;código del B
		retlw 	0x39   ;código del C
		retlw 	0x5E   ;código del D
		retlw 	0x79   ;código del E
		retlw 	0x71   ;código del F

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