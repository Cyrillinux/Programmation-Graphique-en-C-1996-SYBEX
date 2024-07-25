void fillPoly3(unsigned long xac,unsigned long yac,unsigned long xbc,unsigned long ybc,unsigned long xcc,unsigned long ycc,unsigned char far * page,unsigned char coul);
/*
	 IN :
	 - coordonnes A,B,C (sens trigo.) dans le repere ecran
	 - page = page d'affichage
	 - coul = couleur de remplissage

	 OUT :
	 - remplissage du triangle
 */


void fillRect(unsigned long xac,unsigned long yac,unsigned long xbc,unsigned long ybc,unsigned long xcc,unsigned long ycc,unsigned long xdc,unsigned long ydc,unsigned char far * page,unsigned char coul);
/*
	IN  :
	- cordonnees A,B,C,D du rectangle (sens trigo.) dans le repere ecran
	- page = page d'affichage
	- coul = couleur de remplissage

	OUT :
	- remplissage du rectangle
 */
