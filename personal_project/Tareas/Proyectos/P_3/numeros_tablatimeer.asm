;******************************************************************************
	LIST P=18F4550 
	#include <P18F4550.INC>                                            ;directiva para definir el procesad #include <P18F4550.INC>    ;definiciones de variables especificas de
;********************************************************************;Bits de configuraci�n
    CONFIG FOSC = INTOSC_XT                    ;Oscilador INT usado por el uC , XT usado po    CONFIG BOR  = OFF    ;BROWNOUT RESET DESHABILIT    CONFIG PWRT = ON     ;PWR UP Timer habilitado
    CONFIG BOR  = OFF			 ;BROWNOUT RESET DESHABILITADO
    CONFIG PWRT = ON 			 ;PWR UP Timer habilitado
    CONFIG WDT  = OFF			 ;Temporizador vigia apagado
    CONFIG MCLRE=OFF			 ;Reset apagado
    CONFIG PBADEN=OFF
    CONFIG LVP  = OFF
;********************************************************************;Definiciones de variables
  CBLOCK 0x000            ;ejemplo de definici�n de variables en RAM   flags                                  ;definimos la direcci�n 0 como registro de b  ENDC             ;fin del bloque de constantes
		flags
		ciclo
		index
		ENDC
;******************************************************************************
		ORG	0x0000		
       	bra		inicio
       	org 	0x08   ;vector de interrupci�n
       	bra		RST0   ;ramifica servicio interrupci�n T0             org 0x0020
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
		retlw	0x88			;c�digo del A
		retlw	0x83			;c�digo del B
		retlw	0xC6			;c�digo del C
		retlw	0xA1			;c�digo del D
		retlw	0x85			;c�digo del E
		retlw	0x8E			;c�digo del F
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

