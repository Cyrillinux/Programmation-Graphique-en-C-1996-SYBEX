/*************************************************************/
// TITRE  : AFFICHAGE D'UNE IMAGE PAR BLOCS ALEATOIRES
// AUTEUR : Walter SIRACUSA/wiSdom
// PROJET : BLOCS.C, VGA.C, VGA_ASM.ASM
/*************************************************************/

#include <vga.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>

byte Pal[768];
bytef Page1[64000];
bytef *Video=MK_FP(0xa000,0);

typedef struct _bloc //Bloc de dimension 10*10
{
  word x;
  byte y;
}t_bloc;
t_bloc Mtx[32][20];  //Matrice de l'image

void AffBlocs(void);
void main(void);

void AffBlocs(void)
{
  byte i,j,ir,jr;
  word aux;

  //Initialise la matrice
  for(j=0;j<20;j++)
    for(i=0;i<32;i++)
    {
      Mtx[i][j].x=i*10;
      Mtx[i][j].y=j*10;
    }

  //Melange la matrice
  randomize();
  for(j=0;j<20;j++)
    for(i=0;i<32;i++)
    {
      ir=random(20);    //Bloc aleatoire
      jr=random(32);

      //Echange avec un autre bloc
      aux=Mtx[i][j].x;  //Abscisse
      Mtx[i][j].x=Mtx[ir][jr].x;
      Mtx[ir][jr].x=aux;

      aux=Mtx[i][j].y;  //Ordonnee
      Mtx[i][j].y=Mtx[ir][jr].y;
      Mtx[ir][jr].y=aux;
    }

  //Affiche la matrice
  for(j=0;j<20;j++)
    for(i=0;i<32;i++)
    {
      VBL;
      CpyBlk2(Mtx[i][j].x,Mtx[i][j].y,10,10,Mtx[i][j].x,Mtx[i][j].y,Page1,Video);
    }
}

void main(void)
{
  ModeVGA();

  LoadPCX("cygne2",Page1,Pal);
  SetAllPal(Pal);
  CpyScr(Page1,Video);
  LoadPCX("souris2",Page1,Pal);

  AffBlocs();

  getch();
  ModeTxt();
}
