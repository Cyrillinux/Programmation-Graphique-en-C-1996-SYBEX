/*************************************************************/
// TITRE  : LISSAGE, METHODE GENERALE
// AUTEUR : Walter SIRACUSA/wiSdom
// PROJET : LISS1.C, VGA.C, VGA_ASM.ASM
/*************************************************************/

#include <vga.h>
#include <conio.h>
#include <stdlib.h>

byte Pal[768];
bytef Page1[64000];
bytef *Video=MK_FP(0xa000,0);

void Lisse(bytef *pg_s,bytef *pg_d,byte *pl);
void main(void);

void Lisse(bytef *pg_s,bytef *pg_d,byte *pl)
{
  byte j,r0,v0,b0; //Intensites moyennes
  word i,k0=320,k1=0,k3=640,
       p0,p1,p2,p3,p4, //Valeur des pixels d'approximites de p0
       b,e,s,s0;

  for(j=1;j<199 && !kbhit();j++)
  {
    for(i=1;i<319;i++)
    {
      //Lecture des 5 pixels
      p0=pg_s[i+k0]*3;
      p1=pg_s[i+k1]*3;
      p2=pg_s[i+1+k0]*3;
      p3=pg_s[i+k3]*3;
      p4=pg_s[i-1+k0]*3;

      //Moyenne des RVB
      r0=(pl[p0]+pl[p1]+pl[p2]+pl[p3]+pl[p4])/5;
      v0=(pl[p0+1]+pl[p1+1]+pl[p2+1]+pl[p3+1]+pl[p4+1])/5;
      b0=(pl[p0+2]+pl[p1+2]+pl[p2+2]+pl[p3+2]+pl[p4+2])/5;

      //Recherche de la couleur moyenne
      s=189;
      for(b=0;b<768;b+=3)
      {
        s0=abs(pl[b]-r0)+abs(pl[b+1]-v0)+abs(pl[b+2]-b0);
        if(s0<=s)
        {
          s=s0;
          e=b;
        }
      }

      //Nouvelle couleur courante
      pg_d[i+k0]=e/3;
    }

    k0+=320;
    k1+=320;
    k3+=320;
  }
}

void main(void)
{
  byte i;

  ModeVGA();
  LoadPCX("land",Page1,Pal);
  SetAllPal(Pal);
  CpyScr(Page1,Video);

  Lisse(Page1,Video,Pal);

  for(i=0;i<4;i++)
  {
    getch();
    SwpScr(Page1,Video);
  }

  getch();
  ModeTxt();
}
