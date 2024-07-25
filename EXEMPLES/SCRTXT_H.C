/*************************************************************/
// TITRE  : SCROLLTEXT HORIZONTAL
// AUTEUR : Walter SIRACUSA/wiSdom
// PROJET : SCRTXT_H.C, VGA.C, VGA_ASM.ASM
/*************************************************************/

#include <vga.h>
#include <conio.h>
#include <stdlib.h>

#define LG 26  //Largeur d'un caractere
#define HT 22  //Hauteur d'un caractere

//bytef *Page1; // Pointeur vers le buffer de la fonte
//byte *Pal;   // Pointeur vers la palette de la fonte

bytef Page1[64000]; // Pointeur vers le buffer de la fonte
byte Pal[768];   // Pointeur vers la palette de la fonte

byte Chaine[]="EXEMPLE D'UN SCROLLTEXT FACILE A REALISER, IL S'AGIT DU SCROLLTEXT HORIZONTAL...               ";
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
  byte car,     //Caractere courant
       x=0;     //Indice de colonne des zones de caracteres
  word i_car=0, //Indice du caractere courant de la chaine
       lg=0;    //Longueur du texte

    if (Page1 == NULL || Pal == NULL) {
        perror("Erreur d'allocation de mémoire");
        return;
    }

  ModeVGA();
  LoadPCX("scrtxt", Page1, Pal); //Charge la fonte
  SetAllPal(Pal); //Installe les couleurs

  CalculLettre();      //Precalcule les abscisses et les ordonnees
  while(Chaine[lg++]); //Longueur du texte pour terminer
  lg--;                //Ne traite pas '\0'

  while(i_car<lg && !kbhit()) //Quitte en fin de texte ou par une touche
  {
    car=Chaine[i_car];
    CpyBlk2(2,95,318,HT,0,95,Page1,Page1); //Scrolling de 2 colonnes
    CpyBlk2(Tx[car]+x,Ty[car],2,HT,318,95,Page1,Page1); //2 colonnes

    VBL;
    CpyBnd(95,HT,95,Page1,Video);

    if((x+=2)==LG) //Pas du scrolling = 2 pixels
    {
      x=0;     //Pointe en debut de zone
      i_car++; //Passe au caractere suivant
    }
  }
  ModeTxt();
}
