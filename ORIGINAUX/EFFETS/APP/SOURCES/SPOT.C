/*************************************************************/
// TITRE  : BALAYAGE D'UN SPOT SUR UNE IMAGE
// AUTEUR : Walter SIRACUSA/wiSdom
// PROJET : SPOT.C, VGA.C, VGA_ASM.ASM
/*************************************************************/

#include <vga.h>
#include <conio.h>
#include <math.h>

#define LG 160
#define HT 100

byte Pal[768];
bytef Image[LG*HT];    //Image finale
bytef Page1[LG*HT];    //Image reduite
bytef Spot[64000];     //Spot
short Xs[512],Ys[512]; //Positions du spot precalculees

bytef *Video=MK_FP(0xa000,0);

void ReductImage(void);
void CalculSpot(void);
void CalculImage(word x1,word y1);
void main(void);

void ReductImage(void)
{
  word i,j;

  for(j=1;j<199;j+=2)
    for(i=1;i<319;i+=2)
      Page1[i/2+(j/2)*LG]=( //Moyenne sur 5 points
       GetPixel(i,j,Video)+GetPixel(i-1,j,Video)+
       GetPixel(i,j-1,Video)+GetPixel(i+1,j,Video)+
       GetPixel(i,j+1,Video))/5;
}

void CalculSpot(void)
{
  byte y;
  word x;

  for(y=0;y<200;y++)
    for(x=0;x<320;x++)
      Spot[x+y*320]=38-((sqrt((LG-x)*(LG-x)+(HT-y)*(HT-y)))/5);
}

void CalculImage(word x1,word y1)
{
  byte x,y;
  word k=0,off=0,off1;

  x1=(y1<<8)+(y1<<6)+x1;

  for(y=0;y<HT;y++)
  {
    off1=x1+k;
    for(x=0;x<LG;x++)
      Image[off]=Page1[off++]+Spot[off1++];
    k+=320;
  }
}

void main(void)
{
  word e,sp;

  Mode320x100();
  for(e=0;e<256;e++) Ink(e,0,0,0);
  LoadPCX("spot",Video,Pal);
  ReductImage(); //Reduction avec lissage

  ClrScr(Video);
  for(e=0;e<64;e++) Ink(e,0,0,e);  //Couleur du spot
  for(e=64;e<256;e++) Ink(e,0,0,63);
  CalculSpot();

  //Precalcule le mouvement du spot sur 512 degres
  for(e=0;e<512;e++)
  {
    Xs[e]=80+cos(e*0.175*(360/512.0)/2)*80;
    Ys[e]=50+sin(e*0.175*(360/512.0))*50;
  }

  while(!kbhit())
  {
    sp=(e++)&511;

    CalculImage(Xs[sp],Ys[sp]); //Somme les intensites RVB
    VBL;
    Dbl160x100(Image,Video);
  }

  ModeTxt();
}
