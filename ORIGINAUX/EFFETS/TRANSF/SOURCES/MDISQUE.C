/*************************************************************/
// TITRE  : MAPPING DANS UN DISQUE
// AUTEUR : Walter SIRACUSA/wiSdom, thanxx to POLLUX/OLYMPUS
// PROJET : DISQUE.C, VGA.C, VGA_ASM.ASM
/*************************************************************/

#include <vga.h>
#include <conio.h>
#include <math.h>

#define LG 160  // Dimension de l'image
#define HT 128

byte Pal[768];
bytef Page1[64000];
bytef TableL[LG*HT]; //Table des distances
bytef TableC[LG*HT]; //Table des angles
bytef Disque[LG*HT];

bytef *Video=MK_FP(0xa000,0);

void CalculTableL(void);
void CalculTableC(void);
void CalculDisque(void);
void main(void);

void CalculTableL(void)
{
  short i,j,x,y;

  for(j=0;j<HT;j++)
    for(i=0;i<LG;i++)
    {
      x=(LG/2)-i;
      y=(HT/2)-j;

      TableL[j*LG+i]=sqrt(x*x+y*y);  //Distance
    }
}

void CalculTableC(void)
{
  short i,j,x,y;
  double a;

  for(j=0;j<HT;j++)
    for(i=0;i<LG;i++)
    {
      x=(LG/2)-i;
      y=(HT/2)-j;

      if(x==0 && y==0) a=0;
      else a=atan2(y,x)*((HT/2)/3.1416); //Angle sur HT degres

      if(a>0) TableC[j*LG+i]=a;
      else TableC[j*LG+i]=HT+a;
    }
}

void CalculDisque(void)
{
  static ang=1;
  word i,j,p,k2=0,t,k=2240; //k = (28/2)*LG, ajuste le centre

  for(j=0;j<100;j++)  //HT - 28
  {
    p=k;
    t=k2;

    for(i=0;i<LG;i++)  //%HT = &(HT-1)
      Disque[t++]=Page1[((TableL[p]-ang)&127)*MAXX+((TableC[p++]+ang)&127)];

    k+=LG;
    k2+=LG;
  }

  ang+=2;
}

void main(void)
{
  Mode320x100();
  LoadPCX("mdisque",Page1,Pal);
  SetAllPal(Pal);

  CalculTableL();
  CalculTableC();

  while(!kbhit())
  {
    CalculDisque();
    //VBL;
    Dbl160x100(Disque,Video);
  }

  ModeTxt();
}
