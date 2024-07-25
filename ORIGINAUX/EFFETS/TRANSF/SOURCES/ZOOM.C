/*************************************************************/
// TITRE  : ZOOM PLEIN ECRAN D'UNE IMAGE
// AUTEUR : Walter SIRACUSA/wiSdom
// PROJET : ZOOM.C, VGA.C, VGA_ASM.ASM
/*************************************************************/

#include <vga.h>
#include <conio.h>
#include <math.h>

byte Pal[768];      //Palette
bytef Page1[64000]; //Image … zoomer
byte Zoom[256];     //Mouvement precalcule

byte *Video=MK_FP(0xa000,0);

void ZoomFull(word zy,bytef *scr)
{
  word zx,y;
  long x1,x2,ys,dy;
  word x;
  long dx,xs;

  x1=(word)(320-zy*1.6)>>1; //Centrage x avec correction = 320/200 = 1.6
  x2=319-x1;                //Point final en x

  ys=((200-zy)>>1)<<8;      //Centrage y
  dy=(zy<<8)/200;           //Coefficient de capture des lignes
  dx=((x2-x1)<<8)/320;      //Coefficient de capture des colonnes

  //VBL;
  for(y=0;y<200;y++) //Zoom par rapport a y
  {
    xs=((ys>>8)*320+x1)<<8;  //Calcul les colonnes

    for(x=0;x<320;x++)
    {
      *(scr++)=Page1[xs>>8];
      xs+=dx; //Interpole lineairement les colonnes
    }
    ys+=dy; //Interpole lineairement les lignes
  }
}

void main(void)
{
  byte i=0;
  word s;

  ModeVGA();
  LoadPCX("souris2",Page1,Pal);
  SetAllPal(Pal);

  //Precalcule le mouvement
  for(s=0;s<256;s++) Zoom[s]=100+sin(90+s*0.123)*100;

  CpyScr(Page1,Video);
  getch();

  while(!kbhit())  ZoomFull(Zoom[i++],Video);

  ModeTxt();
}
