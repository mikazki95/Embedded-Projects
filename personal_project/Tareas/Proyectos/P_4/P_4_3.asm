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
		tecla
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
		bra 	RSRB
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
		movlw 	0xf0
		movwf 	TRISB,0   ;RC0 y RC1 como salidas
		clrf	PORTB,0
		movlw   0xC7
		movwf 	T0CON,0
		movlw	0xE8
		movwf	INTCON,0 			; ACTIVA INTERRUPCION TMR0, INT0, PRIORIDADES
		bsf		RCON,IPEN,0			;ACTIVA PRIORIDADES DE INTERRUPCION
		bsf		INTCON2,RBIP,0
		bcf		INTCON2,TMR0IP,0	;TMR0 BAJA PRIORIDAD
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
		;movff	selec,PORTA
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
vis		btfss 	flags,1,0
		bra		nom
		btfsc 	flags,2,0
		bra		vis
nom		movlw   0x0E
		movwf   PORTA,0   ;encendemos display unidades
		movff 	cunimin,PORTD
		call 	retardo
		movlw 	0x0D
		movwf 	PORTA,0                                ;encendemos display decenas
		movff 	cdecmin,PORTD
		call 	retardo
		movlw   0x0B
		movwf   PORTA,0   ;encendemos display unidades
		movff 	cunih,PORTD
		call 	retardo
		movlw 	0x07
		movwf 	PORTA,0                                ;encendemos display decenas
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
		btg		flags,2,0
		retfie

;******************************************************************************
RSRB	call 	KBD   ;Llamamos rutina del teclado
  		bcf 	INTCON,RBIF,0  ;LIMPIAMOS BANDERA
  		movwf 	tecla,0   ;guardamos tecla pulsada
 	 	movlw 	0x0E
  		cpfslt 	tecla,0                               ;salta si es que se pulso una tecla entre 0 y 9
  		bra 	op
		movlw 	0x00 
		cpfsgt 	selec,0
		bra		exit
		movlw 	0x01 
		cpfseq 	selec,0
		bra		dmin
  		movff 	tecla,iunimin                          ;carga número de tecla pulsada en decenas
  		movff  	iunimin,TBLPTRL
     	tblrd*   ;lee
  		movff 	TABLAT,cunimin                        ;cdec tiene código 7 segmentos
  		movlw	0x02
		movwf	selec,0
		call	retardo
		bra 	exit
dmin	movlw 	0x02 
		cpfseq 	selec,0
		bra		uh
  		movff 	iunimin,idecmin                          ;carga número de tecla pulsada en decenas
  		movff  	idecmin,TBLPTRL
     	tblrd*   ;lee
  		movff 	TABLAT,cdecmin                        ;cdec tiene código 7 segmentos
  		movff 	tecla,iunimin                          ;carga número de tecla pulsada en decenas
  		movff  	iunimin,TBLPTRL
     	tblrd*   ;lee
  		movff 	TABLAT,cunimin 
		movlw	0x03
		movwf	selec,0
		call	retardo
		bra		exit
uh		movlw 	0x03 
		cpfseq 	selec,0
		bra		dh
		movff 	idecmin,iunih                          ;carga número de tecla pulsada en decenas
  		movff  	iunih,TBLPTRL
     	tblrd*   ;lee
  		movff 	TABLAT,cunih
  		movff 	iunimin,idecmin                          ;carga número de tecla pulsada en decenas
  		movff  	idecmin,TBLPTRL
     	tblrd*   ;lee
  		movff 	TABLAT,cdecmin                        ;cdec tiene código 7 segmentos
  		movff 	tecla,iunimin                          ;carga número de tecla pulsada en decenas
  		movff  	iunimin,TBLPTRL
     	tblrd*   ;lee
  		movff 	TABLAT,cunimin 
		movlw	0x04
		movwf	selec,0
		call	retardo
		bra		exit
dh		movff 	iunih,idech                          ;carga número de tecla pulsada en decenas
  		movff  	idech,TBLPTRL
     	tblrd*   ;lee
  		movff 	TABLAT,cdech
		movff 	idecmin,iunih                          ;carga número de tecla pulsada en decenas
  		movff  	iunih,TBLPTRL
     	tblrd*   ;lee
  		movff 	TABLAT,cunih
  		movff 	iunimin,idecmin                          ;carga número de tecla pulsada en decenas
  		movff  	idecmin,TBLPTRL
     	tblrd*   ;lee
  		movff 	TABLAT,cdecmin                        ;cdec tiene código 7 segmentos
  		movff 	tecla,iunimin                          ;carga número de tecla pulsada en decenas
  		movff  	iunimin,TBLPTRL
     	tblrd*   ;lee
  		movff 	TABLAT,cunimin
		movlw	0x01
		movwf	selec,0 
		call	retardo
op	  	movlw 	0x0E 
		cpfseq 	tecla,0
 	 	bra 	op1
		movlw	0x01
		movwf	selec,0
		bsf 	flags,1,0
		bra		exit
op1	  	movlw 	0x0F 
		cpfseq 	tecla,0
 	 	bra 	op2
		movlw	0x00
		movwf	selec,0
		bcf 	flags,1,0
		movlw 	0x0D 
		cpfsgt 	iunimin,0
		bra		ldm
		movlw	0x0D
		movwf	iunimin,0
		movff  	iunimin,TBLPTRL
     	tblrd*   ;lee
  		movff 	TABLAT,cunimin
ldm		movlw 	0x09 
		cpfsgt 	idecmin,0
		bra		h0
		movlw	0x09
		movwf	idecmin,0
		movff  	idecmin,TBLPTRL
     	tblrd*   ;lee
  		movff 	TABLAT,cdecmin
h0		movlw 	0x05 
		cpfsgt 	idech,0
		bra		h1
		movlw	0x06
		movwf	idech,0
		movff  	idech,TBLPTRL
     	tblrd*   ;lee
  		movff 	TABLAT,cdech
		movlw 	0x07 
		cpfsgt 	iunih,0
		bra		exit
		movlw	0x07
		movwf	iunih,0
		movff  	iunih,TBLPTRL
     	tblrd*   ;lee
  		movff 	TABLAT,cunih
		movlw	0x06
		movwf	idech,0
		movff  	idech,TBLPTRL
     	tblrd*   ;lee
  		movff 	TABLAT,cdech
		bra 	exit
h1		movlw 	0x0D 
		cpfsgt 	iunih,0
		bra		exit
		movlw	0x0D
		movwf	iunih,0
		movff  	iunih,TBLPTRL
     	tblrd*   ;lee
  		movff 	TABLAT,cunih
		bra		exit
op2
		bra		exit
exit	clrf 	PORTB,0
  		retfie
;********************************************************************
KBD  	movlw 	0x0e
  		movwf 	PORTB,0   ;activamos primer renglón
  		btfss 	PORTB,4,0
  		retlw 	0x0E   ;tecla A
  		btfss   PORTB,5,0
  		retlw 	0x07   ;tecla 3
  		btfss 	PORTB,6,0          
       	retlw   0x06   ;tecla 2
  		btfss 	PORTB,7,0          
       	retlw   0x05   ;tecla 1
  		movlw 	0x0D
  		movwf 	PORTB,0   ;activamos segundo renglón
  		btfss 	PORTB,4,0
  		retlw 	0x0F   ;tecla B
  		btfss   PORTB,5,0
  		retlw 	0x0A   ;tecla 6
  		btfss 	PORTB,6,0          
       	retlw   0x09   ;tecla 5
       	btfss 	PORTB,7,0          
       	retlw   0x08   ;tecla 4
       	movlw 	0x0B
  		movwf 	PORTB,0   ;activamos tercer renglón
  		btfss 	PORTB,4,0
 		retlw 	0x10   ;tecla C
  		btfss   PORTB,5,0
  		retlw 	0x0D   ;tecla 9
  		btfss 	PORTB,6,0          
        retlw   0x0C   ;tecla 8
  		btfss 	PORTB,7,0          
        retlw   0x0B	;TECLA 7
  		movlw 	0x07
  		movwf 	PORTB,0   ;activamos cuarto renglón
  		btfss 	PORTB,4,0
  		retlw 	0x07   ;tecla D kuyyyy
  		btfss   PORTB,5,0
  		retlw 	0x08   ;tecla #
  		btfss 	PORTB,6,0          
        retlw   0x04  ;tecla 0
  		btfss 	PORTB,7,0          
        retlw   0x04   ;tecla *  
		retlw  	0xFF                                 ;ninguna tecla pulsada
;******************************************************************************


		org	0x304			;DB directiva que Define Byte             
        DB	0xC0,0XF9,0XA4,0XB0,0X99,0X92,0X82,0XF8,0X80,0X98,0x39,0x5E,0x79,0x71   ;código del F


	    END