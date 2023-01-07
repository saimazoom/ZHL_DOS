
global drawRectangle_
global drawVline_
global drawHline_
global drawLine_
global pfill_
global drawSprite_
global randomNumber_

; Variables
global _L01_img
global _L02_img
global _L03_img
global _L04_img
global _L05_img
global _L06_img
global _L07_img
global _L09_img


segment _DATA public align=4 class=DATA use32

_L01_img: incbin ".\resCGA\L01cga.pcx"
_L02_img: incbin ".\resCGA\L02cga.pcx"
_L03_img: incbin ".\resCGA\L03cga.pcx"
_L04_img: incbin ".\resCGA\L04cga.pcx"
_L05_img: incbin ".\resCGA\L05cga.pcx"
_L06_img: incbin ".\resCGA\L06cga.pcx"
_L07_img: incbin ".\resCGA\L07cga.pcx"
_L09_img: incbin ".\resCGA\L09cga.pcx"

segment _BSS public align=4 class=BSS use32

group DGROUP _BSS _DATA

segment _TEXT public align=1 class=CODE use32
     
putPixel_:

drawRectangle_:

drawVline_:


drawHline_: 


drawLine_:

pfill_:

drawSprite_:

randomNumber_:
