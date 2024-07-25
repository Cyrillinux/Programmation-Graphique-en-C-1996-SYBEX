/*************************************************************/
// TITRE  : FEU FLAMBOYANT
// AUTEUR : Walter SIRACUSA/wiSdom
// PROJET : FIRE.C, VGA.C, VGA_ASM.ASM
/*************************************************************/

#include <vga.h>
#include <conio.h>
#include <stdlib.h>
#include <dos.h>

byte Page1[320*81],Page2[320*81]; //Pages pour le scrolling du feu
byte *P1=Page1,*P2=Page2;         //Pointeurs sur les 2 pages
byte *s,      //Source ScrollFire()
     *d;      //Destination ScrollFire()
dword *src;   //Source ViewFire()

dwordf *dst=MK_FP(0xa000,39360); //Destination ViewFire(), (200 - 77) * 320

void Degrade(byte ed,byte rd,byte vd,byte bd,byte ef,byte rf,byte vf,byte bf);
void ScrollFire(void);
void GenereFire(void);
void ViewFire(void);
void main(void);

void Degrade(byte ed,byte rd,byte vd,byte bd,byte ef,byte rf,byte vf,byte bf)
{
  byte i,n=ef-ed;
  float rr,rv,rb;

  Ink(ed,rd,vd,bd);
  Ink(ef,rf,vf,bf);
  rr=(rf-rd)/(float)n;
  rv=(vf-vd)/(float)n;
  rb=(bf-bd)/(float)n;
  for(i=1;i<n;i++) Ink(ed+i,rd+rr*i,vd+rv*i,bd+rb*i);
}

void ScrollFire(void)
{
  register word i;

  s=P1+321; //P1(1,1)
  d=P2+1;   //P2(1,0)

  for(i=0;i<25280;i++) //320*79 = 25280
  {
    *d=(*(s-321)+(*(s-320))+(*(s-319))+(*(s-1))+ //Moyenne de 8 points
       (*(s+1))+(*(s+319))+(*(s+320))+(*(s+321)))>>3;

    s++; //Point suivant
    d++;
  }
}

void GenereFire(void)
{
  register word n,i;
  byte r,*d=P2+25280; //Point i,79

  //Petits foyers
  for(i=0;i<320;i++) d[i]=32+random(32);

  //Grands foyers
  r=10+random(11);
  for(i=0;i<r;i++)  //Nombre aleatoire de grands foyers avec 10 <=r< 20
  {
    d=P2+random(310); //Point aleatoire: Point x,78 avec 0 <=x< 320

    *(d+25279)=255;  //i-1,79
    *(d+25281)=255;  //i+1,79
  }
}

void ViewFire(void)
{
  byte *p;  //Pointeur auxilliaire
  register word i;

  src=(unsigned long *)P1;

  //Copie lignes [0-76] <=> (77 * 320) / 4
  for(i=0;i<6160;i++) dst[i]=src[i];

  p=P1;   //Echange les pointeurs de pages
  P1=P2;
  P2=p;
}

void main(void)
{
  ModeVGA();

  //Degrades de couleurs
  Degrade(0,0,0,0,15,0,0,20);
  Degrade(15,0,0,20,31,50,0,0);
  Degrade(31,50,0,0,47,63,40,0);
  Degrade(47,63,40,0,63,63,63,20);
  Degrade(63,63,63,20,255,63,63,63);

  while(!kbhit())
  {
    GenereFire();  //Genere le feu
    ScrollFire();  //Scrolling haut avec moyenne
    ViewFire();    //Affiche le feu et echange de pages
  }

  ModeTxt();
}
