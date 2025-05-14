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
		flags
		cont
		cont1  		
  	ENDC      ;fin del bloque de constantes
;******************************************************************************
;Reset vector 
	ORG 0x0000;Inicio del programa principal
		goto	def  
		;org     0x08 		;vector de interrupcion alta prioridad
		;bra 	RSRB
		;org		0x18		;vector de interrupcion baja prioridad
		;bra		RST0	
def		bsf 	OSCCON,IRCF2,0
		bsf 	OSCCON,IRCF1,0
		bsf 	OSCCON,IRCF0,0  	;Oscilador interno a125 kHz
		movlw   0x0F
		movwf   ADCON1,0            ;Puertos Digitales
		bsf		TRISC,0,0
		clrf 	TRISE,0
		movlw   0x07
		movwf 	PORTE,0
;////////////////inicio de ptrograma 
inicio	;bsf		PORTE,0,0
		btg	PORTE,0,0
		call	retardo
		;bcf		PORTE,0,0
		;call	retardo
		goto	inicio
;******************************************************************************
retardo ;setf 	cont,0  
nada	nop
		decfsz	cont1,F,0
		bra		nada
		movlw	0x08
		movwf	cont1,0
		decfsz 	cont,F,0
		bra 	nada
		movlw	0xFF
		movwf	cont,0
		return
;///////////////////////////////////Fin de proglrama ////////////////////////
		END