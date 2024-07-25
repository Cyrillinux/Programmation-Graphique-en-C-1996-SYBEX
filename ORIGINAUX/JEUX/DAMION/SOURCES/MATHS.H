#define COEFM 256     /* multiplicateur des flottants (puissance de 2) */
#define TYPESC long  /* type des valeurs rendues par sinus et cosinus */

void  initSinCos(void);

TYPESC sinq(short angle);

TYPESC cosq(short angle);

