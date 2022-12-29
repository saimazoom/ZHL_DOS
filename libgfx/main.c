// Demo for libgfx library for C64 
// V0.1

#include "libgfx.h"

#define BYTE unsigned char
#define WORD unsigned int

void main (void)
{
	unsigned char a;
	setRamLayout (); // Function only for C64 	
	HighResMode (); // Function only for C64
	// Attributes memory
	clearScreen (0); 
	clearVideoColorMem (PAPER_BLACK); // Ink WHITE | PAPER BLACK
	clearVideoRam (INK_WHITE|PAPER_BLACK); 
	clearVideoBitmap (0);
	splitScreen (13*8); // Install the split screen interrupt handler at the required scan-line
	// Split screen: Top in multicolor (160x320) and bottom in highres color (320x200)
	// Carga una pantalla en memoria de vídeo
	// Imprime un texto 
	print_string (0,13,"Pulsa una tecla.");
	waitForAnyKey();
	print_string (0,14,"OK. Now press the key P.");
	a = getKey();
	print_string (0,15,"Has pulsado la tecla ");
	print_char (0,16,a);
	//scrollArriba (0,0);
	//print_string (0,13,"Fin de la demo.");
	while(1);
}

