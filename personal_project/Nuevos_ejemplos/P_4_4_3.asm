LIST P=18F4550 ;directiva para efinir el procesador
#include <P18F4550.INC> ;definiciones de variables especificas del procesador
;******************************************************************************
;Bits de configuración
CONFIG FOSC = INTOSC_XT ;Oscilador INT usado por el uC , XT usado por el USB
CONFIG BOR = OFF ;BROWNOUT RESET DESHABILITADO
CONFIG PWRT = ON ;PWR UP Timer habilitado
CONFIG WDT = OFF ;Temporizador vigia apagado
CONFIG MCLRE=OFF ;Reset apagado
CONFIG PBADEN=OFF
CONFIG LVP = OFF
;******************************************************************************
;Definiciones de variables
		CBLOCK 0x000 ;ejemplo de definición de variables en RAM de acceso
		cont
		veces
		rep
		mem
		rest
		ENDC
;******************************************************************************
;Reset vector
		ORG 0x0000
;Inicio del programa principal
		bcf 	OSCCON,IRCF2,0
		bcf 	OSCCON,IRCF1,0
		bsf 	OSCCON,IRCF0,0 ;Oscilador interno a125 kHz{7
		movlw 	0x0F
		movwf 	ADCON1,0 ;Puertos Digitales
		;movlw 	0x00
		;movwf 	PORTE,0 ;
		;clrf 	PORTD,0
		movlw 	0xF8
		movwf 	TRISE,0 ;Puerto E Configurado como salida
		setf	PORTE,0
inicio	call 	retardo
		call 	retardo
		call 	retardo
		call 	retardo
		;btfss	PORTB,0,0
		;bsf 	PORTD,0,0 ;enciende LED conectado en RD0
		;movlw	0x08 
		
		;btfss	PORTB,1,0
		btg		PORTE,RE0,0
		bra		inicio


;******************************************************************************
retardo movlw 	0xff
		movwf 	cont,0
nada    nop
		decfsz 	cont,1,0
		bra 	nada
		return
		END