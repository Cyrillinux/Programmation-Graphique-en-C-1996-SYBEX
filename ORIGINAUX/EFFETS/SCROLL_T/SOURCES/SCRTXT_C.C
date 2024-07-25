/*************************************************************/
// TITRE  : SCROLLTEXT CIRCULAIRE
// AUTEUR : Walter SIRACUSA/wiSdom
// PROJET : SCRTXT_C.C, VGA.C, VGA_ASM.ASM
/*************************************************************/

#include <vga.h>
#include <conio.h>
#include <math.h>

#define LG 26  //Largeur d'un caractere
#define HT 22  //Hauteur d'un caractere
#define PAS 28 //Nombre de caracteres dans le cercle = 360/PAS

bytef Pal[768];          //Palette des caracters
bytef Page1[64000];      //Buffer auxilliaire
short Cos[360],Sin[360]; //Cosinus et sinus precalcules

bytef Lettre[30][LG*HT]; //Buffer de 30 caracteres
byte Chaine[]={"             APRES LES SCROLLS VERTICAUX ET HORIZONTAUX, VOICI LE SCROLLTEXT CIRCULAIRE AVEC REBONDISSEMENT SUR LES BORDURES...             "};

bytef *Video=MK_FP(0xa000,0);

void main(void)
{
  byte bx0=0,by0=0;  //Booleens pour le rebondissement
  byte car;          //Caractere courant du texte
  int x0=160,y0=100, //Centre du cercle
      x,y;           //Points du cercle
  word i,ang,        //Compteur et angle sur 360 degres
       pch=0,        //Pointeur sur le texte
       lg=0;         //Longueur du texte

  ModeVGA();
  LoadPCX("scrtxt",Page1,Pal);
  SetAllPal(Pal);

  //Precalcule les tables de sinus et cosinus
  for(i=0;i<360;i++)
  {
    Cos[i]=cos(i*0.01745)*256;
    Sin[i]=sin(i*0.01745)*256;
  }

  //Capture les caracteres (sprites )
  for(i=0;i<11;i++) GetBlk1(i*(1+LG),0,LG,HT,Page1,Lettre[i]);
  for(i=0;i<11;i++) GetBlk1(i*(1+LG),27,LG,HT,Page1,Lettre[11+i]);
  for(i=0;i<8;i++) GetBlk1(i*(1+LG),54,LG,HT,Page1,Lettre[22+i]);

  while(Chaine[lg++]);  //Longueur du texte pour le rebouclage
  lg-=360.0/PAS;        //Ne traite pas la fin de chaine

  i=0;
  while(!kbhit())
  {
    //Decale le texte PAS fois avant de lire un autre caractere
    i+=2;
    if(i==PAS)
    {
      i=0; //Decalage initialisee
      //Nouveau caractere, si fin du texte: reboucle
      if(++pch==lg) pch=0;
    }

    //Rebondissement du cercle
    if(x0<40) bx0=1;
    else if(x0>270) bx0=0;
    if(y0<40) by0=1;
    else if(y0>150) by0=0;
    if(bx0) x0++;
    else x0--;
    if(by0) y0++;
    else y0--;

    ClrScr(Page1);

    for(ang=0;ang<360;ang+=PAS) //360 degres avec un pas = PAS
    {
      //Position d'affichage du nouveau caractere
      x=x0+((Cos[((ang+90)-i)%360]*90)>>8);
      y=y0+((Sin[((ang+90)-i)%360]*85)>>8);

      //Limitation du texte au bordures
      if(x<0) x=0;
      else if(x>295) x=295;
      if(y<0) y=0;
      else if(y>175) y=175;

      //Determine le caractere alphabetique et l'affiche
      car=Chaine[pch+ang/PAS];

      if(car>=65 && car<=90)
        PutSpr(x,y,LG,HT,Page1,Lettre[car-65]);  //A-Z
      else
        if(car==' ')
          PutSpr(x,y,LG,HT,Page1,Lettre[29]);  //Espace
        else
         if(car==39)
           PutSpr(x,y,LG,HT,Page1,Lettre[26]);  //Apostrophe
         else
           if(car==',')
             PutSpr(x,y,LG,HT,Page1,Lettre[27]);  //Virgule
           else
            if(car=='.')
              PutSpr(x,y,LG,HT,Page1,Lettre[28]);  //Point
    }

    VBL;
    CpyScr(Page1,Video);
  }

  ModeTxt();
}
