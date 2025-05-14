;******************************************************************************
	LIST P=18F4550 
	#include <P18F4550.INC>                                            ;directiva para definir el procesad #include <P18F4550.INC>    ;definiciones de variables especificas de
;********************************************************************;Bits de configuración
    CONFIG FOSC = INTOSC_XT                    ;Oscilador INT usado por el uC , XT usado po    CONFIG BOR  = OFF    ;BROWNOUT RESET DESHABILIT    CONFIG PWRT = ON     ;PWR UP Timer habilitado
    CONFIG BOR  = OFF			 ;BROWNOUT RESET DESHABILITADO
    CONFIG PWRT = ON 			 ;PWR UP Timer habilitado
    CONFIG WDT  = OFF			 ;Temporizador vigia apagado
    CONFIG MCLRE=OFF			 ;Reset apagado
    CONFIG PBADEN=OFF
    CONFIG LVP  = OFF
;********************************************************************;Definiciones de variables
  CBLOCK 0x000            ;ejemplo de definición de variables en RAM   flags                                  ;definimos la dirección 0 como registro de b  ENDC             ;fin del bloque de constantes
		flags
		ciclo
		index
		ENDC
;******************************************************************************
		ORG	0x0000		
       	bra		inicio
       	org 	0x08   ;vector de interrupción
       	bra		RST0   ;ramifica servicio interrupción T0             org 0x0020
inicio 	bsf 	OSCCON,IRCF2,0                 ;Inicio del programa principal
       	bsf		OSCCON,IRCF1,0
       	bcf 	OSCCON,IRCF0,0                ;Oscilador interno a 4 MHz
       	movlw   0x0F
       	movwf   ADCON1,0                            ;Puertos Digitales
       	clrf 	PORTD,0
       	clrf 	TRISD,0                                ;Puerto D Configurado como salid                            movlw       0x95
       	movwf 	T0CON,0 			;timer 8 bits,prescaler x64
		movlw	0x09
		movf	INTCON,0		;Activo interrupcion de timer
		movlw	0xff
		movwf 	TMR0H			;precarga
		movlw	0x86
		movwf 	TMR0L			;precarga
ini		clrf	index,0			;inicia conteo en cero
cods	movf	index,W,0
		call	tabla
		movwf	PORTD,0
		call	espera
		incf	index,F,0
		cpfsgt	index
		bra		cods
		bra		ini


;******************************************************************************
tabla	rlcf	index,W,0		;multiplica índice por 2
		addwf   PCL,F,0			;ajusta el PCL de acuerdo al valor del índice
		retlw	0xC0			;código del cero
		retlw	0xf9			;código del uno
		retlw	0xA4			;código del dos
		retlw	0xb0			;código del tres
		retlw	0x99			;código del cuatro
		retlw	0x92			;código del cinco
		retlw	0x82			;código del seis
		retlw	0xb8			;código del siete
		retlw	0x80			;código del ocho
		retlw	0x98			;código del nueve
		retlw	0x88			;código del A
		retlw	0x83			;código del B
		retlw	0xC6			;código del C
		retlw	0xA1			;código del D
		retlw	0x85			;código del E
		retlw	0x8E			;código del F
;****************************************************************
espera 
		btg 	PORTB,2,0
		btfss	flags,0,0
		bra		espera
	    return
;******************************************************************************
RST0	bcf		INTCON,TMR0IF,0
		movlw 	d'134'
		movwf	TMR0L
		movlw	0xff
		movwf 	TMR0H			;precarga
		bsf 	flags,0,0 ;monitor de un 1s
		retfie
	    END

