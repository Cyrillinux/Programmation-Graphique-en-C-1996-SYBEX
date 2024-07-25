/*************************************************************/
// TITRE  : AFFICHAGE D'UN TORE EN MAPPING-PHONG-SHADING
// AUTEUR : Patrice SIRACUSA/wiSdom
// PROJET : EXE11.C, S3D3.C, S3DREAD.C, FILLMAP.C, FILLUNI.C
//          MATHS.C, R3D.ASM, FILLGS.C, FILLPG.C, VGA.C
//          VGA_ASM.ASM
/*************************************************************/

#include <conio.h>

#include "maths.h"
#include "vga.h"
#include "s3d3.h"

/* Recuperer la page de dessin */
extern unsigned char FAR EcranV1[65535];
/* Recuperer l'adresse video */
static bytef *Video = MK_FP(0xA000,0);
/* Recuperer la palette de couleurs */
extern unsigned char FAR Pal256[256*3];

/* Declarer un objet 3D */
obj3D_t *Objet1;

/* Declarer les param. globaux de la 3D */
param3D_t param3D;

static void mapAll(obj3D_t *obj)
{
    short i;
    for(i=0;i<obj->nbFaces;i++)
    {
      obj->fMap[i].xa = 1;
      obj->fMap[i].ya = 2;
      obj->fMap[i].xb = 1;
      obj->fMap[i].yb = 96;
      obj->fMap[i].xc = 95;
      obj->fMap[i].yc = 96;
    }
}

void main(void)
{
  /* Passer en mode graphique */
  ModeVGA();

  /* On va remplir les faces de l'objet sans utiliser de mapping */
  param3D.render = mapping;
  /* PHONG-SHADING */
  param3D.shading = phong;
  /* On affiche les faces en utilisant le tri en z */
  param3D.affMethod = z_order;

  /* init. la lib 3D */
  if(!initLib_3D(&param3D))
    return;

  /* lire l'objet */
  if(!(Objet1 = readObjet_3D("tor.asc")))
    return;

  /* reduire l'objet */
  zoom_3D(Objet1,0.5);

  /* lire la map-image */
  if(!loadMapImage("mapping"))
     return;

  /* affecter des portions d'image a l'objet */
  mapAll(Objet1);

  while(!kbhit())
  {
    /* Faire tourner l'objet en angle relatif */
    relRotX_3D(Objet1,-4);
    relRotY_3D(Objet1,5);
    relRotZ_3D(Objet1,-4);

    /* Afficher l'objet dans la page EcranV1 */
    affScene_3D(EcranV1);

    VBL;  /* Attendre un VBL */

    /* Copier a l'ecran */
    CpyScr(EcranV1,Video);

    /* Effacer la page EcranV1 */
    ClrScr(EcranV1);
  }

  /* Liberer la lib 3D */
  endLib_3D();

  /* Retour au mode texte */
  ModeTxt();
}
