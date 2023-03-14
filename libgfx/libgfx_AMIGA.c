
// Specific input/output routines for AMIGA OS 1.3 

#include "./libgfx.h"
#include <stdlib.h>
#include <stdio.h> 
#include <proto/exec.h>
#include <proto/dos.h>


// Windows declaration 
textwin_t TextWindow;
textwin_t GraphWindow;

struct fzx_state fzx;   // active fzx state

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
    //return getch();
}

void waitForAnyKey()
{
    //getch ();
}


void clearchar (BYTE x, BYTE y, BYTE color)
{
    printf (" ");
}

void clearScreen (BYTE color)
{
    //system("clear");
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
    while (texto[i]!=0) 
        {
            print_char (x,y,texto[i],color);
            i++;
        }
}

void print_char (BYTE x, BYTE y, unsigned char texto, BYTE color)    
{
     printf ("%c", texto);    
}

void setAttr (BYTE x, BYTE Y, BYTE attr)
{

}