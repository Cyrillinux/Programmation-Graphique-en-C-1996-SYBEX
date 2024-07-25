/*************************************************************/
// TITRE  : SCROLLTEXT SUR UN RELIEF
// AUTEUR : Walter SIRACUSA/wiSdom
// PROJET : SCRTXT_R.C, VGA.C, VGA_ASM.ASM
/*************************************************************/

#include <vga.h>
#include <conio.h>

#define LG 26  //Largeur d'un caractere
#define HT 22  //Hauteur d'un caractere

bytef Page1[64000];  //Buffer de la fonte
bytef Pal[768];      //Palette de la fonte
byte Yr[320];        //Ordonn‚es du relief

bytef Chaine[]="SCROLLTEXT SUR UN RELIEF ... PROGRAMME PAR SIRACUSA WALTER DE WISDOM SOFTWARE ...          ";
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
       x=0,     //Indice de colonne des zones de caracteres
       y;       //Ordonnee du relief
  word i,       //Compteur de recheche des ordonnees
       i_car=0, //Indice du caractere courant de la chaine
       lg=0;    //Longueur du texte

  ModeVGA();
  for(i=0;i<256;i++) Ink(i,0,0,0);
  LoadPCX("scrtxt",Page1,Pal);   //Charge la fonte
  LoadPCX("scrtxt_r",Video,Pal); //Charge le relief
  SetAllPal(Pal);                //Installe les couleurs

  //Memorise le relief
  for(i=4;i<172;i++)  //Avance dans les abscisses
  {
    y=0;
    while(!GetPixel(i,y++,Video));  //Recherche l'ordonnee
    Yr[i]=y-HT; //Memorise l'ordonnee
  }

  CalculLettre();      //Precalcule les abscisses et les ordonnees
  while(Chaine[lg++]); //Longueur du texte pour terminer
  lg--;                //Ne traite pas '\0'

  while(i_car<lg && !kbhit()) //Quitte en fin de texte ou par une touche
  {
     //Defilement horizontal
     car=Chaine[i_car];
     CpyBlk2(2,82,318,HT,0,82,Page1,Page1); //Scrolling de 2 colonnes
     CpyBlk2(Tx[car]+x,Ty[car],2,HT,318,82,Page1,Page1); //2 colonnes

     //Affichage suivant le relief (par blocs de 2)
     VBL;
     for(i=4;i<172;i++)
       CpyBlk2(i,82,2,HT,i,Yr[i],Page1,Video);

     if((x+=2)==LG) //Pas du scrolling = 2 pixels
     {
       x=0;     //Pointe en debut de zone
       i_car++; //Passe au caractere suivant
     }
  }

  ModeTxt();
}
