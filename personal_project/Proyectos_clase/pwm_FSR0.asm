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
;Reset vectorORG 0x0000	
		ORG	0x0000
		bcf	OSCCON,IRCF2,0
		bsf 	OSCCON,IRCF0,0	 ;Oscilador interno a125 kHz
		movlw	0x0F
		movwf	ADCON1,0		 ;Puertos Digitales
		clrf 	PORTD,0
		clrf 	TRISD,0 	               ;Puerto D Configurado como salida
		movlw	0x1C
		movwf	CCP1CON		  ;modo pwm
		movlw   0x07
		movwf   T2CON,0 		;CONFIGURACION PREESCALER timer2 x 16
		movlw   d'194'
		movwf   PR2,0			;PERIODO PWM 100ms
		bcf	TRISC,2,0            	;PIN ccp1 SALIDA
		movlw	0x27
		movwf	CCPR1L,0		;CICLO DE TRABAJO AL 20% (20 ms)
nada    LFSR 	FSR0,100h 		;iniciamos con banco1
NEXT 	addwf 	POSTINC0 		;INDF+w después incrementa FSR0
		incf	WREG,0
		BTFSS   FSR0H,1 		;Terminamos con banco 1?
		BRA 	NEXT
limp	clrf 	POSTDEC0 		; limpia localidad  después incrementa FSR0
		movf	FSR0L,F,0
		btfss	STATUS,Z,0
		bra		limp
aqui	bra		aqui  	
		bra	nada
		END
