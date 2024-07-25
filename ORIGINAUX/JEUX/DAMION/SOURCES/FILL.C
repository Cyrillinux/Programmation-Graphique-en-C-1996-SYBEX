#include <vga.h>

void fillPoly3(unsigned long xac,unsigned long yac,unsigned long xbc,unsigned long ybc,unsigned long xcc,unsigned long ycc,unsigned char far * page,byte coul);
/*
	 IN :
	 - coordonnes A,B,C (sens trigo.) dans le repere ecran
	 - page = page d'affichage
	 - coul = couleur de remplissage
 */

void fillPoly3(unsigned long xac,unsigned long yac,unsigned long xbc,unsigned long ybc,unsigned long xcc,unsigned long ycc,unsigned char far * page,byte coul)
{
	unsigned long xi,yi,xd,xf;
	long dx1,dy1,dx2,dy2;
	long delta1,delta2;
	unsigned short l;

		// 0

		if(yac<ybc && yac<ycc && ybc>=ycc)
		{
			dx1 = (xbc-xac)<<8;
			dy1 = ybc-yac;
			dx2 = (xcc-xac)<<8;
			dy2 = ycc-yac;
			delta1 = dx1/dy1;
			delta2 = dx2/dy2;
//    normalement, en plus :  put_pixel(xac,yac,page,coul);
			xd = xac<<8;
			xf = xac<<8;
			for(l = yac+1; l <= ycc; l++)
			{
					xd += delta1;
					xf += delta2;
					if(xd<xf)
						ligneh((unsigned short)(xd>>8),(unsigned short)(xf>>8),l,page,coul);
			}

			if(ybc!=ycc)
			{
				xi = xd>>8;
				yi = ycc;

				dx1 = (xbc-xi)<<8;
				dy1 = ybc-yi;
				dx2 = (xbc-xcc)<<8;
				dy2 = ybc-ycc;
				delta1 = dx1/dy1;
				delta2 = dx2/dy2;
//      normalement, en plus :  put_pixel(xi,yi,page,coul);
				xd = xi<<8;
				xf = xcc<<8;
				for(l = yi+1; l <= ybc; l++)
				{
					xd+=delta1;
					xf+=delta2;
					if(xd<xf)
						ligneh((unsigned short)(xd>>8),(unsigned short)(xf>>8),l,page,coul);
				}
			}

			goto finr;
		}

		// 1

		if(yac<ybc && yac>=ycc && ybc>ycc)
		{
			if(yac!=ycc)
			{
				dx1 = (xac-xcc)<<8;
				dy1 = yac-ycc;
				dx2 = (xbc-xcc)<<8;
				dy2 = ybc-ycc;
				delta1 = dx1/dy1;
				delta2 = dx2/dy2;
//      normalement, en plus :  put_pixel(xcc,ycc,page,coul);
				xd = xcc<<8;
				xf = xcc<<8;
				for(l = ycc+1 ; l <= yac; l++)
				{
					xd+=delta1;
					xf+=delta2;
					if(xd<xf)
						ligneh((unsigned short)(xd>>8),(unsigned short)(xf>>8),l,page,coul);
				}
			}
			if(yac!=ycc)
				xi = xf>>8;
			else
				xi = xcc;
			yi = yac;

			dx1 = (xbc-xac)<<8;
			dy1 = ybc-yac;
			dx2 = (xbc-xi)<<8;
			dy2 = ybc-yi;
			delta1 = dx1/dy1;
			delta2 = dx2/dy2;
//    normalement, en plus :  put_pixel(xac,yac,page,coul);
			xd = xac<<8;
			xf = xi<<8;
			for(l = yi+1; l <= ybc; l++)
			{
				xd += delta1;
				xf += delta2;
				if(xd<xf)
					ligneh((unsigned short)(xd>>8),(unsigned short)(xf>>8),l,page,coul);
			}
			goto finr;
		}

		// 2

		if(yac>=ybc && yac>ycc && ybc>ycc)
		{
			dx1 = (xac-xcc)<<8;
			dy1 = yac-ycc;
			dx2 = (xbc-xcc)<<8;
			dy2 = ybc-ycc;
			delta1 = dx1/dy1;
			delta2 = dx2/dy2;
//    normalement, en plus :  put_pixel(xcc,ycc,page,coul);
			xd = xcc<<8;
			xf = xcc<<8;
			for(l = ycc+1; l <= ybc; l++)
			{
				xd+=delta1;
				xf+=delta2;
				if(xd<xf)
					ligneh((unsigned short)(xd>>8),(unsigned short)(xf>>8),l,page,coul);
			}

			if(yac!=ybc)
			{
			xi = xd>>8;
			yi = ybc;

			dx1 = (xac-xi)<<8;
			dy1 = yac-yi;
			dx2 = (xac-xbc)<<8;
			dy2 = yac-ybc;
			delta1 = dx1/dy1;
			delta2 = dx2/dy2;
//    normalement, en plus :  put_pixel(xi,yi,page,coul);
			xd = xi<<8;
			xf = xbc<<8;
			for(l = yi+1; l <= yac; l++)
			{
				xd+=delta1;
				xf+=delta2;
				if(xd<xf)
					ligneh((unsigned short)(xd>>8),(unsigned short)(xf>>8),l,page,coul);
			}
			}
			goto finr;
		}

		// 3

		if(yac>ybc && ybc<=ycc && yac>ycc)
		{
			if(ybc!=ycc)
			{
				dx1 = (xcc-xbc)<<8;
				dy1 = ycc-ybc;
				dx2 = (xac-xbc)<<8;
				dy2 = yac-ybc;
				delta1 = dx1/dy1;
				delta2 = dx2/dy2;
//      normalement, en plus :  put_pixel(xbc,ybc,page,coul);
				xd = xbc<<8;
				xf = xbc<<8;
				for(l = ybc+1 ; l <= ycc; l++)
				{
					xd+=delta1;
					xf+=delta2;
					if(xd<xf)
						ligneh((unsigned short)(xd>>8),(unsigned short)(xf>>8),l,page,coul);
				}
			}
			if(ybc!=ycc)
				xi = xf>>8;
			else
				xi = xbc;
			yi = ycc;
			dx1 = (xac-xcc)<<8;
			dy1 = yac-ycc;
			dx2 = (xac-xi)<<8;
			dy2 = yac-yi;
			delta1 = dx1/dy1;
			delta2 = dx2/dy2;
//  normalement, en plus :    put_pixel(xcc,ycc,page,coul);
			xd = xcc<<8;
			xf = xi<<8;
			for(l = yi+1 ; l <= yac; l++)
			{
				 xd+=delta1;
				 xf+=delta2;
				 if(xd<xf)
					ligneh((unsigned short)(xd>>8),(unsigned short)(xf>>8),l,page,coul);
			}
			goto finr;
		}

		// 4

		if(yac>ybc && ybc<ycc && yac<=ycc)
		{
			dx1 = (xcc-xbc)<<8;
			dy1 = ycc-ybc;
			dx2 = (xac-xbc)<<8;
			dy2 = yac-ybc;
			delta1 = dx1/dy1;
			delta2 = dx2/dy2;
//  normalement, en plus :    put_pixel(xbc,ybc,page,coul);
			xd = xbc<<8;
			xf = xbc<<8;
			for(l = ybc+1; l <= yac; l++)
			{
				xd+=delta1;
				xf+=delta2;
				if(xd<xf)
					ligneh((unsigned short)(xd>>8),(unsigned short)(xf>>8),l,page,coul);
			}
			if(yac!=ycc)
			{
				xi = xd>>8;
				yi = yac;

				dx1 = (xcc-xi)<<8;
				dy1 = ycc-yi;
				dx2 = (xcc-xac)<<8;
				dy2 = ycc-yac;
				delta1 = dx1/dy1;
				delta2 = dx2/dy2;
//      normalement, en plus :    put_pixel(xi,yi,page,coul);
				xd = xi<<8;
				xf = xac<<8;
				for(l = yi+1 ; l <= ycc; l++)
				{
					 xd += delta1;
					 xf+=delta2;
					 if(xd<xf)
						ligneh((unsigned short)(xd>>8),(unsigned short)(xf>>8),l,page,coul);
				}
			}
			goto finr;
		}

		// 5

		if(yac<=ybc && ybc<ycc && yac<ycc)
		{
			if(yac!=ybc)
			{
				dx1 = (xbc-xac)<<8;
				dy1 = ybc-yac;
				dx2 = (xcc-xac)<<8;
				dy2 = ycc-yac;
				delta1 = dx1/dy1;
				delta2 = dx2/dy2;
//      normalement, en plus :  put_pixel(xac,yac,page,coul);
				xd = xac<<8;
				xf = xac<<8;
				for(l = yac+1; l <= ybc; l++)
				{
					xd+=delta1;
					xf+=delta2;
					if(xd<xf)
						ligneh((unsigned short)(xd>>8),(unsigned short)(xf>>8),l,page,coul);
				}
			}
			if(yac!=ybc)
				xi = xf>>8;
			else
				xi = xac;
			yi = ybc;

			dx1 = (xcc-xbc)<<8;
			dy1 = ycc-ybc;
			dx2 = (xcc-xi)<<8;
			dy2 = ycc-yi;
			delta1 = dx1/dy1;
			delta2 = dx2/dy2;
//    normalement, en plus :  put_pixel(xbc,ybc,page,coul);
			xd = xbc<<8;
			xf = xi<<8;
			for(l = yi+1 ; l <= ycc; l++)
			{
				xd+=delta1;
				xf+=delta2;
				if(xd<xf)
					ligneh((unsigned short)(xd>>8),(unsigned short)(xf>>8),l,page,coul);
			}
			goto finr;
		}
	 // fin remplissage
finr:
}


void fillRect(unsigned long xac,unsigned long yac,unsigned long xbc,unsigned long ybc,unsigned long xcc,unsigned long ycc,unsigned long xdc,unsigned long ydc,unsigned char far * page,byte coul)
/*
	IN  :
	- cordonnees A,B,C,D du rectangle (sens trigo.) dans le repere ecran
	- page = page d'affichage
	- coul = couleur de remplissage

	OUT :
	- remplissage du rectangle
 */
{
	fillPoly3(xac,yac,xbc,ybc,xcc,ycc,page,coul);
	fillPoly3(xac,yac,xcc,ycc,xdc,ydc,page,coul);
}
