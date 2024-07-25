/*************************************************************/
// TITRE  : AFFICHAGE D'UNE IMAGE DANS UN DISQUE
// AUTEUR : Walter SIRACUSA/wiSdom
// PROJET : DISQUE.C, VGA.C, VGA_ASM.ASM
/*************************************************************/

#include <vga.h>
#include <conio.h>
#include <math.h>

byte Pal[768];
bytef Page1[64000];

bytef *Video=MK_FP(0xa000,0);

void PutPixelD(short x,short y);
void AffDisque(void);
void main(void);

void PutPixelD(short x,short y)
{
  word off;

  //pixel : x,y+1
  if(x>=0 && x<320 && (y+1)>=0 && (y+1)<200)
  {
    off=((y+1)<<8)+((y+1)<<6)+x;
    Video[off]=Page1[off];
  }

  //pixel : x,y-1
  if(x>=0 && x<320 && (y-1)>=0 && (y-1)<200)
  {
    off=((y-1)<<8)+((y-1)<<6)+x;
    Video[off]=Page1[off];
  }

  //pixel : x-1,y+1
  if((x-1)>=0 && (x-1)<320 && (y+1)>=0 && (y+1)<200)
  {
    off=((y+1)<<8)+((y+1)<<6)+(x-1);
    Video[off]=Page1[off];
  }

  //pixel : x+1,y-1
  if((x+1)>=0 && (x+1)<320 && (y-1)>=0 && (y-1)<200)
  {
    off=((y-1)<<8)+((y-1)<<6)+(x+1);
    Video[off]=Page1[off];
  }
}

void AffDisque(void)
{
  short xi,yi,di,limite,eta,etap;

  word r;

  for(r=0;r<200;r++)
  {
    VBL;
    xi=0; yi=r; di=2-2*r;   //Algorithme de Bresenham
    e1:
      PutPixelD(OX+xi,OY-yi); //1er cadran
      PutPixelD(OX-xi,OY-yi); //2ie cadran
      PutPixelD(OX-xi,OY+yi); //3ie cadran
      PutPixelD(OX+xi,OY+yi); //4ie cadran

      if(yi<=0) goto e4;
      if(di<0) goto e2;
      if(di>0) goto e3;
      if(di==0) goto e20;
    e2:
      eta=2*di+2*yi-1;
      if(eta<=0) goto e10;
      if(eta>0) goto e20;
    e3:
      etap=2*di-2*xi-1;
      if(etap<=0) goto e20;
      if(etap>0) goto e30;
    e10:
      xi++;
      di=di+2*xi+1;
      goto e1;
    e20:
      xi++;
      yi--;
      di=di+2*xi-2*yi+2;
      goto e1;
    e30:
      yi--;
      di=di-2*yi+1;
      goto e1;
    e4:;
  }
}

void main(void)
{
  word i;

  ModeVGA();
  LoadPCX("cygne2",Page1,Pal);
  SetAllPal(Pal);

  AffDisque();

  getch();
  ModeTxt();
}
