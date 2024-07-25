/*************************************************************/
// TITRE  : DISTORSION SINUSOIDALE CONSTANTE D'UNE IMAGE
// AUTEUR : Walter SIRACUSA/wiSdom
// PROJET : DISTO.C, VGA.C, VGA_ASM.ASM
/*************************************************************/

#include <vga.h>
#include <conio.h>
#include <math.h>

#define X1 56  // Abscisse gauche
#define Y1 0   // Ordonn‚e haut
#define Y2 199 // Ordonn‚e bas

byte Pal[768];      // Palette de l'image
bytef Page1[64000]; // Image initiale
byte Dist[256];     // Distorsion pr‚calculee

bytef *Video=MK_FP(0xa000,0);

void main(void)
{
  word y;
  byte i;

  ModeVGA();

  LoadPCX("samantha",Page1,Pal);
  SetAllPal(Pal);

  //Flash blanc
  CpyScr(Page1,Video);
  getch();
  for(i=0;i<64;i++) {VBL;Ink(0,i,i,i);}
  for(i=63;i>0;i--) {VBL;Ink(0,i,i,i);}
  Ink(0,0,0,0);
  ClrScr(Video);

  // Precalcule le mouvement sinusoidale sur 256 degres
  for(y=0;y<256;y++)
    Dist[y]=50+sin(y*0.017453*1.40625)*40;

  //Distorsion sinusoidale
  while(!kbhit())
  {
    i+=2;
    VBL;
    for(y=Y1;y<=Y2;y++) //Largeur multiple de 4 !
      CpyBlk4(X1,y,212,1,Dist[(i+y)&255],y,Page1,Video);
  }

  ModeTxt();
}
