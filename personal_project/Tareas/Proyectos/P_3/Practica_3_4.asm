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
inicio	movlw	0x04
		movwf	TBLPTRL,0
		movlw	0x03
		movwf	TBLPTRH,0
		clrf	TBLPTRU,0		;tblptr=0x000300
cod		btg		PORTA,0,0
		movff	TABLAT,PORTD
		call	espera
		btfss	PORTB,0,0
		tblrd*-	
		btfsc	PORTB,0,0
		tblrd*+	
		movlw	0x13
		cpfsgt	TBLPTRL
		bra		cero
		bra		inicio
cero	btfsc	PORTB,0,0
		bra		cod
		movlw	0x03
		cpfseq	TBLPTRL
		bra		cod	
		movlw	0x13
		movwf	TBLPTRL
		bra		cod
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
;******************************************************************************
		org	0x304			;DB directiva que Define Byte             
        DB	0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x67,0x77,0x7C,0x39,0x5E,0x79,0x71   ;código del F
	    END