/*************************************************************/
// TITRE  : LISSAGE PAR NIVEAU DE COULEURS
// AUTEUR : Walter SIRACUSA/wiSdom
// PROJET : LISS2.C, VGA.C, VGA_ASM.ASM
/*************************************************************/

#include <vga.h>
#include <conio.h>

byte Pal[768];
bytef Page1[64000];
bytef *Video=MK_FP(0xa000,0);

void LisseN(bytef *pg_s,bytef *pg_d);
void main(void);

void LisseN(bytef *pg_s,bytef *pg_d)
{
  byte j,p0,p1,p2,p3,p4,m; //Valeur des pixels autour de p0
  word i,k0=320,k1=0,k3=640;

  for(j=1;j<199 && !kbhit();j++)
  {
    for(i=1;i<319;i++)
    {
      p0=pg_s[i+k0]; //Lecture des 5 pixels
      p1=pg_s[i+k1];
      p2=pg_s[i+1+k0];
      p3=pg_s[i+k3];
      p4=pg_s[i-1+k0];

      m=(p0+p1+p2+p3+p4)/5; //Pixel moyen
      pg_d[i+k0]=m;
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
  LoadPCX("cygne_g",Page1,Pal);
  SetAllPal(Pal);
  CpyScr(Page1,Video);

  getch();
  for(i=0;i<5;i++)
  {
    LisseN(Page1,Video);
    CpyScr(Video,Page1);
  }

  getch();
  ModeTxt();
}
