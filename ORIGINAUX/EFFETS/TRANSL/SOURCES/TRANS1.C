/*************************************************************/
// TITRE  : TRANSLUCIDITE PAR TRANSLATION
// AUTEUR : SIRACUSA WALTER/wiSdom
// PROJET : TRANS1.C, VGA.C, VGA_ASM.ASM
/*************************************************************/

#include <vga.h>
#include <conio.h>

#define DROITE 77
#define GAUCHE 75
#define HAUT 72
#define BAS  80
#define ESC 27

#define LG 64  //Dimension du bloc
#define HT 64

byte Pal[768];
bytef Page1[64000];
bytef Page2[64000];
bytef *Video=MK_FP(0xa000,0);

void main(void)
{
  word i,j,off1,off2;
  int key,xf=126,yf=66;
  byte fin=0;

  printf("\nLorsque l'image apparaŒt, utilisez les touches fl‚ch‚es.");
  printf("\n< UNE TOUCHE >\n");
  getch();

  ModeVGA();
  LoadPCX("damier",Page1,Pal);
  SetAllPal(Pal);

  do
  {
    CpyScr(Page1,Page2);

    //Affiche un bloc de LGxHT en transparence
    off1=(yf<<8)+(yf<<6)+xf;
    off2=off1+LG;
    for(j=0;j<HT;j++)
    {
      for(i=off1;i<off2;i++) Page2[i]=Page1[i]+32;
      off1+=320;
      off2+=320;
    }

    CpyScr(Page2,Video);

    //Deplacement du bloc au clavier
    key=getch();
    switch(key)
    {
      case DROITE : if(xf<320-LG) xf+=2;break;
      case GAUCHE : if(xf>1) xf-=2;break;
      case HAUT   : if(yf>1) yf-=2;break;
      case BAS    : if(yf<200-HT) yf+=2;break;
      case ESC    : fin=1;
    }
  }
  while(!fin);

  ModeTxt();
}
