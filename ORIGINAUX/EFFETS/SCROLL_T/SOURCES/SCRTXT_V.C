/*************************************************************/
// TITRE  : SCROLLTEXT VERTICAL
// AUTEUR : Walter SIRACUSA/wiSdom
// PROJET : SCRTXT_V.C, VGA.C, VGA_ASM.ASM
/*************************************************************/

#include <vga.h>
#include <conio.h>

#define HT 26 //Hauteur d'un caractere
#define LG 26 //Largeur d'un caractere

bytef Page1[64000];  //Buffer auxilliaire
bytef Page2[64000];  //Buffer de la fonte
bytef Pal[768];      //Palette de la fonte

byte Chaine[]="  PROGRAMS   SIRACUSA W.              ROUTINES     WISDOM                 GRAPHICS   SIRACUSA W.                        ";
word Xc,Yc,           //Abscisse et ordonnee d'une lettre
     Tx[256],Ty[256]; //Abscisses et ordonnees precalculees

bytef *Video=MK_FP(0xa000,0);

void Lettre(byte car);
void CalculLettre(void);
void main(void);

void Lettre(byte car)
{
  if(car>='A' && car<='K')
  {
    Xc=(car-'A')*(LG+1);
    Yc=0;
  }
  else if(car>='L' && car<='V')
  {
    Xc=(car-'L')*(LG+1);
    Yc=27;
  }
  else if(car>='W' && car<='Z')
  {
    Xc=(car-'W')*(LG+1);
    Yc=54;
  }
  else
  {
    Yc=54;
    if(car==39) Xc=108;        //Apostrophe
    else if(car==',') Xc=135;  //Virgule
    else if(car=='.') Xc=162;  //Point
    else if(car==' ') Xc=189;  //Espace
  }
}

void CalculLettre(void)
{
  word i;

  for(i=0;i<256;i++)
  {
    Lettre(i);  //Fixe Xc et Yc
    Tx[i]=Xc;
    Ty[i]=Yc;
  }
}

void main(void)
{
  byte car,      //Caractere courant
       x,        //Compteurs des colonnes
       y;        //Compteur des lignes
  word dx,       //Position du caractere a l'ecran
       i_car=0,  //Indice du caractere courant
       lg=0;     //Longueur du texte

  ModeVGA();
  LoadPCX("scrtxt",Page2,Pal);
  SetAllPal(Pal);

  CalculLettre();      //Precalcule les abscisses et les ordonnees
  while(Chaine[lg++]); //Longueur du texte pour le rebouclage
  lg-=(12+2);          //Ne traite pas la derniere ligne et '\0'

  while(!kbhit())
  {
    for(y=0;y<HT;y+=2)  //Hauteur d'un caractere
    {
      CpyBnd(2,198,0,Page1,Page1);  //Scrolling haut de 2 pixels

      dx=0;
      for(x=0;x<12;x++) //Ligne de 12 caracteres affichables a l'ecran
      {
        car=Chaine[i_car+x];
        CpyBlk2(Tx[car],Ty[car]+y,LG,2,dx,198,Page2,Page1);
        dx+=LG;         //Caractere suivant a l'ecran
      }

      VBL;
      CpyScr(Page1,Video);
    }
    if(i_car<lg) i_car+=12;   //Changement de ligne du texte
    else i_car=0;             //Rebouclage du texte
  }

  ModeTxt();
}
