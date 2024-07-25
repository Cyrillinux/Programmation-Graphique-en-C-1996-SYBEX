/*************************************************************/
// TITRE  : SCROLLING VERTICAL REBOUCLANT
// AUTEUR : Walter SIRACUSA/wiSdom
// PROJET : BOUCLE.C, VGA.C, VGA_ASM.ASM
/*************************************************************/

#include <vga.h>
#include <conio.h>

byte Pal[768];
bytef Page1[64000];
bytef *Video=MK_FP(0xa000,0);

void main(void)
{
  byte j,hy;
  int i,y=0;

  ModeVGA();
  LoadPCX("scroll",Page1,Pal);
  SetAllPal(Pal);
  FBox(0,0,90,80,Page1,0);

  while(!kbhit())
  {
    hy=200-y; //0 <=sy<= 199

    VBL;
    CpyBnd(hy,y,0,Page1,Video);
    CpyBnd(0,hy,y,Page1,Video);
    if(--y<0) y=199;     //Boucle haut
    //if(++y>199) y=0;   //Boucle bas
  }

  ModeTxt();
}
