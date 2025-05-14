;****************************************************************************
	LIST P=18F4550 ;directiva para definir el procesador
#include <P18F4550.INC>               ;definiciones de variables especificas del procesador
;****************************************************************************
;Bits de configuración
		;CONFIG FOSC = INTOSC_XT            ;Oscilador INT usado por  uC , XT para USB
		CONFIG BOR = OFF                           ;BROWNOUT RESET DESHABILITADO
		CONFIG PWRT = ON                         ;PWR UP Timer habilitado
		CONFIG WDT = OFF                          ;Temporizador vigia apagado
		CONFIG MCLRE=OFF                         ;Reset apagado
		CONFIG PBADEN=OFF
		CONFIG LVP = OFF	
;****************************************************************************
flags	equ		0
iadc	equ		2
d		equ		3
u		equ		4
c		equ		5
iadch	equ		6
;Reset vectorORG 0x0000	
		ORG	0x0000
		goto 	inicio
		org		0x08
		goto	RSAD
inicio	bcf	OSCCON,IRCF2,0
		bsf 	OSCCON,IRCF0,0	 ;Oscilador interno a125 kHz
		movlw	0x0E
		movwf	ADCON1,0		 ;Puertos Digitales, solo usamos AN0
		movlw	0xA4
		movwf	ADCON2,0		;8 TAD, reloj del ADC Fosc/4, justificado izq
		movlw	0xc0
		movwf	INTCON,0		;HABILITO interrupcion global y periféricas
		bsf		PIE1,ADIE		;habilito interrupción AD
		clrf 	PORTD,0
		clrf 	TRISD,0 	    ;Puerto D Configurado como salida
		clrf 	PORTB,0
		clrf 	TRISB,0
		clrf 	PORTE,0
		clrf 	TRISE,0
		clrf 	PORTC,0
		clrf 	TRISC,0
		;movlw	0x1C
		;movwf	CCP1CON		  	;modo pwm
		;movlw   0x07
		;movwf   T2CON,0 		;CONFIGURACION PREESCALER timer2 x 16
		;movlw   d'194'
		;movwf   PR2,0			;PERIODO PWM 100ms
		;bcf	TRISC,2,0            	;PIN ccp1 SALIDA
		;movlw	0x27
		;movwf	CCPR1L,0		;CICLO DE TRABAJO AL 20% (20 ms)
		bsf		ADCON0,ADON,0	;adc on
stconv	bsf		ADCON0,1,0 		;inicio  conversion AD	
		clrf	c
		clrf	d
		clrf	u
nada	btfss	flags,0,0
		bra		nada
		bcf		flags,0,0
		movff	ADRESL,iadc
cen		movlw	d'099'
		cpfsgt	iadc
		bra		dec
		incf	c,1
		movlw	d'156'
		addwf	iadc,1
		bra		cen
dec		movlw	0x0A
		cpfslt	d
		bra		sdec
		movlw	d'009'
		cpfsgt	iadc
		bra		uni
		incf	d,1
		movlw	d'246'
		addwf	iadc,1
		bra		dec
uni		movlw	0x0A
		cpfslt	u
		bra		suni
		movlw	d'000'
		cpfsgt	iadc
		bra		sum	
		incf	u,1
		decf	iadc,1
		bra		uni 
sum		movlw	0x00
		cpfsgt	iadch
		bra		wconv
		movlw	0x02
		addwf	c,1
		movlw	0x05
		addwf	d,1
		movlw	0x04
		addwf	u,1
		decf	iadch
		bra		uni
suni	incf	d,1
		clrf	u,0
		bra		dec
sdec	incf	c,1
		clrf	d,0
		bra		sum
wconv	movff	u,PORTD
		movff	d,PORTB
		movff	c,PORTE
		bra		stconv
;******************************************************************
RSAD	bcf		PIR1,ADIF,0
		movff	ADRESL,PORTC
		movff	ADRESL,iadc
		movff	ADRESH,iadch
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
