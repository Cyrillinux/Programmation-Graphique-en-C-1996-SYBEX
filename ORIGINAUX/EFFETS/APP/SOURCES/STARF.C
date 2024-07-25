/*************************************************************/
// TITRE  : CHAMP D'ETOILES AVEC ROTATION
// AUTEUR : Walter SIRACUSA/wiSdom
// PROJET : STARF.C, VGA.C, VGA_ASM.ASM
/*************************************************************/

#include <vga.h>
#include <conio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define ESC 27

#define NB_STAR 800  //Nombre de pixels
#define ECHELLE 150  //Coefficient de grandeur du repere
#define DB_INK 64    //Debut des couleurs dans la palette
#define NB_INK 192   //Nombre de couleurs

short far X[NB_STAR],Y[NB_STAR],   //Coordonnees d'origines
          Z[NB_STAR],              //Profondeurs
          X1[NB_STAR],Y1[NB_STAR]; //Coordonnees courantes

bytef *Video=MK_FP(0xa000,0);

void InitStar(void);
void main(void);

void InitStar(void)
{
  byte c=63;
  word i;

  Ink(0,0,0,0);
  //Initialise les couleurs (intensites decroissantes)
  for(i=0;i<(NB_INK-2);i+=3)
  {
    Ink(DB_INK+i,c,c,c);
    Ink(DB_INK+i+1,c,c,c);
    Ink(DB_INK+i+2,c,c,c);
    c--;
  }

  randomize();

  //Initialise les points
  for(i=0;i<NB_STAR;i++)
  {
    X1[i]=0; //Position courante
    Y1[i]=0;
    X[i]=(random(MAXX)-OX)*ECHELLE; //Position d'origine
    Y[i]=(random(MAXY)-OY)*ECHELLE;
    Z[i]=1+random(NB_INK); //Profondeur
  }
}

void main(void)
{
  word i,off;
  int key;
  short tmp;
  float cn=cos(0.0175),
        sn=sin(0.0175);

  puts("\n[1] STARFIELD NORMAL");
  puts("[2] STARFIELD AVEC ROTATION");
  puts("[ESC] QUITTER");
  key=getch();

  ModeVGA();
  InitStar();

  do
  {
    if(kbhit())  key=getch();

    VBL;
    for(i=0;i<NB_STAR;i++)
    {
      //Active le champ d'etoiles
      Z[i]--;
      if(!Z[i]) Z[i]=NB_INK; //En realite, NB_COUL-1

      //Efface les points precedents dans la zone ecran
      if(X1[i]>0 && X1[i]<MAXX && Y1[i]>0 && Y1[i]<MAXY)
      {
        off=Y1[i]*320+X1[i];
        *(Video+off)=0;
      }

      //Deplace les points courants
      X1[i]=OX+(X[i]/Z[i]);
      Y1[i]=OY+(Y[i]/Z[i]);

      //Ajoute une rotation (copro !)
      if(key=='2')
      {
        tmp=X[i]*cn-Y[i]*sn;
        Y[i]=X[i]*sn+Y[i]*cn;
        X[i]=tmp;
      }

      //Affiche les points dans la zone ecran
      if(X1[i]>0 && X1[i]<MAXX && Y1[i]>0 && Y1[i]<MAXY)  //Zone ecran
      {
        off=Y1[i]*320+X1[i];
        *(Video+off)=DB_INK+Z[i];
      }
    }
  }
  while(key!=ESC);

  ModeTxt();
}
