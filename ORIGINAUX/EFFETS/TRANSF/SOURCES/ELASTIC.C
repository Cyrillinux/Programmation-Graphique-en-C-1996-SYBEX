/*************************************************************/
// TITRE  : ELASTICITE D'UNE IMAGE
// AUTEUR : Walter SIRACUSA/wiSdom
// PROJET : ELASTIC.C, VGA.C, VGA_ASM.ASM
/*************************************************************/

#include <vga.h>
#include <conio.h>
#include <math.h>

//Image source
#define XS1 71  //X gauche
#define XS2 251 //X droite
#define YS 19   //Y haut (sommet)
#define LG 169  //Largeur
#define HT 169  //Hauteur

//Image destination
#define XD1 75  //X gauche
#define XD2 244 //X droite = XD1+L_IMG
#define YD 199  //Y bas (base)

byte Pal[768];      //Palette de l'image
bytef Page1[64000];
bytef Page2[64000];
short Sin[360];

bytef *Video=MK_FP(0xa000,0);

void main(void)
{
  int key;
  word ih,   //Angle faisant varier la hauteur :  0 <=ih< 360
       hm,   //Hauteur maximale ( bornes : 0 <=h<= 2xHT )
       h;    //Hauteur courante variable  ->em
  int em,    //Elargissement maxi. de l'image : -LG/2 <=em<= LG/2  ->ve
      ve,    //Variation de l'elargissement courant:  -em <=ve<= em
      y0;
  word y,    //Ligne courante: 0 <=y< h  pour l'image destination
       rpa,  //Rapport entre 0 <=angle<= 180 et la hauteur courante
       rph,  //Rapport entre la hauteur initiale et la hauteur courante
       irpa, //Equivaut a y*rpa avec rpa = 180/(float)h
       irph; //Equivaut a YS+y*rph avec rph =HT/(float)h

  for(y=0;y<360;y++) Sin[y]=sin(y*0.01745)*256.0;  //Precalcule sinus

  ModeVGA();
  LoadPCX("sam2",Page2,Pal);
  SetAllPal(Pal);

  ih=90;
  hm=HT/2; //Hauteur initiale: image normale

  while(!kbhit() && hm>0) //Sortie clavier OU fin de variation de la hauteur
  {
    if(++ih==360) ih=0;               //Incrementation de l'angle
    h=(HT*256+Sin[(ih*8)%360]*hm)>>8; //Variation de la hauteur
    if(!(ih&3)) if(hm>0) hm--;        //Diminution de la hauteur maximale

    //Calcul de l'elargissement en fonction de la hauteur
    em=LG/2-LG*(h/(2.0*HT));

    //Rapport permettant d'obtenir la courbure maximale selon la hauteur
    rpa=46080/h; //180 degres * 256

    //Rapport permettant de capturer les lignes de l'image source
    rph=43264/h; //H_IMG*256

    irpa=0;
    irph=4864; //YS*256;

    y0=YD-h;

    ClrScr(Page1);
    for(y=0;y<h;y++)
    {
      ve=(Sin[irpa>>8]*em)>>8;  //Courbure variant a chaque ligne

      LineMapH(XS1,irph>>8,XS2,Page2,XD1-ve,y0++,(ve<<1)+LG,Page1);

      irpa+=rpa; //Interpole le rapport angle/hauteur
      irph+=rph; //Interpole le rapport hauteur initiale/courante
    }
    CpyScr(Page1,Video);
  }

  getch();
  ModeTxt();
}
