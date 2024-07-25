/*************************************************************/
// TITRE  : CREATION DE DEGRADES
// AUTEUR : Walter SIRACUSA/wiSdom
// PROJET : DEGRADE.C, VGA.C, VGA_ASM.ASM
/*************************************************************/

#include <vga.h>
#include <conio.h>

bytef *Video=MK_FP(0xa000,0);

void Degrade(byte ed,byte rd,byte vd,byte bd,byte ef,byte rf,byte vf,byte bf);
void main(void);

void Degrade(byte ed,byte rd,byte vd,byte bd,byte ef,byte rf,byte vf,byte bf)
{
  byte i,n;
  float rr,rv,rb;

  if(ef<ed) //Echange les encres de debut et de fin
  {
    n=ed;
    ed=ef;
    ef=n;
  }

  n=ef-ed; //Nombre d'encres utilisees

  Ink(ed,rd,vd,bd);     //Encre de depart
  Ink(ef,rf,vf,bf);     //Encre de fin
  rr=(rf-rd)/(float)n;  //Rapport R
  rv=(vf-vd)/(float)n;  //Rapport V
  rb=(bf-bd)/(float)n;  //Rapport B
  for(i=1;i<n;i++) Ink(ed+i,rd+rr*i,vd+rv*i,bd+rb*i);
}

void main(void)
{
  byte i;

  ModeVGA();

  //Creation de 4 degrades
  Degrade(0,0,0,0,63,46,46,63);       //Bleu
  Degrade(64,10,0,0,127,63,49,37);    //Marron
  Degrade(128,0,10,0,191,35,63,35);   //Vert
  Degrade(192,0,10,10,254,50,63,63);  //Vert

  Ink(255,63,63,63); //Cadre blanc

  //Affichage des degrades
  for(i=0;i<64;i++)
  {
    Line(10,28+i,150,28+i,Video,i);
    Line(170,28+i,310,28+i,Video,64+i);
    Line(10,115+i,150,115+i,Video,128+i);
    Line(170,115+i,310,115+i,Video,192+i);
  }

  //Encadremenet des degrades
  Box(10,27,142,66,Video,255);
  Box(170,27,142,66,Video,255);
  Box(10,114,142,66,Video,255);
  Box(170,114,142,66,Video,255);

  getch();
  ModeTxt();
}
