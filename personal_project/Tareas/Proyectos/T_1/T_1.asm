Include <P18f4550.inc>
list P=18f4550
		cont1 		equ 0
		cont2 		equ 1
		resultado	equ 2

		movlw	0x01
		movwf	cont2,0
		incf	cont1,1,0
suma	addwf	cont1,0 
		mulwf	cont1,1
		movwf	resultado
		bra		suma	

	end