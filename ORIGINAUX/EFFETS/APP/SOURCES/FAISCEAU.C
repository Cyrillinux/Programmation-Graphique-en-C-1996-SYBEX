/*************************************************************/
// TITRE  : FAISCEAUX DE COULEURS
// AUTEUR : Walter SIRACUSA/wiSdom
// PROJET : FAISCEAU.C, VGA_ASM.ASM
/*************************************************************/

#include <vga.h>
#include <conio.h>
#include <math.h>
#include <time.h>

#define LG 20 //Dimension du carre
#define HT 20

word Xmove[360]; //Trajectoire du faisceau
byte Ymove[360];

bytef *Video=MK_FP(0xa000,0);

void Degrade(byte ed,byte rd,byte vd,byte bd,byte ef,byte rf,byte vf,byte bf);
void GenereInk(void);
void GenereMove(void);
void Faisceau(word x,word y,bytef *src);
void main(void);

void Degrade(byte ed,byte rd,byte vd,byte bd,byte ef,byte rf,byte vf,byte bf)
{
  byte i,n;
  float rr,rv,rb;

  if(ef<ed)
  {
    n=ed;
    ed=ef;
    ef=n;
  }

  n=ef-ed;

  Ink(ed,rd,vd,bd);
  Ink(ef,rf,vf,bf);
  rr=(rf-rd)/(float)n;
  rv=(vf-vd)/(float)n;
  rb=(bf-bd)/(float)n;
  for(i=1;i<n;i++) Ink(ed+i,rd+rr*i,vd+rv*i,bd+rb*i);
}

void GenereInk(void)
{
  byte r1,v1,b1,
       r2,v2,b2,
       r3,v3,b3;

  //4 degrades de couleurs
  r1=16+random(16);
  v1=16+random(16);
  b1=16+random(16);
  Degrade(0,0,0,0,63,r1,v1,b1);
  r2=r1+random(16);
  v2=v1+random(16);
  b2=b1+random(16);
  Degrade(63,r1,v1,b1,127,r2,v2,b2);
  r3=r2+random(16);
  v3=v2+random(16);
  b3=b2+random(16);
  Degrade(127,r2,v2,b2,191,r3,v3,b3);
  Degrade(191,r3,v3,b3,255,r3+random(16),v3+random(16),b3+random(16));
}

void GenereMove(void)
{
  byte rx,ry,mx,my;
  word i;

  //Parcours aleatoires
  rx=100+random(40);
  ry=50+random(35);
  mx=1+random(2);
  my=1+random(2);

  for(i=0;i<360;i++)
  {
    Xmove[i]=160+cos(i*mx*(0.0175))*rx;
    Ymove[i]=100+sin(i*my*(0.0175))*ry;
  }
}

void Faisceau(word x,word y,bytef *src)
{
  word i,j,off;

  src+=(y<<8)+(y<<6)+x;

  for(j=0;j<HT;j++)
  {
    for(i=0;i<LG;i++) src[i]++; //Incrementer le pixel courant
    src+=320;
  }
}

void main(void)
{
  word i,j;

  ModeVGA();
  randomize();

  while(!kbhit())
  {
    ClrScr(Video);
    GenereInk();   //Genere 4 degrades aleatoires
    GenereMove();  //Genere un parcours aleatoire

    for(j=0;j<5;j++)
    {
      VBL;
      for(i=0;i<360;i++) Faisceau(Xmove[i],Ymove[i],Video);
    }
  }

  ModeTxt();
}
