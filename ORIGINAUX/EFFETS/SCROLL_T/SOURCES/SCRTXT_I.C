/*************************************************************/
// TITRE  : SCROLLTEXT INCLINE
// AUTEUR : Walter SIRACUSA/wiSdom
// PROJET : SCRTXT_I.C, VGA.C, VGA_ASM.ASM
/*************************************************************/

#include <vga.h>
#include <conio.h>

#define LG 26 //Largeur d'un caractere
#define HT 26 //Hauteur d'un caractere

bytef Page1[64000]; //Buffer auxilliaire
bytef Page2[64000]; //Buffer de la fonte
bytef Pal[768];     //Palette de la fonte

byte Chaine[]=" PROGRAMS   SIRACUSA W.              ROUTINES     WISDOM                 GRAPHICS   SIRACUSA W.                        ";
word InclineX[200];   //Ordonnee des lignes selon l'abscisse
long InclineY[200];   //Abscisse des lignes
word Xc,Yc,           //Abscisse et ordonnee d'une lettre
     Tx[256],Ty[256]; //Abscisses et ordonnees precalculees

bytef *Video=MK_FP(0xa000,0);

void Lettre(byte car);
void CalculLettre(void);
void CalculIncline(void);
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

void CalculIncline(void)
{
  byte yi;
  float ym=0,ym0=1.65;

  for(yi=15;yi<195;yi++)
  {
    ym+=ym0;
    ym0-=0.0083;

    //Centrage des lignes
    InclineX[yi]=(320-(yi+60))/2;
    //Offset destination (ecriture)
    InclineX[yi]=InclineX[yi]+yi*320;

    //Inclinaison croissante
    InclineY[yi]=15+(byte)ym;
    //Offset source (lecture)
    InclineY[yi]=InclineY[yi]*320*256;
  }
}

void main(void)
{
  byte car,      //Caractere courant
       x,        //Compteurs des colonnes
       y;        //Compteur des lignes
  word dx,       //Position du caractere a l'ecran
       i_car=0,  //Indice du caractere courant
       lg=0,     //Longueur du texte
       l0;       //Elargissement des lignes

  register word xi,yi;
  long offs,offd;
  word rp;

  ModeVGA();
  LoadPCX("scrtxt",Page2,Pal);
  SetAllPal(Pal);

  CalculIncline();     //Precalcule l'inclinaison
  CalculLettre();      //Precalcule les abscisses et les ordonnees
  while(Chaine[lg++]); //Longueur du texte pour le rebouclage
  lg-=(12+2);          //Ne traite pas la derniere ligne et '\0'

  while(!kbhit())
  {
    for(y=0;y<HT;y+=2)  //Hauteur d'un caractere
    {
      dx=0;
      for(x=0;x<12;x++) //Ligne de 12 caracteres affichables a l'ecran
      {
        car=Chaine[i_car+x],
        CpyBlk2(Tx[car],Ty[car]+y,LG,2,dx,198,Page2,Page1);
        dx+=LG; //Caractere suivant a l'ecran
      }
      CpyBnd(20,180,18,Page1,Page1); //Scrolling haut de 2 pixels

      VBL;
      l0=75; //60 + (yi = 15)
      for(yi=15;yi<195;yi++)  //Lignes selon l'inclinaison precalculee
      {
        //LineMapH(0,InclineY[yi],255,Page1,InclineX[yi],yi,l0++,Video);
        offd=InclineX[yi];
        offs=InclineY[yi];

        rp=65280/l0; //255*256 = 65280
        for(xi=0;xi<l0;xi++)
        {
          Video[offd++]=Page1[offs>>8];
          offs+=rp;
        }
        l0++;
      }
    }

    if(i_car<lg) i_car+=12;   //Changement de ligne du texte
    else i_car=0;             //Rebouclage du texte
  }

  ModeTxt();
}
