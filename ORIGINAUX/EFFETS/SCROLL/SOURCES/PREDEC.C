/*************************************************************/
// TITRE  : SCROLLING PREDECALE
// AUTEUR : Walter SIRACUSA/wiSdom
// PROJET : PREDEC.C, VGA.C, VGA_ASM.ASM
/*************************************************************/

#include <vga.h>
#include <conio.h>

#define LG 16  //Largeur du motif
#define HT 10  //Hauteur du motif

byte Pal[768];      //Palette
bytef Page1[64000]; //Page temporaire contenant les fresques de motifs
bytef Mtf[LG*HT];   //Buffer du motif
bytef *Video=MK_FP(0xa000,0);

void main(void)
{
  byte i,j;

  ModeVGA();
  LoadPCX("scroll",Page1,Pal);
  SetAllPal(Pal);

  //Capture du motif
  GetBlk1(1,67,LG,HT,Page1,Mtf);
  ClrScr(Page1);

  //Contruction de la fresque principale
  for(i=0;i<320/LG;i++) PutBlk1(i*LG,0,LG,HT,Page1,Mtf);

  //Contruction des fresques decalees
  for(i=1;i<LG;i++)
  {
    CpyBlk1(i,0,320-i,HT,0,HT*i,Page1,Page1);  //Decalage par rapport a ...
    CpyBlk1(0,0,i,HT,320-i,HT*i,Page1,Page1);  //... la premiere fresque
  }

  while(!kbhit())
  {
    for(i=0;i<160;i+=HT) //LG x HT = 160
    {
      VBL;
      for(j=0;j<200;j+=HT) //20 fresques
        CpyBnd(i,HT,j,Page1,Video);
    }
  }

  getch();
  ModeTxt();
}