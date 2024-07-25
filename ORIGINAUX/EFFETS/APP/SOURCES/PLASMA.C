/*************************************************************/
// TITRE  : PLASMA
// AUTEUR : Walter SIRACUSA/wiSdom
// PROJET : PLASMA.C, VGA_ASM.ASM
/*************************************************************/

#include <vga.h>
#include <conio.h>
#include <math.h>
#include <dos.h>

#define LG 160  //Largeur de la fenetre
#define HT 100  //Hauteur de la fenetre

word VPal[256];          //Palette precalculee sur 256 degr‚s
byte Plasma[LG*HT];      //Plasma dimension reelle = LG x HT
bytef Table1[64000];     //Table precalculee
bytef Table2[64000];     //Table precalculee
short Sin[256],Cos[256]; //Cosinus et sinus precalcules

bytef *Video=MK_FP(0xa000,0);

float InitSinCos(word val);
void VariaPal(void);
void CalculTable1(void);
void CalculTable2(void);
void CalculPlasma(word x1,word y1,word x2,word y2,word x3,word y3,word x4,word y4);
void main(void);

float InitSinCos(word val)
{
  word i;

  for(i=0;i<val;i++)
  {
    Cos[i]=(cos((i*(360.0/val))*0.017453293))*256;
    Sin[i]=(sin((i*(360.0/val))*0.017453293))*256;
  }
  return 360.0/val;
}

void VariaPal(void)
{
  word i,
       r,v,b;
  static word r0=1*256,v0=10*256,b0=5*256;

  r0+=77;  //0.3*256
  v0+=128; //0.5*256
  b0+=256; //1.0*256

  r=r0>>8;
  v=v0>>8;
  b=b0>>8;

  for(i=0;i<256;i++)
    Ink(i,VPal[(i+r)&255]>>8,VPal[(i+v)&255]>>8,VPal[(i+b)&255]>>8);
}

void CalculTable1(void)
{
  byte y;
  word x;

  for(y=0;y<200;y++)
    for(x=0;x<320;x++)
      Table1[x+y*320]=sqrt((LG-x)*(LG-x)+(HT-y)*(HT-y))*10;
}

void CalculTable2(void)
{
  byte y;
  word x;

  for(y=0;y<200;y++)
    for(x=0;x<320;x++)
      Table2[x+y*320]=(1+sin(sqrt((LG-x)*(LG-x)+(HT-y)*(HT-y))/8))*80;
}

void CalculPlasma(word x1,word y1,word x2,word y2,word x3,word y3,word x4,word y4)
{
  register byte x,y;
  word k=0,off=0,off1,off2,off3,off4;

  x1=(y1<<8)+(y1<<6)+x1;
  x2=(y2<<8)+(y2<<6)+x2;
  x3=(y3<<8)+(y3<<6)+x3;
  x4=(y4<<8)+(y4<<6)+x4;

  for(y=0;y<HT;y++)
  {
    off1=x1+k;
    off2=x2+k;
    off3=x3+k;
    off4=x4+k;
    for(x=0;x<LG;x++)
      Plasma[off++]=Table1[off1++]+Table2[off2++]+Table2[off3++]+Table2[off4++];
    k+=320;
  }
}

void main(void)
{
  byte x1,y1,x2,y2,x3,y3,x4,y4; //Positions des 4 fenetres
  byte i=0,a1,a2,a3;  //Angles pour le deplacement des fenetres
  word p; //Variation des angles

  Mode320x100(); //Passage en 320x100

  //Precalcule sinus et cosinus sur 256 degres
  InitSinCos(256);

  //Precalcule la variation de la palette sur 256 degres
  for(p=0;p<256;p++)
    VPal[p]=(1+sin(p*(360/256.0)*(3.1416/180.0)))*32*256;

  //Precalcule la table des distances et les distances transformees
  CalculTable1();
  CalculTable2();

  while(!kbhit())
  {
    VariaPal();  //Calcul une palette

    a1=i++;    //Pas d' 1 degre
    a2=(i<<1); //Pas de 2 degres
    a3=(i*3);  //Pas de 3 degres

    //Positions des fenetres :  0 <=x<= LG  et  0 <=y<= HT
    x1=(20480+Cos[a1]*80)>>8; //LG/2 = 80*256 = 20480
    y1=(12800+Sin[a2]*50)>>8; //HT/2 = 50*256 = 12800
    x2=(20480+Sin[a1]*80)>>8;
    y2=(12800+Sin[a3]*50)>>8;
    x3=(20480+Sin[a3]*80)>>8;
    y3=(12800+Cos[a1]*50)>>8;
    x4=(20480+Cos[a3]*80)>>8;
    y4=(12800+Cos[a2]*50)>>8;

    CalculPlasma(x1,y1,x2,y2,x3,y3,x4,y4);  //Somme les fenetres
    VBL;
    Dbl160x100(Plasma,Video); //Affiche le plasma dedouble
  }

  ModeTxt();
}
