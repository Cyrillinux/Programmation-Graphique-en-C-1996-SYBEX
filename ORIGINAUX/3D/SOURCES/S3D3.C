 /*
  * S3D3.C : gestion d'objets 3D bases sur des triangles
  *
  * Auteur : Patrice Siracusa.
  *
  * Mes remerciements a BARTI/NOON, POLLUX/OLYMPUS
  *
  */

 #include <math.h>
 #include <stdio.h>
 #include <alloc.h>
 #include <stdlib.h>
 #include <stdio.h>

 #include "maths.h"
 #include "s3d3.h"
 #include "fill.h"
 #include "vga.h"

 static bytef *Video = MK_FP(0xA000,0);

 /* Parametres globaux de 3D */

 unsigned     short Ox,Oy;  /* centre de l'ecran (160,100 si MCGA) */
 render_t     Rendering;    /* mapping, no_mapping ... */
 affMethod_t  AffMethod;    /* z_order, z_buffer ... */
 shading_t    Shading;      /* flat, gouraud ... */

 /* 3 points courant du triangle a afficher */

 short XA,YA,ZA,XB,YB,ZB,XC,YC,ZC;
 short CoulFace;   /* Couleur courante de la face */

 /* Page de mapping */

 unsigned char FAR PageMap[64000];  /* 320*200 */

 /* 3 points courant dans la page de mapping */

 long  XAmap,YAmap,XBmap,YBmap,XCmap,YCmap;

 /* Palette globale */

 unsigned char FAR Pal256[256*3];  /* 3 : R,G,B */

 /* Z-BUFFER */

 char FAR ZBuffer[64000];   /* -128<=z<=+127 (ecran de 320*200) */

 /* Page de dessin */

 unsigned char FAR EcranV1[64000]; /* 320*200 */

 /* Gouraud */

 /* Composantes z courantes aux sommets a,b,c */

 short Refza,Refzb,Refzc;

 /* Phong */

 /* Composantes X,Y aux sommets a,b,c */

 long RefXa,RefYa,RefXb,RefYb,RefXc,RefYc;
 unsigned char TPhongXY[65535];

 /* Tableau des objets 3D */

 obj3D_t TObjetS[MAX_OBJ];
 short IndObjetS = 0;   /* nb objets courant (indice courant sur TObjetS) */


 /* Affichage par tri en z : Listes des faces */

 short FAR *TabFaces;   /* Liste des faces visibles a afficher */
 short FAR *pTabFaces;  /* pointeur sur cette liste */
 short TailleElem;      /* Taille d'un elem. en octets de cette liste */
 short NbFaces=0;        /* Nb de faces (courant) visibles a afficher */

 short ObjTransZ;
 short CoefFlat;        /* Coef. courant [0..15] d'assombrissement (FLAT-SHADING) */

 /* structure des elements faces a trier */

 typedef struct
 {
   short i;
   short z;

 } elemTriFace_t;

 elemTriFace_t *TabInd;  /* Listes des faces a trier */

/* Structure des vecteurs normaux deja utilises */

 typedef struct
 {
   short xn,yn,zn;
 } vectN_t;

 static vectN_t *TVectN;
 static short IndTVectN;

 /*********************** Fonctions ******************************/

 /*
  * ZOMM_XY(v,p)
  *
  * Renvoie la nlle valeur de v en fct de la profondeur p
  */

 static short ZOOM_XY(short v,short p)
 {
   float squeeze;
   float zoom;

   if(!p)   return(v);

   /* zoom */
   if(p>0)
   {
      zoom = 1+p*COEF_ZOOM;
      return v*zoom;
   }

   /* else squeeze */
   squeeze = 1+p*COEF_SQUEEZE;
   if(squeeze<0.)
     squeeze = 0.;
   return v*squeeze;
 }

 /*
  * initTPhongXY(void)
  *
  * Precalcule les rencontres des compo. x,y des vect. reflex
  * en determine la compos. z puis fixe l'encre resultat
  * Dans le phong je fais tourner les vecteurs a la surface de l'objet
  * donc toutes les normes vallent 1.
  */

  static void initTPhongXY(void)
  {
    long x,y,c,l;
    float xr,yr,zr,rac;

    for(c=0;c<255;c++)
    {
      for(l=0;l<255;l++)
      {
         x=c-127;
         y=l-127;
         xr = (float)x/127.;
         yr = (float)y/127.;
         rac = 1. - (xr*xr) - (yr*yr);
         if(rac>=0)
           zr = sqrt(rac);
         else
           zr = 0.;
         TPhongXY[(l*255L)+c] = zr*255.;
      }
    }
  }

 /*
  * initVectNormaux_3D(obj)
  *
  * Initialise les vecteurs normaux de toutes les faces de l'objet
  */

 static void initVectNormaux_3D(obj3D_t *obj)
 {
   short p0x,p0y,p0z,p1x,p1y,p1z,p2x,p2y,p2z,*p;
   unsigned short face;
   short pta,ptb,ptc;
   long va,vb,vc,norme,mulval;

   for(face = 0; face < obj->nbFaces; face++)
   {
      pta = obj->ptA[face];
      ptb = obj->ptB[face];
      ptc = obj->ptC[face];

      p0x = obj->xCur[pta];
      p0y = obj->yCur[pta];
      p0z = obj->zCur[pta];

      p1x = obj->xCur[ptb];
      p1y = obj->yCur[ptb];
      p1z = obj->zCur[ptb];

      p2x = obj->xCur[ptc];
      p2y = obj->yCur[ptc];
      p2z = obj->zCur[ptc];


      /* produit vectoriel */

      va  =  (long) (p0y - p1y)*(p0z + p1z)
              + (long) (p1y - p2y)*(p1z + p2z)
              + (long) (p2y - p0y)*(p2z + p0z);

      vb  =  (long) (p0z - p1z)*(p0x + p1x)
              + (long) (p1z - p2z)*(p1x + p2x)
              + (long) (p2z - p0z)*(p2x + p0x);

      vc  =  (long) (p0x - p1x)*(p0y + p1y)
              + (long) (p1x - p2x)*(p1y + p2y)
              + (long) (p2x - p0x)*(p2y + p0y);

      /* calcul de la norme du vecteur normal */

      norme = sqrt((float)va*(float)va + (float)vb*(float)vb + (float)vc*(float)vc);

      /* rendre les vecteurs normaux unitaires (norme = 1) */

      if(norme)
      {
        /* On etale les valeurs [0..1] entre [0..MulVal-1] */

        if(Shading == gouraud)
        {
          if(Rendering == mapping)
             mulval = 32;
          else
             mulval = 256;
        }
        else
        {
          if(Shading == phong)
          {
             mulval = 128;
          }
          else
          {
             /* par defaut */
            mulval = 16;
          }
        }

        obj->xVectN[face] = (va*(mulval-1))/norme;
        obj->yVectN[face] = (vb*(mulval-1))/norme;
        obj->zVectN[face] = (vc*(mulval-1))/norme;
      }
      else
      {
        /* Le vecteur normal est nul
         * Il s'agit d'une face qui n'en est pas une
         */
        obj->xVectN[face] = 0;
        obj->yVectN[face] = 0;
        obj->zVectN[face] = 0;
      }
   }
 }

 /*
  * initTVectN(void)
  *
  * Initialise le tableau des vecteurs normaux deja utilises
  */

 static short allocTVectN(void)
 {
   TVectN = (vectN_t *)MALLOC(MAX_FACES*sizeof(vectN_t));
   if(TVectN)
     return 1;
   else
     return 0;
 }

 static void freeTVectN(void)
 {
   FREE(TVectN);
 }

 static void initTVectN(void)
 {
   IndTVectN=0;
 }

 /*
  * existeVectN(short xn,short yn,short zn)
  *
  * Rend 1 si le vecteur normal est deja utilise
  * Rend 0 sinon
  */

 static short existeVectN(short xn,short yn,short zn)
 {
   short i;
   for(i=0;i<IndTVectN;i++)
   {
      if((TVectN[i].xn == xn) && (TVectN[i].yn == yn) && (TVectN[i].zn == zn))
      {
        return 1;
      }
   }
   return 0;
 }

 /*
  * ajouteVectN(short xn,short yn,short zn)
  *
  * Ajoute le vecteur normal en parametre
  * au tableau des vecteurs normaux deja utilises
  */

 static void ajouteVectN(short xn,short yn,short zn)
 {
   TVectN[IndTVectN].xn = xn;
   TVectN[IndTVectN].yn = yn;
   TVectN[IndTVectN].zn = zn;
   IndTVectN++;
 }

 /*
  * initVectReflex_3D(obj)
  *
  * Initialise les vecteurs reflexions de tous les sommets de l'objet
  */

 static void initVectReflex_3D(obj3D_t *obj)
 {
   unsigned short s;
   char a,b,c;
   long xr,yr,zr;
   unsigned short f,nbf;
   short xn,yn,zn;

   allocTVectN();

   for(s=0; s<obj->nbPoints; s++)
   {
      nbf=0;
      xr = yr = zr = 0;
      initTVectN();

      /* rechercher toutes les faces liees au sommet s */

      for(f=0; f<obj->nbFaces; f++)
      {
         a=(s==obj->ptA[f]);
         b=(s==obj->ptB[f]);
         c=(s==obj->ptC[f]);

         if(a || b || c)
         {
            /* cette face est liee au sommet s */

            /* verifier que ce vecteur normal n'est pas deja utilise */

            xn = obj->xVectN[f];
            yn = obj->yVectN[f];
            zn = obj->zVectN[f];

            if(!existeVectN(xn,yn,zn))
            {
                /* Ne pas prendre en compte le vecteur nulle */

                if((xn!=0) || (yn!=0) || (zn!=0))
                {
                    /* on somme les x,y et les z */

                    xr += xn;
                    yr += yn;
                    zr += zn;
                    nbf++;

                    /* marquer ce vecteur normal comme deja utilise */

                    ajouteVectN(xn,yn,zn);
                }
            }
         }
      }
      if(nbf)
      {
         /* faire la moyenne des composantes */

         /* => les vecteurs sont unitaires */

         obj->xReflex[s] = xr/nbf;
         obj->yReflex[s] = yr/nbf;
         obj->zReflex[s] = zr/nbf;
      }
      else
      {
         /* aucune face n'est associee a ce sommet ? */

         obj->xReflex[s] = 0;
         obj->yReflex[s] = 0;
         obj->zReflex[s] = 0;
      }
   }

   freeTVectN();
 }

 /*
  * keepRealTriangle(obj)
  *
  * Elimination des faces reduits a des segments ou des points
  *
  */

 static char memePoint(short x1,short y1,short z1,short x2,short y2,short z2)
 {
   return ((x1 == x2) && (y1 == y2) && (z1 == z2));
 }

 static void keepRealTriangle(obj3D_t *obj)
 {
   unsigned short f,nf;
   short pta,ptb,ptc,p0x,p0y,p0z,p1x,p1y,p1z,p2x,p2y,p2z;
   short *newptA;
   short *newptB;
   short *newptC;

   newptA = (short FAR *)MALLOC(obj->nbFaces*SIZE_SHORT);
   newptB = (short FAR *)MALLOC(obj->nbFaces*SIZE_SHORT);
   newptC = (short FAR *)MALLOC(obj->nbFaces*SIZE_SHORT);

   if(newptA && newptB && newptC)
   {
     nf = 0;
     for(f=0;f<obj->nbFaces;f++)
     {
        pta = obj->ptA[f];
        ptb = obj->ptB[f];
        ptc = obj->ptC[f];

        p0x = obj->xCur[pta];
        p0y = obj->yCur[pta];
        p0z = obj->zCur[pta];

        p1x = obj->xCur[ptb];
        p1y = obj->yCur[ptb];
        p1z = obj->zCur[ptb];

        p2x = obj->xCur[ptc];
        p2y = obj->yCur[ptc];
        p2z = obj->zCur[ptc];

        /* elimination  des faces segments ou points */
        if(   !memePoint(p0x,p0y,p0z,p1x,p1y,p1z)  // A != B
           && !memePoint(p0x,p0y,p0z,p2x,p2y,p2z)  // A != C
           &&  !memePoint(p1x,p1y,p1z,p2x,p2y,p2z)) // B != C
        {
            newptA[nf] = pta;
            newptB[nf] = ptb;
            newptC[nf] = ptc;
            nf++;
        }
     }

     /* reconstruction de la liste des faces */

     obj->nbFaces = nf;
     for(f=0;f<obj->nbFaces;f++)
     {
       obj->ptA[f] = newptA[f];
       obj->ptB[f] = newptB[f];
       obj->ptC[f] = newptC[f];
     }

     FREE(newptA);
     FREE(newptB);
     FREE(newptC);
   }
 }

 /*
  * normVectReflex_3D(obj3D_t *obj,short coef)
  *
  * Normalise les vecteurs reflexions => normes == coef
  */

 static void normVectReflex_3D(obj3D_t *obj,short coef)
 {
   unsigned short s;
   float x,y,z,f,n;

   for(s=0;s<obj->nbPoints;s++)
   {
      x = obj->xReflex[s];
      y = obj->yReflex[s];
      z = obj->zReflex[s];

      n = sqrt((x*x)+(y*y)+(z*z));
      x = x/n;
      y = y/n;
      z = z/n;
      obj->xReflex[s] = x*coef;
      obj->yReflex[s] = y*coef;
      obj->zReflex[s] = z*coef;
   }
 }

 /*
  * initStruct_3D(obj)
  *
  * Init. des sommets courants de l'objet
  * - calcul des vecteurs normaux des faces
  * - calcul des vecteurs reflexions des sommets
  */

 void initStruct_3D(obj3D_t *obj)
 {
   short i;
   unsigned short nbf;

   /* rotations && translations */
   obj->rX = 0;
   obj->rY = 0;
   obj->rZ = 0;
   obj->trX = 0;
   obj->trY = 0;
   obj->trZ = 0;

   /* init. des sommets courants */
   for(i = 0; i < obj->nbPoints; i++)
   {
      obj->xCur[i] = obj->xIni[i];
      obj->yCur[i] = obj->yIni[i];
      obj->zCur[i] = obj->zIni[i];
   }

   /* elimination des triangles reduits a des segments ou des points */
   keepRealTriangle(obj);

    /* init. des vect. normaux */
   initVectNormaux_3D(obj);

   if((Shading == gouraud) || (Shading == phong))
   {
     /* init. des vect. reflexions */
     initVectReflex_3D(obj);

     /* Normaliser ces vect. reflexions */
     if(Shading == phong)
     {
        normVectReflex_3D(obj,127);
     }
     else
     {
        if(Rendering == mapping)
           normVectReflex_3D(obj,31);
        else
           normVectReflex_3D(obj,255);
     }
   }
 }


 /*
  * loadPal256(file)
  *
  * Charge une palette de 256 couleurs dans le tab. Pal256
  */

 short loadPal256(char *file)
 {
   FILE *stream;

   if((stream = fopen(file,"rb")) == NULL)
     return -1;

   if(fread(Pal256,768,1,stream) != 1)
     return -1;

   fclose(stream);

   SetAllPal(Pal256);

   return 1;
 }

 /*
  * loadMapImage(ficImage)
  *
  * Charge une page de mapping dans PageMap
  */

 short loadMapImage(char *ficImage)
 {
    byte r;
    r = LoadPCX(ficImage,PageMap,Pal256);
    if(r)
     SetAllPal(Pal256);
    return (short)r;
 }

 /*
  * relTransX_3D(obj,tx)
  *
  * Translate en X tous les sommets de obj d'un pas relatif de tx
  */

 void relTransX_3D(obj3D_t *obj,short tx)
 {
   short i;

   obj->trX += tx;
   for(i = 0; i < obj->nbPoints; i++) obj->xCur[i]+=tx;
 }

 /*
  * relTransY_3D(obj,ty)
  *
  * Translate en Y tous les sommets de obj d'un pas relatif de ty
  */

 void relTransY_3D(obj3D_t *obj,short ty)
 {
   short i;

   obj->trY += ty;
   for(i = 0; i < obj->nbPoints; i++) obj->yCur[i]+=ty;
 }

 /*
  * relTransZ_3D(obj,tz)
  *
  * Translate en Z tous les sommets de obj d'un pas relatif de tz
  */

 void relTransZ_3D(obj3D_t *obj,short tz)
 {
   short i;

   obj->trZ += tz;
   for(i = 0; i < obj->nbPoints; i++) obj->zCur[i]+=tz;
 }


 /*
  * absTransZ_3D(obj,tx)
  *
  * Translate en X tous les sommets de obj a la valeur tx
  */

 void absTransX_3D(obj3D_t *obj,short tx)
 {
   short i;

   obj->trX = tx;
   for(i = 0; i < obj->nbPoints; i++) obj->xCur[i]=tx;
 }


 /*
  * absTransY_3D(obj,ty)
  *
  * Translate en Y tous les sommets de obj a la valeur ty
  */

 void absTransY_3D(obj3D_t *obj,short ty)
 {
   short i;

   obj->trY = ty;
   for(i = 0; i < obj->nbPoints; i++) obj->yCur[i]=ty;
 }


 /*
  * absTransZ_3D(obj,ty)
  *
  * Translate en Z tous les sommets de obj a la valeur tz
  */

 void absTransZ_3D(obj3D_t *obj,short tz)
 {
   short i;

   obj->trZ = tz;
   for(i = 0; i < obj->nbPoints; i++) obj->zCur[i]=tz;
 }


 /*
  * relRotX_3D(obj,ax)
  *
  * Tourne en X obj d'un angle ax
  */

 void relRotX_3D(obj3D_t *obj,short ax)
 {
   #ifdef table256
     obj->rX = (obj->rX + ax)&255;
   #else
     obj->rX = (obj->rX + ax)%360;
   #endif
 }


 /*
  * relRotY_3D(obj,ay)
  *
  * Tourne en Y obj d'un angle ay
  */

 void relRotY_3D(obj3D_t *obj,short ay)
 {
   #ifdef table256
   obj->rY = (obj->rY + ay)&255;
   #else
   obj->rY = (obj->rY + ay)%360;
   #endif
 }

 /*
  * relRotZ_3D(obj,az)
  *
  * Tourne en Z obj d'un angle az
  */

 void relRotZ_3D(obj3D_t *obj,short az)
 {
   #ifdef table256
   obj->rZ = (obj->rZ + az)&255;
   #else
   obj->rZ = (obj->rZ + az)%360;
   #endif
 }


 /*
  * absRotX_3D(obj,ax)
  *
  * Ammene obj autour de X a l'angle ax
  */

 void absRotX_3D(obj3D_t *obj,short ax)
 {
   #ifdef table256
     obj->rX = ax & 255;
   #else
     obj->rX = ax % 360;
   #endif
 }

 /*
  * absRotY_3D(obj,ax)
  *
  * Ammene obj autour de Y a l'angle ay
  */

 void absRotY_3D(obj3D_t *obj,short ay)
 {
   #ifdef table256
   obj->rY = ay & 255;
   #else
   obj->rY = ay % 360;
   #endif
 }

 /*
  * absRotZ_3D(obj,ax)
  *
  * Ammene obj autour de Z a l'angle az
  */

 void absRotZ_3D(obj3D_t *obj,short az)
 {
   #ifdef table256
   obj->rZ = az & 255;
   #else
   obj->rZ = az % 360;
   #endif
 }


 /*
  * zoom_3D(obj,c)
  *
  * Reduit/Agrandit obj d'un coef. c
  */

 void zoom_3D(obj3D_t *obj,float c)
 {
   short i;

   for(i = 0; i < obj->nbPoints; i++)
   {
      obj->xIni[i] = obj->xIni[i]*c;
      obj->yIni[i] = obj->yIni[i]*c;
      obj->zIni[i] = obj->zIni[i]*c;
   }
 }


 /*
  * actuaObjet_3D(obj)
  *
  * Actualise les sommets de obj. (effectue la rotation)
  */

 static void actuaObjet_3D(obj3D_t *obj)
 {
   short i,a,b;
   short *px,*py,*pz;
   short x;

   for(i = 0; i < obj->nbPoints; i++)
   {

      /*   rotation X */
      a = obj->yIni[i];
      b = obj->zIni[i];
      obj->xCur[i] = obj->xIni[i];
      obj->yCur[i] = ROT_I(obj->rX,a,b);
      obj->zCur[i] = ROT_J(obj->rX,a,b);

      /*  rotation Y */
      a = obj->xCur[i];
      b = obj->zCur[i];
      x = ROT_I(obj->rY,a,b);
      obj->zCur[i] = ROT_J(obj->rY,a,b);
      obj->xCur[i] = x;

      /*  rotation Z */
      a = obj->xCur[i];
      b = obj->yCur[i];
      x = ROT_I(obj->rZ,a,b);
      obj->yCur[i] = ROT_J(obj->rZ,a,b);
      obj->xCur[i] = x;

      /* translation X,Y,Z */
      obj->xCur[i] += obj->trX;
      obj->yCur[i] += obj->trY;
      obj->zCur[i] += obj->trZ;

   }
 }



 /*
  * traceVertices_3D(obj,page)
  *
  * affiche tous les sommets de obj
  */

 void traceVertices_3D(obj3D_t *obj,unsigned char far * page,unsigned char coul)
 {
    short i,a,b,x,y;

    /* mettre a jour l'objet 3D */

    actuaObjet_3D(obj);

    /* afficher tous les sommets */

    for(i = 0; i < obj->nbPoints; i++)
    {
        /* conversion 2D pour affichage */

        a = ZOOM_XY(obj->xCur[i],obj->trZ);
        b = ZOOM_XY(obj->yCur[i],obj->trZ);

        /* affichage */

        x = Ox+a;
        y = Oy-b;
        if(y>=0 && y<200 && x>=0 && x<320)
           page[320L*y+x] = coul;
    }
 }


 /*
  * traceContour_3D(obj,page)
  *
  * affiche toutes les aretes de obj
  */

 void traceContour_3D(obj3D_t *obj,unsigned char far * page,unsigned char coul)
 {
   short face,pta,ptb,ptc,p0x,p0y,p1x,p1y,p2x,p2y;

   /* mettre a jour l'objet 3D */

   actuaObjet_3D(obj);

   for(face = 0; face < obj->nbFaces; face++)
   {
      pta = obj->ptA[face];
      ptb = obj->ptB[face];
      ptc = obj->ptC[face];

      p0x = ZOOM_XY(obj->xCur[pta],obj->trZ);
      p0y = ZOOM_XY(obj->yCur[pta],obj->trZ);

      p1x = ZOOM_XY(obj->xCur[ptb],obj->trZ);
      p1y = ZOOM_XY(obj->yCur[ptb],obj->trZ);

      p2x = ZOOM_XY(obj->xCur[ptc],obj->trZ);
      p2y = ZOOM_XY(obj->yCur[ptc],obj->trZ);

      Line(Ox+p0x,Oy-p0y,Ox+p1x,Oy-p1y,page,coul);
      Line(Ox+p1x,Oy-p1y,Ox+p2x,Oy-p2y,page,coul);
      Line(Ox+p2x,Oy-p2y,Ox+p0x,Oy-p0y,page,coul);
   }
 }



 /*
  * fillPoly3(page)
  *
  * PREC : pas de mapping
  *
  * Affiche le triangle courant dans la page
  */

 void  fillPoly3(unsigned char far * page)
 {
   short xae,yae,xbe,ybe,xce,yce,xde,yde,zae,zbe,zce;
   unsigned char e0,e1;
   unsigned short nind;
   long iga,igb,igc;

   xae = Ox+XA;
   yae = Oy-YA;
   zae = ZA;

   xbe = Ox+XB;
   ybe = Oy-YB;
   zbe = ZB;

   xce = Ox+XC;
   yce = Oy-YC;
   zce = ZC;

   if(Shading == phong)
   {
       if(AffMethod != z_buffer)
      {
          fillPoly3Phong_V2((long)xae,(long)yae,(long)xbe,(long)ybe,(long)xce,(long)yce,page);
          return;
      }
      else
      {
          zae = ZA-ObjTransZ;
          zbe = ZB-ObjTransZ;
           zce = ZC-ObjTransZ;

           fillPoly3PhongZ_V2((long)xae,(long)yae,(long)zae,(long)xbe,(long)ybe,(long)zbe,(long)xce,(long)yce,(long)zce,page);
           return;
      }
   }

   if(Shading == gouraud)
   {

      // calcul des intensites de couleurs aux sommets

      iga = Refza;
      igb = Refzb;
      igc = Refzc;

      if(iga<0)
        iga=0;
      if(igb<0)
        igb=0;
      if(igc<0)
        igc=0;

      if(AffMethod != z_buffer)
      {
          fillPoly3Gouraud_V2((long)xae,(long)yae,(long)iga,(long)xbe,(long)ybe,(long)igb,(long)xce,(long)yce,(long)igc,page);
          return;
      }
      else
      {
          zae = ZA-ObjTransZ;
          zbe = ZB-ObjTransZ;
           zce = ZC-ObjTransZ;

          fillPoly3GouraudZ_V2((long)xae,(long)yae,(long)zae,(long)iga,(long)xbe,(long)ybe,(long)zbe,(long)igb,(long)xce,(long)yce,(long)zce,(long)igc,page);
          return;
      }
   }

   if(Shading == flat)
   {
//      e1 = (CoulFace>>RS_DEG)<<LS_DEG;
//      CoulFace = ((CoulFace-e1)*CoefFlat+(e1<<LS_DEG))>>RS_DEG;
//      CoulFace = e1+CoefFlat;
      CoulFace = CoulFace-15+CoefFlat;
   }

   if(AffMethod != z_buffer)
   {
      fillPoly3_V2((long)xae,(long)yae,(long)xbe,(long)ybe,(long)xce,(long)yce,page,CoulFace);
   }
   else
   {
      zae = ZA-ObjTransZ;
      zbe = ZB-ObjTransZ;
      zce = ZC-ObjTransZ;

      fillPoly3Z_V2((long)xae,(long)yae,(long)zae,(long)xbe,(long)ybe,(long)zbe,(long)xce,(long)yce,(long)zce,page,CoulFace);
   }
 }


 /*
  * fillPoly3Map(page)
  *
  * PREC : mapping
  *
  * Affiche le triangle courant dans la page
  */

 void  fillPoly3Map(unsigned char far * page)
 {
   short xae,yae,xbe,ybe,xce,yce,xde,yde,zae,zbe,zce;
   long iga,igb,igc;

   xae = Ox+XA;
   yae = Oy-YA;
   zae = ZA;

   xbe = Ox+XB;
   ybe = Oy-YB;
   zbe = ZB;

   xce = Ox+XC;
   yce = Oy-YC;
   zce = ZC;

   if(Shading == none)
   {
     if(AffMethod != z_buffer)
     {
        //  no shading + mapping

        fillPoly3Map_V2((long)xae,(long)yae,(long)xbe,(long)ybe,(long)xce,(long)yce,PageMap,page);
     }
     else
     {
        // no shading + mapping + z-buffer

        // pour se rammener -128<z<127

        zae = ZA-ObjTransZ;
        zbe = ZB-ObjTransZ;
        zce = ZC-ObjTransZ;

        fillPoly3MapZ_V2((long)xae,(long)yae,(long)zae,(long)xbe,(long)ybe,(long)zbe,(long)xce,(long)yce,(long)zce,PageMap,page);
     }
   }
   else
   {
     if(AffMethod != z_buffer)
     {
        if(Shading == flat)
        {
            // mapping + flat-shading

            fillPoly3MapFlat_V2((long)xae,(long)yae,(long)xbe,(long)ybe,(long)xce,(long)yce,CoefFlat,PageMap,page);
            return;
        }

        if(Shading == gouraud)
        {
            // mapping + gouraud-shading

            iga = Refza;
            igb = Refzb;
            igc = Refzc;

            if(iga<0)
              iga=0;
            if(igb<0)
              igb=0;
            if(igc<0)
              igc=0;

            fillP3MapGouraud_V2((long)xae,(long)yae,(long)iga,(long)xbe,(long)ybe,(long)igb,(long)xce,(long)yce,(long)igc,PageMap,page);
            return;
        }

         if(Shading == phong)
        {
            // mapping + phong-shading

            fillP3MapPhong_V2((long)xae,(long)yae,(long)xbe,(long)ybe,(long)xce,(long)yce,PageMap,page);
            return;
        }

     }
     else
     {
        // mapping + z-buffer

        zae = ZA-ObjTransZ;
        zbe = ZB-ObjTransZ;
        zce = ZC-ObjTransZ;

        if(Shading == flat)
        {
            // mapping + flat-shading + z-buffer

            fillPoly3MapFlatZ_V2((long)xae,(long)yae,(long)zae,(long)xbe,(long)ybe,(long)zbe,(long)xce,(long)yce,(long)zce,CoefFlat,PageMap,page);
            return;
        }

        if(Shading == gouraud)
        {
            // mapping + gouraud-shading + z-buffer

            iga = Refza;
            igb = Refzb;
            igc = Refzc;

            if(iga<0)
              iga=0;
            if(igb<0)
              igb=0;
            if(igc<0)
              igc=0;

            fillP3MapGouraudZ_V2((long)xae,(long)yae,(long)zae,(long)iga,(long)xbe,(long)ybe,(long)zbe,(long)igb,(long)xce,(long)yce,(long)zce,(long)igc,PageMap,page);
            return;
        }

        if(Shading == phong)
        {
            // mapping + phong-shading + z-buffer

             fillP3MapPhongZ_V2((long)xae,(long)yae,(long)zae,(long)xbe,(long)ybe,(long)zbe,(long)xce,(long)yce,(long)zce,PageMap,page);
            return;
        }
     }
   }
 }


 /*
  *  initZBuffer(zbuffer)
  *
  *  initialise le zbuffer aux valeurs min.
  */

 void initZBuffer(char *zbuffer)
 {
   asm  push eax cx es di
   asm   les di,zbuffer
   asm   mov eax,80808080h  /* -128,-128,-128,-128 */
   asm  mov cx,16000
   asm  rep stosd
   asm   pop di es cx eax
 }


 /*
  *  traceObjet1_3D(obj,page)
  *
  *  PREC : AffMethod != z_order
  *
  *  Determine et affiche toutes les faces visibles de obj
  */

 void traceObjet1_3D(obj3D_t *obj,unsigned char far * page)
 {
    short p0x,p0y,p0z,p1x,p1y,p1z,p2x,p2y,p2z,*p;
    unsigned short face;
    short a,b,x;
    short pta,ptb,ptc;
    long va,vb,vc;

    short ra,rb,rva,rvc,rvb,rx;

    /*  mettre a jour l'objet 3D*/

    actuaObjet_3D(obj);

    /*  determiner les faces visibles */

    for(face = 0; face < obj->nbFaces; face++)
    {
        // Rotation autour des axes x et y
        // de la composante z du vecteur normal de la face

        // rot x
        a = obj->yVectN[face];
        b  = obj->zVectN[face];
        va = obj->xVectN[face];

        // inutile :  vb = ROT_I(obj->rX,a,b);
        vc = ROT_J(obj->rX,a,b);

        // rot y
        a = va;
        b = vc;
        // inutile :  x = ROT_I(obj->rY,a,b);
        vc = ROT_J(obj->rY,a,b);
        // inutile :  va = x;

        // rot Z
        /* inutile
        a = va;
        b = vb;
        x = ROT_I(obj->rZ,a,b);
        vb = ROT_J(obj->rZ,a,b);
        va = x;
        */

        if(vc>0)
        {
            ObjTransZ = obj->trZ;

            /* calculer les 3 points de la face */

            pta = obj->ptA[face];
            ptb = obj->ptB[face];
            ptc = obj->ptC[face];


            XA = ZOOM_XY(obj->xCur[pta],obj->trZ);
            YA = ZOOM_XY(obj->yCur[pta],obj->trZ);
            ZA = obj->zCur[pta];

            XB = ZOOM_XY(obj->xCur[ptb],obj->trZ);
            YB = ZOOM_XY(obj->yCur[ptb],obj->trZ);
            ZB = obj->zCur[ptb];

            XC = ZOOM_XY(obj->xCur[ptc],obj->trZ);
            YC = ZOOM_XY(obj->yCur[ptc],obj->trZ);
            ZC = obj->zCur[ptc];

            if(Rendering == mapping)
            {
              XAmap = obj->fMap[face].xa;
              YAmap = obj->fMap[face].ya;

              XBmap = obj->fMap[face].xb;
              YBmap = obj->fMap[face].yb;

              XCmap = obj->fMap[face].xc;
              YCmap = obj->fMap[face].yc;
            }
            else
            {
               if((Shading == gouraud) || (Shading == phong))
                 CoulFace = obj->coul;
               else
                 CoulFace = obj->indPal[face];
            }

            if(Shading == flat)
            {
                CoefFlat = vc;
                goto suitef1;
            }

            if(Shading == gouraud)
            {
                // Rotation des vecteurs reflexion

                // Rotation autour des axes x et y de la composante z
                // du vecteur reflexion du sommet A
                // rot x
                ra = obj->yReflex[pta];
                rb  = obj->zReflex[pta];
                rva = obj->xReflex[pta];
                rvc = ROT_J(obj->rX,ra,rb);
                // rot y
                ra = rva;
                rb = rvc;
                rvc = ROT_J(obj->rY,ra,rb);
                Refza = rvc;

                // Rotation autour des axes x et y de la composante z
                // du vecteur reflexion du sommet B
                // rot x
                ra = obj->yReflex[ptb];
                rb  = obj->zReflex[ptb];
                rva = obj->xReflex[ptb];
                rvc = ROT_J(obj->rX,ra,rb);
                // rot y
                ra = rva;
                rb = rvc;
                rvc = ROT_J(obj->rY,ra,rb);
                Refzb = rvc;

                // Rotation autour des axes x et y de la composante z
                // du vecteur reflexion du sommet C
                ra = obj->yReflex[ptc];
                rb  = obj->zReflex[ptc];
                rva = obj->xReflex[ptc];
                rvc = ROT_J(obj->rX,ra,rb);
                // rot y
                ra = rva;
                rb = rvc;
                rvc = ROT_J(obj->rY,ra,rb);
                Refzc = rvc;
                goto suitef1;
            }

            if(Shading == phong)
            {
                // Rotation des vecteurs reflexion

                // Rotation autour des axes x,y,z des comp. x,y
                // du vecteur reflexion du sommet A
                // rot x
                ra = obj->yReflex[pta];
                rb  = obj->zReflex[pta];
                rva = obj->xReflex[pta];
                rvb = ROT_I(obj->rX,ra,rb);
                rvc = ROT_J(obj->rX,ra,rb);

                // rot y
                ra = rva;
                rb = rvc;
                rx = ROT_I(obj->rY,ra,rb);
                rvc = ROT_J(obj->rY,ra,rb);
                rva = rx;

                // rot z
                ra = rva;
                rb = rvb;
                rx = ROT_I(obj->rZ,ra,rb);
                rvb = ROT_J(obj->rZ,ra,rb);
                rva = rx;

                RefXa = rva;
                RefYa = rvb;

                // Rotation autour des axes x,y,z des comp. x,y
                // du vecteur reflexion du sommet B
                // rot x
                ra = obj->yReflex[ptb];
                rb  = obj->zReflex[ptb];
                rva = obj->xReflex[ptb];
                rvb = ROT_I(obj->rX,ra,rb);
                rvc = ROT_J(obj->rX,ra,rb);

                // rot y
                ra = rva;
                rb = rvc;
                rx = ROT_I(obj->rY,ra,rb);
                rvc = ROT_J(obj->rY,ra,rb);
                rva = rx;

                // rot z
                ra = rva;
                rb = rvb;
                rx = ROT_I(obj->rZ,ra,rb);
                rvb = ROT_J(obj->rZ,ra,rb);
                rva = rx;

                RefXb = rva;
                RefYb = rvb;

                // Rotation autour des axes x,y,z des comp. x,y
                // du vecteur reflexion du sommet C
                // rot x
                ra = obj->yReflex[ptc];
                rb  = obj->zReflex[ptc];
                rva = obj->xReflex[ptc];
                rvb = ROT_I(obj->rX,ra,rb);
                rvc = ROT_J(obj->rX,ra,rb);

                // rot y
                ra = rva;
                rb = rvc;
                rx = ROT_I(obj->rY,ra,rb);
                rvc = ROT_J(obj->rY,ra,rb);
                rva = rx;

                // rot z
                ra = rva;
                rb = rvb;
                rx = ROT_I(obj->rZ,ra,rb);
                rvb = ROT_J(obj->rZ,ra,rb);
                rva = rx;

                RefXc = rva;
                RefYc = rvb;
            }

         suitef1:
            if(Rendering == mapping)
                fillPoly3Map(page);
            else
                fillPoly3(page);
         }
    }
 }


 /*
  *  traceObjet2_3D(obj)
  *
  *  PREC : AffMethod == z_order
  *
  *  Determine les faces visibles de obj
  *  et remplit la liste des faces a afficher
  */

 void traceObjet2_3D(obj3D_t *obj)
 {
    short p0x,p0y,p0z,p1x,p1y,p1z,p2x,p2y,p2z,*p;
    unsigned short face;
    short a,b,x;
    short pta,ptb,ptc;
    long va,vb,vc;

    short ra,rb,rva,rvb,rvc,rx;

    /*  mettre a jour l'objet 3D*/

    actuaObjet_3D(obj);

    /*  determiner les faces visibles */

    for(face = 0; face < obj->nbFaces; face++)
    {
        /* rot x */

        a = obj->yVectN[face];
        b  = obj->zVectN[face];
        va = obj->xVectN[face];

        // inutile : vb = ROT_I(obj->rX,a,b);
        vc = ROT_J(obj->rX,a,b);

        /* rot y */
        a = va;
        b = vc;
        // inutile : x = ROT_I(obj->rY,a,b);
        vc = ROT_J(obj->rY,a,b);
        // inutile : va = x;

        /* rot Z */
        /* inutile
        a = va;
        b = vb;
        x = ROT_I(obj->rZ,a,b);
        vb = ROT_J(obj->rZ,a,b);
        va = x;
        */

        if(vc>0)
        {
            /* face visible */

            /*
             * Calculer les 3 points A,B,C de la face
             * Modif. de la taille du traingle en fct de la profondeur
             */

            pta = obj->ptA[face];
            ptb = obj->ptB[face];
            ptc = obj->ptC[face];


            XA = ZOOM_XY(obj->xCur[pta],obj->trZ);
            YA = ZOOM_XY(obj->yCur[pta],obj->trZ);
            ZA = obj->zCur[pta];

            XB = ZOOM_XY(obj->xCur[ptb],obj->trZ);
            YB = ZOOM_XY(obj->yCur[ptb],obj->trZ);
            ZB = obj->zCur[ptb];

            XC = ZOOM_XY(obj->xCur[ptc],obj->trZ);
            YC = ZOOM_XY(obj->yCur[ptc],obj->trZ);
            ZC = obj->zCur[ptc];

            /* Ajouter la face a la liste des faces a afficher */

            *(pTabFaces++) = XA;
            *(pTabFaces++) = YA;
            *(pTabFaces++) = ZA;

            *(pTabFaces++) = XB;
            *(pTabFaces++) = YB;
            *(pTabFaces++) = ZB;

            *(pTabFaces++) = XC;
            *(pTabFaces++) = YC;
            *(pTabFaces++) = ZC;


            /* caracteristiques de la face */

            /* mapping */

            if(Rendering == mapping)
            {
                /* XAmap */
                *(pTabFaces++) = obj->fMap[face].xa;
                // *(pTabFaces++) = obj->xMap[pta];

                /* YAmap */
                *(pTabFaces++) = obj->fMap[face].ya;
               //   *(pTabFaces++) = obj->yMap[pta];

                /* XBmap */
                *(pTabFaces++) = obj->fMap[face].xb;
                // *(pTabFaces++) = obj->xMap[ptb];

                /* YBmap */
                *(pTabFaces++) = obj->fMap[face].yb;
                // *(pTabFaces++) = obj->yMap[ptb];

                /* XCmap */
                *(pTabFaces++) = obj->fMap[face].xc;
                // *(pTabFaces++) = obj->xMap[ptc];

                /* YCmap */
                *(pTabFaces++) = obj->fMap[face].yc;
                // *(pTabFaces++) = obj->yMap[ptc];
            }
            else
            {
                /* couleur de la face */

                if((Shading == gouraud) || (Shading == phong))
                   *(pTabFaces++) = obj->coul;
                else
                   *(pTabFaces++) = obj->indPal[face];
            }

            if(Shading == flat)
            {
                /* coef. assombrissement 0 < CoefFlat <= 15 */

                *(pTabFaces++) = vc;
                goto suite1;
            }

            // Gouraud

            if(Shading == gouraud)
            {
                // Rotation des vecteurs reflexion

                // Rotation autour des axes x et y de la composante z
                // du vecteur reflexion du sommet A
                // rot x
                ra = obj->yReflex[pta];
                rb  = obj->zReflex[pta];
                rva = obj->xReflex[pta];
                rvc = ROT_J(obj->rX,ra,rb);
                // rot y
                ra = rva;
                rb = rvc;
                rvc = ROT_J(obj->rY,ra,rb);
                Refza = rvc;

                // Rotation autour des axes x et y de la composante z
                // du vecteur reflexion du sommet B
                // rot x
                ra = obj->yReflex[ptb];
                rb  = obj->zReflex[ptb];
                rva = obj->xReflex[ptb];
                rvc = ROT_J(obj->rX,ra,rb);
                // rot y
                ra = rva;
                rb = rvc;
                rvc = ROT_J(obj->rY,ra,rb);
                Refzb = rvc;

                // Rotation autour des axes x et y de la composante z
                // du vecteur reflexion du sommet C
                ra = obj->yReflex[ptc];
                rb  = obj->zReflex[ptc];
                rva = obj->xReflex[ptc];
                rvc = ROT_J(obj->rX,ra,rb);
                // rot y
                ra = rva;
                rb = rvc;
                rvc = ROT_J(obj->rY,ra,rb);
                Refzc = rvc;

                // intensites de couleurs aux 3 sommets
                *(pTabFaces++) = Refza;
                *(pTabFaces++) = Refzb;
                *(pTabFaces++) = Refzc;
                goto suite1;
            }

            if(Shading == phong)
            {
                // Rotation des vecteurs reflexion

                // Rotation autour des axes x,y,z des comp. x,y
                // du vecteur reflexion du sommet A
                // rot x
                ra = obj->yReflex[pta];
                rb  = obj->zReflex[pta];
                rva = obj->xReflex[pta];
                rvb = ROT_I(obj->rX,ra,rb);
                rvc = ROT_J(obj->rX,ra,rb);

                // rot y
                ra = rva;
                rb = rvc;
                rx = ROT_I(obj->rY,ra,rb);
                rvc = ROT_J(obj->rY,ra,rb);
                rva = rx;

                // rot z
                ra = rva;
                rb = rvb;
                rx = ROT_I(obj->rZ,ra,rb);
                rvb = ROT_J(obj->rZ,ra,rb);
                rva = rx;

                RefXa = rva;
                RefYa = rvb;

                // Rotation autour des axes x,y,z des comp. x,y
                // du vecteur reflexion du sommet B
                // rot x
                ra = obj->yReflex[ptb];
                rb  = obj->zReflex[ptb];
                rva = obj->xReflex[ptb];
                rvb = ROT_I(obj->rX,ra,rb);
                rvc = ROT_J(obj->rX,ra,rb);

                // rot y
                ra = rva;
                rb = rvc;
                rx = ROT_I(obj->rY,ra,rb);
                rvc = ROT_J(obj->rY,ra,rb);
                rva = rx;

                // rot z
                ra = rva;
                rb = rvb;
                rx = ROT_I(obj->rZ,ra,rb);
                rvb = ROT_J(obj->rZ,ra,rb);
                rva = rx;

                RefXb = rva;
                RefYb = rvb;

                // Rotation autour des axes x,y,z des comp. x,y
                // du vecteur reflexion du sommet C
                // rot x
                ra = obj->yReflex[ptc];
                rb  = obj->zReflex[ptc];
                rva = obj->xReflex[ptc];
                rvb = ROT_I(obj->rX,ra,rb);
                rvc = ROT_J(obj->rX,ra,rb);

                // rot y
                ra = rva;
                rb = rvc;
                rx = ROT_I(obj->rY,ra,rb);
                rvc = ROT_J(obj->rY,ra,rb);
                rva = rx;

                // rot z
                ra = rva;
                rb = rvb;
                rx = ROT_I(obj->rZ,ra,rb);
                rvb = ROT_J(obj->rZ,ra,rb);
                rva = rx;

                RefXc = rva;
                RefYc = rvb;

                // ecriture dans la liste
                *(pTabFaces++) = RefXa;
                *(pTabFaces++) = RefYa;
                *(pTabFaces++) = RefXb;
                *(pTabFaces++) = RefYb;
                *(pTabFaces++) = RefXc;
                *(pTabFaces++) = RefYc;
            }

            suite1:

            /* Inserer cette face dans la liste des faces a trier */

            TabInd[NbFaces].i = NbFaces;

            /* Quel Z prendre ? :
             * - un sommet quelconque
             * - le max en z des 3 sommets
             * - la moyenne des 3 sommets
             */

            /* Ici mOyenne des 3 sommets */
            TabInd[NbFaces].z = ZA+ZB+ZC; // division par 3 inutile

            NbFaces++;
        }
    }
 }


 /*
  *  affPoly3(page)
  *
  *  PREC : AffMethod == z_order
  *
  *  Affiche toutes les faces visibles de la scene
  */

 static void affPoly3(unsigned char far * page)
 {
   short f,*pf,pi;

   for(f=0;f<NbFaces;f++)
   {
      pi = TabInd[f].i;
      pf = TabFaces + (TailleElem*pi);

      /* Recuperer les coordonnees A,B,C du triangle */

      XA = *(pf++);
      YA = *(pf++);
      ZA = *(pf++);

      XB = *(pf++);
      YB = *(pf++);
      ZB = *(pf++);

      XC = *(pf++);
      YC = *(pf++);
      ZC = *(pf++);

      if(Rendering == mapping)
      {
          /* Coordonnees de mapping dans la page de Mapping */

          XAmap = *(pf++);
          YAmap = *(pf++);
          XBmap = *(pf++);
          YBmap = *(pf++);
          XCmap = *(pf++);
          YCmap = *(pf++);
      }
      else    /* no_mapping */
      {
          /* Couleur de la face */

          CoulFace = *(pf++);
      }

      if(Shading == flat)
      {
         CoefFlat = *pf;  /* Coef. de FLAT-SHADING */
         goto gAffPoly;
      }

      if(Shading == gouraud)
      {
         Refza = *(pf++);
         Refzb = *(pf++);
         Refzc = *pf;
         goto gAffPoly;
      }

      if(Shading == phong)
      {
        RefXa = *(pf++);
        RefYa = *(pf++);
        RefXb = *(pf++);
        RefYb = *(pf++);
        RefXc = *(pf++);
        RefYc = *pf;
      }

 gAffPoly:
      if(Rendering == mapping)
          fillPoly3Map(page);
      else
          fillPoly3(page);
   }
 }


 /*
  *  setResolScreen(larg,haut)
  *
  *  Fixe la resolution ecran
  */

 void setResolScreen(unsigned short larg,unsigned short haut)
 {
   Ox = larg/2;
   Oy = haut/2;
 }


 /*
  *  setRenderMode(render)
  *
  *  Fixe le rendering
  */

 void setRenderMode(render_t render)
 {
   Rendering = render;
 }


 /*
  *  setShading(shading)
  *
  *  Fixe le shading
  */

 void setShading(shading_t shading)
 {
   Shading = shading;
 }


 /*
  *  setAffMethod(affMethod)
  *
  *  Fixe la methode d'affichage des faces
  */

 void setAffMethod(affMethod_t affMethod)
 {
   AffMethod = affMethod;
 }


 /*
  *  initLib_3D(param3D)
  *
  *  Initialise la lib 3D
  */

 short initLib_3D(param3D_t *param3D)
 {
    short i;

    #ifdef table256
      initSinCos256();
    #else
      initSinCos360();
    #endif

    for(i = 0; i < MAX_OBJ; i++)
      TObjetS[i].entry = -1;

    setResolScreen(320,200);
    setRenderMode(param3D->render);
    setShading(param3D->shading);
    setAffMethod(param3D->affMethod);

    if(Shading == phong)
    {
       /* initialiser le tableau des couples x,y */
       initTPhongXY();
       /* test visuel du tableau des couples x,y */
       #if 0
       {
         long m,y,x;
         y = 0;
         x = 0;
         for(m=0;m<64000;m++)
         {
           if((m%255)==0)
           {
             y+=1;
             x=0;
           }
           else
             x++;

           if(y<200)
           Video[y*320+x] = TPhongXY[m];
         }
         getchar();
       }
       #endif
    }

    switch(AffMethod)
    {
      case z_order :
      {
         /* Allocation de la liste des faces a afficher */

         TailleElem = 9; /* 3 points dans R3 (3*3 coords) */

         switch(Rendering)
         {
            case mapping :
                 TailleElem += 6;  /* 3 points dans R2 (3*2 coords) */
                 break;

            case no_mapping :
                 TailleElem += 1;  /* 1 indice dans la palette */
                 break;
         }

         switch(Shading)
         {
            case flat :
                 TailleElem += 1;  /* CoefFlat (coef. d'assombrissement) */
                 break;

            case gouraud :
                 TailleElem += 3;  /* coefs. d'intensites (z) aux 3 sommets */
                 break;

            case phong:
                 TailleElem += 6; /* comp. x,y aux 3 sommets */
                 break;
         }

         /* Allocation */
         TabFaces = (short FAR*)MALLOC(TailleElem*MAX_FACES);
         if(TabFaces)
         {
            /* allouer TabInd qui nous sert a trier les faces */
            TabInd = (elemTriFace_t*)MALLOC(MAX_FACES*sizeof(elemTriFace_t));
            if(TabInd)
               return 1;
            else
               return 0;
         }
         else
            return 0;
      }
    }

    return 1;
 }


 /*
  *  initLib_3D(param3D)
  *
  *  Quitte la lib 3D
  */

 void endLib_3D(void)
 {
   short i;

   /* Liberer les objets */

   for(i = 0; i < MAX_OBJ; i++)
     if(TObjetS[i].entry != -1)
       freeObjet_3D(i);

   for(i = 0; i < MAX_OBJ; i++)
      TObjetS[i].entry = -1;

   switch(AffMethod)
   {
     case z_order:
     {
        /* Liberer la liste des faces */
        FREE(TabFaces);
        FREE(TabInd);
     }
     break;
   }

 }


 /*
  *  compZ(a,b)
  *
  *  Fonction de comparaison dans le tri des faces
  */

 int compZ(const void *a,const void *b)
 {
   elemTriFace_t *e1,*e2;

   e1 = (elemTriFace_t *)a;
   e2 = (elemTriFace_t *)b;

   return (e1->z - e2->z);
 }


 /*
  *  affScene_3D(page)
  *
  *  Affiche la scene : toutes les faces visibles de tous les objets
  */

 void affScene_3D(unsigned char far * page)
 {
   short i;

   /* cas particuliers */

   if(Rendering == vertices)
   {
     for(i = 0; i < IndObjetS; i++)
      if(TObjetS[i].entry != -1)
          traceVertices_3D(&TObjetS[i],page,TObjetS[i].coul);

     return;
   }

   if(Rendering == lines)
   {
     for(i = 0; i < IndObjetS; i++)
      if(TObjetS[i].entry != -1)
          traceContour_3D(&TObjetS[i],page,TObjetS[i].coul);

     return;
   }

   if(AffMethod == z_order)
   {
     pTabFaces = TabFaces;
     NbFaces = 0;

     /* Remplir le tableau des faces a afficher */
     for(i = 0; i < IndObjetS; i++)
        if(TObjetS[i].entry != -1)
            traceObjet2_3D(&TObjetS[i]);

     /* Trie des faces en Z */
     qsort((void *)TabInd,NbFaces,sizeof(elemTriFace_t),compZ);

     /* Afficher les faces */
     affPoly3(page);

     return;
   }

   if(AffMethod == z_buffer)
   {
      initZBuffer(ZBuffer);
   }

   /* no_order ou z_buffer */

   for(i = 0; i < IndObjetS; i++)
      if(TObjetS[i].entry != -1)
         traceObjet1_3D(&TObjetS[i],page);
 }


 /*
  *  removeFromScene_3D(entry)
  *
  *  Supprime un objet de la scene
  */

 void removeFromScene_3D(short entry)
 {
   TObjetS[entry].entry = -1;
 }


 /*
  *  addToScene_3D(obj)
  *
  *  "Ajoute" un objet a la scene
  */

 void addToScene_3D(obj3D_t *obj)
 /* PREC : l'objet est alloue */
 {
   short i;
   for(i = 0; i < IndObjetS; i++)
     if((TObjetS[i].entry == -1) && ( &TObjetS[i] == obj ))
        TObjetS[i].entry = i;
 }

/*
 * freeObjet_3D(entry)
 *
 * Liberation de la structure obj3D_t
 */

void freeObjet_3D(unsigned short entry)
{
   /* coordonnees */

   FREE(TObjetS[entry].xIni);
   FREE(TObjetS[entry].yIni);
   FREE(TObjetS[entry].zIni);

   FREE(TObjetS[entry].xCur);
   FREE(TObjetS[entry].yCur);
   FREE(TObjetS[entry].zCur);

   /* faces */

   FREE(TObjetS[entry].ptA);
   FREE(TObjetS[entry].ptB);
   FREE(TObjetS[entry].ptC);

   /* Attributs des faces */
   if(Rendering == mapping)
   {
       FREE(TObjetS[entry].fMap);
   }
   else
   {
      if(Shading != gouraud)
      {
        FREE(TObjetS[entry].indPal);
      }
   }

   /* Vecteurs normaux */

   FREE(TObjetS[entry].xVectN);
   FREE(TObjetS[entry].yVectN);
   FREE(TObjetS[entry].zVectN);

   /* Vecteurs reflexions */

   if((Shading == gouraud) || (Shading == phong))
   {
      FREE(TObjetS[entry].xReflex);
     FREE(TObjetS[entry].yReflex);
      FREE(TObjetS[entry].zReflex);
   }
}
