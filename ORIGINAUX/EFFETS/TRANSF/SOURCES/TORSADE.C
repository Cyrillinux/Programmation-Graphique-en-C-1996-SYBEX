/*************************************************************/
// TITRE  : IMAGE TORSADEE
// AUTEUR : Walter SIRACUSA/wiSdom
// PROJET : TORSADE.C, VGA.C, VGA_ASM.ASM
/*************************************************************/

#include <vga.h>
#include <conio.h>
#include <math.h>

//Image source
#define XS1 68  //X gauche (recto)
#define XS2 243 //X droite (verso)
#define YS 20   //Y (sommet)
#define LG 176  //Largeur multiple de 4
#define HT 160  //Hauteur

//Image destination
#define XD 72
#define YD 20

byte Pal[768];       //Palette
bytef Page1[64000];  //Image source
bytef Page2[64000];  //Image torsadee
short Sin[256];      //Torsade precalculee
byte Rj[HT];         //Reajustement de la hauteur

bytef *Video=MK_FP(0xa000,0);

void main(void)
{
  int l;     //Largeur courante
  byte a=0,  //Angle sur 256 degres
       i,    //Compteur
       y;    //Ligne courante
  word rp,   //Rapport de reduction
       irp,  //Interpole rp
       offs, //Offset source
       offd; //Offset destination

  ModeVGA();
  LoadPCX("bimbo2",Page1,Pal);
  SetAllPal(Pal);

  //Precalcule une partie de la torsade
  for(l=0;l<256;l++)
    Sin[l]=sin(l*1.40625*0.01745)*LG; //(360/256) * (PI/180)

  //Precalcule le reajustement
  for(l=0;l<HT;l++)
    Rj[l]=l*0.625;  //HT/256

  while(!kbhit())
  {
    a+=2; //Variation de l'angle

    ClrScr(Page2);

    for(y=0;y<HT;y++) //Traite toutes les lignes
    {
      l=Sin[(a+Rj[y])&255];  //Largeur de la ligne courante

      if(l>0) //Image normale
      {
        rp=45056/l;  //Rapport par decalage avec LG*256
        offs=(YS+y)*320+XS1;  //Offset source
        offd=((LG-l)>>1)+XD+(YD+y)*320; //Offset destination
        irp=0;

        for(i=0;i<l;i++) //Traite toutes les colonnes
        {
          Page2[offd++]=Page1[offs+(irp>>8)];
          irp+=rp;
        }
      }
      else if(l<0) //Image renversee
      {
        l=-l;        //Largeur positive
        rp=45056/l;  //Rapport par decalage avec LG*256
        offs=(YS+y)*320+XS2;  //Offset source
        offd=((LG-l)>>1)+(YD+y)*320+XD; //Offset destination
        irp=0;

        for(i=0;i<l;i++) //Traite toutes les colonnes
        {
          Page2[offd++]=Page1[offs-(irp>>8)];
          irp+=rp;
        }
      }
    }

    VBL;
    CpyBlk4(XD,YD,LG,HT,XD,YD,Page2,Video);
  }

  ModeTxt();
}
