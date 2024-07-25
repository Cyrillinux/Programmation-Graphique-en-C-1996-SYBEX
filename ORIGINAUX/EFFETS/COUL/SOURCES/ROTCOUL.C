/*************************************************************/
// TITRE  : ROTATION DE COULEURS
// AUTEUR : Walter SIRACUSA/wiSdom
// PROJET : ROTCOUL.C, VGA.C, VGA_ASM.ASM
/*************************************************************/

#include <vga.h>
#include <conio.h>

byte Pal[768]; //Palette de couleurs
bytef *Video=MK_FP(0xa000,0);

void RotateInk(void);
void main(void);

void RotateInk(void)
{
  static word ie=3;
  word e;

  for(e=1;e<=199;e++) //Rotation des encres 1(e_deb) a 199(e_fin)
  {
    Ink(e,Pal[ie],Pal[ie+1],Pal[ie+2]);
    ie+=3;
    if(ie>597) ie=3;  //if(ie>e_fin*3) ie=e_deb*3
  }
  ie+=3;
  if(ie>597) ie=3;
}

void main(void)
{
  word i;

  ModeVGA();

  //Degrade noir(0) a violet(99)
  for(i=1;i<100;i++) SetInk(i,i*(63/100.0),0,i*(63/100.0),Pal);
  
  //Degrade violet(100) a noir(199)
  for(i=100;i<200;i++) SetInk(299-i,(i-100)*(63/100.0),0,(i-100)*(63/100.0),Pal);

  //Affiche 200 lignes contenant le degrade
  for(i=1;i<200;i++) Line(0,i,319,i,Video,i);

  while(!kbhit())
  {
    VBL;
    RotateInk();  //Rotation de couleurs
  }

  ModeTxt();
}
