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

WCC386 -3 -ml juego.c -dDOS -dSPANISH -bt=DOS
pause

WCC386 -3 -ml parser.c -dDOS -dSPANISH -bt=DOS
pause

WCC386 -3 -ml .\libgfx\libgfx.c -dDOS -dSPANISH -bt=DOS
pause

WASM -3p .\libgfx\libgfx_DOS.asm 

REM zcc --no-crt crt0_base.asm .\libgfx\printlib.asm .\libgfx\libgfx.c parser.c juego_compressed.c juego_desierto.c -o parser.bin -vn -O3  -lzx7 -m -cleanup -DZX -DSPANISH
REM copy parser.def parser_def.asm


REM appmake +zx -b parser.bin -o parser.tap --noloader --org 24200

REM dir *.bin


