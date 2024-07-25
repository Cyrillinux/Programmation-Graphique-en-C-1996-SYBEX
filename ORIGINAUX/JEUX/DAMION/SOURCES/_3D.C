
 #include <vga.h>

 #include "maths.h"
 #include "_3D.h"
 #include "fill.h"
 #include <math.h>

 #define  LARGF 230 //parametre de la perspective
 #define  TX 10
 #define  TY 10

 //variables globales statiques

 static unsigned  short Ox,Oy;
 static render_t  Rendering;
 static visual_t  VisualType;

 static unsigned char i,j,f,face;
 static long s,va,vb,vc;
 static short a,b,c;
 static short x,y;

 static unsigned short cminx,cmaxx,cminy,cmaxy;
 static unsigned short lig,col,memo,fin;
 static short deb;
 static unsigned char coul;

 void initStruct_3D(obj3D_t *obj)
 {
	 obj->rX = 0;
	 obj->rY = 0;
	 obj->rZ = 0;
	 obj->trX    = 0;
	 obj->trY    = 0;
	 obj->trZ    = 0;

	 for(i = 0; i < obj->nbPoints; i++)
	 {
		 obj->tabPointsA[0][i] = obj->tabPointsI[0][i];
		 obj->tabPointsA[1][i] = obj->tabPointsI[1][i];
		 obj->tabPointsA[2][i] = obj->tabPointsI[2][i];
	 }

	 makeFaces_3D(obj);
 }

 //Translation relative

 void relTransX_3D(obj3D_t *obj,short tx)
 {
	 obj->trX += tx;
	 for(i = 0; i < obj->nbPoints; i++) obj->tabPointsA[0][i]+=tx;
 }

 void relTransY_3D(obj3D_t *obj,short ty)
 {
	 obj->trY += ty;
	 for(i = 0; i < obj->nbPoints; i++) obj->tabPointsA[1][i]+=ty;
 }

 void relTransZ_3D(obj3D_t *obj,short tz)
 {
	 obj->trZ += tz;
	 for(i = 0; i < obj->nbPoints; i++) obj->tabPointsA[2][i]+=tz;
 }

 //Translation absolue

 void absTransX_3D(obj3D_t *obj,short tx)
 {
	 obj->trX = tx;
	 for(i = 0; i < obj->nbPoints; i++) obj->tabPointsA[0][i]=tx;
 }

 void absTransY_3D(obj3D_t *obj,short ty)
 {
	 obj->trY = ty;
	 for(i = 0; i < obj->nbPoints; i++) obj->tabPointsA[1][i]=ty;
 }

 void absTransZ_3D(obj3D_t *obj,short tz)
 {
	 obj->trZ = tz;
	 for(i = 0; i < obj->nbPoints; i++) obj->tabPointsA[2][i]=tz;
 }

 //Rotation relative

 void relRotX_3D(obj3D_t *obj,short ax)
 {
	 obj->rX = (obj->rX + ax)%360;
 }

 void relRotY_3D(obj3D_t *obj,short ay)
 {
	 obj->rY = (obj->rY + ay)%360;
 }

 void relRotZ_3D(obj3D_t *obj,short az)
 {
	 obj->rZ = (obj->rZ + az)%360;
 }

 void rotObjet_3D(obj3D_t *obj)
 {
	 for(i = 0; i < obj->nbPoints; i++)
	 {
			/*  rotation X */
			obj->tabPointsA[0][i] = obj->tabPointsI[0][i];
			obj->tabPointsA[1][i] = ROT_I(obj->rX,obj->tabPointsI[1][i],obj->tabPointsI[2][i]);
			obj->tabPointsA[2][i] = ROT_J(obj->rX,obj->tabPointsI[1][i],obj->tabPointsI[2][i]);

			/*  rotation Y */
			x = ROT_I(obj->rY,obj->tabPointsA[0][i],obj->tabPointsA[2][i]);
			obj->tabPointsA[2][i] = ROT_J(obj->rY,obj->tabPointsA[0][i],obj->tabPointsA[2][i]);
			obj->tabPointsA[0][i] = x;

			/*  rotation Z */
			x = ROT_I(obj->rZ,obj->tabPointsA[0][i],obj->tabPointsA[1][i]);
			obj->tabPointsA[1][i] = ROT_J(obj->rZ,obj->tabPointsA[0][i],obj->tabPointsA[1][i]);
			obj->tabPointsA[0][i] = x;

			/* translation X,Y,Z */
			obj->tabPointsA[0][i] += obj->trX;
			obj->tabPointsA[1][i] += obj->trY;
			obj->tabPointsA[2][i] += obj->trZ;

	 }
 }

 //Faces

 void makeFaces_3D(obj3D_t *obj)
 {
	 for(i = 0; i < obj->nbFaces; i++)
	 {
		 for(j = 0, f = 0; j < 3*obj->nbPFaces[i]; j+=3, f++)
		 {
				 obj->tabFaces[i][j]   = obj->tabPointsA[0][obj->tabPFaces[i][f]];
				 obj->tabFaces[i][j+1] = obj->tabPointsA[1][obj->tabPFaces[i][f]];
				 obj->tabFaces[i][j+2] = obj->tabPointsA[2][obj->tabPFaces[i][f]];
		 }
	 }
 }

 void convert3dTo2d_3D(short x3d,short y3d,short z3d,camera_t *cam,short *x2d,short *y2d)
 {
	 short te,x3dn,y3dn,z3dn;

	 // X
   x3dn = x3d;
	 te = ROT_I(-cam->rx,y3d,z3d);
	 z3dn = ROT_J(-cam->rx,y3d,z3d);
	 y3dn = te;

	 // Y
	 te = ROT_I(-cam->ry,x3dn,z3dn);
	 z3dn = ROT_J(-cam->ry,x3dn,z3dn);
	 x3dn = te;

	 // Z
   te = ROT_I(-cam->rz,x3dn,y3dn);
	 y3dn = ROT_J(-cam->rz,x3dn,y3dn);
	 x3dn = te;

	 *x2d=-((((long)x3dn-cam->tx)*LARGF)/((long)z3dn-cam->tz*TX));
	 *y2d=-((((long)y3dn-cam->ty)*LARGF)/((long)z3dn-cam->tz*TY));
 }


 void traceContour_3D(obj3D_t *obj,unsigned char face,camera_t *cam,unsigned char far *page,unsigned char coul)
 {
		 //Trace le contour de la face dans la page
		 //Conversion en 2D pour affichage
     short mv_x,mv_y;

		 convert3dTo2d_3D(obj->tabFaces[face][0],obj->tabFaces[face][1],obj->tabFaces[face][2],cam,&x,&y);

     mv_x=Ox+x;
     mv_y=Oy-y;

		 for(i = 3; i < 3*obj->nbPFaces[face]; i+=3)
		 {
				//Conversion 2D pour affichage
				convert3dTo2d_3D(obj->tabFaces[face][i],obj->tabFaces[face][i+1],obj->tabFaces[face][i+2],cam,&a,&b);

        Line(mv_x,mv_y,Ox+a,Oy-b,page,coul);
        mv_x=Ox+a;
        mv_y=Oy-b;
		 }

     Line(mv_x,mv_y,Ox+x,Oy-y,page,coul);
 }

 void ligneh(short x1,short x2,short y,unsigned char far *page,char coul)
 {
	 word m,off1,off2;

   off1=y*320+x1;
   off2=off1+(x2-x1);

	 for(m=off1;m<=off2;m++)
      page[m]=coul; // = PutPixel(m,y,page,coul);
 }


 void	traceCAndFillS_3D(obj3D_t *obj,unsigned char face,camera_t *cam,unsigned char far * page)
 {
	 //Trace le contour de la face dans la page
   //et la remplit en tenant compte du shading

	 short xae,yae,xbe,ybe,xce,yce,xde,yde;
	 float cosa,n,d;
	 #define DELTA 0.3

	 convert3dTo2d_3D(obj->tabFaces[face][0],obj->tabFaces[face][1],obj->tabFaces[face][2],cam,&xae,&yae);
	 xae += Ox; yae = Oy-yae;
	 convert3dTo2d_3D(obj->tabFaces[face][3],obj->tabFaces[face][4],obj->tabFaces[face][5],cam,&xbe,&ybe);
	 xbe += Ox; ybe = Oy-ybe;
	 convert3dTo2d_3D(obj->tabFaces[face][6],obj->tabFaces[face][7],obj->tabFaces[face][8],cam,&xce,&yce);
	 xce += Ox; yce = Oy-yce;
	 convert3dTo2d_3D(obj->tabFaces[face][9],obj->tabFaces[face][10],obj->tabFaces[face][11],cam,&xde,&yde);
	 xde += Ox; yde = Oy-yde;

	 n = (float)va*cam->xc+(float)vb*cam->yc+(float)vc*cam->zc;
	 d = sqrt(((float)va*(float)va) + ((float)vb*(float)vb) + ((float)vc*(float)vc)) * sqrt( ((float)cam->xc*(float)cam->xc) + ((float)cam->yc*(float)cam->yc) + ((float)cam->zc*(float)cam->zc));
	 cosa = n/d;

   //Correction delta
	 if(cosa<0.0)
			cosa=0.0;
	 if(cosa>1.0)
			cosa=1.0;

	 Ink(obj->tabFill[face].indPal,(obj->tabFill[face].r*cosa),(obj->tabFill[face].v*cosa),(obj->tabFill[face].b*cosa));

	 fillRect((unsigned long)xae,(unsigned long)yae,(unsigned long)xbe,(unsigned long)ybe,(unsigned long)xce,(unsigned long)yce,(unsigned long)xde,(unsigned long)yde,page,obj->tabFill[face].indPal);
 }

 void traceObjet_3D(obj3D_t *obj,camera_t *cam,unsigned char far * page)
 {
		short p0x,p0y,p0z,p1x,p1y,p1z,p2x,p2y,p2z;

		rotObjet_3D(obj);
		makeFaces_3D(obj);

		for(face = 0; face < obj->nbFaces; face++)
		{
			 //Determiner si la face est visible
			 //Calcul du vecteur normal(a,b,c) a la face i

			 p0x  = obj->tabFaces[face][0];
			 p0y  = obj->tabFaces[face][1];
			 p0z  = obj->tabFaces[face][2];

			 convert3dTo2d_3D(p0x,p0y,p0z,cam,&p0x,&p0y);

			 p1x  = obj->tabFaces[face][3];
			 p1y  = obj->tabFaces[face][4];
			 p1z  = obj->tabFaces[face][5];

			 convert3dTo2d_3D(p1x,p1y,p1z,cam,&p1x,&p1y);

			 p2x  = obj->tabFaces[face][6];
			 p2y  = obj->tabFaces[face][7];
			 p2z  = obj->tabFaces[face][8];

			 convert3dTo2d_3D(p2x,p2y,p2z,cam,&p2x,&p2y);

			 va =   (p0y - p1y)*(p0z + p1z)
					 + (p1y - p2y)*(p1z + p2z)
					 + (p2y - p0y)*(p2z + p0z);

			 vb =   (p0z - p1z)*(p0x + p1x)
					 + (p1z - p2z)*(p1x + p2x)
					 + (p2z - p0z)*(p2x + p0x);

			 vc =   (p0x - p1x)*(p0y + p1y)
					 + (p1x - p2x)*(p1y + p2y)
					 + (p2x - p0x)*(p2y + p0y);

			 if(vc>0) //face visible
          traceCAndFillS_3D(obj,face,cam,page);
		}
 }

 //Point de vue

 void initCamera(camera_t *cam,short x,short y,short z)
 {
	 cam->xc = cam->xi = x;
	 cam->yc = cam->yi = y;
	 cam->zc = cam->zi = z;
	 cam->tx = x;
	 cam->ty = y;
	 cam->tz = z;
	 cam->rx = cam->ry = cam->rz = 0;
 }

 //Translation

 void relTransXCamera(camera_t *cam,short tx)
 {
	 cam->xc += tx;
	 cam->tx += tx;
 }

 void relTransYCamera(camera_t *cam,short ty)
 {
	 cam->yc += ty;
	 cam->ty += ty;
 }

 void relTransZCamera(camera_t *cam,short tz)
 {
	 cam->zc += tz;
	 cam->tz += tz;
 }

 //Rotations

 void relRotXCamera(camera_t *cam,short ax)
 {
	 cam->rx += ax;
	 cam->yc = ROT_I(cam->rx,cam->yi,cam->zi);
	 cam->zc = ROT_J(cam->rx,cam->yi,cam->zi);
 }

 void relRotYCamera(camera_t *cam,short ay)
 {
	 cam->ry += ay;
	 cam->xc = ROT_I(cam->ry,cam->xi,cam->zi);
	 cam->zc = ROT_J(cam->ry,cam->xi,cam->zi);
 }

 void relRotZCamera(camera_t *cam,short az)
 {
	 cam->rz += az;
	 cam->xc = ROT_I(cam->rz,cam->xi,cam->yi);
	 cam->yc = ROT_J(cam->rz,cam->xi,cam->yi);
 }

 //Rendering

 void setResolScreen(unsigned short larg,unsigned short haut)
 {
	 Ox = larg/2;
	 Oy = haut/2;
 }

 void setRenderMode(render_t render)
 {
	 Rendering = render;
 }

 void setVisualType(visual_t visual)
 {
	 VisualType = visual;
 }

