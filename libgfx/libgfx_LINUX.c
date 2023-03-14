#include <wchar.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#include "./libgfx.h"


// Window declaration 
textwin_t TextWindow;
textwin_t GraphWindow;

struct fzx_state fzx;   // active fzx state

void fzx_setcolor (BYTE color) 
{
     fzx.color = color;
}

void fzx_setat(unsigned char x, unsigned char y)
{
     fzx.x = x;
     fzx.y = y;
}

void fzx_putc(unsigned char c)
{
    print_char (fzx.x, fzx.y, c, fzx.color);
}

void fzx_puts(char *s)
{
    print_string (fzx.x, fzx.y, s, fzx.color);
}

unsigned char getKey() 
{
    return getch();
}

void waitForAnyKey()
{
    getch ();
}

/* reads from keypress, doesn't echo */
int getch(void)
{
    struct termios oldattr, newattr;
    int ch;
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return ch;
}


void clearchar (BYTE x, BYTE y, BYTE color)
{
    printf (" ");
}

void clearScreen (BYTE color)
{
    system("clear");
}

void waitForNoKey()
{

}

void scrollArriba (BYTE fila_inicial, BYTE columna_inicial)
{

}

void print_string (BYTE x, BYTE y, unsigned char *texto, BYTE color)
{
    int i=0;
    setlocale(LC_ALL, "");    
    while (texto[i]!=0) 
        {
            print_char (x,y,texto[i],color);
            i++;
        }
}

void print_char (BYTE x, BYTE y, unsigned char texto, BYTE color)    
{
    // Tabla ISO8859-15 en https://es.wikipedia.org/wiki/ISO/IEC_8859-15
    // GCC and the Linux console works in UTF-8, not even comparing in the code works unless the specific charcode is used 
    setlocale(LC_ALL, "");    

    // Takes the ISO 8859-15 code and converts to UNICODE to be printed in the console
    // UNICODE from: https://www.fileformat.info/info/unicode/char/a.htm

    if (texto>127) 
    {
    if (texto==0xE1) printf ("\u00E1"); // á
    if (texto==0xE9) printf ("\u00E9"); // é
    if (texto==0xED) printf("\u00ED"); // í
    if (texto==0xF3) printf("\u00F3"); // ó
    if (texto==0xFA) printf("\u00FA"); // ú
    if (texto==0xF1) printf("\u00F1"); // ñ
    if (texto==0xA1) printf("\u00A1"); // ¡
    if (texto==0xBF) printf("\u00BF"); // ¿
    if (texto==0xC1) printf("\u00C1"); // Á
    if (texto==0xC9) printf("\u00C9"); // É
    if (texto==0xCD) printf("\u00CD"); // Í
    if (texto==0xD3) printf("\u00D3"); // Ó
    if (texto==0xDA) printf("\u00DA"); // Ú
    if (texto==0xD1) printf("\u00D1"); // Ñ    
    } else 
    {
    printf ("%c", texto);
    }
}

void setAttr (BYTE x, BYTE Y, BYTE attr)
{

}