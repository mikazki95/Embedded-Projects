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
		cont
  		ciclo
	ENDC      ;fin del bloque de constantes
;******************************************************************************
;Reset vector 
	ORG 0x0000;Inicio del programa principal
		bsf 	OSCCON,IRCF2,0
		bsf 	OSCCON,IRCF1,0
		bcf 	OSCCON,IRCF0,0  ;Oscilador interno a125 kHz
		movlw   0x0F
		movwf   ADCON1,0            ;Puertos Digitales
		clrf 	PORTD,0
		clrf 	TRISD,0                  ;Puerto D Configurado como salida
		clrf 	TRISA,0
		clrf 	PORTA,0
cero    movlw	0x3F     ;c�digo del cero
		movwf	PORTD,0
		nop
		nop
		nop
		call 	repite
		btg		PORTA,0,0
		btfss	PORTB,0,0
		bra		f
uno		movlw	0x06     ;c�digo del uno
		movwf	PORTD,0
		nop
		nop
		nop
		call 	repite
		btg		PORTA,0,0
		btfss	PORTB,0,0
		bra		cero
dos		movlw	0x5B     ;c�digo del dos
		movwf	PORTD,0
		nop
		nop
		nop
		call 	repite
		btg		PORTA,0,0
		btfss	PORTB,0,0
		bra		uno
tres	movlw	0x4F   ;c�digo del tres
		movwf	PORTD,0
		nop
		nop
		nop
		call 	repite 
		btg		PORTA,0,0
		btfss	PORTB,0,0
		bra		dos
cuatro	movlw	0x66  ;c�digo del cuatro
		movwf	PORTD,0
		nop
		nop
		nop
		call 	repite
		btg		PORTA,0,0
		btfss	PORTB,0,0
		bra		tres
cinco	movlw	0x6D  ;c�digo del cinco
		movwf	PORTD,0
		nop
		nop
		nop
		call 	repite
		btg		PORTA,0,0
		btfss	PORTB,0,0
		bra		cuatro
seis	movlw	0x7D  ;c�digo del seis
		movwf	PORTD,0
		nop
		nop
		nop
		call 	repite
		btg		PORTA,0,0
		btfss	PORTB,0,0
		bra		cinco
siete	movlw	0x07  ;c�digo del siete
		movwf	PORTD,0
		nop
		nop
		nop
		call 	repite 
		btg		PORTA,0,0 
		btfss	PORTB,0,0
		bra		seis
ocho	movlw	0x7F  ;c�digo del ocho
		movwf	PORTD,0
		nop
		nop
		nop
		call 	repite
		btg		PORTA,0,0
		btfss	PORTB,0,0
		bra		siete
nueve	movlw 	0x67  ;c�digo del nueve
		movwf	PORTD,0
		nop
		nop
		nop
		call 	repite
		btg		PORTA,0,0
		btfss	PORTB,0,0
		bra		ocho
a		movlw 	0x77  ;c�digo del nueve
		movwf	PORTD,0
		nop
		nop
		nop
		call 	repite
		btg		PORTA,0,0
		btfss	PORTB,0,0
		bra		nueve
b		movlw 	0x7C  ;c�digo del nueve
		movwf	PORTD,0
		nop
		nop
		nop
		call 	repite
		btg		PORTA,0,0
		btfss	PORTB,0,0
		bra		a
c		movlw 	0x39  ;c�digo del nueve
		movwf	PORTD,0
		nop
		nop
		nop
		call 	repite
		btg		PORTA,0,0
		btfss	PORTB,0,0
		bra		b
d		movlw 	0x5E  ;c�digo del nueve
		movwf	PORTD,0
		nop
		nop
		nop
		call 	repite
		btg		PORTA,0,0
		btfss	PORTB,0,0
		bra		c
e		movlw 	0x79  ;c�digo del nueve
		movwf	PORTD,0
		nop
		nop
		nop
		call 	repite
		btg		PORTA,0,0
		btfss	PORTB,0,0
		bra		d
f		movlw 	0x71  ;c�digo del nueve
		movwf	PORTD,0
		nop
		nop
		nop
		call 	repite
		btg		PORTA,0,0
		btfss	PORTB,0,0
		bra		e
		bra 	cero

;******************************************************************************
retardo movlw 0xFA
		movwf cont,0
nada    nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		decfsz cont,1,0
		bra nada
		return
;******************************************************************************
repite  movlw 0xC8  ;llama 25 veces a la rutina retardo 
		movwf ciclo
		nop
llama  	call retardo
		decfsz   ciclo,F,0
		bra         llama
		return
	END
