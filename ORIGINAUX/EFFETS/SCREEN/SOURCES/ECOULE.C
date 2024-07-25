/*************************************************************/
// TITRE  : AFFICHAGE D'UNE IMAGE PAR ECOULEMENT DE LIGNES
// AUTEUR : Walter SIRACUSA/wiSdom
// PROJET : ECOULE.C, VGA.C, VGA_ASM.ASM
/*************************************************************/

#include <vga.h>
#include <conio.h>

byte Pal[768];
bytef Page1[64000];
bytef *Video=MK_FP(0xa000,0);

void AffEcoule(void);
void main(void);

void AffEcoule(void)
{
  byte l,y;

  for(l=0;l<200;l++)
  {
    VBL;
    for(y=1;y<200-l;y++)
      CpyBnd(199-l,1,y,Page1,Video);
  }
  CpyScr(Page1,Video);
}

void main(void)
{
  ModeVGA();
  LoadPCX("cygne2",Page1,Pal);
  SetAllPal(Pal);

  AffEcoule();

  getch();
  ModeTxt();
}
