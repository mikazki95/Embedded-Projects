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
		dat
		iadc
		iadch
		iutemp
		idtemp
		ictemp
		iutemr
		idtemr
		ictemr
		iutemc
		idtemc
		ictemc
		iuseg
		idseg
		iumin
		idmin
		iuhoras
		pumin
		pdmin
		puhoras
		ban
		ban1	
		selec
		tecla
		cont
		cont1
		contpl	
		stop
		segn 
	ENDC      ;fin del bloque de constantes
;******************************************************************************
;Reset vector 
	ORG 0x0000;Inicio del programa principal
		goto	inicio2  
		org     0x08 		;vector de interrupcion alta prioridad
		bra 	RSAD
		org		0x18		;vector de interrupcion baja prioridad
		bra		RST0 	 
inicio2	bcf 	OSCCON,IRCF2,0
		bsf 	OSCCON,IRCF1,0
		bcf 	OSCCON,IRCF0,0  	;Oscilador interno a125 kHz
		clrf 	TRISD,0             ;Puerto D Configurado como salida
		movlw	0x01
		movwf 	TRISA,0		
		movlw	0xf8
		movwf 	TRISE,0
		clrf 	PORTA,0		
		clrf 	PORTD,0		
		clrf 	PORTE,0
		movlw	b'11000000'
		movwf 	TRISC,0  
		movlw	0xF0
		movwf	TRISB,0
		clrf 	PORTB,0		
		movlw   0x0E
		movwf   ADCON1,0            ;Puertos Digitales
		movlw	0xA4
		movwf	ADCON2,0		;8 TAD, reloj del ADC Fosc/4, justificado izq
		bsf		PIE1,ADIE		;habilito interrupción AD
		movlw	0xE8
		movwf	INTCON,0 			;activamos interrupciones, tmr0, prioridades y RB
		bsf		RCON,IPEN,0			;ACTIVA PRIORIDADES DE INTERRUPCION
		movlw   0xC7
		movwf 	T0CON,0
		movlw	0x01
		movwf	INTCON2,0
;**********////declaraciones de puertos///////*************
;RA0 ENTRADA ANALOGA PARA LA TEMPERTURA;///**/// RA1 ENEBLE DE LCD///**RA2 RW///RA3 RS 	
;RB corresponde al teclado
;RC0 y RC1 salidas para la valbula
;RC4 y RC5 salidas para la chispa
;RC6 y RC7 entradas para el control de flama   	
;DE RD0 a RD7 corresponde a lcd de R0 a R7 correspondientemente 

		bsf		ADCON0,ADON,0	;adc on
		movlw	0x00
		movwf	TBLPTRL,0
		movlw	0x08
		movwf	TBLPTRH,0
		clrf	TBLPTRU,0		;tblptr=0x000500
bini	clrf	iutemp,0
		clrf	dat
		clrf	iutemp
		clrf	idtemp
		clrf	ictemp
		clrf	iumin
		clrf	idmin
		clrf	iuhoras
		clrf	ban 
		clrf	ban1	
		clrf	selec
		clrf	tecla
		clrf	cont
		clrf	contpl,0
		bcf		PORTC,1,0
		movlw	0x3A
		movwf	segn,0
		call	inlcd
		call	limpia

;*************temperatura************************	
		movlw	0x00	;mueve al espacio 10
		movwf	dat
		call	instw
		movlw	0x54	;;T
		movwf	dat
		call	datw
		movlw	0x65	;e	
		movwf	dat
		call	datw
		movlw	0x6d	;m
		movwf	dat
		call	datw
		movlw	0x50	;P
		movwf	dat
		call	datw
		movlw	0x20	;espacio
		movwf	dat
		call	datw
		movlw	0x89	;mueve al espacio 10
		movwf	dat
		call	instw
		movlw	d'167'	;°
		movwf	dat
		call	datw
		movlw	0x43	;C
		movwf	dat
		call	datw
;***********tiempo*********************
		movlw	0xC0	;mueve al espacio 10
		movwf	dat
		call	instw
		movlw	0x54	;;T
		movwf	dat
		call	datw
		movlw	0x69	;i
		movwf	dat
		call	datw
		movlw	0x65	;e	
		movwf	dat
		call	datw
		movlw	0x6d	;m
		movwf	dat
		call	datw
		movlw	0x50	;P
		movwf	dat
		call	datw
		movlw	0x20	;espacio
		movwf	dat
		call	datw
		movlw	0xCB	;mueve al espacio 10
		movwf	dat
		call	instw
		movlw	0X6D	;m
		movwf	dat
		call	datw
		movlw	0x69	;i
		movwf	dat
		call	datw
		movlw	0X6E	;n		
		movwf	dat
		call	datw

		movlw	0x8C	;mueve al espacio 14
		movwf	dat
		call	instw
		movlw	0x54	;;T
		movwf	dat
		call	datw
		movlw	0x65	;e	
		movwf	dat
		call	datw
		movlw	0x6d	;m
		movwf	dat
		call	datw
		movlw	0x52	;R
		movwf	dat
		call	datw
		movlw	0x20	;espacio
		movwf	dat
		call	datw
		movlw	0x94	;mueve al espacio 13
		movwf	dat
		call	instw
		movlw	0XA7	;°
		movwf	dat
		call	datw
		movlw	0x43	;C
		movwf	dat
		call	datw
		
		movlw	0xD0;mueve al espacio 10
		movwf	dat
		call	instw
		movlw	0x54	;;T
		movwf	dat
		call	datw
		movlw	0x69	;i
		movwf	dat
		call	datw
		movlw	0x65	;e	
		movwf	dat
		call	datw
		movlw	0x6d	;m
		movwf	dat
		call	datw
		movlw	0x52	;R
		movwf	dat
		call	datw
		movlw	0x20	;espacio
		movwf	dat
		call	datw
		movlw	0xDB	;mueve al espacio 10
		movwf	dat
		call	instw
		movlw	0X6D	;m
		movwf	dat
		call	datw
		movlw	0x69	;i
		movwf	dat
		call	datw
		movlw	0X6E	;n		
		movwf	dat
		call	datw
prog
wtemp	movlw	0x85	;mueve al espacio 6
		movwf	dat
		call	instw
		movff  	ictemp,TBLPTRL
     	tblrd*   ;lee
  		movff 	TABLAT,dat
		call	datw
		movff  	idtemp,TBLPTRL
     	tblrd*   ;lee
  		movff 	TABLAT,dat
		call	datw
		movff  	iutemp,TBLPTRL
     	tblrd*   ;lee
  		movff 	TABLAT,dat
		call	datw

wtiemp	movlw	0xC6	;mueve al espacio 6
		movwf	dat
		call	instw
		movff  	puhoras,TBLPTRL
     	tblrd*   ;lee
		movff 	TABLAT,dat
		call	datw
		movlw	0x3A	;mueve al espacio 6
		movwf	dat
		call	datw
		movff  	pdmin,TBLPTRL
     	tblrd*   ;lee
  		movff 	TABLAT,dat
		call	datw
		movff  	pumin,TBLPTRL
     	tblrd*   ;lee
  		movff 	TABLAT,dat
		call	datw
reg;********************registro*********************		
wtemr	movlw	0x91	;mueve al espacio 6
		movwf	dat
		call	instw
		movff  	ictemr,TBLPTRL
     	tblrd*   ;lee
  		movff 	TABLAT,dat
		call	datw
		movff  	idtemr,TBLPTRL
     	tblrd*   ;lee
  		movff 	TABLAT,dat
		call	datw
		movff  	iutemr,TBLPTRL
     	tblrd*   ;lee
  		movff 	TABLAT,dat
		call	datw

wtiemr	movlw	0xD6	;mueve al espacio 6
		movwf	dat
		call	instw
		movff  	iuhoras,TBLPTRL
     	tblrd*   ;lee
  		movff 	TABLAT,dat
		call	datw
		movff	segn,dat
		call	datw
		movff  	idmin,TBLPTRL
     	tblrd*   ;lee
  		movff 	TABLAT,dat
		call	datw
		movff  	iumin,TBLPTRL
     	tblrd*   ;lee
  		movff 	TABLAT,dat
		call	datw
;************///control de flama ////////***************
		btfss	ban1,5
		bra		stconv
		btfss	PORTC,6
		call	on1
		btfss	PORTC,7
		call	on1
;*****************////adc/////***************
stconv	bsf		ADCON0,1,0 		;inicio  conversion AD	
		btfss	ban,6,0
		bra		wconv
		clrf	ictemr
		clrf	idtemr
		clrf	iutemr
		bcf		ADCON0,1,0 		;inicio  conversion AD
		bcf		ban,6,0
temc	movlw	d'099'
		cpfsgt	iadc
		bra		temd
		incf	ictemr,1
		movlw	d'156'
		addwfc	iadc,1
		bra		temc
temd	movlw	d'009'
		cpfsgt	iadc
		bra		temu
		incf	idtemr,1
		movlw	d'246'
		addwf	iadc,1
		bra		temd
temu	movlw	d'000'
		cpfsgt	iadc
		bra		sum	
		incf	iutemr,1
		decf	iadc,1
		bra		temu 
sum		movlw	0x00
		cpfsgt	iadch
		bra		wconv
		movlw	0x02
		addwf	ictemr,1
		movlw	0x05
		addwf	idtemr,1
		movlw	0x06
		addwf	iutemr,1
		decf	iadch
		movlw	0x0A
		cpfslt	iutemr
		call	suni
		movlw	0x0A
		cpfslt	idtemr
		call	sdec
		bra		sum
suni	incf	idtemr,1
		clrf	iutemr,0
		retfie
sdec	incf	ictemr,1
		clrf	idtemr,0
		retfie
wconv	
;****************///compara temperatura////***
		btfss	ban,0
		bra		prog
		btfss	ban1,1
		call	on1;bsf		PORTC,1,0
		bsf		ban1,1,0
		btfsc	ban1,0
		bra		comp2
		movff	ictemp,WREG
		cpfseq	ictemr
		bra		mc
		bra		rdt
mc		movff	ictemp,WREG
		cpfslt	ictemr
		bra		ltemc
		bra		vis0
rdt		movff	idtemp,WREG
		cpfseq	idtemr
		bra		md
		bra		rut
md		movff	idtemp,WREG
		cpfslt	idtemr
		bra		ltemc
		bra		vis0		
rut		movff	iutemp,WREG
		cpfsgt	iutemr
		bra		vis0
ltemc	bsf		ban1,0,0
		movff	ictemp,ictemc
		movff	idtemp,idtemc
		movff	iutemp,iutemc
		movlw	0x05
		addwf	iutemc,1
		movlw	0x09
		cpfsgt	iutemc
		bra		comp2	
		incf	idtemc,1
		movlw	d'246'
		addwf	iutemc,1
		movlw	0x09
		cpfsgt	idtemc
		bra		comp2
		incf	ictemc,1
		clrf	idtemc,0		
;**********comparacion limite superior///**
comp2	btfsc	ban1,2
		bra		comp3
		movff	ictemc,WREG
		cpfslt	ictemr
		bra		igcon
		bra		on
igcon	movff	ictemc,WREG
		cpfseq	ictemr
		bra		off	
		movff	idtemc,WREG
		cpfslt	idtemr
		bra		igdon
		bra		on
igdon	movff	idtemc,WREG
		cpfseq	idtemr
		bra		off
		movff	iutemc,WREG
		cpfslt	iutemr
		bra		off;iguon
		bra		on
;************////limite inferior/////////*****
comp3	movff	ictemp,WREG
		cpfsgt	ictemr
		bra		igcon2
		bra		off
igcon2	movff	ictemp,WREG
		cpfseq	ictemr
		bra		on	
		movff	idtemp,WREG
		cpfsgt	idtemr
		bra		igdon2
		bra		off
igdon2	movff	idtemp,WREG
		cpfseq	idtemr
		bra		on
		movff	iutemp,WREG
		cpfsgt	iutemr
		bra		on;iguon2
		bra		off
;**********/////on/of/////*****/
off		bsf		ban1,2,0
		call 	off1
		bra		seg
off1	bcf		ban1,3,0
		clrf	contpl,0
		bcf		PORTC,0,0
		bcf		PORTC,1,0
		return
on		bcf		ban1,2,0
		btfss	ban1,3,0
		call	on1
		bra		seg
on1		bsf		ban1,3,0
		bsf		ban1,5,0
		bsf		PORTC,4,0
		bsf		PORTC,5,0	
		bsf		PORTC,0,0
		bsf		PORTC,1,0
		btfss	ban1,4
		bra		on1
		bcf		ban1,4,0
		incf	contpl,1
		movlw	0x04
		cpfsgt	contpl
		bra		on1	
		bcf		PORTC,4,0
		bcf		PORTC,5,0
		return
;****************/////////reloj///////********
seg		btfss	ban1,0
		bra		reg
		btfss	ban,1
		bra		reg
		bcf		ban,1,0
		btg		ban,5,0
		btfsc	ban,5
		bra		rj
		bsf		PORTE,0,0
		movlw	0x20
		movwf	segn
		bra		ltiem
rj		movlw	0x3A
		movwf	segn
		incf	iuseg,1
		movlw	0x09
		cpfsgt	iuseg
		bra		ltiem
		clrf	iuseg,0
		incf	idseg,1
		movlw	0x05
		cpfsgt	idseg
		bra		ltiem
		clrf	idseg,0
		incf	iumin,1
		movlw	0x09
		cpfsgt	iumin
		bra		ltiem
		clrf	iumin,0
		incf	idmin,1
		bcf		PORTC,0,0
ltiem	movff	iuhoras,WREG	
		cpfseq	puhoras
		bra		vis0
		movff	idmin,WREG	
		cpfseq	pdmin
		bra		vis0
		movff	iumin,WREG	
		cpfseq	pumin
		bra		vis0
		bra		bini
;***********//////////////vis//////**************
vis0	incf	cont1,1
		movlw	0x10
		cpfsgt	cont1
		bra		reg
		movlw	0x1F
		cpfsgt	cont1
		bra		vis
		clrf	cont1
		bra		reg		
vis		btfsc	ban,2
		bra		visr
		movlw	0x18
		movwf	dat
		call	instw
		incf	cont,1
		movlw	0x0F
		cpfsgt	cont
		bra		reg
		bsf		ban,2,0
visr	movlw	0x1C
		movwf	dat
		call	instw
		decf	cont,1
		movlw	0x01
		cpfslt	cont
		bra		reg
		clrf	cont,0
		bcf		ban,2,0
		bra		reg

;******************************************************************
RSAD	btfsc	INTCON,RBIF,0
		bra		RSRB
		bcf		PIR1,ADIF,0
		movff	ADRESL,iadc
		movff	ADRESH,iadch
		bsf		ban,6,0		;monitor de conversiones AD
		retfie					
;*******************************************************
instw 	clrf 	PORTA,0
		movff	dat,PORTD
		bsf		PORTA,1,0
		bcf		PORTA,1,0
	    return
;*******************************************************
datw 	clrf 	PORTA,0
		bsf		PORTA,3,0
		movff	dat,PORTD
		bsf		PORTA,1,0
		call	retardo
		bcf		PORTA,1,0
	    return
;*******************************************************
inlcd	movlw 	0x38
		movwf	dat,0
		call	instw
		movlw 	0x0f
		movwf	dat,0
		call	instw
		return
;*******************************************************
limpia	movlw 	0x01
		movwf	dat,0
		movff	dat,PORTD
		bsf		PORTA,1,0
		call	retardo
		bcf		PORTA,1,0
		call	retardo
	    return
;*******************************************************
ccasa	movlw 	0x03
		movwf	dat,0
		movff	dat,PORTD
		bsf		PORTA,1,0
		call	retardo
		call	retardo
		bcf		PORTA,1,0
		call	retardo
		call	retardo
	    return
;******************************************************************************
RSRB	call 	KBD   ;Llamamos rutina del teclado
  		bcf 	INTCON,RBIF,0  ;LIMPIAMOS BANDERA
  		movwf 	tecla,0   ;guardamos tecla pulsada
 	 	movlw 	0x0A
  		cpfslt 	tecla,0                               ;salta si es que se pulso una tecla entre 0 y 9
  		bra 	op
		btfss	ban,4
		bra		ttiem
		movlw 	0x00 
		cpfsgt 	selec,0
		bra		exit
		movlw 	0x01 
		cpfseq 	selec,0
		bra		dec
  		movff 	tecla,iutemp                          ;carga número de tecla pulsada en decenas
  		movlw	0x02
		movwf	selec,0
		bra 	exit
dec		movlw 	0x02 
		cpfseq 	selec,0
		bra		cen
  		movff 	iutemp,idtemp                          ;carga número de tecla pulsada en decenas
  		movff 	tecla,iutemp                          ;carga número de tecla pulsada en decenas
  		movlw	0x03
		movwf	selec,0
		bra		exit
cen		movlw 	0x03 
		cpfseq 	selec,0
		bra		exit
		movff 	idtemp,ictemp                          ;carga número de tecla pulsada en decenas
  		movff 	iutemp,idtemp                          ;carga número de tecla pulsada en decenas
  		movff 	tecla,iutemp                          ;carga número de tecla pulsada en decenas
  		movlw	0x01
		movwf	selec,0
		bra		exit
ttiem	movlw 	0x00 
		cpfsgt 	selec,0
		bra		exit
		movlw 	0x01 
		cpfseq 	selec,0
		bra		tdmin
  		movff 	tecla,pumin                          ;carga número de tecla pulsada en decenas
  		movlw	0x02
		movwf	selec,0
		bra 	exit
tdmin	movlw 	0x02 
		cpfseq 	selec,0
		bra		tuhoras
  		movff 	pumin,pdmin                          ;carga número de tecla pulsada en decenas
  		movff 	tecla,pumin                          ;carga número de tecla pulsada en decenas
  		movlw	0x03
		movwf	selec,0
		bra		exit
tuhoras	movlw 	0x03 
		cpfseq 	selec,0
		bra		exit
		movff 	pdmin,puhoras                          ;carga número de tecla pulsada en decenas
  		movff 	pumin,pdmin                          ;carga número de tecla pulsada en decenas
  		movff 	tecla,pumin                          ;carga número de tecla pulsada en decenas
  		movlw	0x01
		movwf	selec,0
		bra		exit
op	  	movlw 	0x0A 
		cpfseq 	tecla,0
 	 	bra 	op1
		movlw	0x01
		movwf	selec,0
		bcf		ban,3,0
		bcf 	ban,0,0
		btg		ban,4,0
		call	ccasa
		call	retardo
		bra		exit
op1	  	movlw 	0x0B 
		cpfseq 	tecla,0
 	 	bra 	op2
		clrf	selec,0
		bsf 	ban,3,0
		bsf		ban,0,0
		bra		exit
op2		bra		exit

exit	clrf 	PORTB,0
		retfie
;********************************************************************
KBD  	movlw 	0x0e
  		movwf 	PORTB,0   ;activamos primer renglón
  		btfss 	PORTB,4,0
  		retlw 	0x0A   ;tecla A
  		btfss   PORTB,5,0
  		retlw 	0x03   ;tecla 3
  		btfss 	PORTB,6,0          
       	retlw   0x02   ;tecla 2
  		btfss 	PORTB,7,0          
       	retlw   0x01   ;tecla 1
  		movlw 	0x0D
  		movwf 	PORTB,0   ;activamos segundo renglón
  		btfss 	PORTB,4,0
  		retlw 	0x0B   ;tecla B
  		btfss   PORTB,5,0
  		retlw 	0x06   ;tecla 6
  		btfss 	PORTB,6,0          
       	retlw   0x05   ;tecla 5
       	btfss 	PORTB,7,0          
       	retlw   0x04   ;tecla 4
       	movlw 	0x0B
  		movwf 	PORTB,0   ;activamos tercer renglón
  		btfss 	PORTB,4,0
 		retlw 	0x0C   ;tecla C
  		btfss   PORTB,5,0
  		retlw 	0x09   ;tecla 9
  		btfss 	PORTB,6,0          
        retlw   0x08   ;tecla 8
  		btfss 	PORTB,7,0          
        retlw   0x07	;TECLA 7
  		movlw 	0x07
  		movwf 	PORTB,0   ;activamos cuarto renglón
  		btfss 	PORTB,4,0
  		retlw 	0x0D   ;tecla D kuyyyy
  		btfss   PORTB,5,0
  		retlw 	0x0E   ;tecla #
  		btfss 	PORTB,6,0          
        retlw   0x00  ;tecla 0
  		btfss 	PORTB,7,0          
        retlw   0x0F   ;tecla *  
		retlw  	0xFF                                 ;ninguna tecla pulsada
;******************************************************************************
RST0	bcf		INTCON,TMR0IF,0
		;movlw 	d'194'
		movlw   0x85
		movwf	TMR0L
		bsf 	ban,1,0 ;monitor de un 500ms
		bsf 	ban1,4,0
		btg		PORTE,2,0
		retfie
;********************************************************
retardo setf 	stop,0  
nada	nop
		decfsz 	stop,F,0
		bra 	nada
		movlw	0xff
		movwf	stop,0
		return
;********************************************************

		org	0x800			;DB directiva que Define Byte             
        DB	0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A
		END