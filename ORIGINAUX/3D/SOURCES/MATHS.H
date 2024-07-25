/*******************************************************************/
/* Routines mathematiques :
/*
/* -  Tables precalculees de sinus et de cosinus
/*******************************************************************/

#include <math.h>

#define PI 3.141592654
#define FLOAT_MULT 65536
#define LSHIFT8     8
#define RSHIFT8     8
#define RSHIFT16   16
#define LSHIFT16   16
#define DEG2RAD(a) (((a)*PI)/180.0)
#define RAD2DEG(a) (((a)*180.)/PI)

/* determination de la table utilisee */
#define table256
/* #define table360 */

/* rotation en 3D */
#ifdef table256
   #define ROT_I(a,i,j) (((i)*cos256(a)+(j)*sin256(a))>>RSHIFT16)
   #define ROT_J(a,i,j) ((-(i)*sin256(a)+(j)*cos256(a))>>RSHIFT16)
#else
   #define ROT_I(a,i,j) (((i)*cos360(a)+(j)*sin360(a))>>RSHIFT16)
   #define ROT_J(a,i,j) ((-(i)*sin360(a)+(j)*cos360(a))>>RSHIFT16)
#endif


void initSinCos256(void);
void initSinCos360(void);
void initSinCos512(void);

long sin256(short angle);
long cos256(short angle);

long sin360(short angle);
long cos360(short angle);

long sin512(short angle);
long cos512(short angle);
