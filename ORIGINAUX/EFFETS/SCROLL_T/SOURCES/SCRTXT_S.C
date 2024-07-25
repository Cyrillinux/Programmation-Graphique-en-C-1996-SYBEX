/*************************************************************/
// TITRE  : SCROLLTEXT SINUSOIDAL
// AUTEUR : Walter SIRACUSA/wiSdom
// PROJET : SCRTXT_S.C, VGA.C, VGA_ASM.ASM
/*************************************************************/

#include <vga.h>
#include <conio.h>
#include <math.h>

#define LG 26  //Largeur d'un caractere
#define HT 22  //Hauteur d'un caractere

byte Pal[768];
bytef Page1[64000];
byte Dist[256];

bytef Chaine[]="EN PLUS DU DEFILEMENT HORIZONTAL, CE SCROLLTEXT SUBIT UNE DEFORMATION SINUSOIDALE ...           ";
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
       i=0,     //Angle de 0 … 256 degres
       x=0;     //Indice de colonne des zones de caracteres
  word y,       //Compteur pour les lignes
       i_car=0, //Indice du caractere courant de la chaine
       lg=0;    //Longueur du texte

  ModeVGA();
  LoadPCX("scrtxt",Page1,Pal);
  SetAllPal(Pal);

  //Precalcule la table des sinus pour la deformation sinusoidale
  for(y=0;y<256;y++)
    Dist[y]=3+((sin((y*(360.0/256))*0.017453293))*256)/85;

  CalculLettre();      //Precalcule les abscisses et les ordonnees
  while(Chaine[lg++]); //Longueur du texte pour terminer
  lg--;                //Ne traite pas '\0'

  while(i_car<lg && !kbhit())
  {
     //Defilement horizontal
     car=Chaine[i_car];
     CpyBlk4(10,82,308,HT,8,82,Page1,Page1); //Scrolling de 2 colonnes
     CpyBlk2(Tx[car]+x,Ty[car],2,HT,312,82,Page1,Page1); //2 colonnes

     //Deformation sinusoidale horizontale
     i++;
     VBL;
     for(y=82;y<109;y++) //HT lignes copiees avec 0 a 3 pixels de variation
       CpyBlk4(5,y,312,1,Dist[(i*6+y*18)&255],12+y,Page1,Video);

     if((x+=2)==LG) //Pas du scrolling = 2 pixels
     {
       x=0;     //Pointe en debut de zone
       i_car++; //Passe au caractere suivant
     }
  }

  ModeTxt();
}
