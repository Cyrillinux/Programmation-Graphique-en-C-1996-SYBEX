/*************************************************************/
// TITRE  : TUBE DE PIXLES
// AUTEUR : Walter SIRACUSA/wiSdom
// PROJET : PTUBE.C, VGA_ASM.ASM
/*************************************************************/

#include <vga.h>
#include <conio.h>
#include <math.h>

#define MAXX 320
#define MAXY 200
#define OX 160
#define OY 100

#define Rmin 26
#define Rmax 129

byte Pal[768];
bytef Page1[65000];
short Cos[256],Sin[256];
byte Mrx[135],Mry[135];
long Rx[256];

bytef *Video=MK_FP(0xa000,0);

float InitSinCos(word val);
void CalculMoveXY(void);
void CalculRayonX(void);
void CalculInk(void);
void main(void);

float InitSinCos(word val)
{
  word i;

  //Precalcule la table de sinus et cosinus
  for(i=0;i<val;i++)
  {
    Cos[i]=(cos((i*(360.0/val))*0.017453293))*256;
    Sin[i]=(sin((i*(360.0/val))*0.017453293))*256;
  }
  return 360.0/val;
}

void CalculMoveXY(void)
{
  byte r;

  //Champs de deplacement de chaque ellipse
  for(r=Rmin;r<Rmax;r+=2)
  {
    Mrx[r]=(OX-Rmin)-((OX-Rmin)*((r-Rmin)/(float)(Rmax-Rmin)));
    Mry[r]=(OY-Rmin)-((OY-Rmin)*((r-Rmin)/(float)(Rmax-Rmin)));
  }
}

void CalculRayonX(void)
{
  word p;

  //Rayon horizontal = rayon vertical * (320 / 200)
  for(p=0;p<256;p+=4)
    Rx[p]=Cos[p]*1.6;
}

void CalculInk(void)
{
  byte e;

  //Degrade de gris des ellipses -> effet de profondeur
  for(e=1;e<52;e++)
    Ink(e,12+e,12+e,12+e); //encres 1 (12,12,12) … 51 (63,63,63)
}

void main(void)
{
  byte i=0,e;
  long x,y;
  unsigned long cx,cy;
  word r,p;

  ModeVGA();

  InitSinCos(256);
  CalculMoveXY();
  CalculRayonX();
  CalculInk();

  while(!kbhit())
  {
    e=0;
    ClrScr(Page1);
    VBL;

    //Rayons croissants des ellipses
    for(r=Rmin;r<Rmax;r+=2)
    {
      //Mouvement en fonction du rayon
      cx=40960+(Cos[(i*3+r)&255]*(long)Mrx[r]);
      cy=25600+(Sin[((i<<1)+(r<<1))&255]*(long)Mry[r]);

      e++; //Encre de l'ellipse courante

      //Trace les ellipses
      for(p=0;p<256;p+=4)   //Points de l'ellipse
      {
        //Position du centre de l'ellipse
        x=(cx+(long)Rx[p]*r)>>8;
        y=(cy+(long)Sin[p]*r)>>8;
        //Affichage avec fenˆtrage
        if(0<x && x<320 && 0<y && y<200) Page1[(y<<8)+(y<<6)+x]=e;
      }
    }
    i++;  //Variation du mouvement des rayons

    CpyScr(Page1,Video);
  }

  ModeTxt();
}
