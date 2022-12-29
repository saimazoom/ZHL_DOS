; https://groups.google.com/forum/#!topic/comp.sys.apple2/usZKHLQi-Ak
;
;    Sure thing, it's simple enough. The software stack is implemented with
; a zp pointer called sp. It starts at the top, and moves down as items
; are pushed on the stack. You can .importzp sp and access it directly,
; but it's easier to use the built in library calls for manipulating the
; stack. When your code is called, the arguments are pushed onto the
; stack left to right. If you declare your function __fastcall__ the
; last (rightmost) argument will be in A/X instead of on the stack,
; which saves a few cycles. Your code is responsible for pulling all the
; arguments off the stack, so if you have e.g. 4 bytes of arguments, sp
; should be increased with 4 before your code exits (2 if you use
; __fastcall__). The return value should be in A (lo) and X (hi). X must
; be set even if you just return a single byte, as cc65 doesn't clear X
; for you if the value is cast e.g. from a char to an int.
;
; Some of the available library routines are:
;  popa          pull one byte off stack, increase sp by one. It destroys Y.
;  popax         pull two bytes off stack, increase sp by two. It destroys Y.
;  pusha         dec sp byte one, push one byte onto the stack. It destroys Y.
;  pushax        dec sp by two, push two bytes onto stack. It destroys Y.
;  incsp1        increase sp by one
;  incsp2        increase sp by two (and so on)
.import popa, popax, pusha, pushax
; Zero page memory addresses than can be used
.importzp ptr1, ptr2, ptr3, ptr4, tmp1, tmp2, tmp3, tmp4

.export _putPixel        ; prepend labels with _ so C can see them
.export _scrollArriba
.export _scrollArriba2
.export _drawRectangle
.export _clearScreen
.export _clearVideoColorMem 
.export _clearVideoRam
.export _clearVideoBitmap
.export _waitForAnyKey
.export _drawVline
.export _drawHline
.export _clearchar
.export _waitForNoKey
.export _drawSprite
.export _randomNumber
.export _getKey
.export _splitScreen
.export _print_string
.export _print_char
.export _setRamLayout
.export _HighResMode
.export _setAttr 

; 1000bytes, 40x25, FIXED IN RAM
.define VIDEO_COLOR $D800
.define VIDEO_COLOR_HI $D8 
.define VIDEO_COLOR_LO $00
; 1000 bytes , 40x25 -> Configurable
; VIDEO RAM aka SCREEN RAM aka VIDEO MATRIX aka SCREEN MEMORY
.define VIDEO_RAM $8C00
.define VIDEO_RAM_HI $8C
.define VIDEO_RAM_LO $00
; 8000 bytes -> Configurable
.define VIDEO_BITMAP $A000
.define VIDEO_BITMAP_HI $A0
.define VIDEO_BITMAP_LO $00

; Kernal Routines
;GETIN. Read byte from default input. (If not keyboard, must call OPEN and CHKIN beforehands.)
;Input: –
;Output: A = Byte read.
;Used registers: A, X, Y.
;Real address: ($032A), $F13E.
.define GETIN  $ffe4

reynolds:
.INCBIN "../res/Reynolds.chr"

.code

;---------------------------------------------------------------
; extern void putPixel (BYTE x, BYTE y, BYTE color);
;---------------------------------------------------------------
; Set a pixel color in multicolor mode (160x200)
_putPixel:

    rts 

;---------------------------------------------------------------
; extern void scrollArriba (BYTE fila_inicial, BYTE columna_inicial);
;---------------------------------------------------------------
; Scroll 8 píxels up in highres mode 
; Columna_inicial <- A Register (0-40)
; fila_inicial <- ARGUMENT STACK (0-24)
_scrollArriba:

    rts
;---------------------------------------------------------------
; extern void scrollArriba2 (BYTE linea_inicial, BYTE num, BYTE step);
;---------------------------------------------------------------
_scrollArriba2:
    rts
;---------------------------------------------------------------
; extern void drawRectangle (BYTE xorig, BYTE yorig, BYTE width, BYTE height);
;---------------------------------------------------------------
_drawRectangle:
    rts

;---------------------------------------------------------------
; extern void drawVline (BYTE x0, BYTE y0, BYTE len);
;---------------------------------------------------------------
_drawVline:
    rts

;---------------------------------------------------------------
; extern void drawHline (BYTE x0, BYTE y0, BYTE len);
;---------------------------------------------------------------
_drawHline:
    rts
;---------------------------------------------------------------
; extern void clearchar (BYTE x, BYTE y, BYTE color);
;---------------------------------------------------------------
_clearchar:
    rts

;---------------------------------------------------------------
; extern __FASTCALL__ void clearScreen (BYTE color);
; FASTCALL -> Argument in A(lo) and X(hi). Rightmost first.
;---------------------------------------------------------------
 ; 1) SCREEN RAM/SCREEN MEMORY/VIDEO MATRIX/CHARMEM RAM -> 1000bytes -> 256x8
    ; $9C00-E3FF: 1000 Bytes -> Programmable
   
_clearVideoRam:
    ; A already contains the color 
    STA tmp1 
    LDY #$00
    LDX #$04 ; Number of banks of 256bytes to write
    ; Also called VIDEOBASE in C64 debugger
    LDA #VIDEO_RAM_HI
    STA ptr1+1
    LDA #VIDEO_RAM_LO
    STA ptr1
    LDA tmp1
clearVideoCharMem_Loop:
    STA (ptr1),Y
    INY 
    BNE clearVideoCharMem_Loop ; Loops until Y becames 0 again
            ; 1 bank of 256bytes is complete
    INC ptr1+1 ; Increase the high BYTE
    DEX       ; 1 Bank less   
    BNE clearVideoCharMem_Loop
    RTS

    ; 2) VIDEOBITMAP RAM  -> 160x200 -> 8000 bytes -> (40x8)x25
    ; $C000-$DFFF
    ; In multicolor
    ; Contenido de la memoria de bitmap:
    ; %00: Se toma el color de fondo del registro $d021 (color de fondo 0).
    ; %01: Se toma el color de la RAM de pantalla (nibble alto). ScreenRAM
    ; %10: Se toma el color de la RAM de pantalla (nibble bajo). ScreenRAM.
    ; %11: Se toma el color de la RAM de color.

    ; In high resolution bitmap mode 
    ; Contents is based on pixel activated/not actived like in Spectrum
    ; In high resolution bitmap mode:
    ; Ink is taken from high nibble from SCREEN RAM 
    ; Paper is taken from low nibble from SCREEN RAM 

_clearVideoBitmap: ; Clear up 8192bytes = 32 pages of 256bytes but only 8000 bytes are needed
    ; A already contains the color 
    STA tmp1 
    LDY #$00
    LDX #32 ; Number of banks of 256bytes to write
    ; Also called BITMAP in C64 debugger
    LDA #VIDEO_BITMAP_HI
    STA ptr1+1
    LDA #VIDEO_BITMAP_LO
    STA ptr1
    LDA tmp1
clearVideoBitmap_Loop:
    STA (ptr1),Y
    INY 
    BNE clearVideoBitmap_Loop ; Loops until Y becames 0 again
            ; 1 bank of 256bytes is complete
    INC ptr1+1 ; Increase the high BYTE
    DEX       ; 1 Bank less   
    BNE clearVideoCharMem_Loop
    RTS

; 3) VIDEO COLOR/COLORMEM RAM  -> 1000bytes. Fixed at 0xD800-0xDEE8
_clearVideoColorMem:
    ; A already contains the color 
    LDY #200
clearVideoColorMem_Loop:    
    STA $D800,Y
    STA $D8C8,Y
    STA $D990,Y
    STA $DA58,Y
    DEY
    BNE clearVideoColorMem_Loop
    RTS 

_clearScreen:
    ; Border color to Black
    LDA #$00
    STA $d020
    STA $d021
    RTS

;---------------------------------------------------------------
; extern void waitForAnyKey();
;---------------------------------------------------------------
; https://codebase64.org/doku.php?id=base:robust_string_input
; We make use of Kernal routine to cover the C64 different layouts
_waitForAnyKey:
    JSR GETIN
    beq _waitForAnyKey
    rts
;---------------------------------------------------------------
; extern void waitForNoKey();
;---------------------------------------------------------------
_waitForNoKey:
    rts

;---------------------------------------------------------------
; extern void drawSprite (BYTE *pointer, BYTE xorig, BYTE yorig, BYTE width, BYTE height);
;---------------------------------------------------------------
_drawSprite:
    rts

;---------------------------------------------------------------
; extern WORD randomNumber(); 
;---------------------------------------------------------------
_randomNumber:
    rts
;---------------------------------------------------------------
; extern unsigned char getKey();
;---------------------------------------------------------------
_getKey:
    JSR _waitForAnyKey
    rts

;///////////////////////////////////////////////////////////
;//                      DRAW TEXT                        //
;///////////////////////////////////////////////////////////
;// Functions in printlib.asm

;---------------------------------------------------------------
; extern void print_string (BYTE x, BYTE y, unsigned char *texto);
;---------------------------------------------------------------
; Valid for bitmap high-resolution mode (Speccy mode)
; https://github.com/cc65/wiki/wiki/Parameter-passing-and-calling-conventions
; In general, parameters are pushed from left to right, so the rightmost parameter is the last one pushed (and therefore, the one at the lowest position on the parameter stack
; 6502 is little endian -> First low byte then high byte
; The pointer to the string is pased in AX: A Low BYTE, X HIGH BYTE 
; In example String in 0x0DD2 -> A=AE, X=0D
_print_string:
    ; Pointer to the string comes already in XA
    sta ptr1     ; Stores in PTR1 string address, Low Byte i.e. 0xAE
    stx ptr1 + 1 ; String Address High Byte, i.e. 0x0D
    jsr popa  ; POP Y Coordinate in chars
    STA tmp1
    jsr popa  ; POP X Coordinate in chars
    TAX       ; X holds the X coordinate (0-39)
    LDY tmp1  ; Y holds the y coordinate (0-24)

    JSR Get_Char_Address ; Calculate the bitmap screen address in 16bit ptr3 
                         ; i.e. X=0, Y=13 -> 0x1040 as an offset (PTR3 0x40, PTR3+1 0x10)
                         ; Bitmap screen base is added also. i.e. X=0 Y=13 -> 
                         ; PTR3 is located at 0x0E, PTR3+1 at 0x0F 
    ; Pointers in 6502
    ; https://stackoverflow.com/questions/46673778/read-in-memory-and-inc-6502
    ; At this point -> ptr1 points to the string 
    ;                  ptr3 points to the screen char 
    
    LDY #$00 ; Char offset from the string buffer, max 256 length
    STY tmp4
Print_String_0:
    LDY tmp4         ; TMP4 is located at 0x15
    LDA (ptr1),Y     ; PTR1 works as a 16bit pointer, Get current char
    BEQ print_return ; IF 0 then ends...
    SEC              ; Inverse borrow, we need to set the carry to clear it
    SBC #32          ; Minus 32 to be inline with the font char table 
                     ; which starts at 32
    JSR Print_Char   ; Prints the current char  
    CLC
    LDY tmp4
    INY              ; Increments the pointer to the string buffer
    STY tmp4
    LDA ptr3 ; Increments the video bitmap memory pointer + 8  
    ADC #$08 ; Next Char in video memory
    STA ptr3 
    LDA ptr3+1
    ADC #$00 ; Add the carry in case
    STA ptr3+1
    JMP Print_String_0 ; Jumps back to the start of the loop
print_return:    
    rts
;---------------------------------------------------------------
; extern void print_char (BYTE x, BYTE y, BYTE char);
;---------------------------------------------------------------
; Just 1 char not a pointer in the right most argument
; In general, parameters are pushed from left to right, so the rightmost parameter is the last one pushed (and therefore, the one at the lowest position on the parameter stack
; The right most argument is always stored in A (low) and X (high)
; 6502 is little endian -> First low byte then high byte
_print_char:
    STA tmp4  ; 0x15 in memory page 0   
    jsr popa  ; POP Y Coordinate in chars, POPA destroy X and Y registers
    STA tmp1
    jsr popa  ; POP X Coordinate in chars
    TAX       ; X holds the X coordinate (0-39)
    LDY tmp1  ; Y holds the y coordinate (0-24)

    JSR Get_Char_Address ; Calculate the bitmap screen address in 16bit ptr3 
                         ; i.e. X=0, Y=13 -> 0x1040 as an offset (PTR3 0x40, PTR3+1 0x10)
                         ; Bitmap screen base is added also. i.e. X=0 Y=13 -> 
                         ; PTR3 is located at 0x0E, PTR3+1 at 0x0F 
    ; Pointers in 6502
    ; https://stackoverflow.com/questions/46673778/read-in-memory-and-inc-6502
    ; At this point ->  
    ;                  ptr3 points to the screen char 
    LDA tmp4     
    BEQ print_char_return ; IF 0 then ends...
    SEC              ; Inverse borrow, we need to set the carry to clear it
    SBC #32          ; Minus 32 to be inline with the font char table 
                     ; which starts at 32
    JSR Print_Char   ; Prints the current char  
print_char_return:    
    rts

; Get screen address
; Y = Y character position (0-24)
; X = X character position (0-39)
; Returns 16bit address in PTR3 (0x0E)
; Video_bitmap -> 8000 bytes.
;   In highres bitmap mode: Address: X*8+Y*(256+64)
;  PTR3 (0x0E)
;  TMP2 (0x13)
; i.e. X=0 Y=13 PTR3+1|PTR3 = 0xD040, 53312
Get_Char_Address:
    ; First calculate the offset
    LDA #$00 
    STX ptr3    ; Stores X in PTR3 LOW
    
    STA ptr3+1  ; CLEAR PTR3 HIGH
    ASL ptr3 ; X=X*2
    ROL A
    
    ASL ptr3 ; X=X*4
    ROL A     ; Add the carry bit to the high part    
    ADC #$00  
    ASL ptr3 ; X=X*8
    ROL A     ; Add the carry bit to the high part    
    ; STA ptr3+1
    ; Now caculate Y*320 as Y*256+ Y*64
    ; PTR3+1 | PTR3 -> HIGH | LOW 
    CLC 
    STY tmp1
    ADC tmp1    ; HIGH = HIGH + Y -> ADDR=X*8+Y*256
    STA ptr3+1
    
    CLC
    ASL tmp1 ; Y*2
    LDA #$00
    ROL A     ; Add the carry bit to the high part    
    
    ASL tmp1 ; Y*4
    ROL A     ; Add the carry bit to the high part    
    
    ASL tmp1 ; Y*8
    ROL A     ; Add the carry bit to the high part    
    
    ASL tmp1 ; Y*16
    ROL A     ; Add the carry bit to the high part    
    
    ASL tmp1 ; Y*32
    ROL A     ; Add the carry bit to the high part    
    
    ASL tmp1 ; Y*64
    ROL A     ; Add the carry bit to the high part    
    STA tmp2  ; Stores the High byte for later
    ; Add the result to the stored address in ptr3
    LDA ptr3 ; LOW BYTE 
    ADC tmp1 ; Adds the LOW BYTE to the acumulator 
    STA ptr3 ; Stores the low byte
    LDA ptr3+1 ; HIGH BYTE 
    ADC tmp2 ; ADDs all including the carry 
    STA ptr3+1 ; ADDR = PTR3+1 | PTR3
    
    ; Now add the screen base in 16bit
    LDA ptr3
    ADC #VIDEO_BITMAP_LO
    STA ptr3 
    LDA ptr3+1 
    ADC #VIDEO_BITMAP_HI ; Add the high byte including the carry 
    STA ptr3+1
    RTS

; Print a single character out in videoo bitmmap memory
; Valid for High resolution bitmap mode (speccy mode)
; A: Character be to printed (0-255)
; PTR3: Video screen address 
; Destroys: Y Register, PTR4, tmp1, tmp2

Print_Char:
    ; Calculate offset 
    STA tmp1  ; At 0x12
    LDA #$00  ; High byte
    ASL tmp1  ; char*2
    ROL A     ; Add the carry bit to the high part in the accumulator  
    
    ASL tmp1  ; Char*4 
    ROL A     ; Add the carry bit to the high part in the accumulator  
   
    ASL tmp1 ; Char*8
    ROL A     ; Add the carry bit to the high part in the accumulator  
              ; Now the charnum*8 is stored in tmp2 | tmp1 
    STA tmp2  ; Stores TMP2 for later
   
    LDA #>reynolds  ; HIGH Part off address
    STA ptr4+1    ; Now Font address is stored in PTR4+1|PTR4
    LDA #<reynolds  ; LOW Part of address
    STA ptr4
    
    ; Add the offset to the base address of the font char 
    CLC 
    LDA tmp1    ; A = TMP1 (low byte)
    ADC ptr4 
    STA ptr4 ; Stores the low byte 
    LDA tmp2 
    ADC ptr4+1 
    STA ptr4+1 ; Stores the high byte 
    
    ; PTR4+1 | PTR4 contains now the 16bit address to the font char  
    ; Dump the font into the screen
    LDY #$08
Print_Char_Loop: 
    LDA (ptr4),Y ; Read Byte  from font char
    STA (ptr3),Y ; Write Byte in video memory
    DEY 
    BNE Print_Char_Loop
    RTS
;---------------------------------------------------------------
; extern void set_attr (BYTE x, BYTE Y, BYTE attr);
;---------------------------------------------------------------
_set_attr:
    rts

;---------------------------------------------------------------
; extern void __FASTCALL__ splitScreen (BYTE scanline);
;---------------------------------------------------------------
_splitScreen:
; Initialization
    SEI ;disable maskable IRQs
        
    LDA #%01111111
    STA $DC0D 	;"Switch off" interrupts signals from CIA-1
	AND $D011
    STA $D011 	; Clear most significant bit in VIC's raster register
	LDA #13*8   ; 104d, 0x68
    STA $D012 ;	Set the raster line number where interrupt should occur
	
    LDA #<Irq  ; Retrieves the low part of IRQ 16bit ADDRESS
    STA $0314  
    LDA #>Irq  ; Retrieves the high part of IRQ 16bit ADDRESS
    STA $0315 ;	Set the interrupt vector to point to interrupt service routine below
	LDA #%00000001
    STA $D01A ;	Enable raster interrupt signals from VIC
    CLI 
    RTS

Irq:            ; 0x0C71
    LDA $D011
    AND #%10011111
    ORA #%10111111
    STA $D011 	; Select High Resolution, 320x200 for Text
	LDA $D016
    AND #%11101111
    STA $D016
    
    LDA #<Irq2 ; Load Low Part of IRQ2 Address
    STA $0314  ; 6502 is litte endian, first is the low byte then the High byte
    LDA #>Irq2 ; Load High Part of IRQ2 Address
    STA $0315 ;	Re-direct next interrupt to Irq2 service routine
	LDA #1
    STA $D012  ; Next interrupt to occur at raster line no. 0
	
    ASL $D019  ;	"Acknowledge" the interrupt by clearing the VIC's interrupt flag.
	JMP $EA31 ;	Jump to the beginning KERNAL's standard interrupt service routine. 

Irq2:            ; 0x0C98
 	LDA $D011
    ; https://www.c64-wiki.com/wiki/Multicolor_Bitmap_Mode
    AND #%10011111
    ORA #%10111111
    STA $D011 	; Select multicolor screen mode
	LDA $D016
    ORA #%00010000
    STA $D016

    LDA #<Irq ; High Part of IRQ Address
    STA $0314  
    LDA #>Irq  ; Low Part of IRQ Address
    STA $0315 ;	Re-direct next interrupt back to Irq
	LDA #13*8
    STA $D012 	; Next interrupt to occur at raster line no. 104
    
    LDA #%01111111
	AND $D011
    STA $D011 	; Clear most significant bit in VIC's raster register

    ASL $D019 ;	"Acknowledge" the interrupt by clearing the VIC's interrupt flag.
	JMP $EA81 ;	Jump to the final part of KERNAL's standard interrupt service routine

;---------------------------------------------------------------
;   extern void setRamLayout ();
;---------------------------------------------------------------
; Memory map
;$02-$7f, $fa-$ff zeropage variables
;$0334(820d)-$bfff code and data -> 48KB RAM
;$BANK3: C000-$ffff video bank (graphics)  -> VIDEO MEMORY + ROM: 16384bytes
; Main issue here is that I/O and KERNAL starts at 0xD000, so this leaves out
; the usage of bank3 in graphic bitmap modes.
; VIC Can address up to 16Kbytes
; $DD00 = %xxxxxx11 -> bank0: $0000-$3fff
; $DD00 = %xxxxxx10 -> bank1: $4000-$7fff
; $DD00 = %xxxxxx01 -> bank2: $8000-$bfff
; $DD00 = %xxxxxx00 -> bank3: $c000-$ffff

; En el banco-3
; Handle C64 memory banks
; https://www.lemon64.com/forum/viewtopic.php?t=46112&sid=8b4d0db809b1ba6236b1e559472f213e


_setRamLayout:
    LDA $DD00 
    AND #%11111101  ; VIC pointed to Bank 2
    ; ORA #%000000xx ;<- your desired VIC bank value, see above
    STA $DD00
    ; https://sta.c64.org/cbm64mem.html
    ; https://codebase64.org/doku.php?id=base:vicii_memory_organizing
    ; $D018/53272/VIC+24:   Memory Control Register
    ;+----------+---------------------------------------------------+
    ;| Bits 7-4 |   Video Matrix Base Address (inside VIC addresses)          |
    ;| Bit  3   |   Bitmap-Mode: Select Base Address (inside VIC addrs)   |
    ;| Bits 3-1 |  *Character Dot-Data Base Address (inside VIC addrs)    |
    ;| Bit  0   |   Unused                                          |
    ;+----------+---------------------------------------------------+
    ; 1) SCREEN RAM / SCREEN MEMORY / VIDEO MATRIX -> 1000bytes -> 256x8
    ; 2) VIDEO BITMAP RAM/CHAR MEM in Text Mode  -> 160x200 -> 8000 bytes -> (40x8)x25
    ;   *CHAR MEM/CHAR DOT-DATA is not used in BITMAP modes
    ; 3) VIDEO COLOR/COLORMEM RAM  -> 1000bytes 
    ; i.e. 
    ; Bitmap data 0x2000 - 0x3F3E = 8000 bytes a copiar en RAM video-bitmap
    ; Charmem data 0x3F40 - 0x4327=1000 bytes a copiar en RAM de pantalla en 0x04000 (ScreenRAM)
    ; Colormem data 0x4328 - 0x470F = 1000bytes a copiar en RAM de color en 0xD800 (ColorRAM) fixed address
    ; Background data: 0x4710=1byte con el color de fondo
    
    ; 2) VIDEO BITMAP RAM 
    ; The bits 0 to 3 used in TEXT mode to allocate the char mem.

    ;$D018 = %xxxx0xxx -> bitmap is at $0000 - $2000 (8192 rounded)
    ;$D018 = %xxxx1xxx -> bitmap is at $2000

    LDA $D018
    AND #%11110111  ; 
    ORA #%00001000  ; Video Bitmap from 0xA000 to 0xBFFF (ABS RAM)
    
    ; 3) VIDEO COLOR/COLORMEM RAM -> FIXED VALUE
    ;    RAM de color 0xD800-0xDBE7 (1000 posiciones 40x25) -> Fija en memoria -> EL color de tinta de un caracter, esto no está en el Spectrum.
    ;   Indica el color de un caracter de 40x25

    ; 1) Bits #4-#7: Pointer to screen memory (bits #10-#13), relative to  VIC bank, memory address $DD00. Values:
    ; Screen memor
    ; The VIC always sees the char rom at $1000-$1fff and $9000-$9fff 
    ; %0000, 0: $0000-$03FF, 0-1023  
    ; %0001, 1: $0400-$07FF, 1024-2047.
    ; %0010, 2: $0800-$0BFF, 2048-3071.
    ; %0011, 3: $0C00-$0FFF, 3072-4095. <-- Start of SCREEN RAM / SCREEN MEMORY / VIDEO MATRIX. Called VIDEO BASE in the C64 Debugger.
    ; %0100, 4: $1000-$13FF, 4096-5119. -> Forbidden in bank2
    ; %0101, 5: $1400-$17FF, 5120-6143. -> Forbidden in bank2
    ; %0110, 6: $1800-$1BFF, 6144-7167. -> Forbidden in bank2
    ; %0111, 7: $1C00-$1FFF, 7168-8191. -> Forbidden in bank2
                                        
    ; %1000, 8: $2000-$23FF, 8192-9215. <-- Start of video bitmap memory (8K). Called BITMAP in C64 debugger.
    ; %1001, 9: $2400-$27FF, 9216-10239.
    ; %1010, 10: $2800-$2BFF, 10240-11263.
    ; %1011, 11: $2C00-$2FFF, 11264-12287.
    ; %1100, 12: $3000-$33FF, 12288-13311.
    ; %1101, 13: $3400-$37FF, 13312-14335.
    ; %1110, 14: $3800-$3BFF, 14336-15359.
    ; %1111, 15: $3C00-$3FFF, 15360-16383.
    
    ; Contenido de la memoria de bitmap:
    ;     %00: Se toma el color de fondo del registro $d021 (color de fondo 0).
    ; %01: Se toma el color de la RAM de pantalla (nibble alto). ScreenRAM
    ; %10: Se toma el color de la RAM de pantalla (nibble bajo). ScreenRAM.
    ; %11: Se toma el color de la RAM de color (nibble bajo). El nibble alto no está conectado.
    AND #%00001111
    ORA #%00110000 ; 1) SCREEN-RAM 0x9c00 to 0x9FFF(ABS RAM), $1C00 to $1FFF in relative to VIC-ADDRESS 
    STA $D018
    RTS 

; HighResmode/Standard Bitmap Mode 
; https://www.c64-wiki.com/wiki/Standard_Bitmap_Mode

_HighResMode:
    LDA $D011
    AND #%10011111
    ORA #%10111111
    STA $D011 	; Select High Resolution, 320x200 for Text
	LDA $D016
    AND #%11011111
    STA $D016
    RTS

; extern void __CALLEE__ set_attr (BYTE x, BYTE Y, BYTE attr);
_setAttr:
    RTS 