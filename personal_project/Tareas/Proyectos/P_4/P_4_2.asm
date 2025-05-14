LIST P=18F4550                             ;directiva para definir el procesador
#include <P18F4550.INC> ;definiciones de variables especificas del procesador

;******************************************************************************
;Bits de configuración
    CONFIG FOSC = INTOSC_XT                   ;Oscilador INT usado por el uC , XT usado por el  USB
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
		selec
		iuniseg    ;índice de unidades
  		idecseg    ;índice de decenas
  		iunimin    ;índice de unidades
  		cunimin    ;código de 7 segmentos de unidades
  		idecmin    ;índice de decenas
  		cdecmin    ;código de 7 segmentos de decenas 
  		iunih    ;índice de unidades
  		cunih    ;código de 7 segmentos de unidades
  		idech    ;índice de decenas
  		cdech    ;código de 7 segmentos de decenas 
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
		clrf 	TRISA,0
		clrf 	PORTA,0
		clrf 	TRISC,0
		clrf 	PORTC,0
		movlw 	0x0F
		movwf 	TRISB,0   ;RC0 y RC1 como salidas
		clrf	PORTB,0
		;bcf 	PORTB,0,0
		movlw   0xC7
		movwf 	T0CON,0
		movlw	0xF0
		movwf	INTCON,0 			; ACTIVA INTERRUPCION TMR0, INT0, PRIORIDADES
		movlw	0xD8
		movwf	INTCON3,0
		bsf		RCON,IPEN,0			;ACTIVA PRIORIDADES DE INTERRUPCION
		bcf		INTCON2,TMR0IP,0	;TMR0 BAJA PRIORIDAD
		;movlw	0xc2
		movlw   0x0A
		movwf 	TMR0L
		clrf	selec,0
		clrf	flags,0
inicio	movlw	0x04
		movwf	TBLPTRL,0
		movlw	0x03
		movwf	TBLPTRH,0
		clrf	TBLPTRU,0		;tblptr=0x000300
		movlw	0x04
		movwf	iuniseg,0
		movwf 	idecseg,0
		movwf	iunimin,0
		movwf 	idecmin,0
		movwf 	iunih,0
		movwf 	idech,0
		movlw	0x30
		movwf	cont,0
cod		btg		PORTC,1,0
		movff	selec,PORTA
		movff   iunimin,TBLPTRL 
		tblrd*   ;lee la tabla sin modificar apuntador
		movff 	TABLAT,cunimin                         ;cuni tiene código 7 segmentos
		movff   idecmin,TBLPTRL                         ;ajusta apuntador
		tblrd*                  ;lee la tabla sin modificar apuntador
		movff 	TABLAT,cdecmin
		movff   iunih,TBLPTRL 
		tblrd*   ;lee la tabla sin modificar apuntador
		movff 	TABLAT,cunih                         ;cuni tiene código 7 segmentos
		movff   idech,TBLPTRL                         ;ajusta apuntador
		tblrd*
		movff 	TABLAT,cdech
vis		movlw   0xE0
		movwf   PORTB,0   ;encendemos display unidades
		movff 	cunimin,PORTD
		call 	retardo
		movlw 	0xD0
		movwf 	PORTB,0                                ;encendemos display decenas
		movff 	cdecmin,PORTD
		call 	retardo
		movlw   0xB0
		movwf   PORTB,0   ;encendemos display unidades
		movff 	cunih,PORTD
		call 	retardo
		movlw 	0x70
		movwf 	PORTB,0                                ;encendemos display decenas
		movff 	cdech,PORTD
		call 	retardo
		btfss 	flags,0,0
		bra 	vis
		bcf		flags,0,0
lim		incf	iuniseg
		movlw	0x0D
		cpfsgt	iuniseg
		bra		cod
		incf	idecseg
		movlw	0x04
		movwf	iuniseg,0
		movlw	0x09
		cpfsgt	idecseg
		bra		cod
		incf	iunimin
		movlw	0x04
		movwf	idecseg,0
		movlw	0x0D
		cpfsgt	iunimin
		bra		cod
		incf	idecmin
		movlw	0x04
		movwf	iunimin,0
		movlw	0x09
		cpfsgt	idecmin
		bra		cod
		incf	iunih
		movlw	0x04
		movwf	idecmin,0
		movlw	0x06
		cpfslt	idech
		bra		dos
		movlw	0x0D
		cpfsgt	iunih
		bra		cod
		incf	idech
		movlw	0x04
		movwf	iunih,0
		bra		cod
dos		movlw	0x07
		cpfsgt	iunih
		bra		cod
		bra		inicio
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
		;movlw 	d'194'
		movlw   0x0A
		movwf	TMR0L
		bsf 	flags,0,0 ;monitor de un 1s
		retfie
;******************************************************************************
RSINT0	btfss	INTCON,INT0IF,0
		bra		RSINT1
		bcf		INTCON,INT0IF,0
		INCF	selec,1
		movlw	0x04
		cpfsgt	selec
		retfie
		clrf	selec,0
		bcf		flags,1,0	
		retfie	
;******************************************************************************
RSINT1	btfss	INTCON3,INT1IF,0
		bra		RSINT2
		bcf		INTCON3,INT1IF,0
		movlw	0x00
		cpfseq	selec
		bra		aum
		retfie
aum		movlw	0x01
		cpfseq	selec
		bra		adm
		movlw	0x0C
		cpfsgt	iunimin
		bra		lum
		movlw	0x04
		movwf	iunimin,0
		retfie
lum		incf	iunimin,1
		retfie
adm		movlw	0x02
		cpfseq	selec
		bra		auh
		movlw	0x08
		cpfsgt	idecmin
		bra		ldm
		movlw	0x04
		movwf	idecmin,0
		retfie
ldm		incf	idecmin,1
		retfie
auh		movlw	0x03
		cpfseq	selec
		bra		adh
		movlw	0x05
		cpfsgt	idech
		bra		lduh
		movlw	0x06
		cpfsgt	iunih
		bra		luh
		movlw	0x04
		movwf	iunih,0
		movlw	0x04
		movwf	idech,0
		retfie
luh		incf	iunih,1
		retfie
lduh	movlw	0x0C
		cpfsgt	iunih
		bra		ludh
		movlw	0x04
		movwf	iunih,0
		retfie
ludh	incf	iunih,1
		retfie
adh		movlw	0x05
		cpfsgt	idech
		bra		ldh
		movlw	0x04
		movwf	idech,0
		retfie
ldh		incf	idech,1
		movlw	0x05
		cpfsgt	idech
		retfie
		movlw	0x07
		cpfsgt	iunih
		retfie
		movlw	0x04
		movwf	iunih,0
		retfie
;******************************************************************************
RSINT2	bcf		INTCON3,INT2IF,0
		movlw	0x00
		cpfseq	selec
		bra		dum
		retfie
dum		movlw	0x01
		cpfseq	selec
		bra		ddm
		decf	iunimin,1
		movlw	0x04
		cpfslt	iunimin
		retfie
		movlw	0x0D
		movwf	iunimin
		retfie
ddm		movlw	0x02
		cpfseq	selec
		bra		duh
		decf	idecmin,1
		movlw	0x04
		cpfslt	idecmin
		retfie
		movlw	0x09
		movwf	idecmin
		retfie
duh		movlw	0x03
		cpfseq	selec
		bra		ddh
		decf	iunih,1
		movlw	0x06
		cpfslt	idech
		bra		liduh
		movlw	0x04
		cpfslt	iunih
		retfie
		movlw	0x0D
		movwf	iunih
		retfie
liduh	movlw	0x04
		cpfslt	iunih
		retfie
		movlw	0x07
		movwf	iunih
		retfie
ddh		decf	idech,1
		movlw	0x04
		cpfslt	idech
		retfie
		movlw	0x06
		movwf	idech
		movlw	0x08
		cpfsgt	iunih
		retfie
		movlw	0x08
		movwf	iunih
		retfie
;******************************************************************************

		org	0x304			;DB directiva que Define Byte             
        DB	0xC0,0XF9,0XA4,0XB0,0X99,0X92,0X82,0XF8,0X80,0X98   ;código del F


	    END 