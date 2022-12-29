; Print String Data
; First two bytes of string contain X and Y char position, then the string
; Individual strings are terminated with &FE
; End of data is terminated with &FF
; HL: Address of string
;

; En la pila RET ADDRESS(2bytes), BYTE Y, BYTE X
; Otra forma:
;	   __asm
;		pop bc   ; bc = ret address
;		pop hl   ; hl = int b
;		pop de  ; de = int a
;		push bc  ; put the return address back
; Otro ejemplo:
; El puntero de pila crece hacia abajo 
; void scrollArriba (BYTE fila_inicial, BYTE columna_inicial)
;    ld     hl,$0002
;     add    hl,sp        ; skip over return address on stack
;     ld     b,(hl)       ; Columna inicial
;     inc    hl           ; La pila usa 16bit de ancho, Little Endian. Termina Word1
;     inc    hl           ; Comienza Word2
;     ld     c,(hl)       ; Fila inicial
;     ld     ixh,8        ; Contador de Scanlines
;     inc    hl


PUBLIC _print_string
PUBLIC _print_char
PUBLIC _setAttr
PUBLIC _putPixel
PUBLIC _drawLine
PUBLIC _eraseLine
PUBLIC _fill 
PUBLIC _pfill

; Fonts
; Fonts can be edited easily using the fzx font editor, it can import/export .chr files.
_reynolds:
;BINARY "../res/L01_sala_trampa_sp.scr.rcs.zx7"
;BINARY "./res/Reynolds Bold.chr"
BINARY "../res/Reynolds.chr"

; =====================================================================
; extern void print_string (BYTE x, BYTE y, unsigned char *texto);
; =====================================================================
_print_string:
            POP BC              ; BC = RET ADDRESS
            POP HL              ; HL = STRING ADDRESS
            POP DE              ; DE = E = BYTE Y. Stack width is 16bits. LITTL ENDIAN
            LD  A,E
            POP DE               ; DE = E = BYTE X
            PUSH BC             ; PUT THE RETURN ADDRESS BACK        
            LD  D, A
            ;LD E,(HL)			; E = Fetch the X coordinate
			;LD D,(HL)			; D = Fetch the Y coordinate
			CALL Get_Char_Address		; Calculate the screen address (in DE)
Print_String_0:
            LD A,(HL)			; Fetch the character to print
			INC HL				; Increase HL to the next character
            OR A
            ;CP $FE				; Compare with 0xFE
			;JR Z,Print_String		; If it is equal to 0xFE then loop back to print next string
			RET Z				; If the current character is 0 then return.
			PUSH HL				; Push HL on stack (Print_Char will not preserve HL)
			CALL Print_Char			; Print the character
			POP HL				; Retrieve HL back off the stack
			INC E				; Go to the next screen address
			JR Print_String_0		; Loop back to print next character
			RET

; =====================================================================
;extern void __CALLEE__ print_char (BYTE x, BYTE y, unsigned char texto);
; =====================================================================
 _print_char:
            POP BC              ; BC = RET ADDRESS
            POP HL              ; HL = L = CHARACTER TO BE PRINTED
            POP DE              ; DE = E = BYTE Y. Stack width is 16bits. LITTL ENDIAN
            LD  A,E
            POP DE               ; DE = E = BYTE X
            PUSH BC             ; PUT THE RETURN ADDRESS BACK        
            LD  D, A
            ;LD E,(HL)			; E = Fetch the X coordinate
			;LD D,(HL)			; D = Fetch the Y coordinate
			CALL Get_Char_Address		; Calculate the screen address (in DE)
            LD A,L			; Fetch the character to print
			CALL Print_Char			; Print the character
			RET

; Author:	Dean Belfield           
; Get char address
; D = Y character position (0-23)
; E = X character position (0-31)
; Returns address in DE
;
Get_Char_Address:	LD A,D
			AND %00000111
			RRA
			RRA
			RRA
			RRA
			OR E
			LD E,A
			LD A,D
			AND %00011000
			OR %01000000
			LD D,A
			RET				; Returns screen address in DE

; Get_Pixel_Address
; Author:	Dean Belfield
; Get screen address
; B = Y pixel position
; C = X pixel position
; Returns address in HL and pixel position within character in A
; 7 - 0 
Get_Pixel_Address:	LD A,B				; Calculate Y2,Y1,Y0
			AND %00000111			; Mask out unwanted bits
			OR %01000000			; Set base address of screen
			LD H,A				; Store in H
			LD A,B				; Calculate Y7,Y6
			RRA				; Shift to position
			RRA
			RRA
			AND %00011000			; Mask out unwanted bits
			OR H				; OR with Y2,Y1,Y0
			LD H,A				; Store in H
			LD A,B				; Calculate Y5,Y4,Y3
			RLA				; Shift to position
			RLA
			AND %11100000			; Mask out unwanted bits
			LD L,A				; Store in L
			LD A,C				; Calculate X4,X3,X2,X1,X0
			RRA				; Shift into position
			RRA
			RRA
			AND %00011111			; Mask out unwanted bits
			OR L				; OR with Y5,Y4,Y3
			LD L,A				; Store in L
			LD A,C
			AND 7
			RET

; Move HL down one pixel line
;
Pixel_Address_Down:	INC H			; Go down onto the next pixel line
			LD A,H			; Check if we have gone onto next character boundary
			AND 7
			RET NZ			; No, so skip the next bit
			LD A,L			; Go onto the next character line
			ADD A,32
			LD L,A
			RET C			; Check if we have gone onto next third of screen
			LD A,H			; Yes, so go onto next third
			SUB 8
			LD H,A
			RET

; Move HL up one pixel line
;
Pixel_Address_Up:	DEC H			; Go up onto the next pixel line
			LD A,H			; Check if we have gone onto the next character boundary
			AND 7
			CP 7
			RET NZ
			LD A,L
			SUB 32
			LD L,A
			RET C
			LD A,H
			ADD A,8
			LD H,A
			RET

; Print a single character out
; A:  Character to print
; DE: Screen address to print character at
;
Print_Char:		
            ;LD HL,$3C00			; Address of character set table in ROM
			LD HL, _reynolds-32*8
            LD B,0				; Set BC to A
			LD C,A
			AND $FF				; Clear the carry bit
			RL C				; Multiply BC by 8 (shift left 3 times)
			RL B
			RL C
			RL B
			RL C
			RL B
			ADD HL,BC			; Get the character address in HL
		    LD C,8				; Loop counter
			PUSH DE
Print_Char_1:		LD A,(HL)			; Get the byte from the ROM into A
            ;LD B,A
			;RR B
            ;OR B
            LD (DE),A			; Stick A onto the screen
			
            ;RRC A
            INC D				; Goto next line on screen
			INC HL				; Goto next byte of character
			DEC C				; Decrease the loop counter
			JR NZ,Print_Char_1		; Loop around whilst it is Not Zero (NZ)
			POP DE
			RET				; Return from the subroutine

; =====================================================================
;extern void __CALLEE__ set_attr (BYTE x, BYTE Y, BYTE attr);
; X 0 to 31 
; Y 0 to 23
; http://www.breakintoprogram.co.uk/computers/zx-spectrum/screen-memory-layout
; BYTE *attr = (BYTE*)(0x5800+x+(y*32));  // Direcci�n base
; =====================================================================

_setAttr:
            POP BC              ; BC = RET ADDRESS
            POP HL              ; HL = L = ATTRIBUTE TO BE SET 
            POP DE              ; DE = E = BYTE Y. Stack width is 16bits. LITTL ENDIAN
            LD  A,E				; A = Y
        	POP DE               ; DE = E = BYTE X
            PUSH BC             ; PUT THE RETURN ADDRESS BACK in the STACK       
			LD B,L				; Store in B the attribute value
								; Calculates the attribute address 
		    rrca                ; multiply Y by 32.
       		rrca
       		rrca
            LD 	L,A				; Stores A in L 
			and 3               ; mask bits for high byte.
       		add a,88            ; 88*256=22528, start of attributes.
       		ld h,a              ; high byte done.
       		ld a,l              ; get x*32 again.
       		and 224             ; mask low byte.
       		ld l,a              ; put in l.
       		ld a,e              ; get x displacement.
       		add a,l             ; add to low byte.
       		ld l,a              ; hl=address of attributes.
       		ld (hl),B           ; Write attribute in (HL).
			RET 

; =====================================================================
; https://github.com/breakintoprogram/lib-spectrum/tree/master/lib
; Author:	Dean Belfield
; extern void __CALLEE__ putPixel (BYTE x, BYTE y);
; Input: 
; Output: 
; Plot routine
; Author:	Dean Belfield
; B = Y pixel position
; C = X pixel position
;   Video memory Format: 010xxYYY ZZZCCCCC
;   xx: Screen Third
;   YYY: Scan Line 0-7
;   ZZZ: Row in the Screen Third 0-7 
;   CCCC: Column 0-31
_putPixel:		
			POP DE              ; DE = RET ADDRESS
            POP HL              ; HL = Y Coordinate = BYTE Y
			LD 	B,L				; B = Y pixel Position  
            POP HL              ; BYTE X. Stack width is 16bits. LITTL ENDIAN
            LD  C,L				; C = X Pixel position 
            PUSH DE             ; PUT THE RETURN ADDRESS BACK in the STACK       

			CALL Get_Pixel_Address		; Get screen address in HL, pixel position (0 to 7) in A, B =Y, C=X
			LD BC,Plot_Point		; Address of point lookup table
			ADD A,C				; Add pixel position to get entry in table
			LD C,A
			LD A,(BC)			; Get pixel data from table
			OR (HL)				; OR with screen data
			LD (HL),A			; Write back to screen
			RET
; =====================================================================
; https://github.com/breakintoprogram/lib-spectrum/tree/master/lib
; Author:	Dean Belfield
; extern void __CALLEE__ drawline (BYTE x0, BYTE y0, BYTE x1, BYTE y1);
; Input:
; Output: BC, DE are modified
; Draw Line routine
; B = Y pixel position 1 Y0
; C = X pixel position 1 X0 
; D = Y pixel position 2 Y1
; E = X pixel position 2 X1

_drawLine:		
			POP HL              ; HL = RET ADDRESS
            POP BC              ; BC = Y1 Coordinate = BYTE Y1
			LD 	D,C				; D = Y pixel Position 2 = Y2
            POP BC              ; Stack width is 16bits. LITTL ENDIAN
            LD  E,C				; E = X Pixel position 2 = X2
			POP BC 
			LD	A, C			; A = Y Pixel position 1 
			POP BC 				; C = X Pixel position 1 = X1
			LD  B,A				; B = Y Pixel position 1 = Y1

            PUSH HL             ; PUT THE RETURN ADDRESS BACK in the STACK       

			LD A,D				; Check whether we are going to be drawing up
			CP B
			JR NC,Draw_Line_1

			PUSH BC				; If we are, then this neat trick swaps BC and DE
			PUSH DE				; using the stack, forcing the line to be always
			POP BC				; drawn downwards
			POP DE

Draw_Line_1:		CALL Get_Pixel_Address		; Get screen address in HL, pixel position (0-7) in A
;
; At this point we have
;  A = Pixel position (0-7)
; HL = Screen address of the start point
; BC = Start coordinate (B=Y1, C=X1)
; DE = End coordinates  (D=Y2, E=X2)
;
			LD IX,Plot_Point		; Point to the Plot_Point table
			ADD A,IXL			; Add the pixel position to get entry in table
			LD IXL,A

			LD A,D				; Calculate the line height in B (Y2-Y1)
			SUB B
			LD B,A
	
			LD A,E				; Calculate the line width in C (X2-X1)
			SUB C
			JR C,Draw_Line_X1		; If carry set (negative result) then we are drawing from right to left
;
; This bit of code mods the main loop for drawing left to right
;
			LD C,A				; Store the line width
			LD A,0x2C			; Code for INC L
			LD (Draw_Line_Q1_M3),A		; Mod the code
			LD (Draw_Line_Q2_M3),A
			LD A,0x0A			; Code for RRC D (CB 0A)
			JR Draw_Line_X2			; Skip the next bit
;
; This bit of code mods the main loop for drawing right to left
;
Draw_Line_X1:		NEG				; The width of line is negative, so make it positive again
			LD C,A				; Store the line width
			LD A,0x2D			; Code for DEC L
			LD (Draw_Line_Q1_M3),A
			LD (Draw_Line_Q2_M3),A
			LD A,0x02			; Code for RLC D (CB 02)
;
; We've got the basic information at this point
;
Draw_Line_X2:		LD (Draw_Line_Q1_M2 + 1),A	; A contains the code for RLC D or RRC D, so make the mods
			LD (Draw_Line_Q2_M2 + 1),A
			LD D,(IX+0)			; Get the pixel data from the Point_Plot table
			LD A,B				; Check if B and C are 0
			OR C
			JR Z,Draw_Line_P		; There is no line, so just plot a single point
;
; At this point
; HL = Screen address of the start point
;  B = Line height (YL)
;  C = Line width (XL)
;  D = Pixel data
;
Draw_Line_Q:		LD A,B				; Work out which diagonal we are on
			CP C
			JR NC,Draw_Line_Q2
;
; This bit of code draws the line where B<C (more horizontal than vertical)
;
Draw_Line_Q1:		LD A,C				; A = XL
			LD (Draw_Line_Q1_M1 + 1),A	; Self-mod the code to store XL in loop
			LD C,B				; C = YL
			LD B,A				; B = XL (loop counter)
			LD E,B				; E = XL
			SRL E				; E = XL / 2 (error)
Draw_Line_Q1_L:		LD A,(HL)			; Plot the pixel
			OR D
			LD (HL),A
			LD A,E				; Add the line height to the error (E = E - YL)
			SUB C
			LD E,A
			JR NC,Draw_Line_Q1_M2
Draw_Line_Q1_M1:	ADD A,0				; Add the line width to the error (E = E + XL) - previously self-modded
			LD E,A
			CALL Pixel_Address_Down
Draw_Line_Q1_M2:	RRC D				; Rotate the pixel right or left; more self-modifying code
			JR NC,Draw_Line_Q1_S
Draw_Line_Q1_M3:	INC L				; If we get a carry then move to adjacent screen address; more self modifying code
Draw_Line_Q1_S:		DJNZ Draw_Line_Q1_L		; Loop until the line is drawn
Draw_Line_P:		LD A,(HL)			; Plot the final point
			OR D
			LD (HL),A
			RET
;
; This bit draws the line where B>=C (more vertical than horizontal, or diagonal)
;
Draw_Line_Q2:		LD (Draw_Line_Q2_M1 + 1),A	; Self-mod the code to store YL in loop
			LD E,B				; E = YL
			SRL E				; E = YL / 2 (error)
Draw_Line_Q2_L:		LD A,(HL)			; Plot the pixel
			OR D
			LD (HL),A
			LD A,E				; Add the line width to the error
			SUB C				; 
			JR NC,Draw_Line_Q2_S		; Skip the next bit if we don't get a carry
Draw_Line_Q2_M1: 	ADD A,0				; Add the line height to the error (E = E + XL) - previously self-modded
Draw_Line_Q2_M2:	RRC D				; Rotates the pixel right with carry
			JR NC,Draw_Line_Q2_S
Draw_Line_Q2_M3:	INC L				; If we get a carry then move to adjacent screen address; more self modifying code
Draw_Line_Q2_S:		LD E,A				; Store the error value back in
			CALL Pixel_Address_Down		; And also move down
			DJNZ Draw_Line_Q2_L
			JR Draw_Line_P			; Plot the final point

; -----------------------------------------------------------------------
; Erase Line routine
; Author:	Dean Belfield
; B = Y pixel position 1
; C = X pixel position 1
; D = Y pixel position 2
; E = X pixel position 2
;extern void __CALLEE__ eraseLine (BYTE x0, BYTE y0, BYTE x1, BYTE y1);

_eraseLine:
			POP HL              ; HL = RET ADDRESS
            POP BC              ; BC = Y1 Coordinate = BYTE Y1
			LD 	D,C				; D = Y pixel Position 2 = Y2
            POP BC              ; Stack width is 16bits. LITTL ENDIAN
            LD  E,C				; E = X Pixel position 2 = X2
			POP BC 
			LD	A, C			; A = Y Pixel position 1 
			POP BC 				; C = X Pixel position 1 = X1
			LD  B,A				; B = Y Pixel position 1 = Y1

            PUSH HL             ; PUT THE RETURN ADDRESS BACK in the STACK  

			LD A,D				; Check whether we are going to be drawing up
			CP B
			JR NC,Erase_Line_1

			PUSH BC				; If we are, then this neat trick swaps BC and DE
			PUSH DE				; using the stack, forcing the line to be always
			POP BC				; drawn downwards
			POP DE

Erase_Line_1:		CALL Get_Pixel_Address		; Get screen address in HL, pixel position (0-7) in A
;
; At this point we have
;  A = Pixel position (0-7)
; HL = Screen address of the start point
; BC = Start coordinate (B=Y1, C=X1)
; DE = End coordinates  (D=Y2, E=X2)
;
			LD IX,Unplot_Point		; Point to the Unplot_Point table
			ADD A,IXL			; Add the pixel position to get entry in table
			LD IXL,A

			LD A,D				; Calculate the line height in B (Y2-Y1)
			SUB B
			LD B,A
	
			LD A,E				; Calculate the line width in C (X2-X1)
			SUB C
			JR C,Erase_Line_X1		; If carry set (negative result) then we are drawing from right to left
;
; This bit of code mods the main loop for drawing left to right
;
			LD C,A				; Store the line width
			LD A,0x2C			; Code for INC L
			LD (Erase_Line_Q1_M3),A		; Mod the code
			LD (Erase_Line_Q2_M3),A
			LD A,0x0A			; Code for RRC D (CB 0A)
			JR Erase_Line_X2		; Skip the next bit
;
; This bit of code mods the main loop for drawing right to left
;
Erase_Line_X1:		NEG				; The width of line is negative, so make it positive again
			LD C,A				; Store the line width
			LD A,0x2D			; Code for DEC L
			LD (Erase_Line_Q1_M3),A
			LD (Erase_Line_Q2_M3),A
			LD A,0x02			; Code for RLC D (CB 02)
;
; We've got the basic information at this point
;
Erase_Line_X2:		LD (Erase_Line_Q1_M2 + 1),A	; A contains the code for RLC D or RRC D, so make the mods
			LD (Erase_Line_Q2_M2 + 1),A
			LD D,(IX+0)			; Get the pixel data from the Unplot_Point table
			LD A,B				; Check if B and C are 0
			OR C
			JR NZ,Erase_Line_Q		; There is a line to draw, so skip to the next bit
			LD A,(HL)			; Here we've got a single point line, so plot and return
			AND D
			LD (HL),A
			RET
;
; At this point
; HL = Screen address of the start point
;  B = Line height
;  C = Line width
;  D = Pixel data
;
Erase_Line_Q:		LD A,B				; Work out which diagonal we are on
			CP C
			JR NC,Erase_Line_Q2
;
; This bit of code draws the line where B<C (more horizontal than vertical)
;
Erase_Line_Q1:		LD A,C
			LD (Erase_Line_Q1_M1 + 1),A	; Self-mod the code again to store the line width
			LD C,B
			LD B,A
			LD E,B				; Calculate the error value
			SRL E
Erase_Line_Q1_L:	LD A,(HL)			; Unplot the pixel
			AND D
			LD (HL),A
			LD A,E
			SUB C
			LD E,A
			JR NC,Erase_Line_Q1_M2
Erase_Line_Q1_M1:	ADD A,0				; Add the line height (previously stored; self modifying code)
			LD E,A
			CALL Pixel_Address_Down
Erase_Line_Q1_M2:	RRC D				; Rotate the pixel right or left; more self-modifying code
			JR C,Erase_Line_Q1_S		; Note the change here from the Draw_Line routine
Erase_Line_Q1_M3:	INC L				; If we get no carry then move to adjacent screen address; more self modifying code
Erase_Line_Q1_S:	DJNZ Erase_Line_Q1_L		; Loop until the line is drawn
Erase_Line_P:		LD A,(HL)			; Plot the final pixel
			AND D
			LD (HL),A
			RET
;
; This bit draws the line where B>=C (more vertical than horizontal, or diagonal)
;
Erase_Line_Q2:		LD (Erase_Line_Q2_M1 + 1),A
			LD E,B				; Calculate the error value
			SRL E
Erase_Line_Q2_L:	LD A,(HL)			; Unplot the pixel
			AND D
			LD (HL),A
			LD A,E				; Get the error value
			SUB C				; Add the line length to it (X2-X1)
			JR NC,Erase_Line_Q2_S		; Skip the next bit if we don't get a carry
Erase_Line_Q2_M1: 	ADD A,0				; Add the line height (previously stored; self modifying code)
Erase_Line_Q2_M2:	RRC D				; Rotates the pixel right with carry
			JR C,Erase_Line_Q2_S		; Note the change here from the Draw_Line routine
Erase_Line_Q2_M3:	INC L				; If we get no carry then move to adjacent screen address; more self modifying code
Erase_Line_Q2_S:	LD E,A				; Store the error value back in
			CALL Pixel_Address_Down		; And also move down
			DJNZ Erase_Line_Q2_L
			JR Erase_Line_P			; Plot the final pixel

; Note that the functions above only work if each of these tables are in a byte boundary
;
Plot_Point:
	defb  %10000000,%01000000,%00100000,%00010000,%00001000,%00000100,%00000010,%00000001
Unplot_Point:		
	defb %01111111,%10111111,%11011111,%11101111,%11110111,%11111011,%11111101,%11111110

; =====================================================================
; extern void __CALLEE__ pfill (BYTE x, BYTE y, BYTE pattern); 
;
; Returns address in HL and pixel position within character in A
;   Video memory Format: 010xxYYY ZZZCCCCC
;   xx: Screen Third
;   YYY: Scan Line 0-7
;   ZZZ: Row in the Screen Third 0-7 
;   CCCC: Column 0-31
; 
; This is a simplified scan line fill patterned algorithm 

_pfill:
		POP HL              ; HL = RET ADDRESS
		LD IY, fill_patterns
		POP BC
		LD  A,C 
		ADD A, IYL 
		LD 	IYL,A	; Now IY points to the starting of the pattern 
		POP DE				; E = Y 
		POP BC              ; C = X Coordinate
		LD	B,E				; B = D = Y
		PUSH HL             ; PUT THE RETURN ADDRESS BACK in the STACK       

		LD L,255			; Fill terminator 
		PUSH HL

		CALL Get_Pixel_Address	; Get screen address in HL, pixel position (0-7) in A
		; At this point we have
		;  A = Pixel position (0-7)
		; HL = Screen address of the start point
		;		
		; BC = Start coordinate (B=Y, C=X)
		LD	D, A	; Saves bit position for later 
newline:		
		; move left until hitting a set pixel or the screen edge
		; We do not know if the first byte has set pixels at the right
		; so we mask them out. 
		LD	IX, fill_mask_right
		LD  A,D 
		ADD A,IXL
		LD  A,(IX+0)
		AND (HL)	; Read the masked screen position content 
pseekLeft: 
		OR	A		; A = 0?
		JR	NZ, pfindbitleft ; If not, go to find the left-End of line
		LD  D,7 	; We do not need the initial bit position anymore
		DEC	L 		; 1 column to the left 
		LD A,(HL)
		JR	NZ, pseekLeft ; If we do not hit the left most side 

pfindbitleft: 
							; At the table b0 is at position 7 
		LD 	IX,Plot_Point	; Point to the Plot_Point table
		LD	A,D 			; A = Bit position (0-7) b0...b7 per table
		ADD A,IXL			; Add the pixel position to get entry in table
		LD 	IXL,A
		LD	A, (IX+0)		; Retrieves the mask data from Plot_point
		AND (HL)			; AND with the screen contents -> A 
		OR	A				; A=0?
		JR  NZ, pseekRight   ; If not zero we are finished 
		DEC D 				; Next bit mask
		JR  NZ, pfindbitleft ; Iterate until a bit set is found

		; At this point 
		; HL = Screen address of the scanline filling starting point 
		;  A = Bit Mask 
		;  D = Bit position b0...b7
		; Now move to the right and set pixels per byte as we go
				 
pseekRight:
		INC	D ; The first bit is the one we are already in
pseekRight2:		
		LD	IX, fill_mask_left
		LD  A,D 
		ADD A,IXL
		LD 	IXL,A
		LD  A,(IX+0) ; We do not want to take into consideration the initial bit as it is the one were we finished
		AND (HL)	; Read the masked screen position content 
		OR	A		; A = 0?
		JR	NZ, pfindbitRight ; If not, left-End of line
		LD  IX, fill_mask_left
		CALL 	fillByte	  ; Fill the current byte 
	
	; Check if pixel below is not set	
		
		CALL Pixel_Address_Down ; Moves HL 1 row down 
		LD		A,(HL)
		xor		255
		;OR 		A		; A= 0?
		jr		z,	pseekRight_cont	; If all pixels are set ignore it
		POP		BC	; Discards the old address
		PUSH	HL  ; Push the new address to be filled
		
pseekRight_cont:		
		CALL Pixel_Address_Up ; Moves HL 1 row up 
	
	
		LD D,0 		; b0...b7, Starts a new byte 
		INC L		; 1 Column to the right, If goes from 255 to 0 then...
		JR	NZ, pseekRight2	; Hit the right most part of the screen

scan_line_finished: 
				; Fills the last byte
				; At this point 
				; HL = Screen Address in bytes 
				;  D = bit number, b0...b7 
		LD  IX, fill_mask_right
		call fillByte
		LD		D,0 ; First bit , leftmost 
		POP		HL 	; Pop the next address or 0x xx FF as terminator
		LD 		A,L 
		INC 	A
		LD 		C,255 
		PUSH	BC
		JR 		NZ, newline ; If A goes from 255 to 0 then is the end 
		POP 	BC
		ret

pfindbitRight: 							; At the table b0 is at position 7 
		LD 	IX, Plot_Point	; Point to the Plot_Point table
		LD	A,D 			; A = Bit position (0-7) b0...b7 per table
		ADD A,IXL			; Add the pixel position to get entry in table
		LD 	IXL,A
		LD	A, (IX+0)		; Retrieves the mask data from Plot_point
		AND (HL)			; AND with the screen contents -> A 
		OR	A				; A=0?
		JR  NZ, scan_line_finished   ; If not zero we are finished 
		INC D 				; Next bit mask
		JR  NZ, pfindbitRight ; Iterate until a bit set is found
; Input:
;  IY:Pointer to pattern
;  D: Bit number b0...b7
; HL: Screen address 
fillByte:
		; Set current byte 
		; Retrieve the pattern 
		;   Video memory Format: 010xxYYY ZZZCCCCC
		;   xx: Screen Third
		;   YYY: Scan Line 0-7
		;   ZZZ: Row in the Screen Third 0-7 
		;   CCCC: Column 0-31
		PUSH IY
		LD 	A, H 
		AND %0000111	; Scan Line 
		ADD A, IYL 
		LD IYL,A
		LD E,(IY+0)
		POP IY
		;LD 	E, $FF 		; Pattern 
		;LD  IX, fill_mask_left 
		LD  A,D 
		ADD A, IXL 
		LD 	IXL, A
		LD 	A, (IX+0)	; A=Mask
		AND E			; AND between the Pattern and the Mask 		
		OR	(HL)		; OR with screen content
		LD	(HL),A		; Write in screen byte
		ret

fill_mask_left:
defb  %11111111,%01111111,%00111111,%00011111,%00001111,%00000111,%00000011,%00000001,%00000000

; The bit that is checked is set to 1 
fill_mask_right:		
defb %10000000,%11000000,%11100000,%11110000,%11111000,%11111100,%11111110,%11111111

; Fill Patterns 
fill_patterns:
; Solid
defb %11111111
defb %11111111
defb %11111111
defb %11111111
defb %11111111
defb %11111111
defb %11111111
defb %11111111
; Grid 80% 
defb %01110111
defb %11111111
defb %11011101
defb %11111111
defb %01110111
defb %11111111
defb %11011101
defb %11111111

; Grid 75%
defb %11101110
defb %01010101
defb %10111011
defb %01010101
defb %11101110
defb %01010101
defb %10111011
defb %01010101

; Grid 50% 
defb %10101010
defb %01010101
defb %10101010
defb %01010101
defb %10101010
defb %01010101
defb %10101010
defb %01010101
; Grid 12% 
defb %10001000
defb %00000000
defb %00100010
defb %00000000
defb %10001000
defb %00000000
defb %00100010
defb %00000000

; Horizontal rain
defb %00000000
defb %00000011
defb %00011000
defb %11000000
defb %00000000
defb %00011000
defb %00000011
defb %01100000
