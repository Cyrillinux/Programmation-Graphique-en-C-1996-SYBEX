/*******************************************************************/
/* Routines mathematiques :
/*
/* -  Tables precalculees de sinus et de cosinus
/*******************************************************************/

#include "maths.h"

static long  TSIN[512];    /* 512 == precision maximal */
static long  TCOS[512];

void initSinCos256(void)
{
  float angle = 0.;
  float pas = 360./256.;   /* pas ~= 1.4 degree */
  short i=0;

  for(i=0; i<256; i++)
  {
    TSIN[i]=sin(DEG2RAD(angle))*FLOAT_MULT;
    TCOS[i]=cos(DEG2RAD(angle))*FLOAT_MULT;
    angle+=pas;
  }
}

void initSinCos360(void)
{
  short angle;

  for(angle=0; angle<360; angle++) /* pas == 1 degree */
  {
    TSIN[angle]=sin(DEG2RAD(angle))*FLOAT_MULT;
    TCOS[angle]=cos(DEG2RAD(angle))*FLOAT_MULT;
  }
}

void initSinCos512(void)
{
  float angle = 0;
  float pas = 360./512.;  /* pas ~= 0.7 degree */
  short i=0;

  for(i=0; i<512; i++)
  {
    TSIN[i]=sin(DEG2RAD(angle))*FLOAT_MULT;
    TCOS[i]=cos(DEG2RAD(angle))*FLOAT_MULT;
    angle+=pas;
  }
}

/* sinus/cosinus 256 */

long sin256(short angle)
{
 if(angle>=0)
    return TSIN[angle&255];
 else
    return -TSIN[-angle&255];
}

long cos256(short angle)
{
 if(angle>=0)
    return TCOS[angle&255];
 else
    return TCOS[-angle&255];
}

/* sinus/cosinus 360 */

long sin360(short angle)
{
 if(angle>=0)
    return TSIN[angle%360];
 else
    return -TSIN[-angle%360];
}

long cos360(short angle)
{
 if(angle>=0)
    return TCOS[angle%360];
 else
    return TCOS[-angle%360];
}

/* sinus/cosinus 512 */

long sin512(short angle)
{
 if(angle>=0)
    return TSIN[angle&511];
 else
    return -TSIN[-angle&511];
}

long cos512(short angle)
{
 if(angle>=0)
    return TCOS[angle&511];
 else
    return TCOS[-angle&511];
}
