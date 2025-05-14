;****************************************************************************
	LIST P=18F4550 ;directiva para definir el procesador
#include <P18F4550.INC>               ;definiciones de variables especificas del procesador
;****************************************************************************
;Bits de configuración
		CONFIG FOSC = INTOSC_XT            ;Oscilador INT usado por  uC , XT para USB
		CONFIG BOR = OFF                           ;BROWNOUT RESET DESHABILITADO
		CONFIG PWRT = ON                         ;PWR UP Timer habilitado
		CONFIG WDT = OFF                          ;Temporizador vigia apagado
		CONFIG MCLRE=OFF                         ;Reset apagado
		CONFIG PBADEN=OFF
		CONFIG LVP = OFF	
;****************************************************************************
flags	equ		0
iadc	equ		2
iadch	equ		6
;Reset vectorORG 0x0000	
		ORG	0x0000
		goto 	inicio
		org		0x08
		goto	RSAD
inicio	bcf	OSCCON,IRCF2,0
		bsf 	OSCCON,IRCF0,0	 ;Oscilador interno a125 kHz
		clrf 	PORTB,0
		clrf 	TRISB,0
		movlw	0x01
		movwf	TRISA,0
		movlw	0x0E
		movwf	ADCON1,0		 ;Puertos Digitales, solo usamos AN0
		movlw	0x24
		movwf	ADCON2,0		;8 TAD, reloj del ADC Fosc/4, justificado izq
		movlw	0xc0
		movwf	INTCON,0		;HABILITO interrupcion global y periféricas
		bsf		PIE1,ADIE		;habilito interrupción AD
		bsf		ADCON0,ADON,0	;adc on
stconv	bsf		ADCON0,1,0 		;inicio  conversion AD	
		bra		stconv
;******************************************************************
RSAD	bcf		PIR1,ADIF,0
		movff	ADRESH,PORTB
		btg		PORTC,1,0 
		bsf		flags,0,0		;monitor de conversiones AD
		retfie
;******************************************************************
RST0	bcf		INTCON,TMR0IF,0
		;movlw 	d'194'
		movlw   0x0A
		movwf	TMR0L
		bsf 	flags,0,0 ;monitor de un 1s
		btg		flags,2,0
		retfie
		END
