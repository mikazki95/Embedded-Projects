 	LIST P=18F4550 
	#include <P18F4550.INC>                                            ;directiva para definir el procesad #include <P18F4550.INC>    ;definiciones de variables especificas de
;********************************************************************;Bits de configuración
    ;CONFIG FOSC = INTOSC_XT                    ;Oscilador INT usado por el uC , XT usado po    CONFIG BOR  = OFF    ;BROWNOUT RESET DESHABILIT    CONFIG PWRT = ON     ;PWR UP Timer habilitado
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
;********************************************************************                            ORG 0x0000                                ; vector de reset
;Reset vector
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
       	movwf 	T0CON,0                                ;timer 16 bits prescalerX64
       	movlw 	0XA0
       	movwf 	INTCON,0  ;interrupcion TMR0 habilitada 
       	movlw 	0xE1
       	movwf 	TMR0H,0
       	movlw 	0x7c
       	movwf 	TMR0L,0  ;valor de precarga para 500ms acero              movlw 0xC0   ;código del cero
cero	movlw 	0xC0   ;código del cero
       	movwf 	PORTD,0
       	call 	repite
       	btfss 	PORTB,0,0
       	bra 	nueve
uno    	movlw 	0xF9   ;código del uno
       	movwf 	PORTD,0
       	call 	repite
       	btfss 	PORTB,0,0
       	bra 	cero
dos    	movlw 	0xA4   ;código del dos
       	movwf 	PORTD,0
       	call 	repite
       	btfss 	PORTB,0,0
       	bra 	uno   
tres   	movlw 	0xB0   ;código del tres
       	movwf 	PORTD,0
       	call 	repite
       	btfss 	PORTB,0,0
       	bra 	dos
cuatro 	movlw 	0x99   ;código del cuatro
       	movwf 	PORTD,0
       	call 	repite
       	btfss	PORTB,0,0
       	bra 	tres
cinco  	movlw 	0x92   ;código del cinco
       	movwf 	PORTD,0
       	call 	repite
       	btfss 	PORTB,0,0
       	bra 	cuatro
seis   	movlw 	0x82   ;código del seis
       	movwf 	PORTD,0
       	call 	repite
       	btfss 	PORTB,0,0
       	bra 	cinco  
siete  	movlw 	0xB8   ;código del siete
       	movwf 	PORTD,0
       	call    repite
       	btfss 	PORTB,0,0
       	bra 	seis
ocho   	movlw 	0x80   ;código del ocho
       	movwf 	PORTD,0
       	call 	repite
       	btfss 	PORTB,0,0
       	bra 	siete
nueve  	movlw 	0x98   ;código del nueve
       	movwf 	PORTD,0
       	call 	repite
       	btfss 	PORTB,0,0
       	bra 	ocho
       	bra 	cero
;******************************************************************************
repite 	btfss 	flags,0,0
       	bra 	repite
       	bcf 	flags,0,0
       	return
;*****************************************************************************
RST0   	bcf 	INTCON,TMR0IF,0 ;apagamos bandera timer0
       	movlw 	0xE1
       	movwf 	TMR0H,0
       	movlw 	0x7c
       	movwf 	TMR0L,0  ;valor de precarga para 500ms a 4MHz
       	bsf 	flags,0,0
       	retfie
       	END               