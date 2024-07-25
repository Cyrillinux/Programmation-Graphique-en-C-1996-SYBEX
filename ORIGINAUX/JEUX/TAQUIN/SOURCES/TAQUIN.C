/*************************************************************/
// TITRE  : TAQUIN
// AUTEUR : Walter SIRACUSA/wiSdom
// PROJET : TAQUIN.C, VGA.C, VGA_ASM.ASM
/*************************************************************/

#include <vga.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <alloc.h>

#define LG 40         //Largeur d'un bloc
#define HT 40         //Hauteur d'un bloc
#define ESC 27
#define DROITE 77
#define GAUCHE 75
#define HAUT 72
#define BAS 80

#define MELANGE 200  //Nombre de melange
#define VIDE 0       //Bloc vide

byte Pal[768];
bytef Page1[64000];
byte Taquin[4][4];    //160 / LG = 4, 160 / HT = 4
bytef Blk[16][LG*HT]; //4 * 4 = 16 blocs de l'image

bytef *Video=MK_FP(0xa000,0);

void main(void);
void LoadTaquin(void);
void InitTaquin(char *x,char *y);
void AffTaquin(void);
void DepBloc(char x,char y,char sx,char sy);

void LoadTaquin(void)
{
  byte i,j;

  LoadPCX("taquin",Page1,Pal); //Lecture de la page des motifs
  SetAllPal(Pal);
  FBox(80,20,LG,HT,Page1,0);  //Efface le bloc VIDE
  Box(79,19,162,162,Page1,7); //Contour du taquin
  Box(78,18,164,164,Page1,7); //Contour du taquin

  //Memorise les blocs, Blk[VIDE] = bloc vide
  for(j=0;j<4;j++)
    for(i=0;i<4;i++)
      GetBlk4(80+i*LG,20+j*HT,LG,HT,Page1,Blk[i+j*4]);
}

void InitTaquin(char *x,char *y)
{
  char i,j,xt,yt;
  byte fnd,k=0;

  //Initialise le tableau
  for(j=0;j<4;j++)
    for(i=0;i<4;i++)
      Taquin[i][j]=k++;

  randomize();

  for(k=0;k<MELANGE;k++)
  {
    //Recherche la position du bloc VIDE
    fnd=0;
    for(j=0;j<4;j++)
    {
      for(i=0;i<4;i++)
        if(Taquin[i][j]==VIDE)
        {
          fnd=1;
          break;
        }
      if(fnd) break;
    }

    //Selectionne un bloc d'approximite du bloc VIDE
    fnd=0;
    while(!fnd)
    {
      if(random(2))
      {
        xt=i+(1-random(3));
        yt=j;
      }
      else
      {
        xt=i;
        yt=j+(1-random(3));
      }
      if(xt>=0 && xt<4 && yt>=0 && yt<4) fnd=1;
    }

    //Deplace le bloc d'approximite
    Taquin[i][j]=Taquin[xt][yt];
    Taquin[xt][yt]=VIDE;
  }

  //Renvoie la position du bloc vide
  *x=xt;
  *y=yt;
}

void AffTaquin(void)
{
  byte i,j;

  for(j=0;j<4;j++)
    for(i=0;i<4;i++)
      PutBlk4(80+i*LG,20+j*HT,LG,HT,Page1,Blk[Taquin[i][j]]);
  CpyBlk4(80,20,160,160,80,20,Page1,Video);
}

void DepBloc(char x,char y,char sx,char sy)
{
  word i,j,k;

  AffTaquin();

  i=80+x*40; //Position du bloc a l'ecran
  j=20+y*40;

  for(k=0;k<40;k++)
  {
    Box(i,j,LG,HT,Page1,0); //Efface la position precedente
    if(!sy)  i+=sx; //Deplace le bloc horizontalement
    else j+=sy;    //Deplace le bloc verticalement
    PutBlk4(i,j,LG,HT,Page1,Blk[Taquin[x][y]]); //Affiche le bloc
    CpyBlk4(80,20,160,160,80,20,Page1,Video);
  }
}

void main(void)
{
  int key;    //Lecture du clavier
  byte i,j,k,
       win;   //Booleen de fin
  char xt,yt; //Position du bloc VIDE

  ModeVGA();

  LoadTaquin();
  InitTaquin(&xt,&yt);
  CpyScr(Page1,Video);
  getch();

  do
  {
    AffTaquin();

    //Test le taquin
    win=1;
    k=0;
    for(j=0;j<4;j++)
      for(i=0;i<4;i++)
        if(Taquin[i][j]!=k++)
        {
          win=0;
          break;
        }

    //Deplace le bloc selon la direction
    key=getch();
    switch(key)
    {
      case DROITE : if(xt>0)
                    {
                      DepBloc(xt-1,yt,1,0);
                      Taquin[xt][yt]=Taquin[xt-1][yt];
                      Taquin[xt-1][yt]=VIDE;
                      xt--;
                    }break;

      case GAUCHE : if(xt<3)
                    {
                      DepBloc(xt+1,yt,-1,0);
                      Taquin[xt][yt]=Taquin[xt+1][yt];
                      Taquin[xt+1][yt]=VIDE;
                      xt++;
                    }break;

      case BAS    : if(yt>0)
                    {
                      DepBloc(xt,yt-1,0,1);
                      Taquin[xt][yt]=Taquin[xt][yt-1];
                      Taquin[xt][yt-1]=VIDE;
                      yt--;
                    }break;

      case HAUT   : if(yt<3)
                    {
                      DepBloc(xt,yt+1,0,-1);
                      Taquin[xt][yt]=Taquin[xt][yt+1];
                      Taquin[xt][yt+1]=VIDE;
                      yt++;
                    }break;

    }
  }while(key!=ESC && !win);

  ModeTxt();
  if(win) puts("\nExcellent !");

  textcolor(LIGHTBLUE);
  cputs("TAQUIN par Walter Siracusa/wiSdom\n");
  textcolor(LIGHTGRAY);
}
