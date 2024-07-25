/*************************************************************/
// TITRE  : SCROLLING SUR QUATRE PAGES AVEC SPRITES
// AUTEUR : Walter SIRACUSA/wiSdom
// PROJET : FULL.C, VGA.C, VGA_ASM.ASM
/*************************************************************/

#include <vga.h>
#include <conio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define LG 15     //Largeur d'un sprite
#define HT 15     //Hauteur d'un sprite
#define N_SPR 15  //Nombre de sprites

//MOUVEMENT DU SCROLLING
word  Cx[360];
bytef Cy[360];

//SCROLLING
byte Pal[768];    //Palette
bytef Pg1[64000]; //Page 1
bytef Pg2[64000]; //Page 2
bytef Pg3[64000]; //Page 3
bytef Pg4[64000]; //Page 4
bytef Pgx[64000]; //Page auxilliaire

bytef *Video=MK_FP(0xa000,0);

//SPRITES
bytef Spr[4][LG*HT];        //4 types de sprites de 15x15
byte Bxs[N_SPR],Bys[N_SPR], //Booleen
     Vs[N_SPR],Ts[N_SPR];   //Vitesse et type
int Xs[N_SPR],Ys[N_SPR];    //Position

void main(void)
{
  word s,i=270,
       x,y,      //Position du scrolling: 0 <=x<= 320 et  0 <=y<= 200
       lg,ht;

  ModeVGA();
  LoadPCX("scroll",Pgx,Pal);
  SetAllPal(Pal);

  //Precalcule le mouvement du scrolling
  for(x=0;x<360;x++)
  {
    Cx[x]=160+cos(360-(x*0.01745*2))*161;
    Cy[x]=100+sin(360-(x*0.01745))*101;
  }

  //Initialisation des sprites
  for(s=0;s<4;s++) GetBlk1(24+s*16,1,LG,HT,Pgx,Spr[s]);  //Capture
  randomize();
  for(s=0;s<N_SPR;s++)
  {
    Bxs[s]=random(2);    //Booleen pour le rebondissement
    Bys[s]=random(2);
    Xs[s]=5+random(300); //Position
    Ys[s]=5+random(180);
    Ts[s]=random(4);     //Type
    Vs[s]=1+random(3);   //Vitesse
  }

  //Construction des 4 pages
  for(y=0;y<200;y+=20)
    for(x=0;x<320;x+=20)
    {
      CpyBlk1(0,21*GetPixel(1+x/20,44+y/20,Pgx),20,20,x,y,Pgx,Pg1);
      CpyBlk1(0,21*GetPixel(17+x/20,44+y/20,Pgx),20,20,x,y,Pgx,Pg2);
      CpyBlk1(0,21*GetPixel(1+x/20,54+y/20,Pgx),20,20,x,y,Pgx,Pg3);
      CpyBlk1(0,21*GetPixel(17+x/20,54+y/20,Pgx),20,20,x,y,Pgx,Pg4);
    }

  while(!kbhit())
  {
    //MOUVEMENT
    if(++i==360) i=0;
    x=Cx[i];
    y=Cy[i];

    //SCROLLING
    lg=320-x;    //Abscisse/Largeur
    ht=200-y;    //Ordonn‚e/Hauteur

    CpyBlk(x,y,lg,ht,0,0,Pg1,Pgx);
    CpyBlk(0,y,x,ht,lg,0,Pg2,Pgx);
    CpyBlk(x,0,lg,y,0,ht,Pg3,Pgx);
    CpyBlk(0,0,x,y,lg,ht,Pg4,Pgx);

    //SPRITES
   for(s=0;s<N_SPR;s++)
   {
      if(Bxs[s]) Xs[s]+=Vs[s];    //Deplacement
      else Xs[s]-=Vs[s];
      if(Bys[s]) Ys[s]+=Vs[s];
      else Ys[s]-=Vs[s];

      if(Xs[s]>300) Bxs[s]=0;     //Rebondissement
      else if(Xs[s]<5) Bxs[s]=1;
      if(Ys[s]>180) Bys[s]=0;
      else if(Ys[s]<5) Bys[s]=1;

      PutSpr(Xs[s],Ys[s],LG,HT,Pgx,Spr[Ts[s]]);  //Affichage
    }

    VBL;
    CpyScr(Pgx,Video);  //Affichage scrolling avec sprites
  }

  ModeTxt();
}
