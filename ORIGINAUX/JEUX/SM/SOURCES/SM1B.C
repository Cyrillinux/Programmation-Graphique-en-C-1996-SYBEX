/* ⁄ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒø
   ≥   ∞±±≤≤≤€ SPRINT MASTER beta €≤≤≤±±∞   ≥
   ≥        par Siracusa Walter/wiSdom      ≥
   ≥  PROJET : SM1B.C, VGA.C, VGA_ASM.ASM   ≥
   ¿‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹Ÿ */


/* INCLUSION DES BIBLIOTHEQUES */

#include <vga.h>
#include <conio.h>
#include <stdio.h>
#include <math.h>
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>

/* DECLARATION DES CONSTANTES */

#define PORT_CLAVIER 0x60      //Adresse du port clavier
#define T_o 36                 //Gauche player 1
#define T_p 37                 //Droite player 1
#define T_SPACE 56             //Vitesse player 1
#define T_4 75                 //Gauche player 2
#define T_5 77                 //Droite player 2
#define T_RET 82               //Vitesse player 2
#define ESC 1
#define VT_ROT1 verglas1       //Temps de rotation
#define VT_ROT2 verglas2
#define VT_MAX 4               //Vitesse maximale
#define VT_ACC 10              //Temps d'accÇlÇration
#define VT_DEC 14              //Temps de dÇcÇlÇration
#define TIME_DERAP1 spr1_vt*3  //Temps de dÇrapage (Vitesse: Pas = 1 pixel)
#define TIME_CHOC1 spr1_vt*2   //Temps de rebondissement lors d'un choc
#define TIME_DERAP2 spr2_vt*3
#define TIME_CHOC2 spr2_vt*2
#define TIME_POWER 150         //Temps d'apparition d'un power
#define NB_TOUR 5              //Nombre de tour du circuit

#define NEIGE 150              //Nombre de points de neige

/* DECLARATION DES VARIABLES GLOBALES */

//[0][spr_dir]ø droite
//[1][spr_dir]Ÿ
char  derap_droite[2][16]={-1,-1,-1,-1,0,1,1,1,1,1,1,1,0,-1,-1,-1,
         0,-1,-1,-1,-1,-1,-1,-1,0,1,1,1,1,1,1,1},
//[0][spr_dir]ø gauche
//[1][spr_dir]Ÿ
      derap_gauche[2][16]={1,1,1,1,0,-1,-1,-1,-1,-1,-1,-1,0,1,1,1,
        0,1,1,1,1,1,1,1,0,-1,-1,-1,-1,-1,-1,-1},
//[0][spr_dir]ø
//[1][spr_dir]Ÿ
      off_feux[2][16]={7,3,0,-3,-4,-2,0,3,7,11,13,16,18,17,15,11,
           18,15,13,10,6,2,1,-2,-4,-1,1,2,6,10,13,15},
//DÇrapage
      spr1_derapx,spr1_derapy,spr1_derapt=0,
      spr2_derapx,spr2_derapy,spr2_derapt=0;

byte key, //Derniäre touche appuyÇe (pour la sortie avec ESC)
//Touches de controle
     spr1_droite=0,spr1_gauche=0,spr1_vitesse=0,
     spr2_droite=0,spr2_gauche=0,spr2_vitesse=0,
//Direction courante
     spr1_dir,
     spr2_dir,
//Vitesse courante
     spr1_vt,spr1_vtmax=VT_MAX,
     spr2_vt,spr2_vtmax=VT_MAX,
//Choc
     spr1_choc=0,spr1_choci=0,spr1_chocy,choc_derap1=0,
     spr2_choc=0,spr2_choci=0,spr2_chocy,choc_derap2=0,
//Barre du power
     spr1_power=20,
     spr2_power=20,
//Tour de piste
     spr1_tour=0,spr1_boolt=0,
     spr2_tour=0,spr2_boolt=0,
//Power courant
     power_y=0,power_bool=0;


bytef pal[768];            //Palette de couleurs RVB
bytef page1[64000];        //Page temporaire

//Buffers des sprites
bytef fume[6][9*9];        //FumÇe lors d'un choc
bytef feux[16][6*6];       //Feux des rÇacteurs
bytef spr1[16][19*18];     //Sprites voitures
bytef spr2[16][19*18];
bytef power[13][16*12];    //Animation du Power
bytef lastlap1[60*14];     //Last Lap
bytef lastlap2[60*14];
bytef power_fnd[16*12];    //Arriere plan
bytef spr1_fnd[26*26];
bytef spr1_fnd_fume[10*9];
bytef spr2_fnd[26*26];
bytef spr2_fnd_fume[10*9];
bytef buf_lastlap1[60*14];
bytef buf_lastlap2[60*14];

//Tests des collisions
bytef test[16][2][10]={4,5,6,7,9,10,12,13,14,15,
           6,5,3,1,0,0,1,3,5,6,
           6,8,9,10,12,14,15,15,15,15,
           3,2,1,0,0,0,2,3,5,6,
           8,10,12,14,16,17,17,17,16,16,
           2,2,1,1,1,2,5,7,9,11,
           9,12,14,16,17,18,18,18,16,15,
           2,2,3,3,3,5,7,8,10,12,
           12,13,15,17,18,18,18,15,13,12,
           3,4,5,6,7,9,11,13,14,15,
           12,14,15,17,18,18,18,16,15,14,
           15,14,14,14,12,10,9,17,5,4,
           8,10,12,14,16,17,17,17,17,16,
           16,16,17,17,17,16,14,13,11,7,
           5,5,8,9,10,12,14,15,15,15,
           13,14,15,16,17,17,17,15,13,11,
           4,5,6,7,8,11,12,13,14,15,
           11,12,14,16,17,17,16,14,12,11,
           3,3,3,4,6,8,9,10,12,13,
           11,13,15,17,17,17,16,15,14,13,
           2,2,1,1,1,2,4,6,8,10,
           7,9,11,13,16,17,17,17,16,16,
           3,2,2,0,0,1,3,6,9,11,
           5,7,8,9,12,14,14,15,15,14,
           6,5,3,1,0,0,0,3,5,6,
           3,4,5,6,7,9,11,13,14,15,
           11,9,6,4,1,0,0,2,3,4,
           3,2,2,3,3,5,7,10,12,13,
           2,2,1,1,1,2,5,7,9,11,
           10,8,6,4,2,1,1,2,2,
           3,3,3,4,6,8,10,12,13,14,
           8,5,2,0,0,0,2,3,5,7};

word pause=0,       //Temporisation du programme
     i_parcour=0,   //Indice du trajet pour spr2 (mode 1 joueur)
     spr1_x,spr1_y, //Position courante
     spr2_x,spr2_y,
     spr1_chocx,    //Position du choc
     spr2_chocx,
     power_x=0,

//Positions d'apparitions des powers
     pos_power[2][10]={35,275,259,91,13,20,95,125,161,288,
           113,83,23,21,64,164,137,72,101,155};

wordf parcour[3210];  //Trajet de spr2 (= taille fichier/2)

//IncrÇments des positions en fonction de [spr_dir]
float dep_x[16]={0,0.5,0.9,0.9,1,0.9,0.9,0.5,0,-0.5,-0.9,-0.9,-1,-0.9,-0.9,-0.5},
      dep_y[16]={-1,-0.9,-0.9,-0.5,0,0.5,0.9,0.9,1,0.9,0.9,0.5,0,-0.5,-0.9,-0.9};

//Controle clavier sous IT
typedef struct
{
  char val;
  char stat;
}_SCAN;

_SCAN far SCAN_CODES[128]={
          {   0,0 }, // Derniere touche appuyÇe,relachÇe
          {  27,0 }, // 1 = ESC
          { '1',0 }, // 2
          { '2',0 }, // 3
          { '3',0 }, // 4
          { '4',0 }, // 5
          { '5',0 }, // 6
          { '6',0 }, // 7
          { '7',0 }, // 8
          { '8',0 }, // 9
          { '9',0 }, // 10
          { '0',0 }, // 11
          { '¯',0 }, // 12
          { '=',0 }, // 13
          { ' ',0 }, // 14 = DEL
          { ' ',0 }, // 15 = TAB
          { 'a',0 }, // 16
          { 'z',0 }, // 17
          { 'e',0 }, // 18
          { 'r',0 }, // 19
          { 't',0 }, // 20
          { 'y',0 }, // 21
          { 'u',0 }, // 22
          { 'i',0 }, // 23
          { 'o',0 }, // 24
          { 'p',0 }, // 25
          { '^',0 }, // 26
          { '$',0 }, // 27
          { ' ',0 }, // 28 = RETURN
          { ' ',0 }, // 29 = CTRL
          { 'q',0 }, // 30
          { 's',0 }, // 31
          { 'd',0 }, // 32
          { 'f',0 }, // 33
          { 'g',0 }, // 34
          { 'h',0 }, // 35
          { 'j',0 }, // 36
          { 'k',0 }, // 37
          { 'l',0 }, // 38
          { 'm',0 }, // 39
          { 'ó',0 }, // 40
          { '˝',0 }, // 41 = ˝
          { ' ',0 }, // 42 = SHIFT GAUCHE
          { '*',0 }, // 43
          { 'w',0 }, // 44
          { 'x',0 }, // 45
          { 'c',0 }, // 46
          { 'v',0 }, // 47
          { 'b',0 }, // 48
          { 'n',0 }, // 49
          { ',',0 }, // 50
          { ';',0 }, // 51
          { ':',0 }, // 52
          { '!',0 }, // 53
          { ' ',0 }, // 54 = SHIFT DROITE
          { '*',0 }, // 55
          { ' ',0 }, // 56 = ALT et ALT Gr
          { ' ',0 }, // 57 = SPACE
          { ' ',0 }, // 58 = CAPS
          { ' ',0 }, // 59 = VER NUM
          { ' ',0 }, // 60 = F1
          { ' ',0 }, // 61 = F2
          { ' ',0 }, // 62 = F3
          { ' ',0 }, // 63 = F4
          { ' ',0 }, // 64 = F5
          { ' ',0 }, // 65 = F6
          { ' ',0 }, // 66 = F7
          { ' ',0 }, // 67 = F8
          { ' ',0 }, // 68 = F9
          { ' ',0 }, // 69 = F10
          { ' ',0 }, // 70 = SCROLL LOCK
          { ' ',0 }, // 71 = HOME
          { ' ',0 }, // 72 = UP
          { ' ',0 }, // 73 = PG UP
          { '-',0 }, // 74
          { ' ',0 }, // 75 = LEFT
          { ' ',0 }, // 76
          { ' ',0 }, // 77 = RIGHT
          { '+',0 }, // 78
          { ' ',0 }, // 79 = FIN
          { ' ',0 }, // 80 = DOWN
          { ' ',0 }, // 81 = PG DOWN
          { ' ',0 }, // 82 = INSER
          { '.',0 }, // 83 = SUPPR
        };

byte buf_neige[NEIGE],y_neige[NEIGE],vt_neige[NEIGE], //Neige
     verglas1=2,verglas2=2; //DÇrapage variable sur le verglas
word x_neige[NEIGE];

bytef *Video=MK_FP(0xa000,0);

/* PROTOTYPES DES FONCTIONS */

void main(void);
void far interrupt(*oldit_9)(void);
void far interrupt scankbd(void);
void init(void);
void lire_keys(void);
void controle_spr1(void);
void controle_spr2(void);
void derap_spr1(char dpx,char dpy,char dpt);
void derap_spr2(char dpx,char dpy,char dpt);
void dep_spr1(char depx,char depy);
void dep_spr2(char depx,char depy);
void controle2_spr2(word x,byte y,byte direction);
void affichage(void);
void screen(void);

/* FONCTIONS */

void far interrupt scankbd(void)
{
 char c,d;

 c=inportb(PORT_CLAVIER);

 if (c<0) SCAN_CODES[SCAN_CODES[0].stat=(c&0x7f)].stat=0;
     else SCAN_CODES[SCAN_CODES[0].val=c].stat=1;

 outportb(0x20,0x20);
}

void screen(void)
{
  int y;
  byte y2;
  word r,xr,off;

  for(y=-35;y<200;y++)
  {
    if(y>=0) CpyBnd(y,1,y,page1,Video);
    for(y2=1;y2<35;y2++)
    {
      if(y+y2>=0 && y+y2<200)
      {
        for(r=0;r<(35-y2);r++)
        {
          off=(y+y2)*320+random(320);
          Video[off]=page1[off];
        }
      }
    }
  }
}

void derap_spr1(char dpx,char dpy,char dpt)
{
  spr1_derapx=dpx;
  spr1_derapy=dpy;
  spr1_derapt=dpt;
}

void derap_spr2(char dpx,char dpy,char dpt)
{
  spr2_derapx=dpx;
  spr2_derapy=dpy;
  spr2_derapt=dpt;
}

void init(void)
{
  byte i;

  ModeVGA();
  ClrScr(Video);
  LoadPCX("sprite",page1,pal);

  for(i=0;i<16;i++)  //Positions des sprites (voitures)
    GetBlk1(1+i*(19+1),1,19,18,page1,spr1[i]);
  for(i=0;i<16;i++)
   GetBlk1(1+i*(19+1),22,19,18,page1,spr2[i]);

  for(i=0;i<6;i++)   //FumÇe lors d'un choc
    GetBlk1(1+i*(9+1),43,9,9,page1,fume[i]);

  for(i=0;i<16;i++)  //Feux des reacteurs
    GetBlk1(63+i*(6+1),43,6,6,page1,feux[i]);

  for(i=0;i<13;i++)   //Power
    GetBlk1(1+i*(16+1),55,16,12,page1,power[i]);

  GetBlk2(224,53,60,14,page1,lastlap1);  //Capture de Last Lap
  GetBlk2(1,70,60,14,page1,lastlap2);

  for(i=0;i<NEIGE;i++) //Initialisation de la neige
  {
    x_neige[i]=random(320);
    y_neige[i]=random(180);
    vt_neige[i]=1+random(3);
  }

  LoadPCX("piste1",page1,pal);
  SetAllPal(pal);
}

void lire_keys(void)
{
  if(SCAN_CODES[T_o].stat) { spr1_gauche=1;spr1_droite=0;}
         else spr1_gauche=0;
  if(SCAN_CODES[T_p].stat) { spr1_droite=1;spr1_gauche=0;}
         else spr1_droite=0;
  if(SCAN_CODES[T_SPACE].stat) spr1_vitesse=1;
         else spr1_vitesse=0;
  if(SCAN_CODES[T_4].stat) { spr2_gauche=1;spr2_droite=0;}
         else spr2_gauche=0;
  if(SCAN_CODES[T_5].stat) { spr2_droite=1;spr2_gauche=0;}
         else spr2_droite=0;
  if(SCAN_CODES[T_RET].stat) spr2_vitesse=1;
         else spr2_vitesse=0;
}

void controle_spr1(void)
{
  static byte rt_cpt1=0,vta_cpt1=0,vtd_cpt1=0;

  //Rotation a droite
  if(spr1_droite)
  {
    if(++rt_cpt1>VT_ROT1)
    {
      rt_cpt1=0;
      if(spr1_dir==15) spr1_dir=0;
      else spr1_dir++;

      //DÇrapage s'il n'y a pas de choc:
      if(!choc_derap1 && spr1_vt>2) derap_spr1(derap_droite[0][spr1_dir],derap_droite[1][spr1_dir],TIME_DERAP1);
    }
  }

  //Rotation a gauche
  if(spr1_gauche)
  {
    if(++rt_cpt1>VT_ROT1)
    {
      rt_cpt1=0;
      if(!spr1_dir) spr1_dir=15;
      else spr1_dir--;

      //DÇrapage s'il n'y a pas de choc:
      if(!choc_derap1 && spr1_vt>2) derap_spr1(derap_gauche[0][spr1_dir],derap_gauche[1][spr1_dir],TIME_DERAP1);
    }
  }

  //AccÇlÇration
  if(spr1_vitesse && spr1_tour!=NB_TOUR && spr2_tour!=NB_TOUR)
  {
    if(spr1_vt<spr1_vtmax)
    {
      if(++vta_cpt1>VT_ACC)
      {
        vta_cpt1=0;
        spr1_vt++;
      }
    }
  }
  else //DÇcÇlÇration
  {
    if(spr1_vt>0)
    {
      if(++vtd_cpt1>VT_DEC)
      {
        vtd_cpt1=0;
        spr1_vt--;
      }
    }
  }

  //DÇplacement
  dep_spr1(dep_x[spr1_dir]*spr1_vt,dep_y[spr1_dir]*spr1_vt);
  if(spr1_derapt) //En dÇrapage ou Choc
  {
    spr1_derapt--;
    dep_spr1(spr1_derapx,spr1_derapy);
  }
  else choc_derap1=0;
}

void controle_spr2(void)
{
  static byte rt_cpt1=0,vta_cpt1=0,vtd_cpt1=0;

  //Rotation a droite
  if(spr2_droite)
  {
    if(++rt_cpt1>VT_ROT2)
    {
      rt_cpt1=0;
      if(spr2_dir==15) spr2_dir=0;
      else spr2_dir++;

      //DÇrapage s'il n'y a pas de choc:
      if(!choc_derap2 && spr2_vt>2) derap_spr2(derap_droite[0][spr2_dir],derap_droite[1][spr2_dir],TIME_DERAP2);
    }
  }

  //Rotation a gauche
  if(spr2_gauche)
  {
    if(++rt_cpt1>VT_ROT2)
    {
      rt_cpt1=0;
      if(!spr2_dir) spr2_dir=15;
      else spr2_dir--;

      //DÇrapage s'il n'y a pas de choc:
      if(!choc_derap2 && spr2_vt>2) derap_spr2(derap_gauche[0][spr2_dir],derap_gauche[1][spr2_dir],TIME_DERAP2);
    }
  }

  //AccÇlÇration
  if(spr2_vitesse && spr1_tour!=NB_TOUR && spr2_tour!=NB_TOUR)
  {
    if(spr2_vt<spr2_vtmax)
    {
      if(++vta_cpt1>VT_ACC)
      {
        vta_cpt1=0;
        spr2_vt++;
      }
    }
  }
  else //DÇcÇlÇration
  {
    if(spr2_vt>0)
    {
      if(++vtd_cpt1>VT_DEC)
      {
        vtd_cpt1=0;
        spr2_vt--;
      }
    }
  }

  //DÇplacement
  dep_spr2(dep_x[spr2_dir]*spr2_vt,dep_y[spr2_dir]*spr2_vt);
  if(spr2_derapt) //En dÇrapage ou choc
  {
    spr2_derapt--;
    dep_spr2(spr2_derapx,spr2_derapy);
  }
  else choc_derap2=0;
}

void dep_spr1(char depx,char depy)
{
  char dir_x,dir_y,pos_aux=-1;
  byte pos,coul,
       c1=0,c2=0,c3=0,c4=0,c5=0;

  //Direction courante (-1 , 0 , 1)
  if(depx) dir_x=depx/abs(depx);
  else dir_x=0;
  if(depy) dir_y=depy/abs(depy);
  else dir_y=0;

  //Tests des couleurs
  for(pos=0;pos<10;pos++)
  {
    coul=GetPixel(spr1_x+depx+test[spr1_dir][0][pos],spr1_y+depy+test[spr1_dir][1][pos],page1);
    if(coul>15 && coul<20)
    {
      if(pos_aux==-1) pos_aux=pos;  //MÇmorisation de la position du choc
      c1=1; //Choc sur obstacle
    }
    else
      if(coul>47 && coul<64) c2=1; //Power
      else
        if(coul==32 || coul==33) c3=1; //Ligne d'arrivÇe
      else
        if(coul==39) c4=1;  //Booleen de la la ligne d'arrivÇe
      else
        if(coul>33 && coul<36) c5=1; //Verglas
  }

  if(c1)
  {
    if(spr1_power>=spr1_vt) spr1_power-=spr1_vt/2;  //Perte de Power
    else
    {
      spr1_power=0;
      spr1_vtmax=VT_MAX-1;
      spr1_vt=VT_MAX-1;
    }
    if(spr1_vt==VT_MAX)  //Choc maximal -> FumÇ
    {
      spr1_choci=0;
      spr1_chocx=spr1_x+test[spr1_dir][0][pos_aux]-4;
      spr1_chocy=spr1_y+test[spr1_dir][1][pos_aux]-4;
      spr1_choc=1;
    }
    choc_derap1=1;  //Empäche un dÇrapage
    derap_spr1(-dir_x,-dir_y,TIME_CHOC1);
    if(!spr1_vitesse) spr1_vt=0;  //Moteur arretÇ -> Rebondissement
    else if(spr1_vt) spr1_vt--;
  }
  else
  {
    spr1_x+=depx;
    spr1_y+=depy;
  }

  if(c2 && spr1_power<10)
  {
    spr1_vtmax=VT_MAX;
    spr1_power=20;
    PutBlk2(power_x,power_y,16,12,page1,power_fnd);
    power_bool=0;
  }

  if(c3)
  {
    if(dir_x==1) spr1_boolt=0;  //1 = Direction non souhaitÇe
    if(dir_x==-1 && spr1_boolt) //-1 = Direction souhaitÇe
    {
      spr1_tour++;
      spr1_boolt=0;
    }
  }

  if(c4 && dir_x==-1) spr1_boolt=1;   //-1 = Direction souhaitÇe

  if(c5) verglas1=0;
  else verglas1=2;
}

void dep_spr2(char depx,char depy)
{
  char dir_x,dir_y,pos_aux=-1;
  byte pos,coul,
       c1=0,c2=0,c3=0,c4=0,c5=0;

  //Direction courante (-1 , 0 , 1)
  if(depx) dir_x=depx/abs(depx);
  else dir_x=0;
  if(depy) dir_y=depy/abs(depy);
  else dir_y=0;

  //Test des couleurs
  for(pos=0;pos<10;pos++)
  {
    coul=GetPixel(spr2_x+depx+test[spr2_dir][0][pos],spr2_y+depy+test[spr2_dir][1][pos],page1);
    if(coul>15 && coul<20)
    {
      if(pos_aux==-1) pos_aux=pos;  //MÇmorisation de la position du choc
      c1=1; //Choc sur obstacle
    }
    else
      if(coul>47 && coul<64) c2=1; //Power
      else
      if(coul==32 || coul==33) c3=1; //Ligne d'arrivÇe
      else
      if(coul==39) c4=1;  //Booleen de la la ligne d'arrivÇe
      else
      if(coul>33 && coul<36) c5=1; //Verglas
  }

  if(c1)
  {
    if(spr2_power>=spr2_vt) spr2_power-=spr2_vt/2;  //Perte de Power
    else
    {
      spr2_power=0;
      spr2_vtmax=VT_MAX-1;
      spr2_vt=VT_MAX-1;
    }
    if(spr2_vt==VT_MAX) //Choc maximal -> FumÇ
    {
      spr2_choci=0;
      spr2_chocx=spr2_x+test[spr2_dir][0][pos_aux]-4;
      spr2_chocy=spr2_y+test[spr2_dir][1][pos_aux]-4;
      spr2_choc=1;
    }
    choc_derap2=1;  //Empäche un dÇrapage
    derap_spr2(-dir_x,-dir_y,TIME_CHOC2);
    if(!spr2_vitesse) spr2_vt=0;  //Moteur arretÇ -> Rebondissement
    else if(spr2_vt) spr2_vt--;
  }
  else
  {
    spr2_x+=depx;
    spr2_y+=depy;
  }

  if(c2 && spr2_power<10)
  {
    spr2_vtmax=VT_MAX;
    spr2_power=20;
    PutBlk2(power_x,power_y,16,12,page1,power_fnd);
    power_bool=0;
  }

  if(c3)
  {
    if(dir_x==1) spr2_boolt=0;  //1 = Direction non souhaitÇe
    if(dir_x==-1 && spr2_boolt) //-1 = Direction souhaitÇe
    {
      spr2_tour++;
      spr2_boolt=0;
    }
  }

  if(c4 && dir_x==-1) spr2_boolt=1;   //-1 = Direction souhaitÇe

  if(c5) verglas2=0;
  else verglas2=2;
}

void controle2_spr2(word x,byte y,byte direction)
{
  byte pos,coul,
       c3=0;

  //Test des couleurs
  for(pos=0;pos<10;pos++)
  {
    coul=GetPixel(x+test[direction][0][pos],y+test[direction][1][pos],page1);
    if(coul==32 || coul==33) c3=1; //Ligne d'arrivÇe
    else
      if(coul==39) spr2_boolt=1;  //Booleen de la ligne d'arrivÇe
  }

  if(c3 && spr2_boolt)
  {
    spr2_tour++;
    spr2_boolt=0;
  }
}

void affichage(void)
{
  static byte spr1_feuxi=0,spr2_feuxi=0,
        power_cpt=0,poweri=0,
        time1_lp=0,time2_lp=0;
  byte i,bool1_lp,bool2_lp,yfl1,yfl2;

  //Barres d'energie
  FBox(78,1,spr1_power,2,page1,2);
  FBox(77+spr1_power,1,22-spr1_power,2,page1,19);
  FBox(218,1,spr2_power,2,page1,12);
  FBox(217+spr2_power,1,22-spr2_power,2,page1,19);

  //Autorisation d'apparition d'un power
  if(++power_cpt>TIME_POWER)
  {
    power_cpt=0;
    if(!power_bool && (spr1_vtmax==VT_MAX-1 || spr2_vtmax==VT_MAX-1))
    {
      i=random(10);
      power_x=pos_power[0][i];
      power_y=pos_power[1][i];
      GetBlk2(power_x,power_y,16,12,page1,power_fnd);
      power_bool=1;
    }
  }

  //Affichage d'un power
  if(power_bool)
  {
    if(++poweri==13) poweri=0;
    PutBlk2(power_x,power_y,16,12,page1,power_fnd);
    PutSpr(power_x,power_y,16,12,page1,power[poweri]);
  }

  //Spr1
  GetBlk2(spr1_x-4,spr1_y-4,26,26,page1,spr1_fnd);  //Sprite,ombre,rÇacteur
  PutSpr(spr1_x,spr1_y,19,18,page1,spr1[spr1_dir]); //Sprite

  //Feux du rÇacteur
  if(spr1_vt==VT_MAX)
  {
    if(spr1_feuxi)
    {
      PutSpr(spr1_x+off_feux[0][spr1_dir],spr1_y+off_feux[1][spr1_dir],6,6,page1,feux[spr1_dir]);
      spr1_feuxi=0;
    }
    else spr1_feuxi++;
  }

  //FumÇe
  GetBlk2(spr1_chocx,spr1_chocy,10,9,page1,spr1_fnd_fume);
  if(spr1_choc)
  {
    PutSpr(spr1_chocx,spr1_chocy,9,9,page1,fume[spr1_choci]);
    if(++spr1_choci==6) spr1_choc=0;
  }

  //Spr2
  GetBlk2(spr2_x-4,spr2_y-4,26,26,page1,spr2_fnd);  //Sprite,ombre,rÇacteur
  PutSpr(spr2_x,spr2_y,19,18,page1,spr2[spr2_dir]); //Sprite

  //RÇacteur
  if(spr2_vt==VT_MAX)
  {
    if(spr2_feuxi)
    {
      PutSpr(spr2_x+off_feux[0][spr2_dir],spr2_y+off_feux[1][spr2_dir],6,6,page1,feux[spr2_dir]);
      spr2_feuxi=0;
    }
    else spr2_feuxi++;
  }

  //FumÇe
  GetBlk2(spr2_chocx,spr2_chocy,10,9,page1,spr2_fnd_fume);
  if(spr2_choc)
  {
    PutSpr(spr2_chocx,spr2_chocy,9,9,page1,fume[spr2_choci]);
    if(++spr2_choci==6) spr2_choc=0;
  }

  //Neige
  for(i=0;i<NEIGE;i++)
  {
    y_neige[i]+=vt_neige[i];
    if(y_neige[i]>195)
    {
      x_neige[i]=random(320);
      y_neige[i]=0;
      vt_neige[i]=1+random(3);
    }
    buf_neige[i]=page1[x_neige[i]+y_neige[i]*320];
    page1[x_neige[i]+y_neige[i]*320]=255;
  }

  //LAST LAP spr1
  if(time1_lp<120 && spr1_tour==NB_TOUR-1)
  {
    ++time1_lp;
    GetBlk2(130,50,60,14,page1,buf_lastlap1);
    if(time1_lp<90 || (time1_lp>90 && time1_lp%2))
      PutSpr(130,50,60,14,page1,lastlap1);
    bool1_lp=1;
  }
  else bool1_lp=0;

  //LAST LAP spr2
  if(time2_lp<120 && spr2_tour==NB_TOUR-1)
  {
    ++time2_lp;
    GetBlk2(130,110,60,14,page1,buf_lastlap2);
    if(time2_lp<90 || (time2_lp>90 && time2_lp%2))
     PutSpr(130,110,60,14,page1,lastlap2);
    bool2_lp=1;
  }
  else bool2_lp=0;

  //Temporisation du programme
  if(pause)
    for(i=0;i<pause;i++) VBL;

  CpyScr(page1,Video);

  //Restitution des fonds
  if(bool1_lp) PutBlk2(130,50,60,14,page1,buf_lastlap1);
  if(bool2_lp) PutBlk2(130,110,60,14,page1,buf_lastlap2);

  for(i=0;i<NEIGE;i++)
    if(buf_neige[i]!=255) page1[x_neige[i]+y_neige[i]*320]=buf_neige[i];

  PutBlk2(spr2_chocx,spr2_chocy,10,9,page1,spr2_fnd_fume);
  PutBlk2(spr2_x-4,spr2_y-4,26,26,page1,spr2_fnd);
  PutBlk2(spr1_chocx,spr1_chocy,10,9,page1,spr1_fnd_fume);
  PutBlk2(spr1_x-4,spr1_y-4,26,26,page1,spr1_fnd);
}

/* PROGRAMME PRINCIPAL */

void main(void)
{
  int handle;
  byte win=0;
  word i,fin_parcour,
       pc=0;  //Mode 1(=1) ou 2(=0) joueurs;

  //DÇpart spr1
  spr1_x=200;
  spr1_y=7;
  spr1_dir=12;
  spr1_vt=2;

  //DÇpart spr2
  spr2_x=200;
  spr2_y=27;
  spr2_dir=12;
  spr2_vt=2;

  clrscr();
  printf("\n                         --=-SPRINT-MASTER-beta-=--");
  printf("\n\nCLAVIER:");
  printf("\n1er Joueur: [j],[k] et [Alt]");
  printf("\n2ie Joueur: [4],[6] et [Inser]");
  printf("\n\nVITESSE: (0=Rapide Ö 255=Lent)\n>");
  scanf("%d",&pause);

  printf("\nNOMBRE DE JOUEURS: (1,2)\n>");
  scanf("%d",&pc);
  pc=2-pc;

  randomize();

  //Chargement aleatoire du trajet de spr2
  if(pc)
  {
    if(random(2)) handle=open("tour1_p1.sm",O_RDWR);
    else handle=open("tour2_p1.sm",O_RDWR);
    read(handle,parcour,filelength(handle));
    fin_parcour=(filelength(handle)/2)-100;
    close(handle);
  }

  oldit_9=getvect(0x9);
  setvect(0x9,scankbd);
  init();
  screen();

  do
  {
    lire_keys();
    controle_spr1();

    //Lecture du trajet
    if(pc)
    {
      if(parcour[i_parcour]) //x != 0 (fin du trajet)
      {
        if(i_parcour>50) spr2_vt=VT_MAX;      //Apparition du feu
        if(i_parcour>fin_parcour) spr2_vt=2;  //Disparition du feu
        spr2_x=parcour[i_parcour++];    //x
        spr2_y=parcour[i_parcour++];    //y
        spr2_dir=parcour[i_parcour++];  //dir
        controle2_spr2(spr2_x,spr2_y,spr2_dir);
      }
      else spr2_vt=0;
    }
    else controle_spr2();

    affichage();

    //Test du gagnant
    if(spr1_tour==NB_TOUR && !win) win=1;
    if(spr2_tour==NB_TOUR && !win) win=2;

  }while(!SCAN_CODES[ESC].stat && (spr1_tour!=NB_TOUR || spr1_vt) && (spr2_tour!=NB_TOUR || spr2_vt));

  setvect(0x9,oldit_9);
  ClrScr(page1);
  screen();

  /* Fin de SPRINT MASTER beta */

  ModeTxt();
  textbackground(BLACK);
  clrscr();

  if(win==1) printf("\n The WINNER is RED\n");
  else if(win==2) printf("\n The WINNER is GREEN\n");
  else printf("\n No WINNER\n");

  textcolor(YELLOW);
  cputs("\n\r-SPRINT-MASTER-beta- by Siracusa walter/wiSdom\n\n\r");
  textcolor(WHITE);
}
