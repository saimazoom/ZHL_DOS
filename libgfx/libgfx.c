
#if defined(C64) || defined(CPC)
     #include "./libgfx.h"
#else 
     #include "./libgfx/libgfx.h"
#endif 

#ifdef DOS 
    #include <conio.h>
    #include <graph.h>
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include <i86.h>
#endif 

// Windows declaration 
textwin_t TextWindow;
textwin_t GraphWindow;

struct fzx_state fzx;   // active fzx state

#ifdef ZX 

     void setRAMPage (BYTE banco)
     {     
     #asm
     ;-----------------------------------------------------------------------
     ; SetRAMPage: Establece un banco de memoria sobre $c000
     ; Entrada: B = banco (0-7) a paginar entre $c000-$ffff
     ;-----------------------------------------------------------------------
     ; Extraído del Manual de Usuario de Spectrum, Página 221
     ;
          ld     hl, $0002
          add    hl, sp
          ld     a, (hl) ; A= Banco
          ld     b, a
          ld     A, ($5B5C)
          and    $F8
          or     B
          ld     BC, $7FFD
          ld     ($5B5C), A
          out    (C), A
     ;     ret
     #endasm
     
     }

     void setRAMBack()
     {
     
     #asm
     ld      A, ($5B5C)
     and     $F8
     ld      BC, $7FFD
     di
     ld      ($5B5C), A
     out     (C), A
     ei
     ;ld      a,$0F0
     ;ld i,a
     ;ei
     #endasm
     
     }

     // Formato de memoria de v�deo: 010xxYYY ZZZCCCCC
     // xx: Tercio
     // YYY: Scan Line 0-7
     // ZZZ: Caracter 0-7 Fila
     // CCCC: Columna 0-31
     /*
     Tercio 0
     Tercio 1 18432 0x4800
     Tercio 2
     */
     void scrollArriba (BYTE fila_inicial, BYTE columna_inicial)
     {
     // ScrollVertical de 1 fila partiendo de una fila inicial
     // Fila: 0 a 24 -> 0000 0000 a 0000 1100
     //                 0000 0000 a 0000 0111
     //                 0000 1000 a 0000 1111
     //                 0001 0000 a 0001 0111
     #asm
          ld     hl,$0002
          add    hl,sp        ; skip over return address on stack
          ld     b,(hl)       ; Columna inicial
          inc    hl           ; La pila usa 16bit de ancho, Little Endian. Termina Word1
          inc    hl           ; Comienza Word2
          ld     c,(hl)       ; Fila inicial
          ld     ixh,8        ; Contador de Scanlines
          inc    hl
     SA1start:
          push   bc           ; B=Columa inicial, C=Fila actual
          ld     hl, 16384    ; Origen de pantalla Y=0, HL = 0x4000
          ld     a,c          ; A = fila inicial
          and    $18          ; Estos dos bit indican el tercio
          or     h
          ld     h,a          ; H Apunta al tercio correcto, Scanline =0
          ld     a,c
          and    $07          ; Fila 0-7
          rlca
          rlca
          rlca
          rlca
          rlca
          or     b             ;Bits 0-4 Columna inicial
          ld     l,a          ; Ahora HL apunta a la direcci�n de la fila de destino

          ld     de, 16384    ; DE debe apuntar a la fila de origen (1 fila por debajo de HL)
          ld     a,c
          inc    a
          and    $18          ; Estos dos bit indican el tercio
          or     d
          ld     d,a          ; H Apunta al tercio correcto, Scanline =0
          ld     a,c
          inc    a            ; Fila A = Fila HL+1
          and    $07          ; Fila 0-7
          rlca
          rlca
          rlca
          rlca
          rlca
          or     b             ; Bits 0-5 Columna inicial
          ld     e,a          ; Ahora DE apunta a la direcci�n de la fila de origen

          push   de
          push   hl

          ld     a,32
          sub    b
          ld     b,a
          ld     ixl,b        ; N�mero de columnas  acopiar por scanline
     SA1loops:                ; Bucle por scanline...
          ld     b,ixl

     SA1loop:                ; Bucle por columna
          ld     a,(de)
          ld     (hl),a      ; Escribe el contenido de la direcci�n DE en la direcci�n apuntada por HL
          ld     a,c
          cp     23           ; Si estamos en la �ltima linea de origen rellena con 0
          jp     nz, notfillzero
          ld     (hl),0
     notfillzero:
          inc     de
          inc     hl
          djnz    SA1loop
                              ; Termina de copiar todas las columnas
          pop    hl
          pop    de
          ld     b,ixh
          djnz   nextscan
          jp     nextrow
     nextscan:
          ld     ixh,b
          inc    h
          inc    d           ; Incrementa DE=DE+256, siguiente scanline
          push   de
          push   hl
          jp     SA1loops
     nextrow:
          pop     bc           ; B=Columa inicial, C=Fila
          ld      ixh,8
          ld      a,c
          inc     a           ;
          ld      c,a         ; Almacena Siguiente fila de origen
          cp      24          ; La �ltima fila de origen es la 23
          jp      nz,SA1start
                              ; Limpia la �ltima fila
     #endasm
     }

     void drawRectangle (BYTE xorig, BYTE yorig, BYTE width, BYTE height)
     {

     }
     // Formato de memoria de v�deo: 010xxYYY ZZZCCCCC
     // xx: Tercio
     // YYY: Scan Line 0-7
     // ZZZ: Caracter 0-7 Fila
     // CCCC: Columna 0-31

     // 29.12.2019: La rutina no funciona correctamente y se queda bloqueada. 
     void clsScreen (BYTE effect)
     {
          /*
          switch (effect)
          {
               case 0: // Persiana arriba-abajo
          #asm
                    ld  	hl, $4000      ; Inicio de la memoria de video
                    ld 		b, 	32          ; 32bytes por l�nea
                    ld      c,  $00        ; Contador de fila de caracteres
                    ld      d,  4           ; 4 l�neas por caracter (alternas)
                    ld      e,  0           ; Contador de tercios..
     inicio:     ld 		(hl), $FF		; Pone el Caracter actual a todo 1
               inc 	l               ; Avanza 1 caracter a la derecha...
               dec		b			    ; Decrementa el contador
               jr		nz, inicio      ; 32 veces...
               inc		h
               inc     h               ; Salta 2 scanlines
               ld      l,  0           ; Vuelve el contador de columnas a 0
               ld      a,  l
               add     a,  c           ; A�ade la fila
               ld      l,  a
               ld 		b, 	32          ; 32 caracteres para la siguiente fila...
               dec     d               ; Contador de l�neas del caracter...
               halt
               jr      nz, inicio      ; Se termina la fila de caracteres actual...
               ld      a,  e
               ld      h,  $40
               add     a,  h
               ld      h,  a
               ld      d,  4
               ld      a,  $20        ; Para sumar 1 al contador de filas...
               add     a,  c           ; Suma 1 al contador de filas...
               ld      c,  a
               ld      l,  a           ; La parte baja de HL contiene ZZZ00000
               jr      nc, inicio      ; Fin de Tercio

               ld      c,  $00
               ld      l,  0
               ld      b,  32

               ld      a,  e
               add     a,  8         ; Incrementa el contador de tercios
               ld      e,  a
               ld      h,  $40        ; Reinicia el contador de scanlines...
               add     a,  h           ; H+E
               ld      h,  a
               cp      $58
               jr      nz, inicio
          #endasm

               break;
          case 1: // Persiana abajo-arriba
          #asm
                    ld  	hl, 22496       ; Inicio de la pen�ltima l�nea de la memoria de video
                    ld 		b, 	32          ; 32bytes por l�nea
                    ld      c,  $E0        ; Contador de fila de caracteres
                    ld      d,  4           ; 4 l�neas por caracter (alternas)
                    ld      e,  $10        ; Contador de tercios..., Tercer Tercio
     inicio2:
               ld 		(hl), $FF		; Pone el Caracter actual a todo 1
                    inc 	l               ; Avanza 1 caracter a la derecha...
                    dec		b			    ; Decrementa el contador
                    jr		nz, inicio2     ; 32 veces...
                    dec		h
                    dec     h               ; Salta 2 scanlines
                    ld      l,  0           ; Vuelve el contador de columnas a 0
                    ld      a,  l
                    add     a,  c           ; A�ade la fila de caracteres
                    ld      l,  a
                    ld 		b, 	32          ; 32 caracteres para la siguiente fila...
                    dec     d               ; Contador de l�neas del caracter...
                    halt
                    jr      nz, inicio2      ; Se termina la fila de caracteres actual...

               ld      a,  e
               ld      h,  $47        ; Pen�ltimo Scanline...
               add     a,  h
               ld      h,  a           ; H = 0x46 + E
               ld      d,  4

               ld      a,  c            ; Para sumar 1 al contador de filas...
               sub     a,  $20         ; Resta 1 al contador de filas...
               ld      c,  a
               ld      l,  a           ; La parte baja de HL contiene ZZZ00000
               jr      nc, inicio2      ; Fin de Tercio

               ld      c,  $E0
               ld      l,  c
               ld      b,  32

               ld      a,  e
               sub     a,  8         ; Decrementa el contador de tercios
               jr      c,  fin         ; Si sale un n�mero < 0
               ld      e,  a
               ld      h,  $47        ; Reinicia el contador de scanlines...
               add     a,  h           ; H+E
               ld      h,  a
               jr      inicio2
     fin:    nop
          #endasm
          break;
          }
          */
     }

     void drawGFX (BYTE *pointer, BYTE xorig, BYTE yorig)
     {
     // Vuelca la pantalla
     // Vuelve al banco 0
     }

     void drawSprite (BYTE *pointer, BYTE xorig, BYTE yorig, BYTE width, BYTE height)
     {

     }

     void waitForAnyKey()
     {
     #asm
     Wait_For_Keys_Pressed:
          XOR A                        ; A = 0
          IN A, (254)
          OR 224
          INC A
          JR Z, Wait_For_Keys_Pressed
     #endasm
     }

     void waitForNoKey()
     {
     #asm
     Wait_For_Keys_NotPressed:
          XOR A                        ; A = 0
          IN A, (254)
          OR 224
          INC A
          JR NZ, Wait_For_Keys_NotPressed
     #endasm
     }

     // Produces a random number between 0-65535
     WORD randomNumber()
     {
          /*
     #asm
          ; http://wikiti.brandonw.net/index.php?title=Z80_Routines:Math:Random
          ; Xorshift is a class of pseudorandom number generators discover by George Marsaglia and detailed in his 2003 paper, Xorshift RNGs. 
          ; 16-bit xorshift pseudorandom number generator by John Metcalf
          ; 20 bytes, 86 cycles (excluding ret)
          ; returns   hl = pseudorandom number
          ; corrupts   a
          ; generates 16-bit pseudorandom numbers with a period of 65535
          ; using the xorshift method:
          ; hl ^= hl << 7
          ; hl ^= hl >> 9
          ; hl ^= hl << 8
          ; some alternative shift triplets which also perform well are:
          ; 6, 7, 13; 7, 9, 13; 9, 7, 13.

     xrnd:
          ld hl,1       ; seed must not be 0
          ld a,h
          rra
          ld a,l
          rra
          xor h
          ld h,a
          ld a,l
          rra
          ld a,h
          rra
          xor l
          ld l,a
          xor h
          ld h,a

          ld (xrnd+1),hl
          RET
     #endasm
     */
     }

     unsigned char getKey()
     {
     //    http://www.animatez.co.uk/computers/zx-spectrum/keyboard/
     //    https://chuntey.wordpress.com/2012/12/19/how-to-write-zx-spectrum-games-chapter-2/
     //    Teclas especiales: Borrar SHIFT+0, se devuelve como 0
     //                       ALTGR + O como ;

     #asm
     Read_Keyboard:      LD HL,Keyboard_Map      ; Point HL at the keyboard list
                         LD D,8                  ; This is the number of ports (rows) to check
                         LD C,0xFE                ; C is always FEh for reading keyboard ports
                         LD IXL,0
     Read_Keyboard_0:    LD B,(HL)               ; Get the keyboard port address from table
                         INC HL                  ; Increment to list of keys
                         IN A,(C)                ; Read the row of keys in
                         AND 0x1F                 ; We are only interested in the first five bits
                         LD E,5                  ; This is the number of keys in the row
     Read_Keyboard_1:    SRL A                   ; Shift A right; bit 0 sets carry bit
                         JR  C,nokey             ; If the bit is 1, we have not found our key
                         LD  B,A
                         LD  A,(HL)
                         CP '#'                   ; No retorna con la tecla SHIFT
                         JR  NZ,  Read_Keyboard_2 ; Salimos con el c�digo de la tecla
                         LD  IXL, 4              ; C�digo SHIFT
                         LD  A,B
     nokey:              INC HL                  ; Go to next table address
                         DEC E                   ; Decrement key loop counter
                         JR  NZ,Read_Keyboard_1   ; Loop around until this row finished
                         DEC D                   ; Decrement row loop counter
                         JR  NZ,Read_Keyboard_0   ; Loop around until we are done
                         AND A                   ; Clear A (no key found)
                         JR  Z, Read_Keyboard
     Read_Keyboard_2:
                         LD  A,(HL)               ; We have found a key at this point; fetch the character code!
                         CP '0'                   ; Comprueba si est� borrando...
                         JR  NZ, Return_Keyboard
                         LD  A, IXL
                         CP  A, 4                 ; C�digo SHIFT
                         JR  Z, Return_Keyboard
                         LD  A, 4
     Return_Keyboard:    LD  H,0
                         LD  L,A
                         RET

     Keyboard_Map:       DEFB 0xFE,'#','z','x','c','v'
                         DEFB 0xFD,'a','s','d','f','g'
                         DEFB 0xFB,'q','w','e','r','t'
                         DEFB 0xF7,'1','2','3','4','5'
                         DEFB 0xEF,'0','9','8','7','6'
                         DEFB 0xDF,'p','o','i','u','y'
                         DEFB 0xBF,13,'l','k','j','h'
                         DEFB 0x7F,' ',';','m','n','b'
     #endasm
     }

     void clearchar (BYTE x, BYTE y, BYTE color)
     {
     // http://www.animatez.co.uk/computers/zx-spectrum/screen-memory-layout/
     // Input: x 0 to 31 0000 0000 to 0001 1111
     //        y 0 to 23 0000 0000 to 0001 0111
     // Formato de memoria de v�deo: 010xxYYY ZZZCCCCC
     // xx: Tercio
     // YYY: Scan Line 0-7
     // ZZZ: Fila 0-7 del tercio
     // CCCC: Columna 0-31

     // Tercio 1 0-7 -> 0x00 a 0x07 -> 0100 0000 a 0000 0111
     // Tercio 2 8-15-> 0x08 a 0x0F -> 0100 1000 a 0000 1111
     // Tercio 3 16-23->0x10 a 0x17 -> 0101 0000 a 0001 1111

     print_char (x,y,' ');
     setAttr (x,y,color);
     }

     void clearScreen (BYTE color)
     {
     // Pasar esto a asm...
          memset(16384, 0, 6144);    // Pixels...
          memset(22528, color, 768); // Attributes
          // The quickest and simplest way to set the border colour is to write to port 254. The 3 least significant bits of the byte we send determine the colour.
          #asm
          ;; Set border colour to black
               ld a,0x00
               out ($fe),a
          #endasm
     }
#endif // end of ZX Spectrum graphic functions 

void fzx_setcolor (BYTE color) 
{
     fzx.color = color;
}

void fzx_setat(unsigned char x, unsigned char y)
{
     fzx.x = x;
     fzx.y = y;
     #ifdef DOS 
          #ifdef TEXT 
               _settextposition (y+1,x+1);
          #endif
          #if defined CGA || defined EGA || defined VGA || defined SVGA
               
               _settextposition (y+1-TextWindow.y,x+1-TextWindow.x); // Uses as a reference the text window 
               
               _moveto( x*8, y*8 ); // Uses as a reference the whole screen
          #endif 
     #endif
}

void fzx_putc(unsigned char c)
{
    print_char (fzx.x, fzx.y, c, fzx.color);
}

void fzx_puts(char *s)
{
    print_string (fzx.x, fzx.y, s, fzx.color);
}


void paint_pic (unsigned char *bytestring)
{
/* 
	https://wiki.scummvm.org/index.php/AGI/Specifications/Pic#General_actions

	0xF0: Change picture color and enable picture draw.
    0xF1: Disable picture draw.
    0xF2: Change priority color and enable priority draw.
    0xF3: Disable priority draw.
    0xF4: Draw a Y corner.
    0xF5: Draw an X corner.
    0xF6: Absolute line (long lines).
    0xF7: Relative line (short lines).
    0xF8: Fill.
    0xF9: Change pen size and style.
    0xFA: Plot with pen.
    0xFB--0xFE: Unused in most AGI games.
*/
     // (0,0) is top left corner
     // Code from veced.html
     BYTE cmd;
	BYTE args=0;
	BYTE arg_array[4];
	BYTE arg_counter=0;
     BYTE array_counter=0;
	BYTE x0,y0,x1,y1;

     while (*bytestring!=255)
     {
          // Byte values over 0xF0 once the arguments are retrieved, belongs to commands 
		if (*bytestring>0xF0 && arg_counter==0)
		{
               // 0xFF: End-of-pic 
			if (*bytestring==0xFF) 
			{
				return; 
			}

			// Commands 
			// Higher than 0xF0 is reserved for commands and End-of-pic (0xFF)
			// 0xF6: Absolute line (long lines) X0 Y0 X1 Y1 X2 Y2...
			if (*bytestring==0xF6)
			{
               	x0=0;
				y0=0;
				x1=1;
				y1=1;
				cmd = 0xF6;
				args = 4; // Y0,X0 Y1,X1
				//arg_array[]=""
                    array_counter=0;
				arg_counter=0;		
                    // Advance to next iteration
                    bytestring++;
                    continue;                    
			}
			// 0xF7: Relative Line X1|Y1
			if (*bytestring==0xF7)
			{
				cmd = 0xF7;
				args = 1; // Both coordinates fits in 1byte 
				//arg_array = [] 
                    array_counter=0;
				arg_counter = 0; 
				arg_array[array_counter++]=y0;
				arg_array[array_counter++]=x0;	
                    bytestring++;
                    continue;			
			}
			// 0xFA: PLOT X0 Y0
			if (*bytestring==0xFA) 
			{
				x0=0;
				y0=0;
				cmd = 0xFA;
				args = 2; // X0,Y0 
				//arg_array = [];
                    array_counter=0;
				arg_counter=0;
                    bytestring++;
                    continue;				
			}
               // 0xF8: FILL X0 Y0 Pattern
               if (*bytestring==0xF8) 
			{
				x0=0;
				y0=0;
				cmd = 0xF8;
				args = 3; // X0,Y0, Pattern 
				//arg_array = [];
                    array_counter=0;
				arg_counter=0;
                    bytestring++;
                    continue;				
			}               
		}

		// Parameters 
		if (arg_counter<args)
		{
			// Post processing
			// RLINE 
			if (cmd == 0xF7)
			{                    
				y1 = (*bytestring & 0x0F);
				// Negative numbers -> 0000 1000
                    if (y1>7) arg_array[array_counter++]=y0-(y1&0x07);
                         else arg_array[array_counter++]=y0+y1;
				x1 = (*bytestring>>4) & 0x0F;
		          if (x1>7) arg_array[array_counter++]=x0-(x1&0x07);
                         else arg_array[array_counter++]=x0+x1;
                    arg_counter++;
              	}
			else 
               {
				arg_array[array_counter++]=*bytestring;
               }
			arg_counter++;
		}
				
		// All arguments have been retrieved
		// END of RLINE 
		// END of LINE
		if ((cmd==0xF6 || cmd==0xF7) && arg_counter>=args)
		{			
			drawLine (arg_array[1], arg_array[0], arg_array[3], arg_array[2]);
			// Line works as a string of coordinates
			x0 = arg_array[3];
			y0 = arg_array[2];
			//arg_array=[]
               arg_counter=0;
               array_counter=0;
			arg_array[array_counter++]=y0;
			arg_array[array_counter++]=x0;
			args=2;
     	}

		// END of PLOT 
		if (cmd==0xFA && arg_counter>=args)
		{
			x0 = arg_array[1];
			y0 = arg_array[0];
			putPixel (x0, y0);
			//arg_array.push(bytestring[i]) // X0
			//arg_array=[]
               array_counter=0;
			args=2;
			arg_counter=0;
		}
          // END of FILL 
	     if (cmd==0xF8 && arg_counter>=args)
		{
			x0 = arg_array[1];
			y0 = arg_array[0];
			pfill (x0, y0,0);
               array_counter=0;
			args=3;
			arg_counter=0;
		}

       bytestring++;
	}
}

#ifdef DOS
     void waitForAnyKey() 
     {
          while (!kbhit());
          getch(); // Extrae la tecla del buffer 
     }
     void waitForNoKey () 
     {
          while (kbhit());
     
     }

     unsigned char getKey ()
     {
          unsigned char keycode;
          keycode = getch();

          while (keycode==0 || keycode==224 || keycode==239 || keycode == 96 || keycode == 249)
          {
               getch (); // Throws away the next char 
               keycode = getch();
          }
          // Always return lower case 

          return tolower (keycode);
     }
     
     // Function: TextMode
     // Input: None
     // Output: Set the mode in text 80x25 in CGA/EGA/VGA modes. 
     void TextMode () 
     {
     
          union REGS regs;

          // set text mode 80x25 (CGA, EGA, VGA)
          _setvideomode ( _TEXTC80);
          
          // Disable CGA blinking http://www.techhelpmanual.com/87-screen_attributes.html
          // On MDA and CGA, you can turn off blinking by clearing bit 5 of the Mode Select Register (0x03D8).  
          // https://www.seasip.info/VintagePC/cga.html
          outp (0x03D8, inp(0x03D8)&0xDF);

          // Disable EGA/VGA  blinking in http://www.techhelpmanual.com/113-int_10h__video_services.html 
          regs.h.ah = 0x10; // Set/Get Palette Registers Service
          regs.h.al = 0x03; // toggle intensity/blinking 
          regs.w.bx = 0x00; // 0-Enable intensity, 1-Enable blinking, by default we choose intensity 
          int386( 0x10, &regs, &regs );
          _wrapon(_GWRAPOFF);     
     
     }

     // 0: Palette 0 
     // 1: Palette 1
     // 2: Palette 0 High Brightness
     // 3: Palette 0 Low Brightness

     void setCGAPalette (BYTE pal)
     {
          union REGS regs;
          #ifdef CGA

               regs.h.bh = 0x01; // BH set to 01 set palette 
               regs.h.bl = pal & 0x1;  // BL palette ID ; (cyan, magenta, white) 
               regs.w.ax = 0x0B00;	// Interrupt service 
               int386( 0x10, &regs, &regs );
                    
               // Set to Low brightness manually, because Watcom set palette to 0 or 1 is not working. 
               regs.w.cx=0;
               regs.w.dx=0;
               regs.h.bh = 0x00;  //  BH Set to 00 background / intensity
               // We assume color 0 is Black 
               if (pal>1)               // High intensity palette
                    regs.h.bl = 0x10;  // -> BL Black background, low intensity.  The low four bits of BL give the background colour, and bit 4 gives the foreground intensity.
               else 
                    regs.h.bl=0x00;     // Low intensity palette
               regs.w.ax = 0x0B00;	// Interrupt service 
               int386( 0x10, &regs, &regs );

          #endif 
     }
     // Function: HighResMode
     // Input: None
     // Output: Set the mode in graphics. CGA 320x200 4 colors, EGA 320x200 16 colors
     void HighResMode ()
     {
          union REGS regs;
          #ifdef CGA 
               _setvideomode ( _MRES4COLOR);

              	// https://en.wikipedia.org/wiki/INT_10H
			// PC XT technical reference, page A-53.
               // https://www.seasip.info/VintagePC/cga.html
			// Select CGA Mode 4 320x200 4 colors
               // A note on alternative palette mode 5 (https://www.vogons.org/viewtopic.php?t=90079): 
               // EGA and VGA do not support the CGA mode 5 palette. IBM could have initialized the EGA palette in mode 5 to match the CGA mode 5 palette, but they chose to not do so. Mode 5 was intended as grayscale mode on the composite output, a feature that was dropped on the EGA.
               // A lot of clone EGA and VGA card provide a CGA emulation mode that does support the mode 5 palette though.
                  
          	//regs.w.ax=0x04; // set CGA 320x200 MODE 04
    		     //int386( 0x10, &regs, &regs ); // Set the mode manually to avoid issues with watcom setvideomode 
               setmodeCGA();                
          #endif 

          #ifdef EGA
               _setvideomode ( _MRES16COLOR);               
          #endif 

          #ifdef VGA
               _setvideomode (_VRES16COLOR); // 640x480 16colors
          #endif 

          #ifdef SVGA 
               _setvideomode (_VRES256COLOR); // 640x480 256colors
          #endif
     }
     // Function: clearScreen 
     // Description: Clears the complete screen in both text and graphic modes. 
     // Input: Color in the form PAPER (4bit) | INK (4bit)
     // Usage: clearScreen (PAPER_BLUE|INK_WHITE)
     void clearScreen (BYTE color)
     {
          long colors[ 16 ] = {
               _BLACK, _BLUE, _GREEN, _CYAN,
               _RED, _MAGENTA, _BROWN, _WHITE,
               _GRAY, _LIGHTBLUE, _LIGHTGREEN, _LIGHTCYAN,
               _LIGHTRED, _LIGHTMAGENTA, _YELLOW, _BRIGHTWHITE
          };

          #ifdef TEXT 
               // Separates the byte color 
               _setbkcolor (colors[(color&0xF0)>>4]);
               _settextcolor (color&0x0F);
               _clearscreen( _GWINDOW ); // The screen is defined by the text window 
          #endif 

          #if defined CGA || defined EGA || defined VGA || defined SVGA 
               _setbkcolor (colors[(color&0xF0)>>4]);
               _settextcolor (color&0x0F);
               _clearscreen( _GCLEARSCREEN );          
          #endif 
     }

     void clearchar (BYTE x, BYTE y, BYTE color)
     {
          #ifdef TEXT                
          #endif

          #ifdef CGA 
          #endif 

          #ifdef EGA
          #endif
     }

     void drawLine (BYTE x0, BYTE y0, BYTE x1, BYTE y1)
     {
          #ifdef CGA 
          #endif 

          #ifdef EGA
          #endif
     }

     void eraseLine (BYTE x0, BYTE y0, BYTE x1, BYTE y1)
     {
          #ifdef CGA 
          #endif 

          #ifdef EGA
          #endif

     }

     void fill (BYTE x, BYTE y)
     {
          #ifdef CGA 
          #endif 

          #ifdef EGA
          #endif
     }

     // Function: print_string
     // Input: x 
     //        y 
     //        texto
     //        color 
     // Output:
     // Usage:

     void print_string (BYTE x, BYTE y, unsigned char *texto, BYTE color)
     {

          int i, n;
          BYTE *CGA_VRAM=(BYTE*)0xB8000L;
          // Note: watcom printing library uses (1,1) as top, left coordinate Y,X. We need to translate that to MiniF system which uses 0,0 for left, top coordinates. 
          fzx_setat(x, y);
          #ifdef TEXT
               // _outtext (texto); // Watcom outtext prints in different color chars > 127 
               // In Text Mode, First byte is the color, Second byte is the char, 
               CGA_VRAM += fzx.y*160+fzx.x*2;
               for (i=0;i<strlen(texto);i++)
                    {
                    *CGA_VRAM = texto[i]; // Fill the 2nd byte with the char 
                    CGA_VRAM++;
                    *CGA_VRAM = fzx.color; // Fill the 1st byte with the attribute 
                    CGA_VRAM++;
                    }
          #endif 
          #if defined CGA || defined EGA || defined VGA || defined SVGA
               _outgtext (texto);
          #endif 

     }

     void print_char (BYTE x, BYTE y, unsigned char texto, BYTE color)
     {          
          BYTE *CGA_VRAM=(BYTE*)0xB8000L;

          // Note: watcom printing library uses (1,1) as top, left coordinate Y,X. We need to translate that to MiniF system which uses 0,0 for left, top coordinates. 
          fzx_setat(x, y);
      
          #ifdef TEXT
               if (texto>31 && texto<127) // Filters unwanted control codes
               {
               // _outtext (&texto);  // Watcom outtext has a bug that prints in different color chars > 127 
               // We print the char manually in text mode 
               CGA_VRAM += fzx.y*160+fzx.x*2;
               *CGA_VRAM = texto; // Fill the 2nd byte with the char 
               CGA_VRAM++;
               *CGA_VRAM = fzx.color; // Fill the 1st byte with the attribute 
               CGA_VRAM++;
               // Set cursor position
               fzx_setat(x+1, y);
               }
          #endif  
     
          #if defined CGA || defined EGA || defined VGA || defined SVGA
               if (texto==' ') { 
                    _outtext (" "); // Esta función sobrescribe con el color de fondo cuando se usa espacio
                    }
                    else _outgtext (&texto); // Esta función imprime con OR en pantalla, es decir un espacio no se imprime. 
          #endif
     }

     // Function: setAttr
     // Description: Changes the background(PAPER) and foreground(INK) color of a text cell. The text in the cell is preserved. 
     // Input: 
     //        x:
     //        y:
     //        color: 
     // Output:
     // Usage: 
     void setAttr (BYTE x, BYTE y, BYTE attr)
     {
          // Note: watcom printing library uses (1,1) as top, left coordinate Y,X. We need to translate that to MiniF system which uses 0,0 for left, top coordinates. 
          
          long colors[ 16 ] = {
               _BLACK, _BLUE, _GREEN, _CYAN,
               _RED, _MAGENTA, _BROWN, _WHITE,
               _GRAY, _LIGHTBLUE, _LIGHTGREEN, _LIGHTCYAN,
               _LIGHTRED, _LIGHTMAGENTA, _YELLOW, _BRIGHTWHITE
          };

          #ifdef TEXT 
               _settextposition (y+1,x+1);
               // Separates the byte color 
               _setbkcolor (colors[(attr&0xF0)>>4]);
               _settextcolor ((short)attr&0x0F);
               _clearscreen( _GWINDOW );
          #endif 
     }

     // Function: scrollArriba 
     // Input:
     // Output:
     // Usage: 
     void scrollArriba (BYTE fila_inicial, BYTE columna_inicial)
     {
          _scrolltextwindow (_GSCROLLUP);
     }

     // Function:
     // Input:
     // Output:
     // Usage:

    void loadPCX (unsigned char *filename, unsigned long offset)
    {
          // Info https://www.fileformat.info/format/pcx/egff.htm
          // https://qbmikehawk.neocities.org/docs/zsoft_pc_paintbrush.txt

          FILE *fp;
          PCX_HEADER header;
          BYTE n, count, data;
          WORD x,y;
          BYTE *bitmap; 
          BYTE *VGA_VRAM=(BYTE*)0xA0000L;
          BYTE *EGA_VRAM=(BYTE*)0xA0000L;
          BYTE *CGA_VRAM=(BYTE*)0xB8000L;

          //printf ("%s \n", filename);
          fp = fopen(filename, "rb");
          if (!fp) {
               printf ("Error Cannot open %s", filename);
               return ;
          }
          fseek (fp, offset, SEEK_SET);

          // Header is 128bytes
          //printf ("%u", sizeof (PCX_HEADER));
          n = fread(&header, sizeof(PCX_HEADER), 1, fp);
          if (n == -1) {
               printf ("(Error) Cannot read PCX header");
               fclose(fp);
               return ;
          }
          
          // PCX with 4 colors (2bit per pixel) --> CGA 
          // PCX with 16 colors (4bit per pixel) --> EGA, VGA 
          //printf ("%u x %u", header.width, header.height); // --> 319x199 for a 320x200 screen
          //getch();
          bitmap = (BYTE *) malloc ( (header.width+1)*(header.height+1)); // Size in bytes 
          // printf ("bpp %u, bitplanes %u \n",header.bpp, header.num_color_planes);
          // CGA bits-per-plane 2, 1 bitplane 
          // printf ("Encoding: %u", header.encoding);
          
          // Extract every line from the file 
          x=0; y=0;
          while (y< header.height) 
          {    
               // Restart the pointer 

               CGA_VRAM=(BYTE*)0xB8000L;

               if ((y&0x01))  // Odd lines 
               {
                    CGA_VRAM+= 8192 + (y>>1)*80;
               }
               else { // even lines 
                    CGA_VRAM+= (y>>1)*80; // 80 bytes per line, 100*80=8000 bytes per interlaced screen for a total of 16000 bytes
               }

               while (x<header.width)
               {
                    data = fgetc(fp);
                    // printf ("%u ", data);               
                    if ((data & 0xC0) == 0xC0) {
                    // was an RLE count, pixel is next byte
                    count = data & 0x3F;
                    data = fgetc(fp);                
                    } else {
                    count = 1;
                    }
                    // printf ("(%u x %u)", count,data);
                    //getch();
                    while (count) {                         
                         *CGA_VRAM++=data;
                         x+=4; // Each byte holds up to 4 p�xel in bpp 2 
                         count--; // 1 byte less       
                    }
               }     
               x=0;
               y++;
          }
                    
          //_fmemcpy ((BYTE*)CGA_VRAM, (BYTE*)bitmap,(header.width+1)*(header.height+1) );
          //*CGA_VRAM = 0x55; 

          free (bitmap);                    
          fclose (fp);     
    }


#endif 
