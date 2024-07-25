/*
 * Parametrage des fonctions de remplissage
 */

/* Shifts pour interpolation */

#define LS 8      /* left-shift */
#define RS 8      /* right-shift */

/*
 * Puiss(2,LS_DEG) couleurs par degrade
 */

#define LS_DEG 4
#define RS_DEG 4

/* Fonctions */

/* trace de lignes */

void ligneh(short xd,short xf,short l,unsigned char * page,unsigned char coul);
void lignemap(short x1,short y1,short x2,short y2,unsigned char * ecr1,short x3,short y3,short L,unsigned char * ecr2);
void lignemapflat(short x1,short y1,short x2,short y2,unsigned char * ecr1,short x3,short y3,short L,short cosa,unsigned char * ecr2);
void lignehg(short xd,short ideb,short xf,short ifin,short l,unsigned char * page);
void lignehgz(short xd,short zd,short ideb,short xf,short zf,short ifin,short l,unsigned char * page);
void lignemapg(short x1,short y1,short x2,short y2,unsigned char * ecr1,short x3,short y3,short L,short ideb,short ifin,unsigned char * ecr2);
void lignemapgz(short x1,short y1,short x2,short y2,unsigned char far * ecr1,short x3,short y3,short L,short ideb,short ifin,short zd,short zf,unsigned char far * ecr2);
void lignemapp(short x1,short y1,short x2,short y2,unsigned char * ecr1,short x3,short y3,short L,unsigned char * ecr2,short xid,short yid,short xif,short yif);
void lignemappz(short x1,short y1,short x2,short y2,unsigned char * ecr1,short x3,short y3,short L,unsigned char * ecr2,short zd,short zf,short xid,short yid,short xif,short yif);
void lignehp(short xd,short xid,short yid,short xf,short xif,short yif,short l,unsigned char * page);
void lignehpz(short xd,short zd,short xid,short yid,short xf,short zf,short xif,short yif,short l,unsigned char * page);


/* remplissage */

void fillPoly3_V2(long xa,long ya,long xb,long yb,long xc,long yc,unsigned char * page,unsigned char coul);
void fillPoly3Z_V2(long xa,long ya,long za,long xb,long yb,long zb,long xc,long yc,long zc,unsigned char * page,unsigned char coul);
void fillPoly3Map_V2(long xa,long ya,long xb,long yb,long xc,long yc,unsigned char * pagemap,unsigned char * page);
void fillPoly3MapFlat_V2(long xa,long ya,long xb,long yb,long xc,long yc,short cosa,unsigned char * pagemap,unsigned char * page);
void fillPoly3MapZ_V2(long xa,long ya,long za,long xb,long yb,long zb,long xc,long yc,long zc,unsigned char * pagemap,unsigned char * page);
void fillPoly3MapFlatZ_V2(long xa,long ya,long za,long xb,long yb,long zb,long xc,long yc,long zc,short cosa,unsigned char * pagemap,unsigned char * page);
void fillPoly3Gouraud_V2(long xa,long ya,long ia,long xb,long yb,long ib,long xc,long yc,long ic,unsigned char * page);
void fillPoly3GouraudZ_V2(long xa,long ya,long za,long ia,long xb,long yb,long zb,long ib,long xc,long yc,long zc,long ic,unsigned char * page);
void fillP3MapGouraud_V2(long xa,long ya,long ia,long xb,long yb,long ib,long xc,long yc,long ic,unsigned char * pagemap,unsigned char * page);
void fillP3MapGouraudZ_V2(long xa,long ya,long za,long ia,long xb,long yb,long zb,long ib,long xc,long yc,long zc,long ic,unsigned char * pagemap,unsigned char * page);
void fillP3MapPhong_V2(long xa,long ya,long xb,long yb,long xc,long yc,unsigned char * pagemap,unsigned char * page);
void fillP3MapPhongZ_V2(long xa,long ya,long za,long xb,long yb,long zb,long xc,long yc,long zc,unsigned char * pagemap,unsigned char * page);
void fillPoly3Phong_V2(long xa,long ya,long xb,long yb,long xc,long yc,unsigned char * page);
void fillPoly3PhongZ_V2(long xa,long ya,long za,long xb,long yb,long zb,long xc,long yc,long zc,unsigned char * page);

/* Normal-Fill : Appeller les fcts C ou ASM */

// DEFINIR SI APPEL C :
// #define ligneh  ligneh_C
#define lignehz lignehz_C

/* Mapping : Appeller les fonctions C ou ASM */

/* DEFINIR SI APPEL C :*/
//#define lignemap        lignemap_C
/* DEFINIR SI APPEL C : */
// #define lignemapflat   lignemapflat_C
#define lignemapz        lignemapz_C
#define lignemapflatz  lignemapflatz_C

/* Gouraud : appeller les fonctions C */
#define lignehg      lignehg_C
#define lignehgz    lignehgz_C
#define lignemapg   lignemapg_C
#define lignemapgz  lignemapgz_C

/* Phong : appeller les fonctions C */
#define lignehp      lignehp_C
#define lignehpz    lignehpz_C
#define lignemapp   lignemapp_C
#define lignemappz  lignemappz_C

/* Macros de SWAP */

#define SWAP(a,b)  { SwapTmp=a; \
                     a=b;   \
                     b=SwapTmp; }
