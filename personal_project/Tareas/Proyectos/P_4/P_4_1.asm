;******************************************************************************

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
  		ciclo
		cont
		iun    ;índice de unidades
  		cuni    ;código de 7 segmentos de unidades
  		idec    ;índice de decenas
  		cdec    ;código de 7 segmentos de decenas 
  		icen    ;índice de unidades
  		ccen    ;código de 7 segmentos de unidades
  		imil    ;índice de decenas
  		cmil    ;código de 7 segmentos de decenas 
  	ENDC      ;fin del bloque de constantes
;******************************************************************************
;Reset vector 
	ORG 0x0000;Inicio del programa principal
		goto	inicio2  
		org     0x08 		;vector de interrupcion alta prioridad
		bra 	RSINT0
		org		0x18		;vector de interrupcion baja prioridad
		bra		RST0
inicio2	bcf 	OSCCON,IRCF2,0
		bsf 	OSCCON,IRCF1,0
		bcf 	OSCCON,IRCF0,0  	;Oscilador interno a125 kHz
		movlw   0x0F
		movwf   ADCON1,0            ;Puertos Digitales
		clrf 	PORTD,0
		clrf 	TRISD,0             ;Puerto D Configurado como salida
		clrf 	PORTA,0
		clrf 	TRISA,0
		movlw 	0x01
		movwf 	TRISB,0   ;RC0 y RC1 como salidas
		;bcf 	PORTB,0,0
		movlw   0xC7
		movwf 	T0CON,0
		movlw	0xF0
		movwf	INTCON,0 			; ACTIVA INTERRUPCION TMR0, INT0, PRIORIDADES
		bsf		RCON,IPEN,0			;ACTIVA PRIORIDADES DE INTERRUPCION
		bcf		INTCON2,TMR0IP,0	;TMR0 BAJA PRIORIDAD
		;movlw	0xc2
		movlw   0x0B
		movwf 	TMR0L
inicio	movlw	0x04
		movwf	TBLPTRL,0
		movlw	0x03
		movwf	TBLPTRH,0
		clrf	TBLPTRU,0		;tblptr=0x000300
		movlw	0x04
		movwf	iun,0
		movwf 	idec,0
		movwf 	icen,0
		movwf 	imil,0
		movlw	0x30
		movwf	cont,0
cod		btg		PORTA,0,0
		movff   iun,TBLPTRL 
		tblrd*   ;lee la tabla sin modificar apuntador
		movff 	TABLAT,cuni                         ;cuni tiene código 7 segmentos
		movff   idec,TBLPTRL                         ;ajusta apuntador
		tblrd*                  ;lee la tabla sin modificar apuntador
		movff 	TABLAT,cdec
		movff   icen,TBLPTRL 
		tblrd*   ;lee la tabla sin modificar apuntador
		movff 	TABLAT,ccen                         ;cuni tiene código 7 segmentos
		movff   imil,TBLPTRL                         ;ajusta apuntador
		tblrd*
		movff 	TABLAT,cmil
vis		movlw   0xFD
		movwf   PORTB,0   ;encendemos display unidades
		movff 	cuni,PORTD
		call 	retardo
		movlw 	0xFB
		movwf 	PORTB,0                                ;encendemos display decenas
		movff 	cdec,PORTD
		call 	retardo
		movlw   0xF7
		movwf   PORTB,0   ;encendemos display unidades
		movff 	ccen,PORTD
		call 	retardo
		movlw 	0xEF
		movwf 	PORTB,0                                ;encendemos display decenas
		movff 	cmil,PORTD
		call 	retardo
		btfss 	flags,0,0
		bra 	vis
		bcf		flags,0,0
		btfss 	flags,1,0
		bra		asc
		decf	iun
		movlw	0x04
		cpfslt	iun
		bra		cod
		decf	idec
		movlw	0x0D
		movwf	iun,0
		movlw	0x04
		cpfslt	idec
		bra		cod
		decf	icen
		movlw	0x0D
		movwf	idec,0
		movlw	0x04
		cpfslt	icen
		bra		cod
		decf	imil
		movlw	0x0D
		movwf	icen,0
		movlw	0x04
		cpfslt	imil
		bra		cod
		movlw	0x0D
		movwf	imil,0
		bra		cod
		movlw	0x0D
		movwf	imil,0
		bra 	cod
asc		incf	iun
		movlw	0x0D
		cpfsgt	iun
		bra		cod
		incf	idec
		movlw	0x04
		movwf	iun,0
		movlw	0x0D
		cpfsgt	idec
		bra		cod
		incf	icen
		movlw	0x04
		movwf	idec,0
		movlw	0x0D
		cpfsgt	icen
		bra		cod
		incf	imil
		movlw	0x04
		movwf	icen,0
		movlw	0x0D
		cpfsgt	imil
		bra		cod
		movlw	0x04
		movwf	imil,0
		bra		cod
;******************************************************************************
retardo ;setf 	cont,0  
nada	nop
		decfsz 	cont,F,0
		bra 	nada
		movlw	0x30
		movwf	cont,0
		return
;*******************************************************
espera 	btfss	flags,0,0
		bra		espera
		bcf		flags,0,0
	    return
;****************************************************************
RST0	bcf		INTCON,TMR0IF,0
		movlw   0x0B
		movwf	TMR0L
		bsf 	flags,0,0 ;monitor de un 1s
		retfie
;******************************************************************************
RSINT0	bcf		INTCON,INT0IF,0
		btg 	flags,1,0 ;monitor de un SUM/RES
		retfie
;******************************************************************************

		org	0x304			;DB directiva que Define Byte             
        DB	0xC0,0XF9,0XA4,0XB0,0X99,0X92,0X82,0XF8,0X80,0X98   ;código del F


	    END