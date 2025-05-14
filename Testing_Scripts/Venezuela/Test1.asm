;CBLOCK 0x000     ;ejemplo de definición de variables en RAM de acceso
aux_PORT0	equ 031h
aux_PORT1	equ 032h
IPH0_E		equ 033h
IPL0_E		equ 034h
;IPH0_E		equ 0B7h
;IPL0_E		equ 0B8h

;ENDC      ;fin del bloque de constantes

		org 0000h        ; Inicio del programa en la dirección 0000h
    		jmp inicio       ; Salto a la etiqueta 'inicio'

	    	org 0003h        ; Vector de interrupción para INT0
	    	jmp ISR_INT0     ; Salto a la rutina de servicio de interrupción para INT0
	
	    	org 0013h        ; Vector de interrupción para INT1
	    	jmp ISR_INT1     ; Salto a la rutina de servicio de interrupción para INT1
;//////////////////////////////////////////////////////////////////////
inicio:
		mov ie,#133d
		mov tcon,#5d
		mov IPH0_E,#0d
		mov IPL0_E,#0d
		mov Aux_port0,#0A5h
		mov Aux_port1,#05Ah
		setb P3.6
		setb P3.7
		setb P2.2
		setb P2.3
inicio2:
		setb 	P3.6
		setb 	P3.7
		setb 	P2.2
		setb 	P2.3
		mov 	P0,#0FFh
		setb 	P2.4
		setb 	P2.5
		clr 	P3.5
		nop
		nop
		nop
		nop
		nop
		clr	P2.4
		clr	P2.5
		setb 	P3.5
		jmp	inicio2
;//////////////////////////////////////////////////////////////////////
; Rutina de servicio de interrupción para INT0
ISR_INT0:
		jnb 	P3.4, state0_1  ; salta si es cero
		jnb 	P3.1, state0_0  ; salta si es cero
		reti
state0_0:
		setb 	P3.7
		clr	P3.6
		setb 	P2.2
		clr 	P2.3
		jmp ISR_INT0
state0_1:
    	; Código a ejecutar cuando ocurre la interrupción INT0
		jb 	P3.1, state0_2  ; Salta si es 1
    		reti             ; Retorno de la interrupción
state0_2:
		mov 	AUX_PORT0,P0
		jmp 	ISR_INT0
;///////////////////////////////////////////////////////////////////
; Rutina de servicio de interrupción para INT1
ISR_INT1:
    ; Código a ejecutar cuando ocurre la interrupción INT1
		jnb 	P3.4, state1_1  ; STATE_2
		jnb 	P3.1, state1_0  ; STATE_3
		mov 	AUX_PORT1,P1
		mov 	P0,AUX_PORT1
		jmp	ISR_INT1
		;reti
state1_0:
		clr 	P3.7
		setb	P3.6
		setb 	P2.2
		clr 	P2.3
		jmp ISR_INT1
state1_1:
    	; Código a ejecutar cuando ocurre la interrupción INT0
		jb 	P3.1, state1_2  ; Salta a "etiqueta_si_es_1" si P3.2 es 1
    		reti             ; Retorno de la interrupción
state1_2:
		mov 	P0,AUX_PORT0
		jmp ISR_INT1


		end