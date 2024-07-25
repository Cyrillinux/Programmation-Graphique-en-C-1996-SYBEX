/*************************************************************/
// TITRE  : LISSAGE LINEAIRE
// AUTEUR : Walter SIRACUSA/wiSdom
// PROJET : LISS3.C, VGA.C, VGA_ASM.ASM
/*************************************************************/

#include <vga.h>
#include <conio.h>

byte Pal[768];
bytef Page1[64000];
bytef *Video=MK_FP(0xa000,0);

void LisseL(bytef *pg_s,bytef *pg_d);
void main(void);

void LisseL(bytef *pg_s,bytef *pg_d)
{
  byte j,e1,e2,e3;
  word i,k=0,tmp;

  for(j=0;j<200;j++)
  {
    e1=pg_s[k-1];
    e2=pg_s[k];
    for(i=1;i<320;i++)
    {
      tmp=i+k;
      e3=pg_s[tmp];
      pg_d[tmp-1]=(e1+e2+e3)/3;
      e1=e2;
      e2=e3;
    }
    k+=320;
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
  for(i=0;i<8;i++)
  {
    LisseL(Page1,Video);
    CpyScr(Video,Page1);
  }

  getch();
  ModeTxt();
}
