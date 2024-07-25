 /*
  * LIBRAIRIE S3D - Auteur : Patrice Siracusa 1995
  *
  * Librairie de manipulation d'objets 3D polygonaux
  *
  */

 #define FAR far          /* significatif sous D.O.S 16 bits */

 #define COEF_ZOOM 0.007
 #define COEF_SQUEEZE 0.005

 #define SIZE_SHORT 2
 #define SIZE_LONG 4
 #define MALLOC farmalloc
 #define FREE farfree
 #define MAX_OBJ  10
 #define MAX_FACES 1000

 /***** types *****/


 typedef struct
 {
   short xa,ya,xb,yb,xc,yc;

 } map_t;


 /* structure d'un objet 3D */

 typedef struct
 {
   short entry;

   /* Coordonnees */

   unsigned short nbPoints;

   short FAR *xIni;
   short FAR *yIni;
   short FAR *zIni;

   short FAR *xCur;
   short FAR *yCur;
   short FAR *zCur;

   /* Faces */

   unsigned short nbFaces;

   short FAR *ptA;
   short FAR *ptB;
   short FAR *ptC;

   /* Attributs des faces */

   /* si mapping, 3 coords. par face */
   map_t FAR *fMap;

   /* sinon (!mapping) un indice de palette par face */
   short FAR *indPal;

   /* ou couleur unique de l'objet */
   short coul;

   /* Vecteurs normaux des faces */

   short FAR *xVectN;
   short FAR *yVectN;
   short FAR *zVectN;

   /* Vect. normaux à la surface aux niveaux des sommets */
   
   short FAR *xReflex;
   short FAR *yReflex;
   short FAR *zReflex;

   /* rotation */

   short rX;  /* nbre de rotations autour de l'axe en X */
   short rY;
   short rZ;

   /* translation */

   short trX;     /* nbre de translations en X */
   short trY;
   short trZ;

 } obj3D_t;

 /*********************** Structure du rendu **************************/

 typedef enum
 {
    none,       /* aucun shading, couleur propre */
    flat,       /* FLAT-SHADING */
    gouraud,    /* GOURAUD-SHADING */
    phong       /* PHONG-SHADING */

 } shading_t;

 typedef enum
 {
    vertices,    /* sommets */
    lines,       /* fil de fer (aretes toutes visibles) */
    no_mapping,  /* faces remplies sans mapping */
    mapping      /* faces remplies en mapping */

 } render_t;


 typedef enum
 {
   no_order,     /* par d'ordre dans l'affichage des faces */
   z_order,      /* faces affiches dans l'ordre des z */
   z_buffer      /* z-buffer des faces */

 } affMethod_t;


 /*
  *  Parametres de la 3D
  */

 typedef
 struct param3D_t
 {
   unsigned      short resolX;    /* resol. ecran en X en Y */
   unsigned      short resolY;
   render_t      render;
   shading_t     shading;
   affMethod_t  affMethod;

 } param3D_t;

 /************************** Fonctions **********************************/


 /** allocation/desallocation **/

 obj3D_t *readObjet_3D(char *file);

 void freeObjet_3D(unsigned short entry);

 /** initialisation **/

 void initStruct_3D(obj3D_t *obj);

 /** translations **/

 void relTransX_3D(obj3D_t *obj,short tx);

 void relTransY_3D(obj3D_t *obj,short ty);

 void relTransZ_3D(obj3D_t *obj,short tz);

 void absTransX_3D(obj3D_t *obj,short tx);

 void absTransY_3D(obj3D_t *obj,short ty);

 void absTransZ_3D(obj3D_t *obj,short tz);

 /** rotations **/

 void relRotX_3D(obj3D_t *obj,short ax);

 void relRotY_3D(obj3D_t *obj,short ay);

 void relRotZ_3D(obj3D_t *obj,short az);

 void absRotX_3D(obj3D_t *obj,short ax);

 void absRotY_3D(obj3D_t *obj,short ay);

 void absRotZ_3D(obj3D_t *obj,short az);

 /** zoom **/

 void zoom_3D(obj3D_t *obj,float c);

 /****** rendering *****/

 void setRenderMode_3D(render_t render);

 short initLib_3D(param3D_t *param3D);

 void endLib_3D(void);

 short loadPal256(char *file);

 void affScene_3D(unsigned char *page);

 short loadMapImage(char *ficImage);

 void removeFromScene_3D(short entry);

 void addToScene_3D(obj3D_t *obj);
