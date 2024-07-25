/*************************************************************/
// TITRE  : AFFICHAGE D'UNE IMAGE PAR DEROULEMENT
// AUTEUR : Walter SIRACUSA/wiSdom
// PROJET : ROULO.C, VGA.C, VGA_ASM.ASM
/*************************************************************/

#include <vga.h>
#include <conio.h>

bytef Page1[64000];
bytef Pal[768];
bytef *Video=MK_FP(0xa000,0);

byte Roulo[23]={62,58,54,50,46,43,40,37,35,33,32,
                31,30,29,27,25,22,19,16,12,8,4,0};

void AffRoulo(void);
void main(void);

void AffRoulo(void)
{
  word yr,y,i;

  for(i=0;i<23;i++) Roulo[i]+=23; //Ajoute l'epaisseur

  for(y=0;y<200;y++)
  {
    VBL;
    for(i=0;i<23;i++)
    {
      yr=y+Roulo[i];
      if(yr<200) CpyBnd(yr,1,y+i,Page1,Video);
    }
    CpyBnd(y,1,y,Page1,Video);
  }
}

void main(void)
{
  ModeVGA();

  LoadPCX("souris2",Page1,Pal);
  SetAllPal(Pal);

  AffRoulo();

  getch();
  ModeTxt();
}
