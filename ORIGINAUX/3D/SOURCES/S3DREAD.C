 /*
  * S3DREAD.C : Lecture d'objets 3D .ASC
  *
  * Auteur : Patrice Siracusa.
  *
  */

#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>

#include "s3d3.h"

#define MAX_CHAR_PERLINE 255

extern obj3D_t TObjetS[MAX_OBJ];
extern short IndObjetS;
extern render_t Rendering;
extern shading_t Shading;

/*
 * Lire le fichier en parametre afin
 * de remplir la structure obj3D_t
 */

 obj3D_t *readObjet_3D(char *file)
 {

   char buff[MAX_CHAR_PERLINE];
   FILE *stream;
   unsigned short vertices,faces;
   short vertex,v,face;
   short A,B,C;
   float X,Y,Z;
   long nbLigs = 0;
   short i,t;

   if((stream = fopen(file,"r")) == NULL)
     return NULL;

   fgets(buff,MAX_CHAR_PERLINE,stream);
   fgets(buff,MAX_CHAR_PERLINE,stream);
   fgets(buff,MAX_CHAR_PERLINE,stream);
   fscanf(stream,"%s",buff);
   fscanf(stream,"%s",buff);
   fscanf(stream,"%d",&vertices);
   fscanf(stream,"%s",buff);
   fscanf(stream,"%d",&faces);
   fgets(buff,MAX_CHAR_PERLINE,stream);
   fgets(buff,MAX_CHAR_PERLINE,stream);

   /* tester le nbre de faces */
   if(faces > MAX_FACES)
   {
     return NULL;
   }

   /* remplir la structure des objets simples */
   TObjetS[IndObjetS].nbPoints = vertices;
   TObjetS[IndObjetS].nbFaces = faces;


   /* allocation */

   /* coordonnees */

   if(!(TObjetS[IndObjetS].xIni=(short FAR *)MALLOC(vertices*SIZE_SHORT)))
     return NULL;
   if(!(TObjetS[IndObjetS].yIni=(short FAR *)MALLOC(vertices*SIZE_SHORT)))
     return NULL;
   if(!(TObjetS[IndObjetS].zIni=(short FAR *)MALLOC(vertices*SIZE_SHORT)))
     return NULL;

   if(!(TObjetS[IndObjetS].xCur=(short FAR *)MALLOC(vertices*SIZE_SHORT)))
     return NULL;
   if(!(TObjetS[IndObjetS].yCur=(short FAR *)MALLOC(vertices*SIZE_SHORT)))
     return NULL;
   if(!(TObjetS[IndObjetS].zCur=(short FAR *)MALLOC(vertices*SIZE_SHORT)))
     return NULL;

   /* vecteurs reflexions */

   if((Shading == gouraud) || (Shading == phong))
   {
      if(!(TObjetS[IndObjetS].xReflex=(short FAR *)MALLOC(vertices*SIZE_SHORT)))
        return NULL;
     if(!(TObjetS[IndObjetS].yReflex=(short FAR *)MALLOC(vertices*SIZE_SHORT)))
       return NULL;
     if(!(TObjetS[IndObjetS].zReflex=(short FAR *)MALLOC(vertices*SIZE_SHORT)))
       return NULL;
   }

   /* faces */

   if(!(TObjetS[IndObjetS].ptA = (short FAR *)MALLOC(faces*SIZE_SHORT)))
      return NULL;
   if(!(TObjetS[IndObjetS].ptB = (short FAR *)MALLOC(faces*SIZE_SHORT)))
      return NULL;
   if(!(TObjetS[IndObjetS].ptC = (short FAR *)MALLOC(faces*SIZE_SHORT)))
      return NULL;


   switch(Rendering)
   {
     case mapping:

          if(!(TObjetS[IndObjetS].fMap = (map_t FAR *)MALLOC(faces*sizeof(map_t))))
             return NULL;

          break;

     case no_mapping:

          /* une couleur par face sauf en gouraud et phong */
          if((Shading == flat) || (Shading == none))
          {
            if(!(TObjetS[IndObjetS].indPal = (short FAR *)MALLOC(faces*SIZE_SHORT)))
               return NULL;
          }
          break;
   }


   /* vect. normaux */

   if(!(TObjetS[IndObjetS].xVectN = (short FAR *)MALLOC(faces*SIZE_SHORT)))
     return NULL;
   if(!(TObjetS[IndObjetS].yVectN = (short FAR *)MALLOC(faces*SIZE_SHORT)))
     return NULL;
   if(!(TObjetS[IndObjetS].zVectN = (short FAR *)MALLOC(faces*SIZE_SHORT)))
     return NULL;


   nbLigs = 5;
   /* coordonnees */
   for(vertex=0;vertex<vertices;vertex++)
   {
      fscanf(stream,"%s",buff);  // vertex
      fscanf(stream,"%d",&v);    // nø
      fscanf(stream,"%s",buff);  // :
      fscanf(stream,"%s",buff);  // X:
      fscanf(stream,"%f",&X);
      fscanf(stream,"%s",buff);  // Y:
      fscanf(stream,"%f",&Y);
      fscanf(stream,"%s",buff);  // Z:
      fscanf(stream,"%f",&Z);

      //
      TObjetS[IndObjetS].xIni[vertex] = X;
      TObjetS[IndObjetS].yIni[vertex] = Y;
      TObjetS[IndObjetS].zIni[vertex] = Z;


      nbLigs++;
      if(nbLigs == 61) // saut de page
      {

        fgets(buff,MAX_CHAR_PERLINE,stream);
        fgets(buff,MAX_CHAR_PERLINE,stream);
        fgets(buff,MAX_CHAR_PERLINE,stream);
        fgets(buff,MAX_CHAR_PERLINE,stream);
        fgets(buff,MAX_CHAR_PERLINE,stream);
        nbLigs=0;

      }
   }
   fgets(buff,MAX_CHAR_PERLINE,stream);
   fgets(buff,MAX_CHAR_PERLINE,stream);
   nbLigs+=1;

   /* faces */

   for(face=0; face<faces; face++)
   {
     fscanf(stream,"Face %d:    A:%d B:%d C:%d %s",&v,&A,&B,&C,buff);
     nbLigs++;
     if(nbLigs == 61)  // saut de page
     {

        fgets(buff,MAX_CHAR_PERLINE,stream);
        fgets(buff,MAX_CHAR_PERLINE,stream);
        fgets(buff,MAX_CHAR_PERLINE,stream);
        fgets(buff,MAX_CHAR_PERLINE,stream);
        fgets(buff,MAX_CHAR_PERLINE,stream);
        nbLigs=0;

     }

     fgets(buff,MAX_CHAR_PERLINE,stream);
     nbLigs++;
     if(nbLigs == 61)     // saut de page
     {

        fgets(buff,MAX_CHAR_PERLINE,stream);
        fgets(buff,MAX_CHAR_PERLINE,stream);
        fgets(buff,MAX_CHAR_PERLINE,stream);
        fgets(buff,MAX_CHAR_PERLINE,stream);
        fgets(buff,MAX_CHAR_PERLINE,stream);
        nbLigs=0;

     }

     fgets(buff,MAX_CHAR_PERLINE,stream);
     if(nbLigs == 61)   // saut de page
     {

        fgets(buff,MAX_CHAR_PERLINE,stream);
        fgets(buff,MAX_CHAR_PERLINE,stream);
        fgets(buff,MAX_CHAR_PERLINE,stream);
        fgets(buff,MAX_CHAR_PERLINE,stream);
        fgets(buff,MAX_CHAR_PERLINE,stream);
        nbLigs=0;

     }

     TObjetS[IndObjetS].ptA[face] = A;
     TObjetS[IndObjetS].ptB[face] = B;
     TObjetS[IndObjetS].ptC[face] = C;

   }

   TObjetS[IndObjetS].entry = IndObjetS;

   /* init. de l'objet */
   initStruct_3D(&TObjetS[IndObjetS]);

   /* passer au suivant */
   IndObjetS++;

   fclose(stream);
   return &TObjetS[IndObjetS-1];
 }
