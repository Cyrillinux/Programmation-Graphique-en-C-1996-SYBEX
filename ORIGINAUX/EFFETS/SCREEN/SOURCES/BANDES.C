/*************************************************************/
// TITRE  : AFFICHAGE D'UNE IMAGE PAR GLISSEMENT DE BANDES
// AUTEUR : Walter SIRACUSA/wiSdom
// PROJET : BANDES.C, VGA.C, VGA_ASM.ASM
/*************************************************************/

#include <vga.h>
#include <conio.h>

byte Pal[768];
bytef Page1[64000];
bytef *Video=MK_FP(0xa000,0);

void AffBandes(void);
void main(void);

void AffBandes(void)
{
  word x; //Abscisse courante
  byte h; //Hauteur courante

  for(h=1;h<200;h+=2)
  {
    VBL;
    for(x=0;x<320;x+=16)
      CpyBlk4(x,199-h,8,h,x,0,Page1,Video); //Bandes descendantes
    for(x=8;x<320;x+=16)
      CpyBlk4(x,0,8,h,x,199-h,Page1,Video); //Bandes montantes
  }
  CpyScr(Page1,Video);
}

void main(void)
{
  ModeVGA();
  LoadPCX("souris2",Page1,Pal);
  SetAllPal(Pal);

  AffBandes();

  getch();
  ModeTxt();
}
