/*************************************************************/
// TITRE  : REDUCTION VERTICALE DE DEUX IMAGES
// AUTEUR : Walter SIRACUSA/wiSdom
// PROJET : REDUC_Y.C, VGA.C, VGA_ASM.ASM
/*************************************************************/

#include <vga.h>
#include <conio.h>

byte Pal[768];
bytef Page1[64000];  //1er image
bytef Page2[64000];  //2ie image

bytef *Video=MK_FP(0xa000,0);

void ReducY(byte yb);
void main(void);

void ReducY(byte yb)
{
  byte yd;
  float ys,dy;

  //1er bande
  ys=0;        //Ordonnee source
  dy=200.0/yb; //Rapport entre l'image source et destination

  for(yd=0;yd<yb;yd++)
  {
    CpyBnd(ys,1,yd,Page1,Video);
    ys+=dy;
  }

  //2ie bande
  ys=0;
  dy=200.0/(200-yb);

  for(yd=yb;yd<200;yd++)
  {
    CpyBnd(ys,1,yd,Page2,Video);
    ys+=dy;
  }
}

void main(void)
{
  byte y=199;

  ModeVGA();
  LoadPCX("souris2",Page1,Pal);
  LoadPCX("cygne2",Page2,Pal);
  SetAllPal(Pal);

  while(!kbhit())
  {
    y-=2;       //Deplace la bande
    if(y<2)
    {
      y=199;    //Repositionne l'ordonnee de la bande
      SwpScr(Page1,Page2);  //Permutation des 2 pages
    }

    VBL;
    ReducY(y);
  }

  getch();
  ModeTxt();
}
