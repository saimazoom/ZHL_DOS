del *.o
del *.sym
del *.map
del *.tap
del *.err

@echo ===================================
@echo Text Compression
@echo ===================================

@echo Extrae Texto Parser
C:\Python27\python.exe textool.py -x t -i juego.c -o juego.txt -c // -l 2
@echo Comprime Texto y genera el diccionario
C:\Python27\python.exe tc.py -i juego.txt -o juegoc.txt -l 0 

@echo Inserta Texto Parser
copy juego.c juego_compressed.c
C:\Python27\python.exe textool.py -r t -i juegoc.txt -o juego_compressed.c -l 2 -c // 

@echo ===================================
@echo Compila Parser (Pass 1)
@echo ===================================


zcc --no-crt crt0_base.asm .\libgfx\printlib.asm .\libgfx\libgfx.c parser.c juego_compressed.c juego_desierto.c -o parser.bin -vn -O3  -lzx7 -m -cleanup -DZX -DSPANISH
REM copy parser.def parser_def.asm


appmake +zx -b parser.bin -o parser.tap --noloader --org 24200

dir *.bin


