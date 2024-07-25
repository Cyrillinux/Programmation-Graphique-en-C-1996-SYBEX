/*************************************************************/
// TITRE  : AFFICHAGE D'UN OBJET EN GOURAUD-SHADING
// AUTEUR : Patrice SIRACUSA/wiSdom, thanks to POLLUX/OLYMPUS
// PROJET : EXE6.C, S3D3.C, S3DREAD.C, FILLMAP.C, FILLUNI.C
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

void main(void)
{
  /* Passer en mode graphique */
  ModeVGA();

  /* On va remplir les faces de l'objet sans utiliser de mapping */
  param3D.render = no_mapping;
  /* GOURAUD-SHADING */
  param3D.shading = gouraud;
  /* On affiche les faces en utilisant le tri en z */
  param3D.affMethod = z_order;

  /* init. la lib 3D */
  if(!initLib_3D(&param3D))
    return;

  /* lire l'objet */
  if(!(Objet1 = readObjet_3D("jar.asc")))
    return;

  /* reduire sa taille de moitie */
  zoom_3D(Objet1,0.5);

  /* lire la palette */
  if(!loadPal256("goum.PAL"))
    return;

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
