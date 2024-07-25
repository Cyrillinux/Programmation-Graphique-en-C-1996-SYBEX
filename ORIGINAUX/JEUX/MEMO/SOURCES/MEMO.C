/*************************************************************/
// TITRE  : MEMO
// AUTEUR : Walter SIRACUSA/wiSdom
// PROJET : MEMO.C, VGA.C, MOUSE.C, VGA_ASM.ASM
/*************************************************************/

#include <vga.h>
#include <mouse.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <alloc.h>

#define LG 40         //Largeur d'un motif
#define HT 40         //Hauteur d'un motif
#define ESC 27

#define ERROR 35      //Nombre d'erreur maxi.
#define VIEW -1       //Motif decouvert

#include "memo.img"   //Donnees de la page contenant l'image

byte Pal[768];
bytef *Page2;         //Page temporaire pour les motifs
bytef Mtf[21][LG*HT]; //1 motif principal et 20 autres differents
char Memo[8][5];      //320 / LG = 8, 200 / HT = 5

bytef *Video=MK_FP(0xa000,0);

void LoadMemo(void);
void main(void);
void InitMemo(void);
void AffMemo(void);

void LoadMemo(void)
{
  byte i,j,k=0;

  Page2=farmalloc(64000); //Alloue une page temporaire
  if(!Page2)
  {
    ModeTxt();
    puts("\Pas assez de memoire...\n");
    exit(0);
  }

  LoadPCX("memo",Page2,Pal); //Lecture de la page des motifs
  SetAllPal(Pal);

  //Memorise le motif principal et les 20 autres differents
  for(j=0;j<3;j++)
    for(i=0;i<7;i++)
      GetBlk4(i*LG,j*HT,LG,HT,Page2,Mtf[k++]);

  farfree(Page2); //Libere la memoire allouee
}

void InitMemo(void)
{
  byte i,j,k=1;
  char m1,m2;
  byte x1,y1,x2,y2;

  //Remplit le tableau par des paires de valeurs (de 1 … 20)
  for(j=0;j<5;j++)
    for(i=0;i<8;i++)
    {
      Memo[i][j]=k;
      if((i+j*20)&1) k++;
    }

  //Melange le tableau
  randomize();
  for(j=0;j<5;j++)
    for(i=0;i<8;i++)
    {
      x1=random(8);
      y1=random(5);
      m1=Memo[x1][y1];

      x2=random(8);
      y2=random(5);
      m2=Memo[x2][y2];

      Memo[x2][y2]=m1;
      Memo[x1][y1]=m2;
    }
}

void AffMemo(void)
{
  byte i,j;

  for(j=0;j<5;j++)
    for(i=0;i<8;i++)
      PutBlk4(i*LG,j*HT,LG,HT,Video,Mtf[0]);
}

void main(void)
{
  int key;     //Lecture du clavier au vol
  char m1,m2,  //Premier et deuxieme motif
       clic=0; //Deuxieme motif selectionne
  word xm,ym,  //Position de la souris
       xm1,ym1,xm2,ym2; //Position du premier et du deuxieme motif
  byte good=0,bad=0;

  ModeVGA();

  LoadMemo();
  InitMemo();
  InitMouse();
  AffMemo();
  MouseON();

  do
  {
    if(GetMouse(&xm,&ym)==1)
    {
      //1er motif (non decouvert)
      xm1=(xm/LG)*LG;
      ym1=(ym/HT)*HT;
      m1=Memo[xm1/LG][ym1/HT];

      if(m1!=VIEW)
      {
        MouseOFF();
        PutBlk4(xm1,ym1,LG,HT,Video,Mtf[m1]);
        while(GetMouse(&xm,&ym)==1); //Attend relachement
        MouseON();

        do
        {
          if(GetMouse(&xm,&ym)==1)
          {
            //2ie motif (different du premier et non decouvert)
            xm2=(xm/LG)*LG;
            ym2=(ym/HT)*HT;
            m2=Memo[xm2/LG][ym2/HT];

            if(!((xm2==xm1 && ym2==ym1) || m2==VIEW))
            {
              MouseOFF();
              PutBlk4(xm2,ym2,LG,HT,Video,Mtf[m2]);
              while(GetMouse(&xm,&ym)==1); //Attend relachement
              MouseON();
              clic=1;
            }
          }
          if(kbhit()) key=getch();
        }while(key!=ESC && clic!=1);
      }
    }

    //Comparaison des deux motifs
    if(clic==1)
    {
      if(m1==m2) //Motifs identiques
      {
        MouseOFF();
        CpyBlk4(xm1,ym1,LG,HT,xm1,ym1,Page1,Video);
        CpyBlk4(xm2,ym2,LG,HT,xm2,ym2,Page1,Video);
        MouseON();
        Memo[xm1/LG][ym1/HT]=VIEW;
        Memo[xm2/LG][ym2/HT]=VIEW;
        good++;
      }
      else       //Motifs differents
      {
        MouseOFF();
        PutBlk4(xm1,ym1,LG,HT,Video,Mtf[0]);
        PutBlk4(xm2,ym2,LG,HT,Video,Mtf[0]);
        MouseON();
        bad++;
      }
      clic=0;
    }

    if(kbhit())  key=getch();
  }while(key!=ESC && good!=20 && bad!=ERROR);

  if(good==20) getch();

  ModeTxt();
  if(good==20) puts("\nExcellent !");
  if(bad==ERROR) puts("\nTrop d'erreurs !");

  textcolor(LIGHTBLUE);
  cputs("MEMO par Walter Siracusa/wiSdom\n");
  textcolor(LIGHTGRAY);
}
