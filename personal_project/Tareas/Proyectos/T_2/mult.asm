Include <P18f4550.inc>
list P=18f4550
		cont1 		equ 0
		cont2 		equ 1
		cont3		equ 2
		resultado	equ 3

		movlw	0x02
		movwf	cont2
		movlw	0x03
		movwf	cont1
		movlw	0x04
		movwf	cont3
mult	mulwf	cont2
		movff	PROD,WREG
		mulwf	cont1
		movff	PROD,WREG
		movwf	resultado
		bra		mult	

	end