/*************************************************************/
// TITRE  : DAMION 94
// AUTEUR : Patrice & Walter SIRACUSA / wiSdom
// PROJET : DAMION.C, MATHS.C, _3D.C, FILL.C, VGA_ASM.ASM,
//          VGA.C
/*************************************************************/

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>

#include <vga.h>
#include "_3D.h"

#define RETURN 13
#define ESC 27
#define ESPACE 32
#define HAUT 72
#define GAUCHE 75
#define DROITE 77
#define BAS 80
#define PROF  150    //Profondeur
#define PAS_PROF 30  //Multiple de PROF
#define SPACE 50     //Pixels de l'espace

word cursx=0,cursy=0; //Position du curseur

typedef struct _damion
{
  byte etat, //0 = enfoncee ou 1 = sortie
       coul; //BLUE(haut),GREEN(droite),RED(bas),CYAN(gauche) ou MAGENTA(tous)
  word zetat; //Niveau d'elevation 0 = enfoncee ou PROF = sortie
}t_damion;
t_damion Damion[5][5];

int far Xspace[SPACE];  //Espace horizontal (droite-gauche)
bytef Yspace[SPACE];
bytef Vspace[SPACE];

bytef page1[64000];
bytef *Video=MK_FP(0xa000,0);

short Cote = 20*4;
obj3D_t Damion3D[5][5];
camera_t  Camera0;

void affDamion(void);

void CaseBleu(void)
{
  if(cursy>0) Damion[cursx][cursy-1].etat=1-Damion[cursx][cursy-1].etat;
  else Damion[cursx][4].etat=1-Damion[cursx][4].etat;
}

void CaseVerte(void)
{
  if(cursx<4) Damion[cursx+1][cursy].etat=1-Damion[cursx+1][cursy].etat;
  else Damion[0][cursy].etat=1-Damion[0][cursy].etat;
}

void CaseRouge(void)
{
  if(cursy<4) Damion[cursx][cursy+1].etat=1-Damion[cursx][cursy+1].etat;
  else Damion[cursx][0].etat=1-Damion[cursx][0].etat;
}

void CaseCyan(void)
{
  if(cursx>0) Damion[cursx-1][cursy].etat=1-Damion[cursx-1][cursy].etat;
  else Damion[4][cursy].etat=1-Damion[4][cursy].etat;
}

byte modifDamion(byte aff)
{
  word i,j,z;

  Damion[cursx][cursy].etat=1-Damion[cursx][cursy].etat;

  switch(Damion[cursx][cursy].coul)  //Modifie 'etat' des autres cubes
  {
    case BLUE:    CaseBleu();break;
    case GREEN:   CaseVerte();break;
    case RED:     CaseRouge();break;
    case CYAN:    CaseCyan();break;
    case MAGENTA: CaseBleu();CaseVerte();CaseRouge();CaseCyan();break;
  }

  for(z=0;z<PROF;z+=PAS_PROF) //Elevation des cubes: 'zetat' par rapport … 'etat'
  {
    for(j=0;j<5;j++)
    {
      for(i=0;i<5;i++)
      {
        if(Damion[i][j].etat && Damion[i][j].zetat!=PROF) Damion[i][j].zetat+=PAS_PROF;
        if(!Damion[i][j].etat && Damion[i][j].zetat) Damion[i][j].zetat-=PAS_PROF;
      }
    }
    if(aff) affDamion();
  }

  for(j=0;j<5;j++)   //Teste si le damion est reussi -> 1, sinon -> 0
    for(i=0;i<5;i++)
      if(Damion[i][j].etat) return 0;
  return 1;
}

void InitDamion(void)
{
  byte cursx0,cursy0;
  word i,j;

  randomize();

  for(j=0;j<5;j++)
  {
    for(i=0;i<5;i++)
    {
      Damion[i][j].etat=0;   //Valeur de reference (Damion enfonce)
      Damion[i][j].zetat=0;
      switch(random(5))  //Place les cubes aleatoirement
      {
        case 0: Damion[i][j].coul=BLUE;break;
        case 1: Damion[i][j].coul=GREEN;break;
        case 2: Damion[i][j].coul=RED;break;
        case 3: Damion[i][j].coul=CYAN;break;
        case 4: Damion[i][j].coul=MAGENTA;break;
      }
    }
  }

  cursx0=random(5);
  cursy0=random(5);

  for(cursy=0;cursy<5;cursy++)  //Solution unique
    for(cursx=0;cursx<5;cursx++)
      if(cursx!=cursx0 || cursy!=cursy0) modifDamion(0);

  cursx=cursx0;cursy=cursy0;    //Curseur indiquant la solution unique
}

/******************************/

static void InitDamion3D(void)
{
  #define ECART 50
  short i,j,tx,ty,ipal,f,d1,d2;

  //Init. des coordonnees

  for(i = 0; i < 5; i++)
  {
     tx = (i-2)*Cote*2+ECART;

     d1 = i*ECART;

     for(j = 0; j < 5; j++)
     {

        Damion3D[i][j].nbPoints = 8;
        Damion3D[i][j].tabPointsI[0][0] = (Cote+tx)+d1;
        Damion3D[i][j].tabPointsI[0][1] = (-Cote+tx)+d1;
        Damion3D[i][j].tabPointsI[0][2] = (-Cote+tx)+d1;
        Damion3D[i][j].tabPointsI[0][3] = (Cote+tx)+d1;
        Damion3D[i][j].tabPointsI[0][4] = (Cote+tx)+d1;
        Damion3D[i][j].tabPointsI[0][5] = (-Cote+tx)+d1;
        Damion3D[i][j].tabPointsI[0][6] = (-Cote+tx)+d1;
        Damion3D[i][j].tabPointsI[0][7] = (Cote+tx)+d1;

        ty = (j-2)*Cote*2;
        d2 = -(j*ECART);

        Damion3D[i][j].tabPointsI[1][0] = (Cote-ty)+d2;
        Damion3D[i][j].tabPointsI[1][1] = (Cote-ty)+d2;
        Damion3D[i][j].tabPointsI[1][2] = (-Cote-ty)+d2;
        Damion3D[i][j].tabPointsI[1][3] = (-Cote-ty)+d2;
        Damion3D[i][j].tabPointsI[1][4] = (Cote-ty)+d2;
        Damion3D[i][j].tabPointsI[1][5] = (Cote-ty)+d2;
        Damion3D[i][j].tabPointsI[1][6] = (-Cote-ty)+d2;
        Damion3D[i][j].tabPointsI[1][7] = (-Cote-ty)+d2;

        Damion3D[i][j].tabPointsI[2][0] = Cote;
        Damion3D[i][j].tabPointsI[2][1] = Cote;
        Damion3D[i][j].tabPointsI[2][2] = Cote;
        Damion3D[i][j].tabPointsI[2][3] = Cote;
        Damion3D[i][j].tabPointsI[2][4] = -Cote;
        Damion3D[i][j].tabPointsI[2][5] = -Cote;
        Damion3D[i][j].tabPointsI[2][6] = -Cote;
        Damion3D[i][j].tabPointsI[2][7] = -Cote;

        Damion3D[i][j].nbFaces = 6;
        Damion3D[i][j].nbPFaces[0] = Damion3D[i][j].nbPFaces[1] = Damion3D[i][j].nbPFaces[2] = Damion3D[i][j].nbPFaces[3] = 4;
        Damion3D[i][j].nbPFaces[4] = Damion3D[i][j].nbPFaces[5] = 4;

        Damion3D[i][j].tabPFaces[0][0] = 0;
        Damion3D[i][j].tabPFaces[0][1] = 1;
        Damion3D[i][j].tabPFaces[0][2] = 2;
        Damion3D[i][j].tabPFaces[0][3] = 3;

        Damion3D[i][j].tabPFaces[1][0] = 5;
        Damion3D[i][j].tabPFaces[1][1] = 4;
        Damion3D[i][j].tabPFaces[1][2] = 7;
        Damion3D[i][j].tabPFaces[1][3] = 6;

        Damion3D[i][j].tabPFaces[2][0] = 4;
        Damion3D[i][j].tabPFaces[2][1] = 0;
        Damion3D[i][j].tabPFaces[2][2] = 3;
        Damion3D[i][j].tabPFaces[2][3] = 7;

        Damion3D[i][j].tabPFaces[3][0] = 1;
        Damion3D[i][j].tabPFaces[3][1] = 5;
        Damion3D[i][j].tabPFaces[3][2] = 6;
        Damion3D[i][j].tabPFaces[3][3] = 2;

        Damion3D[i][j].tabPFaces[4][0] = 4;
        Damion3D[i][j].tabPFaces[4][1] = 5;
        Damion3D[i][j].tabPFaces[4][2] = 1;
        Damion3D[i][j].tabPFaces[4][3] = 0;

        Damion3D[i][j].tabPFaces[5][0] = 3;
        Damion3D[i][j].tabPFaces[5][1] = 2;
        Damion3D[i][j].tabPFaces[5][2] = 6;
        Damion3D[i][j].tabPFaces[5][3] = 7;
     }
  }

  //Init des indices de palette

  ipal = 10;
  for(i=0;i<5;i++)
  {
    for(j=0;j<5;j++)
    {
       for(f=0;f<6;f++)
       {
         //Pour chaque face d'un meme cube, un indice de palette diff.
         Damion3D[i][j].tabFill[f].indPal = ipal;
         ipal++;
       }
    }
  }

  //Init des couleurs des cubes

  InitDamion(); //Damion aleatoire

  for(i=0;i<5;i++)
  {
    for(j=0;j<5;j++)
    {
      //Pour chaque face d'un meme cube, la meme couleur
      switch(Damion[i][j].coul)
      {
        case BLUE    : for(f=0;f<6;f++)
                       {
                         Damion3D[i][j].tabFill[f].r = 0;
                         Damion3D[i][j].tabFill[f].v = 0;
                         Damion3D[i][j].tabFill[f].b = 63;
                       }break;
        case GREEN   : for(f=0;f<6;f++)
                       {
                         Damion3D[i][j].tabFill[f].r = 63;
                         Damion3D[i][j].tabFill[f].v = 55;
                         Damion3D[i][j].tabFill[f].b = 0;
                       }break;
        case RED     : for(f=0;f<6;f++)
                       {
                         Damion3D[i][j].tabFill[f].r = 63;
                         Damion3D[i][j].tabFill[f].v = 0;
                         Damion3D[i][j].tabFill[f].b = 0;
                       }break;
        case CYAN    : for(f=0;f<6;f++)
                       {
                         Damion3D[i][j].tabFill[f].r = 0;
                         Damion3D[i][j].tabFill[f].v = 50;
                         Damion3D[i][j].tabFill[f].b = 0;
                       }break;
        case MAGENTA : for(f=0;f<6;f++)
                       {
                         Damion3D[i][j].tabFill[f].r = 63;
                         Damion3D[i][j].tabFill[f].v = 0;
                         Damion3D[i][j].tabFill[f].b = 63;
                       }break;
      }
    }
  }
}

void initPalDamion(void)
{
  char i,j,f;

  for(i=0;i<5;i++)
    for(j=0;j<5;j++)
      for(f=0;f<6;f++)
        Ink(Damion3D[i][j].tabFill[f].indPal,Damion3D[i][j].tabFill[f].r,Damion3D[i][j].tabFill[f].v,Damion3D[i][j].tabFill[f].b);
}

void affDamion(void)
{
   char i,j,face;

   ClrScr(page1);

   for(i=0;i<SPACE;i++)   //Espace
   {
     Xspace[i]-=Vspace[i];
     if(Xspace[i]<0)
     {
       Xspace[i]=319;
       Yspace[i]=random(199);
     }
     page1[Xspace[i]+Yspace[i]*320]=Vspace[i];
   }

   for(i=0;i<5;i++)       //Damion
     for(j=0;j<5;j++)
     {
       absTransZ_3D(&Damion3D[i][j],Damion[i][j].zetat);
       traceObjet_3D(&Damion3D[i][j],&Camera0,page1);
       if((i==cursx) && (j==cursy))  //Contour du cube selectionne
         for(face=0;face<6;face++)
         {
           traceContour_3D(&Damion3D[cursx][cursy],0,&Camera0,page1,5+Damion[cursx][cursy].etat);
           traceContour_3D(&Damion3D[cursx][cursy],2,&Camera0,page1,5+Damion[cursx][cursy].etat);
           traceContour_3D(&Damion3D[cursx][cursy],5,&Camera0,page1,5+Damion[cursx][cursy].etat);
         }
     }

   CpyScr(page1,Video);
}

void moveCamera(void)
{
  relRotYCamera(&Camera0,24);
  relRotXCamera(&Camera0,-36);
  relTransXCamera(&Camera0,42);
  relTransYCamera(&Camera0,-90);
  relTransZCamera(&Camera0,-12);
}

void initSpace(void)
{
  word i;

  randomize();

  for(i=1;i<5;i++) Ink(i,(i+2)*10,(i+2)*10,(i+2)*10);  //4 Couleurs grises

  for(i=0;i<SPACE;i++)
  {
    Xspace[i]=random(319);
    Yspace[i]=random(199);
    Vspace[i]=random(4)+1;  //Vitesse en fonction de la couleur
  }
}

void main(void)
{
  char end = 1;
  char key;
  short i,j;
  byte fin_damion=0;

  clrscr();
  puts("\n- DAMION -");
  getch();

  ModeVGA();
  initSinCos();

  initSpace();
  InitDamion3D();
  initPalDamion();

  //Init struct damion
  for(i=0;i<5;i++)
     for(j=0;j<5;j++)
         initStruct_3D(&Damion3D[i][j]);

  setResolScreen(320,200);

  initCamera(&Camera0,0,0,600);

  setVisualType(perspective);

  Ink(5,50,50,50);  //Curseur bas
  Ink(6,60,60,60);  //Curseur haut
  setRenderMode(fill_HF_S);

  moveCamera();

  for(i=0;i<40;i++)  //Disparition du Damion
  {
    relTransZCamera(&Camera0,-10);
    affDamion();
  }

  affDamion();

  do
  {
    if(kbhit())
    {
      key=getch();
      switch(key)
      {
        case DROITE : if(cursx<4)
                      {
                        relRotYCamera(&Camera0,3);
                        cursx++;
                      }break;
        case GAUCHE : if(cursx>0)
                      {
                        relRotYCamera(&Camera0,-3);
                        cursx--;
                      }
                      break;
        case HAUT   : if(cursy>0)
                      {
                        relRotXCamera(&Camera0,3);
                        cursy--;
                      }
                      break;
        case BAS    : if(cursy<4)
                      {
                        relRotXCamera(&Camera0,-3);
                        cursy++;
                      }break;
        case ESPACE : fin_damion=modifDamion(1);
                      break;
        case ESC    : end = 0;
      }
    }

    affDamion();

  }while(end && !fin_damion);

  if(fin_damion)       //S'il est reussi,
    for(i=0;i<60;i++)  //disparition du Damion
    {
      relTransZCamera(&Camera0,10);
      affDamion();
    }

  ModeTxt();
  if(fin_damion) printf("\nDAMION r‚ussi !\n");
  else printf("\nPerdu par abandon...\n");
}
