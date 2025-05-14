		LIST P=18F4550                             ;directiva para definir el procesador
#include <P18F4550.INC> ;definiciones de variables especificas del procesador

;******************************************************************************
;Bits de configuración
    ;CONFIG FOSC = INTOSC_XT                   ;Oscilador INT usado por el uC , XT usado por el  USB
    CONFIG BOR  = OFF    ;BROWNOUT RESET DESHABILITADO
    CONFIG PWRT = ON     ;PWR UP Timer habilitado
    CONFIG WDT  = OFF    ;Temporizador vigia apagado
    CONFIG MCLRE=OFF    ;Reset apagado
    CONFIG PBADEN=OFF
    CONFIG LVP  = OFF
;******************************************************************************
;Definiciones de variables
	CBLOCK 0x000     ;ejemplo de definición de variables en RAM de acceso
		cletra0
		cletra1
		cletra2
		cletra3
		cletra4
		cletra5
		cletra6
		cletra7
		stop
	ENDC  
;********************************************
		ORG 0x0000;Inicio del programa principal
		goto	inicio2  
		org     0x08 		;vector de interrupcion alta prioridad
		bra 	RSRB
		org		0x18		;vector de interrupcion baja prioridad
		bra		RST0 	 
inicio2	bcf 	OSCCON,IRCF2,0
		bsf 	OSCCON,IRCF1,0
		bcf 	OSCCON,IRCF0,0  
		clrf 	TRISD,0             ;Puerto D Configurado como salida
		movlw	0x80
		movwf 	TRISA,0
		movlw	0xf8
		movwf 	TRISE,0
		clrf 	PORTA,0
		clrf 	PORTD,0
		clrf 	PORTE,0
		movlw	0x01
		movwf 	TRISC,0  
		movlw	0xF0
		movwf	TRISB,0
		clrf 	PORTB,0
		movlw   0x0F
		movwf   ADCON1,0  
		bsf		PIE1,ADIE		;habilito interrupción AD
		movlw	0xE8
		movwf	INTCON,0 			;activamos interrupciones, tmr0, prioridades y RB
		bsf		RCON,IPEN,0			;ACTIVA PRIORIDADES DE INTERRUPCION
		movlw   0xC7
		movwf 	T0CON,0
		movlw	0x81
		movwf	INTCON2,0
		movlw	0x00
		movwf	cletra0,0
		movlw	0x00
		movwf	cletra1,0
		movlw	0xEE
		movwf	cletra2,0
		movlw	0xAA
		movwf	cletra3,0
		movlw	0xEE
		movwf	cletra4,0
		movlw	0x8C
		movwf	cletra5,0
		movlw	0x8A
		movwf	cletra6,0
		movlw	0x00
		movwf	cletra7,0
vis		setf	PORTC,0
		movlw	0xFE
		movwf	PORTA,0
		movff	cletra0,PORTD	
		movlw	0xFD
		movwf	PORTA,0
		movff	cletra1,PORTD
		movlw	0xFB
		movwf	PORTA,0
		movff	cletra2,PORTD
		movlw	0xF7
		movwf	PORTA,0
		movff	cletra3,PORTD
		setf	PORTA,0
		movlw	0xFD
		movwf	PORTC,0
		movff	cletra4,PORTD
		movlw	0xFB
		movwf	PORTC,0
		movff	cletra5,PORTD
		setf	PORTC,0
		movlw	0xDF
		movwf	PORTA,0
		movff	cletra6,PORTD
		movlw	0xBF
		movwf	PORTA,0
		movff	cletra7,PORTD
		bra		vis
;******************************************************************************
RST0	bcf		INTCON,TMR0IF,0
		;movlw 	d'194'
		movlw   0x85
		movwf	TMR0L
		retfie
;********************************************************
retardo setf 	stop,0  
nada	nop
		decfsz 	stop,F,0
		bra 	nada
		movlw	0xff
		movwf	stop,0
		return
;******************************************************************************
RSRB	;call 	KBD   ;Llamamos rutina del teclado
  		bcf 	INTCON,RBIF,0  ;LIMPIAMOS BANDERA
		retfie
	END