
 #define CONTOUR_COUL 255
 #define FAR far
 #define ROT_I(a,i,j) (((i)*cosq(a)+(j)*sinq(a))/256)
 #define ROT_J(a,i,j) ((-(i)*sinq(a)+(j)*cosq(a))/256)

 /***** types *****/

 /* structure d'une face */

 typedef

 struct fill_t
 {
	 unsigned char indPal;   /* indice de couleur dans la palette */
	 unsigned char r; 			 /* niveaux "en pleine lumiere" (angle = 0ø) */
	 unsigned char v;
	 unsigned char b;

 } fill_t;

 /* structure d'un objet 3D simple */

 typedef

 struct obj3D_t
 {
	 /* coordonnees */

	 unsigned char nbPoints;
	 short FAR tabPointsI[3][8];
	 short FAR tabPointsA[3][8];

	 /* faces */

	 unsigned char nbFaces;
	 unsigned char FAR nbPFaces[6];
	 short FAR  tabPFaces[6][4];
	 short FAR  tabFaces[6][12];
	 fill_t FAR tabFill[6];

	 /* rotation */

	 short rX;  /* nbre de rotations autour de l'axe en X */
	 short rY;
	 short rZ;

	 /* translation */

	 short trX;     /* nbre de translations en X */
	 short trY;
	 short trZ;

 } obj3D_t;

 /***** structure du rendu *****/

 typedef enum {
								flat,

							} shading_t;

 typedef enum {
								fill_HF_S      /* faces remplies avec eclairage */

								} render_t;


 typedef enum {
								perspective    /* projection perspective */

							} visual_t;


 typedef struct camera_t
 {
	 short xi;   /* position de la camera initiale */
	 short yi;
	 short zi;
	 short xc;   /* position de la camera actuelle */
	 short yc;
	 short zc;
	 short rx;   /* nbre de rotations en X */
	 short ry;
	 short rz;
	 short tx;
	 short ty;
	 short tz;

 } camera_t;


 /***** fonctions *****/

 /** init struct **/

 void initStruct_3D(obj3D_t *obj);

 /** translation relative **/

 void relTransX_3D(obj3D_t *obj,short tx);

 void relTransY_3D(obj3D_t *obj,short ty);

 void relTransZ_3D(obj3D_t *obj,short tz);

 void absTransX_3D(obj3D_t *obj,short tx);

 void absTransY_3D(obj3D_t *obj,short ty);

 void absTransZ_3D(obj3D_t *obj,short tz);

 /** rotation relative **/

 /* angle en degree */

 void relRotX_3D(obj3D_t *obj,short ax);

 void relRotY_3D(obj3D_t *obj,short ay);

 void relRotZ_3D(obj3D_t *obj,short az);

 /** faces **/

 void makeFaces_3D(obj3D_t *obj);

 void traceContour_3D(obj3D_t *obj,unsigned char face,camera_t *cam,unsigned char far *page,unsigned char coul);

 void	traceCAndFillS_3D(obj3D_t *obj,unsigned char face,camera_t *cam,unsigned char far *page);

 void traceFace_3D(obj3D_t *obj,unsigned char face,camera_t *cam,unsigned char far *page,unsigned char coul);

 void traceObjet_3D(obj3D_t *obj,camera_t *cam,unsigned char far *page);

 /***** camera *****/

 void initCamera(camera_t *cam,short x,short y,short z);

 /***** translation *****/

 void relTransXCamera(camera_t *cam,short tx);

 void relTransYCamera(camera_t *cam,short ty);

 void relTransZCamera(camera_t *cam,short tz);

 /***** rotation *****/

 void relRotXCamera(camera_t *cam,short ax);

 void relRotYCamera(camera_t *cam,short ay);

 void relRotZCamera(camera_t *cam,short az);

 /****** rendering *****/

 void setResolScreen(unsigned short larg,unsigned short haut);

 void setRenderMode(render_t render);

 void setVisualType(visual_t visual);
