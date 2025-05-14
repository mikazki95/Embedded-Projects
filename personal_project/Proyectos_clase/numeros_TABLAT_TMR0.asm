;******************************************************************************

 LIST P=18F4550                                                ;directiva para definir el procesador
 #include <P18F4550.INC>   ;definiciones de variables especificas del procesad
;******************************************************************************
;Bits de configuración
    ;CONFIG FOSC = INTOSC_XT                                        ;Oscilador INT usado por el uC , XT usado por el  U    CONFIG BOR  = OFF    ;BROWNOUT RESET DESHABILITADO
    CONFIG PWRT = ON     ;PWR UP Timer habilitado
    CONFIG WDT  = OFF    ;Temporizador vigía apagado
    CONFIG MCLRE=OFF    ;Reset apagado
    CONFIG PBADEN=OFF
    CONFIG LVP  = OFF
;******************************************************************************
;Definiciones de variables
  	CBLOCK 0x000   ;ejemplo de definición de variables en RAM de acce  flags    ;banderas
  	flags	;banderas	
	iun    ;índice de unidades
  	cuni    ;código de 7 segmentos de unidades
  	idec    ;índice de decenas
  	cdec    ;código de 7 segmentos de decenas 
  	cont
  	ENDC    ;fin del bloque de constantes
;******************************************************************************
;Reset vector
 		ORG 	0x0000
 		bra 	inicio
 		org 	0x08   ;vector de alta prioridad
 		bra 	RST0   ;ramifica servicio interrupción T0
 		org 	0x18   ;vector de baja prioridad
		bra 	RSINT
		org 	0x0020
inicio	bcf 	OSCCON,IRCF2,0
		bsf 	OSCCON,IRCF1,0
		bcf 	OSCCON,IRCF0,0   ;Oscilador interno a 4 MHz
		movlw   0x0F
		movwf   ADCON1,0                           ;Puertos Digitales
		clrf 	PORTD,0
		clrf 	TRISD,0                                ;Puerto D Configurado como salida
		movlw 	0x00
		movwf 	TRISC,0   ;RC0 y RC1 como salidas
		clrf 	PORTC,0
		movlw   0xC7
		movwf   T0CON,0                               ;timer 16 bits prescalerX64
		movlw 	0XF0
		movwf 	INTCON,0  ;interrupciones TMR0,prioridad habilitada 
		bsf 	RCON,IPEN,0  ;habilitamos prioridades de interrupción
		movlw 	0x0B
		movwf 	TMR0L,0  ;valor de precarga para 500ms a 4MHz
		movlw   0x88
		movwf   INTCON3,0                           ;habilitamos int1 en baja prioridad
		clrf 	TBLPTRL,0
		movlw 	0x03
		movwf 	TBLPTRH,0
		clrf 	TBLPTRU,0  ;tblptr=0x000300
		clrf 	iun,0
		clrf 	idec,0   ;iniciamos en 0
lee 	movff   iun,TBLPTRL                         ;ajusta apuntador
		tblrd*   ;lee la tabla sin modificar apuntador
		movff 	TABLAT,cuni                         ;cuni tiene código 7 segmentos
		movff   idec,TBLPTRL                         ;ajusta apuntador
		tblrd*                  ;lee la tabla sin modificar apuntador
		movff 	TABLAT,cdec                        ;cdec tiene código 7 segmentos
loop 	movlw   0x02
		movwf   PORTC,0   ;encendemos display unidades
		movff 	cuni,PORTD
		call 	retardo
		movlw 	0x01
		movwf 	PORTC,0                                ;encendemos display decenas
		movff 	cdec,PORTD
		call 	retardo
		btfss 	flags,0,0
		bra 	loop
		bcf 	flags,0,0
		incf 	iun,F,0
		movf 	iun,W,0
		xorlw	0x0a
		btfss 	STATUS,Z,0                         ;verifica límite de tabla
		bra     lee
		clrf 	iun,0
		incf 	idec,F,0
		movf 	idec,W,0
		xorlw 	0x0a
		btfss 	STATUS,Z,0
		bra     lee
		clrf 	idec,0
		goto 	lee
;******************************************************************************
retardo setf 	cont,0  
nada	nop
		decfsz 	cont,F,0
		bra 	nada
		return
;******************************************************************************
RST0    bcf 	INTCON,TMR0IF,0  ;apagamos bandera timer0
		movlw 	0x0B
		movwf 	TMR0L,0   ;valor de precarga para 500ms a 4MHz
		bsf 	flags,0,0
		retfie
;********************************************************************
RSINT   bcf 	INTCON3,INT1IF,0  ;Limpiamos bandera de interrupción
		btg		flags,1,0                                                ;bit monitor de interrupción
        retfie
;********************************************************************
		org 	0x300    ;DB directiva que Define Byte
		DB 		0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x67,0x77,0x7C,0x39,0x5E,0x79,0x71
        END
