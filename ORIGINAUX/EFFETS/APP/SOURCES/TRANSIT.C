/*************************************************************/
// TITRE  : TRANSITION D'UNE IMAGE A UNE AUTRE
// AUTEUR : Walter SIRACUSA/wiSdom
// PROJET : TRANSIT.C, VGA.C, VGA_ASM.ASM
/*************************************************************/

#include <vga.h>
#include <conio.h>

byte PalS[768], //Palette de l'image source
     PalD[768]; //Palette de l'image destination
short TransP[768]; //Rapport entre les deux palettes

bytef ImageS[64000]; //Image source
short huge TransI[64000]; //Rapport entre les deux images

bytef *Video=MK_FP(0xa000,0);

void main(void);
void InitTrans(bytef *src,bytef *dst);
void AffTrans(byte t,bytef *scr);

void InitTrans(bytef *src,bytef *dst)
{
  word i;

  for(i=17600;i<48000;i++)  //y = 55 a y = 150
    TransI[i]=((dst[i]-src[i])*256L)/63;

  for(i=0;i<192;i++)  //encres 0 a 63
    TransP[i]=((PalD[i]-PalS[i])<<8)/63;
}

void AffTrans(byte t,bytef *scr)
{
  word i;

  for(i=17600;i<48000;i++)  //y = 55 a y = 150
    scr[i]=((ImageS[i]<<8)+(long)TransI[i]*t)>>8;

  VBL;
  for(i=0;i<192;i++)  //encres 0 a 63
    PalD[i]=((PalS[i]<<8)+TransP[i]*t)>>8;

  SetAllPal(PalD);
}

void main(void)
{
  word i;

  ModeVGA();

  for(i=0;i<256;i++) Ink(i,0,0,0);
  LoadPCX("wisdom1",ImageS,PalS); //Image source
  LoadPCX("wisdom2",Video,PalD);  //Image destination

  InitTrans(ImageS,Video);
  CpyScr(ImageS,Video); //Source , Destination
  SetAllPal(PalS);

  //Affichage : Source -> Destination
  getch();
  for(i=0;i<64;i+=2) AffTrans(i,Video);
  AffTrans(63,Video);

  //Affichage : Destination -> Source
  getch();
  for(i=63;i>1;i-=2) AffTrans(i,Video);
  AffTrans(0,Video);

  getch();
  ModeTxt();
}
