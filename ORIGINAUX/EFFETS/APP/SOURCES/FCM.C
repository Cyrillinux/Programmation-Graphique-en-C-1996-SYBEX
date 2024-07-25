/****************************************************************************/
// TITRE  : COMETES DE FEU
// AUTEUR : Walter SIRACUSA/wiSdom
// PROJET : FCM.C, VGA.C, VGA_ASM.ASM
/****************************************************************************/

#include <vga.h>
#include <conio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define NB 4 //Nombre de cometes

byte Pal[768];
bytef Page1[64000];
bytef Buf[5*4];              //Buffer d'une comete
word X[360][NB],Y[360][NB];  //Position des cometes

bytef *Video=MK_FP(0xa000,0);

void main(void)
{
  byte e1=0,e2=0,e3;
  word i=0,x,y,k,off;
  word rx,ry;

  ModeVGA();
  LoadPCX("fcm",Page1,Pal);
  SetAllPal(Pal);

  GetBlk1(14,38,5,4,Page1,Buf);  //Capture la comete
  FBox(13,37,19,42,Page1,0); //Efface la comete
  CpyScr(Page1,Video);

  //Precalcule le mouvement des cometes
  randomize();
  rx=50+random(150);
  ry=50+random(150);

  for(y=0;y<360;y++)
    for(x=0;x<NB;x++)
    {
      X[y][x]=140+sin((y+rx*x)*0.01745)*80;
      Y[y][x]=90+cos((y+ry*x)*0.01745)*64;
    }

  while(!kbhit())
  {
    //Simplification du lissage lineaire
    k=12220;  // = 60+38*320
    for(y=0;y<116;y++)
    {
      off=k;
      for(x=0;x<220;x++)
      {
        e3=Page1[off];
        Page1[off++]=(e1+e2+e3+e1)>>2;
        e1=e2;
        e2=e3;
      }
      k+=320;
    }

    //Affiche les cometes
    if(++i==360) i=0;
    for(x=0;x<NB;x++) PutSpr(X[i][x],Y[i][x],5,4,Page1,Buf);

    //Affiche la zone des cometes
    CpyBlk4(60,38,220,116,60,38,Page1,Video);
  }

  ModeTxt();
}
