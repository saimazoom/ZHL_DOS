del *.o
del *.sym
del *.map
del *.tap
del *.err

@echo ===================================
@echo Text Compression
@echo ===================================

@echo Extrae Texto Parser
REM C:\pyd24210\bin\python\python24.exe textool.py -x t -i juego.c -o juego.txt -c // -l 2
@echo Comprime Texto y genera el diccionario
REM C:\pyd24210\bin\python\python24.exe tc.py -i juego.txt -o juegoc.txt -l 0 

@echo Inserta Texto Parser
REM copy juego.c juego_compressed.c

REM C:\pyd24210\bin\python\python24.exe textool.py -r t -i juegoc.txt -o juego_compressed.c -l 2 -c // 

@echo ===================================
@echo Compila Parser (Pass 1)
@echo ===================================

WCC386 juego.c -dDOS -dSPANISH -bt=DOS -dCGA
pause

WCC386 parser.c -dDOS -dSPANISH -bt=DOS -dCGA
pause

WCC386 .\libgfx\libgfx.c -dDOS -dSPANISH -bt=DOS -dCGA
pause

REM WASM .\libgfx\libgfx_DOS.asm 
REM ..\nasm\nasm.exe -f obj .\libgfx\libgfx_DOS_NASM.asm -o .\libgfx_DOS_NASM.o

wlink name zhl.exe file {libgfx.obj juego.obj parser.obj } 


