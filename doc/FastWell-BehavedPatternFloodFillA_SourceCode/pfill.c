
/* Alvin Albrecht 01.2003 */

#include <stdlib.h>
#include <graphics.h>
#include <spritepack.h>

extern uchar patterns[];
#asm
._patterns

defb @11111111
defb @11111111
defb @11111111
defb @11111111
defb @11111111
defb @11111111
defb @11111111
defb @11111111

defb @10101010
defb @01010101
defb @10101010
defb @01010101
defb @10101010
defb @01010101
defb @10101010
defb @01010101

defb @00000000
defb @01111110
defb @01100110
defb @01100110
defb @01100110
defb @01100110
defb @01111110
defb @00000000

defb @10001000
defb @01000100
defb @00100010
defb @00010001
defb @10001000
defb @01000100
defb @00100010
defb @00010001

defb @00010001
defb @00100010
defb @01000100
defb @10001000
defb @00010001
defb @00100010
defb @01000100
defb @10001000

defb @10011001
defb @01100110
defb @01100110
defb @10011001
defb @10011001
defb @01100110
defb @01100110
defb @10011001

defb @00100010
defb @01010101
defb @10001000
defb @00000000
defb @00100010
defb @01010101
defb @10001000
defb @00000000

defb @11111111
defb @10000000
defb @10100010
defb @10010100
defb @10001000
defb @10010100
defb @10100010
defb @10000000
#endasm


main()
{
   int x,y,r,n;

   while (1) {
      clg();
      for (n=0; n!=10; n++) {
         do {
            x = rand() % 256;
            y = rand() % 192;
            r = rand() % 40;
         } while (((x-r)<0) || ((y-r)<0) || ((x+r)>255) || ((y+r)>191));
         circle(x,y,r,1);
      }
      x = rand() % 256;
      y = rand() % 192;
      r = (rand() % 8)*8;
      sp_PFill(x, y, patterns + r, 300);   /* max stack space used = 3*300+30 bytes */
      sp_WaitForKey();
   }
}
