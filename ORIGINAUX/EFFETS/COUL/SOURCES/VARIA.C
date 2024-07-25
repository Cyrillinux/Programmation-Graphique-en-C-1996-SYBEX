/*************************************************************/
// TITRE  : VARIATION DE LA PALETTE
// AUTEUR : Walter SIRACUSA/wiSdom
// PROJET : VARIA.C, VGA.C, VGA_ASM.ASM
/*************************************************************/

#include <vga.h>
#include <conio.h>
#include <math.h>

word VPal[256]; //Variation de la palette precalculee sur 256 degres
bytef *Video=MK_FP(0xa000,0);

void VariaPal(void);
void main(void);

void VariaPal(void)
{
  word i,r,v,b;
  static word r0=1*256,v0=10*256,b0=5*256;

  r0+=77;  //0.3*256
  v0+=128; //0.5*256
  b0+=256; //1.0*256

  r=r0>>8;
  v=v0>>8;
  b=b0>>8;

  for(i=1;i<256;i++)
    Ink(i,VPal[(i+r)&255]>>8,VPal[(i+v)&255]>>8,VPal[(i+b)&255]>>8);
}

void main(void)
{
  word i;

  ModeVGA();

  //Precalcule la variation de la palette
  for(i=0;i<256;i++)
    VPal[i]=(1+sin(i*(360/256.0)*(3.1416/180.0)))*32*256;

  VariaPal(); //Initialise les couleurs

  //320 colonnes sur 256 encres
  for(i=0;i<320;i++)
    Line(i,0,i,199,Video,i*(256/320.0));

  while(!kbhit())
  {
    VariaPal();  //Variation progressive de la palette
    VBL;
  }

  ModeTxt();
}
