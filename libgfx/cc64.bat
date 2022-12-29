
del *.o
del *.sym
del *.map
del *.tap
del *.err

ECHO ----------------------------------
cl65 -t c64 -D C64 -Or ./libgfx_c64.asm ./main.c -o libgfx.prg -m listing.txt

dir *.prg


