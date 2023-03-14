BITS 32 
CPU 386 

segment _DATA32 public align=4 class=DATA use32
	global reynolds

segment _BSS32 public align=4 class=BSS use32

group DGROUP _BSS32 _DATA32

segment _TEXT32 public align=1 class=CODE use32
        global  putPixel_, drawRectangle_, drawVLine_, drawHline_, pfill_, drawSprite_, randomNumber_, setmodeCGA_, setmodeTEXT_
        
setmodeCGA_:
	MOV	AX, 0x0004 ; CGA 320x200 4 color graphics 
     	INT	10h 
     	ret 

setmodeTEXT_:
	MOV	AX, 0x0003; 80x25 16 color text 
     	INT	10h 
     	ret
     	
putPixel_:
	ret

drawRectangle_:
	ret
	
drawVline_:
	ret

drawHline_: 
	ret

drawLine_:
	ret
pfill_:
	ret
drawSprite_:
	ret
randomNumber_:
	ret
