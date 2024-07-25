/*************************************************************/
// TITRE  : SCROLLING VERTICAL PAR DECALAGE
// AUTEUR : Walter SIRACUSA/wiSdom
// PROJET : DECALE.C, VGA.C, VGA_ASM.ASM
/*************************************************************/

#include <vga.h>
#include <conio.h>
#include <stdlib.h>

bytef *Video=MK_FP(0xa000,0);

void main(void)
{
  ModeVGA();

  while(!kbhit())
  {
    PutPixel(random(320),random(199),Video,random(256));
    CpyBnd(1,199,0,Video,Video);
  }

  ModeTxt();
}
