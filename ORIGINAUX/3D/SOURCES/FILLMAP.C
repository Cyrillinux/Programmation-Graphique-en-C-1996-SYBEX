/*
 * Remplissage de triangles
 * Patrice Siracusa (1995)
 *
 * Fonctions de remplissage en mapping
 *
 * - sans z-buffer
 *   + flat-shading
 *   + gouraud-shading
 *   + phong-shading
 *
 * - avec z-buffer
 *   + flat-shading
 *   + gouraud-shading
 *   + phong-shading
 */

 /* Include */

#include "vga.h"
#include "maths.h"
#include "fill.h"

/* Defines & MACROS */

/*  Variables globales */

/*  Variables locales-globales */

static long SwapTmp;
static short p;
static long xd,xf;
static long delta1,delta2;
static short l;
static long zc,dz;
static short pz;
static long zd,zf,dz1,dz2;
static long xdm,ydm,xfm,yfm,dmap1,dmap2,p1,p2;
static long pac,pab,pbc;
static long ideb,ifin,di1,di2;
static long xideb,xifin,yideb,yifin,dxi1,dxi2,dyi1,dyi2;

/* Extern */

extern char far ZBuffer[64000]; /* -128<=z<=+127 */
extern long XAmap,YAmap,XBmap,YBmap,XCmap,YCmap;
extern long RefXa,RefYa,RefXb,RefYb,RefXc,RefYc;
extern unsigned char TPhongXY[65535];

/* Fonctions */

/*
 * Remplissage sans Z-BUFFER
 */

/* BASIC-MAPPING */

void lignemap_C(short x1,short y1,short x2,short y2,unsigned char far * ecr1,short x3,short y3,short L,unsigned char far * ecr2)
{
  short l;
  long  v1,w1,x,y;
  byte  coul;
  short offp;

  if(L==0)
    return;

  /* clipping haut et bas */
  if(y3<0 || y3>199)
    return;

  /* mapping */
  v1 = (((long)x2-(long)x1)<<LS)/(long)L;
  w1 = (((long)y2-(long)y1)<<LS)/(long)L;

  y  = (long)y1<<LS;
  x =  (long)x1<<LS;

  /* clipping droit */
  if(x3+L > 319)
     L = L-(x3+L-319);

  /* offset pixel a afficher */
  offp = y3*320+x3;

  for(l=0; l<L; l++)
  {
    y += w1;
    x += v1;
    if(x3>=0)  /* clipping gauche */
    {
      coul = ecr1[(y>>RS)*320+(x>>RS)];
      ecr2[offp] = coul;
    }
    x3++;
    offp++;
  }
}

void fillPoly3Map_V2(long xa,long ya,long xb,long yb,long xc,long yc,unsigned char far * pagemap,unsigned char far * page)
{
  /* Trie des trois points de haut en bas */

  if(ya>yb)
  {
    SWAP(xa,xb)
    SWAP(ya,yb)
    /* mapping */
    SWAP(XAmap,XBmap)
    SWAP(YAmap,YBmap)
  }

  if(ya>yc)
  {
    SWAP(xa,xc)
    SWAP(ya,yc)
    /* mapping */
    SWAP(XAmap,XCmap)
    SWAP(YAmap,YCmap)
  }

  if(yb>yc)
  {
    SWAP(xb,xc)
    SWAP(yb,yc)
    /* mapping */
    SWAP(XBmap,XCmap)
    SWAP(YBmap,YCmap)
  }


  /* Remplissage */

  /* mapping */

  if(ya!=yc)
  {
    pac = ya-yc;
    delta1 = ((xa-xc)<<LS)/pac;
    /* mapping */
    p1 = ((YAmap-YCmap)<<LS)/pac;
    dmap1 = ((XAmap-XCmap)<<LS)/pac;
    ydm = YAmap<<LS;
    yfm = ydm;
    xdm = XAmap<<LS;
    xfm = xdm;
  }
  else
  {
    delta1 = 0;
    /* mapping */
    p1 = (YAmap-YCmap)<<LS;
    dmap1 = (XAmap-XCmap)<<LS;
    yfm = YCmap<<LS;
    ydm = YAmap<<LS;
    xfm = XCmap<<LS;
    xdm = XAmap<<LS;
  }

  if(ya!=yb)
  {
    pab = yb-ya;
    delta2 = ((xb-xa)<<LS)/pab;
    /* mapping */
    p2 = ((YBmap-YAmap)<<LS)/pab;
    dmap2 = ((XBmap-XAmap)<<LS)/pab;
    ydm = YAmap<<LS;
    yfm = ydm;
    xdm = XAmap<<LS;
    xfm = xdm;
  }
  else
  {
    delta2 = 0;
    /* mapping */
    p2 = (YBmap-YAmap)<<LS;
    dmap2 = (XBmap-XAmap)<<LS;
    yfm = YBmap<<LS;
    ydm = YAmap<<LS;
    xfm = XBmap<<LS;
    xdm = XAmap<<LS;
  }

  xd = xa<<LS;
  xf = xd;

  if(delta1<delta2)
  {
    for(l = ya+1; l <= yb; l++)
    {
       ydm += p1;
       xdm += dmap1;
       yfm += p2;
       xfm += dmap2;
       xd += delta1;
       xf += delta2;
       lignemap((short)(xdm>>RS),(short)(ydm>>RS),(short)(xfm>>RS),(short)(yfm>>RS),pagemap,(short)(xd>>RS),l,(short)(xf>>RS)-(short)(xd>>RS),page);
    }
  }
  else
  {
    for(l = ya+1; l <= yb; l++)
    {
       ydm += p1;
       xdm += dmap1;
       yfm += p2;
       xfm += dmap2;
       xd += delta1;
       xf += delta2;
       lignemap((short)(xfm>>RS),(short)(yfm>>RS),(short)(xdm>>RS),(short)(ydm>>RS),pagemap,(short)(xf>>RS),l,(short)(xd>>RS)-(short)(xf>>RS),page);
     }
  }

  if(yb!=yc)
  {
    pbc = yb-yc;
    delta2 = ((xb-xc)<<LS)/pbc;
    xf = xb<<LS;

    /* mapping */
    p2 = ((YBmap-YCmap)<<LS)/pbc;
    dmap2 = ((XBmap-XCmap)<<LS)/pbc;

    if(delta2<delta1)
    {
       for(l = yb+1; l <= yc; l++)
       {
         ydm += p1;
         xdm += dmap1;
         yfm += p2;
         xfm += dmap2;
         xd += delta1;
         xf += delta2;
         lignemap((short)(xdm>>RS),(short)(ydm>>RS),(short)(xfm>>RS),(short)(yfm>>RS),pagemap,(short)(xd>>RS),l,(short)(xf>>RS)-(short)(xd>>RS),page);
       }
    }
    else
    {
       for(l = yb+1; l <= yc; l++)
       {
         ydm += p1;
         xdm += dmap1;
         yfm += p2;
         xfm += dmap2;
         xd += delta1;
         xf += delta2;
         lignemap((short)(xfm>>RS),(short)(yfm>>RS),(short)(xdm>>RS),(short)(ydm>>RS),pagemap,(short)(xf>>RS),l,(short)(xd>>RS)-(short)(xf>>RS),page);
       }
    }
  }
}

/* MAPPING + FLAT-SHADING */

void lignemapflat_C(short x1,short y1,short x2,short y2,unsigned char far * ecr1,short x3,short y3,short L,short cosa,unsigned char far * ecr2)
{
  short  l;
  long x,y,v1,w1;
  byte  e1,coul;
  long  nind;
  short offp;

  if(L==0)
    return;

  /* clipping haut et bas */
  if(y3<0 || y3>199)
    return;

  v1 = (((long)x2-(long)x1)<<LS)/(long)L;
  w1 = (((long)y2-(long)y1)<<LS)/(long)L;
  y  = (long)y1<<LS;
  x =  (long)x1<<LS;

  /* clipping droit */
  if(x3+L > 319)
       L = L-(x3+L-319);

  /* offset pixel a afficher */
  offp =  y3*320+x3;

  for(l=0; l<L; l++)
  {
    y += w1;
    x += v1;

    if(x3>=0)  /* clipping gauche */
    {
      coul = ecr1[(y>>RS)*320+(x>>RS)];

      /* formule generale :
       * Image de mapping pouvant contenir
       * n'importe qu'elle couleur dans les degrades a 16 couleurs
       */
      #if 0
      e1 = (coul>>RS_DEG)<<LS_DEG;
      nind = ((long)coul-(long)e1)*(long)cosa;
      nind += e1<<LS_DEG;
      ecr2[offp] = nind>>RS_DEG;
      #endif

      /* formule adaptee :
       * Seuls les derniers niveaux de chaque degrades sont utilises
       */
      ecr2[offp] = coul -15 + cosa; // 16 couleurs par degrade
    }
    x3++;
    offp++;
  }
}

void fillPoly3MapFlat_V2(long xa,long ya,long xb,long yb,long xc,long yc,short cosa,unsigned char far * pagemap,unsigned char far * page)
{
  /* Trie des trois points de haut en bas */

  if(ya>yb)
  {
    SWAP(xa,xb)
    SWAP(ya,yb)
    /* mapping */
    SWAP(XAmap,XBmap)
    SWAP(YAmap,YBmap)
  }

  if(ya>yc)
  {
    SWAP(xa,xc)
    SWAP(ya,yc)
    /* mapping */
    SWAP(XAmap,XCmap)
    SWAP(YAmap,YCmap)
  }

  if(yb>yc)
  {
    SWAP(xb,xc)
    SWAP(yb,yc)
    /* mapping */
    SWAP(XBmap,XCmap)
    SWAP(YBmap,YCmap)
  }


  /* Remplissage */

  /* mapping */

  if(ya!=yc)
  {
    pac = ya-yc;
    delta1 = ((xa-xc)<<LS)/pac;
    /* mapping */
    p1 = ((YAmap-YCmap)<<LS)/pac;
    dmap1 = ((XAmap-XCmap)<<LS)/pac;
    ydm = YAmap<<LS;
    yfm = ydm;
    xdm = XAmap<<LS;
    xfm = xdm;
  }
  else
  {
    delta1 = 0;
    /* mapping */
    p1 = (YAmap-YCmap)<<LS;
    dmap1 = (XAmap-XCmap)<<LS;
    yfm = YCmap<<LS;
    ydm = YAmap<<LS;
    xfm = XCmap<<LS;
    xdm = XAmap<<LS;
  }

  if(ya!=yb)
  {
    pab = yb-ya;
    delta2 = ((xb-xa)<<LS)/pab;
    /* mapping */
    p2 = ((YBmap-YAmap)<<LS)/pab;
    dmap2 = ((XBmap-XAmap)<<LS)/pab;
    ydm = YAmap<<LS;
    yfm = ydm;
    xdm = XAmap<<LS;
    xfm = xdm;
  }
  else
  {
    delta2 = 0;
    /* mapping */
    p2 = (YBmap-YAmap)<<LS;
    dmap2 = (XBmap-XAmap)<<LS;
    yfm = YBmap<<LS;
    ydm = YAmap<<LS;
    xfm = XBmap<<LS;
    xdm = XAmap<<LS;
  }

  xd = xa<<LS;
  xf = xd;

  if(delta1<delta2)
  {
    for(l = ya+1; l <= yb; l++)
    {
       ydm += p1;
       xdm += dmap1;
       yfm += p2;
       xfm += dmap2;
       xd += delta1;
       xf += delta2;
       lignemapflat((short)(xdm>>RS),(short)(ydm>>RS),(short)(xfm>>RS),(short)(yfm>>RS),pagemap,(short)(xd>>RS),l,(short)(xf>>RS)-(short)(xd>>RS),cosa,page);
    }
  }
  else
  {
    for(l = ya+1; l <= yb; l++)
    {
       ydm += p1;
       xdm += dmap1;
       yfm += p2;
       xfm += dmap2;
       xd += delta1;
       xf += delta2;
       lignemapflat((short)(xfm>>RS),(short)(yfm>>RS),(short)(xdm>>RS),(short)(ydm>>RS),pagemap,(short)(xf>>RS),l,(short)(xd>>RS)-(short)(xf>>RS),cosa,page);
    }
  }

  if(yb!=yc)
  {
    pbc = yb-yc;
    delta2 = ((xb-xc)<<LS)/pbc;
    xf = xb<<LS;

    /* mapping */
    p2 = ((YBmap-YCmap)<<LS)/pbc;
    dmap2 = ((XBmap-XCmap)<<LS)/pbc;

    if(delta2<delta1)
    {
      for(l = yb+1; l <= yc; l++)
      {
         ydm += p1;
         xdm += dmap1;
         yfm += p2;
         xfm += dmap2;
         xd += delta1;
         xf += delta2;
         lignemapflat((short)(xdm>>RS),(short)(ydm>>RS),(short)(xfm>>RS),(short)(yfm>>RS),pagemap,(short)(xd>>RS),l,(short)(xf>>RS)-(short)(xd>>RS),cosa,page);
      }
    }
    else
    {
      for(l = yb+1; l <= yc; l++)
      {
         ydm += p1;
         xdm += dmap1;
         yfm += p2;
         xfm += dmap2;
         xd += delta1;
         xf += delta2;
         lignemapflat((short)(xfm>>RS),(short)(yfm>>RS),(short)(xdm>>RS),(short)(ydm>>RS),pagemap,(short)(xf>>RS),l,(short)(xd>>RS)-(short)(xf>>RS),cosa,page);
      }
    }
  }
}

/* MAPPING + GOURAUD-SHADING */

void lignemapg_C(short x1,short y1,short x2,short y2,unsigned char far * ecr1,short x3,short y3,short L,short ideb,short ifin,unsigned char far * ecr2)
{
  short l;
  long  v1,w1,x,y;
  byte  coul,e1;
  long di,ic;
  short offp;

  if(L==0)
    return;

  /* clipping haut et bas */
  if(y3<0 || y3>199)
    return;

  /* mapping */
  v1 = (((long)x2-(long)x1)<<LS)/(long)L;
  w1 = (((long)y2-(long)y1)<<LS)/(long)L;
  y  = (long)y1<<LS;
  x =  (long)x1<<LS;

  /* gouraud */
  if(L == 0)
   di = 0;
  else
   di = ((long)(ifin-ideb)<<LS)/L;
  ic = (long)ideb<<LS;

  /* clipping droit */
  if(x3+L > 319)
     L = L-(x3+L-319);

  /* offset pixel a afficher */
  offp = y3*320+x3;

  for(l=0; l<L; l++)
  {
    y += w1;
    x += v1;
    ic+=di;
    if(x3>=0) /* clipping gauche */
    {
      /* formule adaptee :
       * Seuls les derniers niveaux
       * de chaque degrade sont utilises
       */
      coul = ecr1[(y>>RS)*320+(x>>RS)];
      ecr2[offp] = coul -31 + (ic>>RS);    // degrade de 32 couleurs
    }
    x3++;
    offp++;
  }
}

void fillP3MapGouraud_V2(long xa,long ya,long ia,long xb,long yb,long ib,long xc,long yc,long ic,unsigned char far * pagemap,unsigned char far * page)
{
  /* Trie des trois points de haut en bas */

  if(ya>yb)
  {
    SWAP(xa,xb)
    SWAP(ya,yb)
    /* mapping */
    SWAP(XAmap,XBmap)
    SWAP(YAmap,YBmap)
    /* gouraud */
    SWAP(ia,ib)
  }

  if(ya>yc)
  {
    SWAP(xa,xc)
    SWAP(ya,yc)
    /* mapping */
    SWAP(XAmap,XCmap)
    SWAP(YAmap,YCmap)
    /* gouraud */
    SWAP(ia,ic)
  }

  if(yb>yc)
  {
    SWAP(xb,xc)
    SWAP(yb,yc)
    /* mapping */
    SWAP(XBmap,XCmap)
    SWAP(YBmap,YCmap)
    /* gouraud */
    SWAP(ib,ic)
  }


  /* Remplissage */

  /* mapping */

  if(ya!=yc)
  {
    pac = ya-yc;
    delta1 = ((xa-xc)<<LS)/pac;
    /* mapping */
    p1 = ((YAmap-YCmap)<<LS)/pac;
    dmap1 = ((XAmap-XCmap)<<LS)/pac;
    ydm = YAmap<<LS;
    yfm = ydm;
    xdm = XAmap<<LS;
    xfm = xdm;
    /* gouraud */
    di1 = ((ia-ic)<<LS)/pac;
  }
  else
  {
    delta1 = 0;
    /* mapping */
    p1 = (YAmap-YCmap)<<LS;
    dmap1 = (XAmap-XCmap)<<LS;
    yfm = YCmap<<LS;
    ydm = YAmap<<LS;
    xfm = XCmap<<LS;
    xdm = XAmap<<LS;
    /* gouraud */
    di1 = 0;
  }

  if(ya!=yb)
  {
    pab = yb-ya;
    delta2 = ((xb-xa)<<LS)/pab;
    /* mapping */
    p2 = ((YBmap-YAmap)<<LS)/pab;
    dmap2 = ((XBmap-XAmap)<<LS)/pab;
    ydm = YAmap<<LS;
    yfm = ydm;
    xdm = XAmap<<LS;
    xfm = xdm;
    /* gouraud*/
    di2 = ((ib-ia)<<LS)/pab;
  }
  else
  {
    delta2 = 0;
    /* mapping */
    p2 = (YBmap-YAmap)<<LS;
    dmap2 = (XBmap-XAmap)<<LS;
    yfm = YBmap<<LS;
    ydm = YAmap<<LS;
    xfm = XBmap<<LS;
    xdm = XAmap<<LS;
    /* gouraud */
    di2 = 0;
  }

  xd = xa<<LS;
  xf = xd;

  /* gouraud */
  ideb = ia<<LS;
  ifin = ideb;

  if(delta1<delta2)
  {
    for(l = ya+1; l <= yb; l++)
    {
       ydm += p1;
       xdm += dmap1;
       yfm += p2;
       xfm += dmap2;
       xd += delta1;
       xf += delta2;
       ideb += di1;
       ifin += di2;
       lignemapg((short)(xdm>>RS),(short)(ydm>>RS),(short)(xfm>>RS),(short)(yfm>>RS),pagemap,(short)(xd>>RS),l,(short)(xf>>RS)-(short)(xd>>RS),(short)(ideb>>RS),(short)(ifin>>RS),page);
    }
  }
  else
  {
    for(l = ya+1; l <= yb; l++)
    {
       ydm += p1;
       xdm += dmap1;
       yfm += p2;
       xfm += dmap2;
       xd += delta1;
       xf += delta2;
       ideb += di1;
       ifin += di2;
       lignemapg((short)(xfm>>RS),(short)(yfm>>RS),(short)(xdm>>RS),(short)(ydm>>RS),pagemap,(short)(xf>>RS),l,(short)(xd>>RS)-(short)(xf>>RS),(short)(ifin>>RS),(short)(ideb>>RS),page);
    }
  }

  if(yb!=yc)
  {
    pbc = yb-yc;
    delta2 = ((xb-xc)<<LS)/pbc;
    xf = xb<<LS;

    /* mapping */
    p2 = ((YBmap-YCmap)<<LS)/pbc;
    dmap2 = ((XBmap-XCmap)<<LS)/pbc;

    /* gouraud */
    di2 = ((ib-ic)<<LS)/pbc;
    ifin = ib<<LS;

    if(delta2<delta1)
    {
      for(l = yb+1; l <= yc; l++)
      {
         ydm += p1;
         xdm += dmap1;
         yfm += p2;
         xfm += dmap2;
         xd += delta1;
         xf += delta2;
         ideb += di1;
         ifin += di2;
         lignemapg((short)(xdm>>RS),(short)(ydm>>RS),(short)(xfm>>RS),(short)(yfm>>RS),pagemap,(short)(xd>>RS),l,(short)(xf>>RS)-(short)(xd>>RS),(short)(ideb>>RS),(short)(ifin>>RS),page);
      }
    }
    else
    {
      for(l = yb+1; l <= yc; l++)
      {
         ydm += p1;
         xdm += dmap1;
         yfm += p2;
         xfm += dmap2;
         xd += delta1;
         xf += delta2;
         ideb += di1;
         ifin += di2;
         lignemapg((short)(xfm>>RS),(short)(yfm>>RS),(short)(xdm>>RS),(short)(ydm>>RS),pagemap,(short)(xf>>RS),l,(short)(xd>>RS)-(short)(xf>>RS),(short)(ifin>>RS),(short)(ideb>>RS),page);
      }
    }
  }
}

/* MAPPING + PHONG-SHADING */

void lignemapp_C(short x1,short y1,short x2,short y2,unsigned char far * ecr1,short x3,short y3,short L,unsigned char far * ecr2,short xid,short yid,short xif,short yif)
{
  short l;
  long  v1,w1,x,y;
  byte  coul;
  short offp;
  long  xdi,ydi,xic,yic,el,xp,yp;

  if(L==0)
    return;

  /* clipping haut et bas */
  if(y3<0 || y3>199)
    return;

  /* mapping */
  v1 = (((long)x2-(long)x1)<<LS)/(long)L;
  w1 = (((long)y2-(long)y1)<<LS)/(long)L;
  y  = (long)y1<<LS;
  x =  (long)x1<<LS;

  /* phong */
  if(xd == xf)
  {
    xdi = 0;
    ydi = 0;
  }
  else
  {
    xdi = ((long)(xif-xid)<<LS)/L;
    ydi = ((long)(yif-yid)<<LS)/L;
  }
  xic = (long)xid<<LS;
  yic = (long)yid<<LS;

  /* clipping droit */
  if(x3+L > 319)
     L = L-(x3+L-319);

  /* offset pixel a afficher */
  offp = y3*320+x3;

  for(l=0; l<L; l++)
  {
    y += w1;
    x += v1;
    if(x3>=0)  /* clipping gauche */
    {
      coul = ecr1[(y>>RS)*320+(x>>RS)];
      /* phong */
      xp = xic>>RS;
      yp = yic>>RS;
      /* dev. de :  el = ((yp+127)*255L)+(xp+127); */
      el = (yp<<8)-yp+xp+32512;
      ecr2[offp] = coul - 31 + (TPhongXY[el]>>3);
    }
    x3++;
    offp++;
    /* phong */
    xic += xdi;
    yic += ydi;
  }
}

void fillP3MapPhong_V2(long xa,long ya,long xb,long yb,long xc,long yc,unsigned char far * pagemap,unsigned char far * page)
{
  /* Trie des trois points de haut en bas */

  if(ya>yb)
  {
    SWAP(xa,xb)
    SWAP(ya,yb)
    /* mapping */
    SWAP(XAmap,XBmap)
    SWAP(YAmap,YBmap)
    /* phong */
    SWAP(RefXa,RefXb)
    SWAP(RefYa,RefYb)
  }

  if(ya>yc)
  {
    SWAP(xa,xc)
    SWAP(ya,yc)
    /* mapping */
    SWAP(XAmap,XCmap)
    SWAP(YAmap,YCmap)
    /* phong */
    SWAP(RefXa,RefXc)
    SWAP(RefYa,RefYc)
  }

  if(yb>yc)
  {
    SWAP(xb,xc)
    SWAP(yb,yc)
    /* mapping */
    SWAP(XBmap,XCmap)
    SWAP(YBmap,YCmap)
    /* phong */
    SWAP(RefXb,RefXc)
    SWAP(RefYb,RefYc)
  }


  /* Remplissage */

  if(ya!=yc)
  {
    pac = ya-yc;
    delta1 = ((xa-xc)<<LS)/pac;
    /* mapping */
    p1 = ((YAmap-YCmap)<<LS)/pac;
    dmap1 = ((XAmap-XCmap)<<LS)/pac;
    ydm = YAmap<<LS;
    yfm = ydm;
    xdm = XAmap<<LS;
    xfm = xdm;
    /* phong */
    dxi1 = ((RefXa-RefXc)<<LS)/pac;
    dyi1 = ((RefYa-RefYc)<<LS)/pac;
  }
  else
  {
    delta1 = 0;
    /* mapping */
    p1 = (YAmap-YCmap)<<LS;
    dmap1 = (XAmap-XCmap)<<LS;
    yfm = YCmap<<LS;
    ydm = YAmap<<LS;
    xfm = XCmap<<LS;
    xdm = XAmap<<LS;
    /* phong */
    dxi1 = 0;
    dyi1 = 0;
  }

  if(ya!=yb)
  {
    pab = yb-ya;
    delta2 = ((xb-xa)<<LS)/pab;
    /* mapping */
    p2 = ((YBmap-YAmap)<<LS)/pab;
    dmap2 = ((XBmap-XAmap)<<LS)/pab;
    ydm = YAmap<<LS;
    yfm = ydm;
    xdm = XAmap<<LS;
    xfm = xdm;
    /* phong */
    dxi2 = ((RefXb-RefXa)<<LS)/pab;
    dyi2 = ((RefYb-RefYa)<<LS)/pab;
  }
  else
  {
    delta2 = 0;
    /* mapping */
    p2 = (YBmap-YAmap)<<LS;
    dmap2 = (XBmap-XAmap)<<LS;
    yfm = YBmap<<LS;
    ydm = YAmap<<LS;
    xfm = XBmap<<LS;
    xdm = XAmap<<LS;
    /* phong */
    dxi2 = 0;
    dyi2 = 0;
  }

  xd = xa<<LS;
  xf = xd;

   /* phong */
  xideb = RefXa<<LS;
  xifin = xideb;
  yideb = RefYa<<LS;
  yifin = yideb;

  if(delta1<delta2)
  {
    for(l = ya+1; l <= yb; l++)
    {
       xd += delta1;
       xf += delta2;
       /* mapping */
       ydm += p1;
       xdm += dmap1;
       yfm += p2;
       xfm += dmap2;
       /* phong */
       xideb += dxi1;
       xifin += dxi2;
       yideb += dyi1;
       yifin += dyi2;

       lignemapp((short)(xdm>>RS),(short)(ydm>>RS),(short)(xfm>>RS),(short)(yfm>>RS),pagemap,(short)(xd>>RS),l,(short)(xf>>RS)-(short)(xd>>RS),page,(short)(xideb>>RS),(short)(yideb>>RS),(short)(xifin>>RS),(short)(yifin>>RS));
    }
  }
  else
  {
    for(l = ya+1; l <= yb; l++)
    {
       xd += delta1;
       xf += delta2;
       /* mapping */
       ydm += p1;
       xdm += dmap1;
       yfm += p2;
       xfm += dmap2;
       /* phong */
       xideb += dxi1;
       xifin += dxi2;
       yideb += dyi1;
       yifin += dyi2;

       lignemapp((short)(xfm>>RS),(short)(yfm>>RS),(short)(xdm>>RS),(short)(ydm>>RS),pagemap,(short)(xf>>RS),l,(short)(xd>>RS)-(short)(xf>>RS),page,(short)(xifin>>RS),(short)(yifin>>RS),(short)(xideb>>RS),(short)(yideb>>RS));
     }
  }

  if(yb!=yc)
  {
    pbc = yb-yc;
    delta2 = ((xb-xc)<<LS)/pbc;
    xf = xb<<LS;

    /* mapping */
    p2 = ((YBmap-YCmap)<<LS)/pbc;
    dmap2 = ((XBmap-XCmap)<<LS)/pbc;

    /* phong */
    dxi2 = ((RefXb-RefXc)<<LS)/pbc;
    dyi2 = ((RefYb-RefYc)<<LS)/pbc;
    xifin = RefXb<<LS;
    yifin = RefYb<<LS;

    if(delta2<delta1)
    {
       for(l = yb+1; l <= yc; l++)
       {
          xd += delta1;
         xf += delta2;
         /* mapping */
         ydm += p1;
         xdm += dmap1;
         yfm += p2;
         xfm += dmap2;
         /* phong */
         xideb += dxi1;
         xifin += dxi2;
         yideb += dyi1;
          yifin += dyi2;

         lignemapp((short)(xdm>>RS),(short)(ydm>>RS),(short)(xfm>>RS),(short)(yfm>>RS),pagemap,(short)(xd>>RS),l,(short)(xf>>RS)-(short)(xd>>RS),page,(short)(xideb>>RS),(short)(yideb>>RS),(short)(xifin>>RS),(short)(yifin>>RS));
       }
    }
    else
    {
       for(l = yb+1; l <= yc; l++)
       {
          xd += delta1;
         xf += delta2;
         /* mapping */
         ydm += p1;
         xdm += dmap1;
         yfm += p2;
         xfm += dmap2;
         /* phong */
         xideb += dxi1;
         xifin += dxi2;
         yideb += dyi1;
          yifin += dyi2;

         lignemapp((short)(xfm>>RS),(short)(yfm>>RS),(short)(xdm>>RS),(short)(ydm>>RS),pagemap,(short)(xf>>RS),l,(short)(xd>>RS)-(short)(xf>>RS),page,(short)(xifin>>RS),(short)(yifin>>RS),(short)(xideb>>RS),(short)(yideb>>RS));
       }
    }
  }
}

/*
 * Remplissage avec Z-BUFFER
 */

/* MAPPING + Z-BUFFER */

void lignemapz_C(short x1,short y1,short x2,short y2,unsigned char far * ecr1,short x3,short y3,short zd,short zf,short L,unsigned char far * ecr2)
{
  short l;
  long  v1,w1,x,y;
  byte  coul;
  long  zc,dz;
  short pz;

  /* ligne nulle a afficher */
  if(L<=0)
    return;

  /* clipping haut et bas */
  if(y3<0 || y3>199)
    return;

  /* mapping */
  v1 = (((long)x2-(long)x1)<<LS)/(long)L;
  w1 = (((long)y2-(long)y1)<<LS)/(long)L;
  y  = (long)y1<<LS;
  x =  (long)x1<<LS;

  /* z-buffer */
  if(L==0)
     dz=0;
  else
     dz = ((long)(zf-zd)<<LS)/L;
  zc = (long)zd<<LS;

  /* clipping droit */
  if(x3+L > 319)
     L = L-(x3+L-319);

  pz = y3*320+x3;

  for(l=0; l<L; l++)
  {
    y += w1; /* pixel(x,y) dans mapping */
    x += v1;
    if(x3>=0)  /* clipping gauche */
    {
      if((zc>>RS) > ZBuffer[pz])
      {
        coul = ecr1[(y>>RS)*320+(x>>RS)]; /* coul = pixel(x,y) dans mapping */
        ecr2[pz] = coul;
        ZBuffer[pz] = zc>>RS; /* z-buffer */
      }
    }
    x3++;   /* pixel dest */
    pz++;   /* pixel dest */
    zc+=dz; /* z-buffer */
  }
}

void fillPoly3MapZ_V2(long xa,long ya,long za,long xb,long yb,long zb,long xc,long yc,long zc,unsigned char far * pagemap,unsigned char far * page)
{
  /* Trie des trois points de haut en bas */

  if(ya>yb)
  {
    SWAP(xa,xb)
    SWAP(ya,yb)
    /* z-buffer */
    SWAP(za,zb)
    /* mapping */
    SWAP(XAmap,XBmap)
    SWAP(YAmap,YBmap)
  }

  if(ya>yc)
  {
    SWAP(xa,xc)
    SWAP(ya,yc)
    /* z-buffer */
    SWAP(za,zc)
    /* mapping */
    SWAP(XAmap,XCmap)
    SWAP(YAmap,YCmap)
  }

  if(yb>yc)
  {
    SWAP(xb,xc)
    SWAP(yb,yc)
    /* z-buffer */
    SWAP(zb,zc)
    /* mapping */
    SWAP(XBmap,XCmap)
    SWAP(YBmap,YCmap)
  }


  /* Remplissage */

  /* mapping */

  if(ya!=yc)
  {
    pac = ya-yc;
    delta1 = ((xa-xc)<<LS)/pac;
    /* mapping */
    p1 = ((YAmap-YCmap)<<LS)/pac;
    dmap1 = ((XAmap-XCmap)<<LS)/pac;
    ydm = YAmap<<LS;
    yfm = ydm;
    xdm = XAmap<<LS;
    xfm = xdm;
    /* z-buffer */
    dz1 = ((za-zc)<<LS)/pac;
  }
  else
  {
    delta1 = 0;
    /* mapping */
    p1 = (YAmap-YCmap)<<LS;
    dmap1 = (XAmap-XCmap)<<LS;
    yfm = YCmap<<LS;
    ydm = YAmap<<LS;
    xfm = XCmap<<LS;
    xdm = XAmap<<LS;
    /* z-buffer */
    dz1 = 0;
  }

  if(ya!=yb)
  {
    pab = yb-ya;
    delta2 = ((xb-xa)<<LS)/pab;
    /* mapping */
    p2 = ((YBmap-YAmap)<<LS)/pab;
    dmap2 = ((XBmap-XAmap)<<LS)/pab;
    ydm = YAmap<<LS;
    yfm = ydm;
    xdm = XAmap<<LS;
    xfm = xdm;
    /* z-buffer */
    dz2 = ((zb-za)<<LS)/pab;
  }
  else
  {
    delta2 = 0;
    /* mapping */
    p2 = (YBmap-YAmap)<<LS;
    dmap2 = (XBmap-XAmap)<<LS;
    yfm = YBmap<<LS;
    ydm = YAmap<<LS;
    xfm = XBmap<<LS;
    xdm = XAmap<<LS;
    /* z-buffer */
    dz2 = 0;
  }

  xd = xa<<LS;
  xf = xd;

  /* z-buffer */
  zd = za<<LS;
  zf = zd;

  if(delta1<delta2)
  {
    for(l = ya+1; l <= yb; l++)
    {
       ydm += p1;
       xdm += dmap1;
       yfm += p2;
       xfm += dmap2;
       zd += dz1;
       zf += dz2;
       xd += delta1;
       xf += delta2;
       lignemapz((short)(xdm>>RS),(short)(ydm>>RS),(short)(xfm>>RS),(short)(yfm>>RS),pagemap,(short)(xd>>RS),l,(short)(zd>>RS),(short)(zf>>RS),(short)(xf>>RS)-(short)(xd>>RS),page);
    }
  }
  else
  {
    for(l = ya+1; l <= yb; l++)
    {
       ydm += p1;
       xdm += dmap1;
       yfm += p2;
       xfm += dmap2;
       zd += dz1;
       zf += dz2;
       xd += delta1;
       xf += delta2;
       lignemapz((short)(xfm>>RS),(short)(yfm>>RS),(short)(xdm>>RS),(short)(ydm>>RS),pagemap,(short)(xf>>RS),l,(short)(zf>>RS),(short)(zd>>RS),(short)(xd>>RS)-(short)(xf>>RS),page);
    }
  }

  if(yb!=yc)
  {
    pbc = yb-yc;
    delta2 = ((xb-xc)<<LS)/pbc;
    xf = xb<<LS;

    /* mapping */
    p2 = ((YBmap-YCmap)<<LS)/pbc;
    dmap2 = ((XBmap-XCmap)<<LS)/pbc;

    /* z-buffer */
    dz2 = ((zb-zc)<<LS)/pbc;
    zf = zb<<LS;

    if(delta2<delta1)
    {
      for(l = yb+1; l <= yc; l++)
      {
         ydm += p1;
         xdm += dmap1;
         yfm += p2;
         xfm += dmap2;
         zd += dz1;
         zf += dz2;
         xd += delta1;
         xf += delta2;
         lignemapz((short)(xdm>>RS),(short)(ydm>>RS),(short)(xfm>>RS),(short)(yfm>>RS),pagemap,(short)(xd>>RS),l,(short)(zd>>RS),(short)(zf>>RS),(short)(xf>>RS)-(short)(xd>>RS),page);
      }
    }
    else
    {
      for(l = yb+1; l <= yc; l++)
      {
         ydm += p1;
         xdm += dmap1;
         yfm += p2;
         xfm += dmap2;
         zd += dz1;
         zf += dz2;
         xd += delta1;
         xf += delta2;
         lignemapz((short)(xfm>>RS),(short)(yfm>>RS),(short)(xdm>>RS),(short)(ydm>>RS),pagemap,(short)(xf>>RS),l,(short)(zf>>RS),(short)(zd>>RS),(short)(xd>>RS)-(short)(xf>>RS),page);
      }
    }
  }
}

/* MAPPING + FLAT-SHADING + Z-BUFFER */

void lignemapflatz_C(short x1,short y1,short x2,short y2,unsigned char far * ecr1,short x3,short y3,short zd,short zf,short L,short cosa,unsigned char far * ecr2)
{
  short l;
  long  v1,w1,x,y;
  byte  coul;
  long  zc,dz;
  short pz;
  byte e1;
  long nind;

  /* ligne nulle a afficher */
  if(L<=0)
    return;

  /* clipping haut et bas */
  if(y3<0 || y3>199)
    return;

  /* mapping */
  v1 = (((long)x2-(long)x1)<<LS)/(long)L;
  w1 = (((long)y2-(long)y1)<<LS)/(long)L;
  y  = (long)y1<<LS;
  x =  (long)x1<<LS;

  /* z-buffer */
  if(L==0)
     dz=0;
  else
     dz = ((long)(zf-zd)<<LS)/L;
  zc = (long)zd<<LS;

  /* clipping droit */
  if(x3+L > 319)
     L = L-(x3+L-319);

  pz = y3*320+x3;

  for(l=0; l<L; l++)
  {
    y += w1; /* pixel(x,y) dans mapping */
    x += v1;
    if(x3>=0) /* clipping gauche */
    {
      if((zc>>RS) > ZBuffer[pz])
      {
        coul = ecr1[(y>>RS)*320+(x>>RS)]; /* coul = pixel(x,y) dans mapping */

        /* formule generale :
         * Image de mapping pouvant contenir
         * n'importe qu'elle couleur dans les degrades
         */
        #if 0
        e1 = (coul>>RS_DEG)<<LS_DEG;
        nind = ((long)coul-(long)e1)*(long)cosa;
        nind += e1<<LS_DEG;
        ecr2[pz] = nind>>RS_DEG;
        #endif

        /* formule adaptee :
         * Seuls les derniers niveaux de chaque degrades sont utilises
         */
        ecr2[pz] = coul -15 + cosa; // 16 couleurs par degrade

        ZBuffer[pz] = zc>>RS; /* z-buffer */
      }
    }
    x3++;   /* pixel dest */
    pz++;   /* pixel dest */
    zc+=dz; /* z-buffer */
  }
}

void fillPoly3MapFlatZ_V2(long xa,long ya,long za,long xb,long yb,long zb,long xc,long yc,long zc,short cosa,unsigned char far * pagemap,unsigned char far * page)
{
  /* Trie des trois points de haut en bas */

  if(ya>yb)
  {
    SWAP(xa,xb)
    SWAP(ya,yb)
    /* z-buffer */
    SWAP(za,zb)
    /* mapping */
    SWAP(XAmap,XBmap)
    SWAP(YAmap,YBmap)
  }

  if(ya>yc)
  {
    SWAP(xa,xc)
    SWAP(ya,yc)
    /* z-buffer */
    SWAP(za,zc)
    /* mapping */
    SWAP(XAmap,XCmap)
    SWAP(YAmap,YCmap)
  }

  if(yb>yc)
  {
    SWAP(xb,xc)
    SWAP(yb,yc)
    /* z-buffer */
    SWAP(zb,zc)
    /* mapping */
    SWAP(XBmap,XCmap)
    SWAP(YBmap,YCmap)
  }


  /* Remplissage */

  /* mapping */

  if(ya!=yc)
  {
    pac = ya-yc;
    delta1 = ((xa-xc)<<LS)/pac;
    /* mapping */
    p1 = ((YAmap-YCmap)<<LS)/pac;
    dmap1 = ((XAmap-XCmap)<<LS)/pac;
    ydm = YAmap<<LS;
    yfm = ydm;
    xdm = XAmap<<LS;
    xfm = xdm;
    /* z-buffer */
    dz1 = ((za-zc)<<LS)/pac;
  }
  else
  {
    delta1 = 0;
    /* mapping */
    p1 = (YAmap-YCmap)<<LS;
    dmap1 = (XAmap-XCmap)<<LS;
    yfm = YCmap<<LS;
    ydm = YAmap<<LS;
    xfm = XCmap<<LS;
    xdm = XAmap<<LS;
    /* z-buffer */
    dz1 = 0;
  }

  if(ya!=yb)
  {
    pab = yb-ya;
    delta2 = ((xb-xa)<<LS)/pab;
    /* mapping */
    p2 = ((YBmap-YAmap)<<LS)/pab;
    dmap2 = ((XBmap-XAmap)<<LS)/pab;
    ydm = YAmap<<LS;
    yfm = ydm;
    xdm = XAmap<<LS;
    xfm = xdm;
    /* z-buffer */
    dz2 = ((zb-za)<<LS)/pab;
  }
  else
  {
    delta2 = 0;
    /* mapping */
    p2 = (YBmap-YAmap)<<LS;
    dmap2 = (XBmap-XAmap)<<LS;
    yfm = YBmap<<LS;
    ydm = YAmap<<LS;
    xfm = XBmap<<LS;
    xdm = XAmap<<LS;
    /* z-buffer */
    dz2 = 0;
  }

  xd = xa<<LS;
  xf = xd;

  /* z-buffer */
  zd = za<<LS;
  zf = zd;

  if(delta1<delta2)
  {
    for(l = ya+1; l <= yb; l++)
    {
       ydm += p1;
       xdm += dmap1;
       yfm += p2;
       xfm += dmap2;
       zd += dz1;
       zf += dz2;
       xd += delta1;
       xf += delta2;
       lignemapflatz((short)(xdm>>RS),(short)(ydm>>RS),(short)(xfm>>RS),(short)(yfm>>RS),pagemap,(short)(xd>>RS),l,(short)(zd>>RS),(short)(zf>>RS),(short)(xf>>RS)-(short)(xd>>RS),cosa,page);
    }
  }
  else
  {
    for(l = ya+1; l <= yb; l++)
    {
       ydm += p1;
       xdm += dmap1;
       yfm += p2;
       xfm += dmap2;
       zd += dz1;
       zf += dz2;
       xd += delta1;
       xf += delta2;
       lignemapflatz((short)(xfm>>RS),(short)(yfm>>RS),(short)(xdm>>RS),(short)(ydm>>RS),pagemap,(short)(xf>>RS),l,(short)(zf>>RS),(short)(zd>>RS),(short)(xd>>RS)-(short)(xf>>RS),cosa,page);
    }
  }

  if(yb!=yc)
  {
    pbc = yb-yc;
    delta2 = ((xb-xc)<<LS)/pbc;
    xf = xb<<LS;

    /* mapping */
    p2 = ((YBmap-YCmap)<<LS)/pbc;
    dmap2 = ((XBmap-XCmap)<<LS)/pbc;

    /* z-buffer */
    dz2 = ((zb-zc)<<LS)/pbc;
    zf = zb<<LS;

    if(delta2<delta1)
    {
      for(l = yb+1; l <= yc; l++)
      {
         ydm += p1;
         xdm += dmap1;
         yfm += p2;
         xfm += dmap2;
         zd += dz1;
         zf += dz2;
         xd += delta1;
         xf += delta2;
         lignemapflatz((short)(xdm>>RS),(short)(ydm>>RS),(short)(xfm>>RS),(short)(yfm>>RS),pagemap,(short)(xd>>RS),l,(short)(zd>>RS),(short)(zf>>RS),(short)(xf>>RS)-(short)(xd>>RS),cosa,page);
      }
    }
    else
    {
      for(l = yb+1; l <= yc; l++)
      {
         ydm += p1;
         xdm += dmap1;
         yfm += p2;
         xfm += dmap2;
         zd += dz1;
         zf += dz2;
         xd += delta1;
         xf += delta2;
         lignemapflatz((short)(xfm>>RS),(short)(yfm>>RS),(short)(xdm>>RS),(short)(ydm>>RS),pagemap,(short)(xf>>RS),l,(short)(zf>>RS),(short)(zd>>RS),(short)(xd>>RS)-(short)(xf>>RS),cosa,page);
      }
    }
  }
}

/* MAPPING + GOURAUD-SHADING + Z-BUFFER */

void lignemapgz_C(short x1,short y1,short x2,short y2,unsigned char far * ecr1,short x3,short y3,short L,short ideb,short ifin,short zd,short zf,unsigned char far * ecr2)
{
  short l;
  long  v1,w1,x,y;
  byte  coul,e1;
  long di,ic;
  short offp;

  if(L==0)
    return;

  /* clipping haut et bas */
  if(y3<0 || y3>199)
    return;

  /* mapping */
  v1 = (((long)x2-(long)x1)<<LS)/(long)L;
  w1 = (((long)y2-(long)y1)<<LS)/(long)L;
  y  = (long)y1<<LS;
  x =  (long)x1<<LS;

  /* gouraud */
  if(L == 0)
   di = 0;
  else
   di = ((long)(ifin-ideb)<<LS)/L;
  ic = (long)ideb<<LS;

  /* z-buffer */
  if(L==0)
     dz=0;
  else
     dz = ((long)(zf-zd)<<LS)/L;
  zc = (long)zd<<LS;

  /* clipping droit */
  if(x3+L > 319)
     L = L-(x3+L-319);

  /* offset pixel a afficher */
  offp = y3*320+x3;

  for(l=0; l<L; l++)
  {
    y += w1;
    x += v1;
    ic+=di;
    if(x3>=0) /* clipping gauche */
    {
      if((zc>>RS) > ZBuffer[offp])
      {

        /* formule adaptee :
         * Seuls les derniers niveaux
         * de chaque degrade sont utilises
         */
         coul = ecr1[(y>>RS)*320+(x>>RS)];
         ecr2[offp] = coul -31 + (ic>>RS);    // degrade de 32 couleurs
         ZBuffer[offp] = zc>>RS; /* z-buffer */
      }
    }
    x3++;
    offp++;
    zc+=dz; /* z-buffer */
  }
}

void fillP3MapGouraudZ_V2(long xa,long ya,long za,long ia,long xb,long yb,long zb,long ib,long xc,long yc,long zc,long ic,unsigned char far * pagemap,unsigned char far * page)
{
  /* Trie des trois points de haut en bas */

  if(ya>yb)
  {
    SWAP(xa,xb)
    SWAP(ya,yb)
    /* z-buffer */
    SWAP(za,zb)
    /* mapping */
    SWAP(XAmap,XBmap)
    SWAP(YAmap,YBmap)
    /* gouraud */
    SWAP(ia,ib)
  }

  if(ya>yc)
  {
    SWAP(xa,xc)
    SWAP(ya,yc)
    /* z-buffer */
    SWAP(za,zc)
    /* mapping */
    SWAP(XAmap,XCmap)
    SWAP(YAmap,YCmap)
    /* gouraud */
    SWAP(ia,ic)
  }

  if(yb>yc)
  {
    SWAP(xb,xc)
    SWAP(yb,yc)
    /* z-buffer */
    SWAP(zb,zc)
    /* mapping */
    SWAP(XBmap,XCmap)
    SWAP(YBmap,YCmap)
    /* gouraud */
    SWAP(ib,ic)
  }


  /* Remplissage */

  /* mapping */

  if(ya!=yc)
  {
    pac = ya-yc;
    delta1 = ((xa-xc)<<LS)/pac;
    /* mapping */
    p1 = ((YAmap-YCmap)<<LS)/pac;
    dmap1 = ((XAmap-XCmap)<<LS)/pac;
    ydm = YAmap<<LS;
    yfm = ydm;
    xdm = XAmap<<LS;
    xfm = xdm;
    /* gouraud */
    di1 = ((ia-ic)<<LS)/pac;
    /* z-buffer */
    dz1 = ((za-zc)<<LS)/pac;
  }
  else
  {
    delta1 = 0;
    /* mapping */
    p1 = (YAmap-YCmap)<<LS;
    dmap1 = (XAmap-XCmap)<<LS;
    yfm = YCmap<<LS;
    ydm = YAmap<<LS;
    xfm = XCmap<<LS;
    xdm = XAmap<<LS;
    /* gouraud */
    di1 = 0;
    /* z-buffer */
    dz1 = 0;
  }

  if(ya!=yb)
  {
    pab = yb-ya;
    delta2 = ((xb-xa)<<LS)/pab;
    /* mapping */
    p2 = ((YBmap-YAmap)<<LS)/pab;
    dmap2 = ((XBmap-XAmap)<<LS)/pab;
    ydm = YAmap<<LS;
    yfm = ydm;
    xdm = XAmap<<LS;
    xfm = xdm;
    /* gouraud*/
    di2 = ((ib-ia)<<LS)/pab;
    /* z-buffer */
    dz2 = ((zb-za)<<LS)/pab;
  }
  else
  {
    delta2 = 0;
    /* mapping */
    p2 = (YBmap-YAmap)<<LS;
    dmap2 = (XBmap-XAmap)<<LS;
    yfm = YBmap<<LS;
    ydm = YAmap<<LS;
    xfm = XBmap<<LS;
    xdm = XAmap<<LS;
    /* gouraud */
    di2 = 0;
    /* z-buffer */
    dz2 = 0;
  }

  xd = xa<<LS;
  xf = xd;

  /* gouraud */
  ideb = ia<<LS;
  ifin = ideb;

  /* z-buffer */
  zd = za<<LS;
  zf = zd;

  if(delta1<delta2)
  {
    for(l = ya+1; l <= yb; l++)
    {
       ydm += p1;
       xdm += dmap1;
       yfm += p2;
       xfm += dmap2;
       xd += delta1;
       xf += delta2;
       ideb += di1;
       ifin += di2;
       zd += dz1;
       zf += dz2;

       lignemapgz((short)(xdm>>RS),(short)(ydm>>RS),(short)(xfm>>RS),(short)(yfm>>RS),pagemap,(short)(xd>>RS),l,(short)(xf>>RS)-(short)(xd>>RS),(short)(ideb>>RS),(short)(ifin>>RS),(short)(zd>>RS),(short)(zf>>RS),page);
    }
  }
  else
  {
    for(l = ya+1; l <= yb; l++)
    {
       ydm += p1;
       xdm += dmap1;
       yfm += p2;
       xfm += dmap2;
       xd += delta1;
       xf += delta2;
       ideb += di1;
       ifin += di2;
       zd += dz1;
       zf += dz2;

       lignemapgz((short)(xfm>>RS),(short)(yfm>>RS),(short)(xdm>>RS),(short)(ydm>>RS),pagemap,(short)(xf>>RS),l,(short)(xd>>RS)-(short)(xf>>RS),(short)(ifin>>RS),(short)(ideb>>RS),(short)(zd>>RS),(short)(zf>>RS),page);
    }
  }

  if(yb!=yc)
  {
    pbc = yb-yc;
    delta2 = ((xb-xc)<<LS)/pbc;
    xf = xb<<LS;

    /* mapping */
    p2 = ((YBmap-YCmap)<<LS)/pbc;
    dmap2 = ((XBmap-XCmap)<<LS)/pbc;

    /* gouraud */
    di2 = ((ib-ic)<<LS)/pbc;
    ifin = ib<<LS;

    /* z-buffer */
    dz2 = ((zb-zc)<<LS)/pbc;
    zf = zb<<LS;

    if(delta2<delta1)
    {
      for(l = yb+1; l <= yc; l++)
      {
         ydm += p1;
         xdm += dmap1;
         yfm += p2;
         xfm += dmap2;
         xd += delta1;
         xf += delta2;
         ideb += di1;
         ifin += di2;
         zd += dz1;
         zf += dz2;

         lignemapgz((short)(xdm>>RS),(short)(ydm>>RS),(short)(xfm>>RS),(short)(yfm>>RS),pagemap,(short)(xd>>RS),l,(short)(xf>>RS)-(short)(xd>>RS),(short)(ideb>>RS),(short)(ifin>>RS),(short)(zd>>RS),(short)(zf>>RS),page);
      }
    }
    else
    {
      for(l = yb+1; l <= yc; l++)
      {
         ydm += p1;
         xdm += dmap1;
         yfm += p2;
         xfm += dmap2;
         xd += delta1;
         xf += delta2;
         ideb += di1;
         ifin += di2;
         zd += dz1;
         zf += dz2;

         lignemapgz((short)(xfm>>RS),(short)(yfm>>RS),(short)(xdm>>RS),(short)(ydm>>RS),pagemap,(short)(xf>>RS),l,(short)(xd>>RS)-(short)(xf>>RS),(short)(ifin>>RS),(short)(ideb>>RS),(short)(zd>>RS),(short)(zf>>RS),page);
      }
    }
  }
}

/* MAPPING + PHONG-SHADING + Z-BUFFER */


void lignemappz_C(short x1,short y1,short x2,short y2,unsigned char far * ecr1,short x3,short y3,short L,unsigned char far * ecr2,short zd,short zf,short xid,short yid,short xif,short yif)
{
  short l;
  long  v1,w1,x,y;
  byte  coul;
  short offp;
  long  xdi,ydi,xic,yic,el,xp,yp;

  if(L==0)
    return;

  /* clipping haut et bas */
  if(y3<0 || y3>199)
    return;

  /* mapping */
  v1 = (((long)x2-(long)x1)<<LS)/(long)L;
  w1 = (((long)y2-(long)y1)<<LS)/(long)L;
  y  = (long)y1<<LS;
  x =  (long)x1<<LS;

  /* phong */
  if(xd == xf)
  {
    xdi = 0;
    ydi = 0;
  }
  else
  {
    xdi = ((long)(xif-xid)<<LS)/L;
    ydi = ((long)(yif-yid)<<LS)/L;
  }
  xic = (long)xid<<LS;
  yic = (long)yid<<LS;

  /* z-buffer */
  if(L==0)
     dz=0;
  else
     dz = ((long)(zf-zd)<<LS)/L;
  zc = (long)zd<<LS;

  /* clipping droit */
  if(x3+L > 319)
     L = L-(x3+L-319);

  /* offset pixel a afficher */
  offp = y3*320+x3;

  for(l=0; l<L; l++)
  {
    y += w1;
    x += v1;
    if(x3>=0)  /* clipping gauche */
    {
      if((zc>>RS) > ZBuffer[offp])
      {
         coul = ecr1[(y>>RS)*320+(x>>RS)];
        /* phong */
        xp = xic>>RS;
        yp = yic>>RS;
        /* dev. de : el = ((yp+127)*255L)+(xp+127); */
        el = (yp<<8)-yp+xp+32512;
        ecr2[offp] = coul - 31 + (TPhongXY[el]>>3);
        /* z-buffer */
        ZBuffer[offp] = zc>>RS;
      }
    }
    x3++;
    offp++;
    /* phong */
    xic += xdi;
    yic += ydi;
    /* z-buffer */
    zc+=dz;
  }
}

void fillP3MapPhongZ_V2(long xa,long ya,long za,long xb,long yb,long zb,long xc,long yc,long zc,unsigned char far * pagemap,unsigned char far * page)
{
  /* Trie des trois points de haut en bas */

  if(ya>yb)
  {
    SWAP(xa,xb)
    SWAP(ya,yb)
    /* z-buffer */
    SWAP(za,zb)
    /* mapping */
    SWAP(XAmap,XBmap)
    SWAP(YAmap,YBmap)
    /* phong */
    SWAP(RefXa,RefXb)
    SWAP(RefYa,RefYb)
  }

  if(ya>yc)
  {
    SWAP(xa,xc)
    SWAP(ya,yc)
    /* z-buffer */
    SWAP(za,zc)
    /* mapping */
    SWAP(XAmap,XCmap)
    SWAP(YAmap,YCmap)
    /* phong */
    SWAP(RefXa,RefXc)
    SWAP(RefYa,RefYc)
  }

  if(yb>yc)
  {
    SWAP(xb,xc)
    SWAP(yb,yc)
    /* z-buffer */
    SWAP(zb,zc)
    /* mapping */
    SWAP(XBmap,XCmap)
    SWAP(YBmap,YCmap)
    /* phong */
    SWAP(RefXb,RefXc)
    SWAP(RefYb,RefYc)
  }


  /* Remplissage */

  if(ya!=yc)
  {
    pac = ya-yc;
    delta1 = ((xa-xc)<<LS)/pac;
    /* z-buffer */
    dz1 = ((za-zc)<<LS)/pac;
    /* mapping */
    p1 = ((YAmap-YCmap)<<LS)/pac;
    dmap1 = ((XAmap-XCmap)<<LS)/pac;
    ydm = YAmap<<LS;
    yfm = ydm;
    xdm = XAmap<<LS;
    xfm = xdm;
    /* phong */
    dxi1 = ((RefXa-RefXc)<<LS)/pac;
    dyi1 = ((RefYa-RefYc)<<LS)/pac;
  }
  else
  {
    delta1 = 0;
    /* z-buffer */
    dz1 = 0;
    /* mapping */
    p1 = (YAmap-YCmap)<<LS;
    dmap1 = (XAmap-XCmap)<<LS;
    yfm = YCmap<<LS;
    ydm = YAmap<<LS;
    xfm = XCmap<<LS;
    xdm = XAmap<<LS;
    /* phong */
    dxi1 = 0;
    dyi1 = 0;
  }

  if(ya!=yb)
  {
    pab = yb-ya;
    delta2 = ((xb-xa)<<LS)/pab;
    /* z-buffer */
    dz2 = ((zb-za)<<LS)/pab;
    /* mapping */
    p2 = ((YBmap-YAmap)<<LS)/pab;
    dmap2 = ((XBmap-XAmap)<<LS)/pab;
    ydm = YAmap<<LS;
    yfm = ydm;
    xdm = XAmap<<LS;
    xfm = xdm;
    /* phong */
    dxi2 = ((RefXb-RefXa)<<LS)/pab;
    dyi2 = ((RefYb-RefYa)<<LS)/pab;
  }
  else
  {
    delta2 = 0;
    /* z-buffer */
    dz2 = 0;
    /* mapping */
    p2 = (YBmap-YAmap)<<LS;
    dmap2 = (XBmap-XAmap)<<LS;
    yfm = YBmap<<LS;
    ydm = YAmap<<LS;
    xfm = XBmap<<LS;
    xdm = XAmap<<LS;
    /* phong */
    dxi2 = 0;
    dyi2 = 0;
  }

  xd = xa<<LS;
  xf = xd;

  /* z-buffer */
  zd = za<<LS;
  zf = zd;

   /* phong */
  xideb = RefXa<<LS;
  xifin = xideb;
  yideb = RefYa<<LS;
  yifin = yideb;

  if(delta1<delta2)
  {
    for(l = ya+1; l <= yb; l++)
    {
       xd += delta1;
       xf += delta2;
       /* z-buffer */
       zd += dz1;
       zf += dz2;
       /* mapping */
       ydm += p1;
       xdm += dmap1;
       yfm += p2;
       xfm += dmap2;
       /* phong */
       xideb += dxi1;
       xifin += dxi2;
       yideb += dyi1;
       yifin += dyi2;

       lignemappz((short)(xdm>>RS),(short)(ydm>>RS),(short)(xfm>>RS),(short)(yfm>>RS),pagemap,(short)(xd>>RS),l,(short)(xf>>RS)-(short)(xd>>RS),page,(short)(zd>>RS),(short)(zf>>RS),(short)(xideb>>RS),(short)(yideb>>RS),(short)(xifin>>RS),(short)(yifin>>RS));
    }
  }
  else
  {
    for(l = ya+1; l <= yb; l++)
    {
       xd += delta1;
       xf += delta2;
       /* z-buffer */
       zd += dz1;
       zf += dz2;
       /* mapping */
       ydm += p1;
       xdm += dmap1;
       yfm += p2;
       xfm += dmap2;
       /* phong */
       xideb += dxi1;
       xifin += dxi2;
       yideb += dyi1;
       yifin += dyi2;

       lignemappz((short)(xfm>>RS),(short)(yfm>>RS),(short)(xdm>>RS),(short)(ydm>>RS),pagemap,(short)(xf>>RS),l,(short)(xd>>RS)-(short)(xf>>RS),page,(short)(zf>>RS),(short)(zd>>RS),(short)(xifin>>RS),(short)(yifin>>RS),(short)(xideb>>RS),(short)(yideb>>RS));
     }
  }

  if(yb!=yc)
  {
    pbc = yb-yc;
    delta2 = ((xb-xc)<<LS)/pbc;
    xf = xb<<LS;

    /* z-buffer */
    dz2 = ((zb-zc)<<LS)/pbc;
    zf = zb<<LS;

    /* mapping */
    p2 = ((YBmap-YCmap)<<LS)/pbc;
    dmap2 = ((XBmap-XCmap)<<LS)/pbc;

    /* phong */
    dxi2 = ((RefXb-RefXc)<<LS)/pbc;
    dyi2 = ((RefYb-RefYc)<<LS)/pbc;
    xifin = RefXb<<LS;
    yifin = RefYb<<LS;

    if(delta2<delta1)
    {
       for(l = yb+1; l <= yc; l++)
       {
          xd += delta1;
         xf += delta2;
         /* z-buffer */
         zd += dz1;
         zf += dz2;
         /* mapping */
         ydm += p1;
         xdm += dmap1;
         yfm += p2;
         xfm += dmap2;
         /* phong */
         xideb += dxi1;
         xifin += dxi2;
         yideb += dyi1;
          yifin += dyi2;

         lignemappz((short)(xdm>>RS),(short)(ydm>>RS),(short)(xfm>>RS),(short)(yfm>>RS),pagemap,(short)(xd>>RS),l,(short)(xf>>RS)-(short)(xd>>RS),page,(short)(zd>>RS),(short)(zf>>RS),(short)(xideb>>RS),(short)(yideb>>RS),(short)(xifin>>RS),(short)(yifin>>RS));
       }
    }
    else
    {
       for(l = yb+1; l <= yc; l++)
       {
          xd += delta1;
         xf += delta2;
         /* z-buffer */
         zd += dz1;
         zf += dz2;
         /* mapping */
         ydm += p1;
         xdm += dmap1;
         yfm += p2;
         xfm += dmap2;
         /* phong */
         xideb += dxi1;
         xifin += dxi2;
         yideb += dyi1;
         yifin += dyi2;

         lignemappz((short)(xfm>>RS),(short)(yfm>>RS),(short)(xdm>>RS),(short)(ydm>>RS),pagemap,(short)(xf>>RS),l,(short)(xd>>RS)-(short)(xf>>RS),page,(short)(zf>>RS),(short)(zd>>RS),(short)(xifin>>RS),(short)(yifin>>RS),(short)(xideb>>RS),(short)(yideb>>RS));
       }
    }
  }
}
