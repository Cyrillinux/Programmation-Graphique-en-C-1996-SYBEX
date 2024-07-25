
/* ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
   ³                                WISPAL                                  ³
   ³                        Siracusa walter\wiSdom                          ³
   ³                              (c)1995/96                                ³
   ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*/

//Projet : WPAL.C, VGA.C, VGA_C.C

#include <vga.h>
#include <conio.h>
#include <ctype.h>
#include "wpal.h"

byte MtxFnt[256][8][8]; 

bytef PalD[768]={0,0,0,4,4,4,8,8,8,12,12,12,16,16,16,20,20,20,25,25,25,29,29,
29,33,33,33,37,37,37,41,41,41,46,46,46,50,50,50,54,54,54,58,58,58,63,63,63,13,
0,0,16,0,0,19,0,0,22,0,1,26,0,1,29,0,1,32,0,2,36,0,2,39,0,2,42,0,3,46,0,3,49,
0,3,52,0,4,56,0,4,59,0,4,63,0,5,10,0,0,13,1,1,16,3,3,19,5,5,23,7,7,26,8,8,29,
10,10,33,12,12,36,14,14,39,16,16,43,17,17,46,19,19,49,21,21,53,23,23,56,25,25,
60,27,27,0,0,10,1,1,13,2,2,17,3,3,20,4,4,24,5,5,27,6,6,31,7,7,34,8,8,38,9,9,
41,10,10,45,11,11,48,12,12,52,13,13,55,14,14,59,15,15,63,2,2,25,4,4,27,7,7,30,
10,10,32,13,13,35,16,16,37,19,19,40,22,22,42,25,25,45,28,28,47,31,31,50,34,34,
52,37,37,55,40,40,57,43,43,60,46,46,63,10,0,19,13,3,21,16,6,24,20,9,27,23,12,
30,27,15,33,30,18,36,33,21,39,37,25,42,40,28,45,44,31,48,47,34,51,50,37,54,54,
40,57,57,43,60,61,47,63,13,1,13,16,1,16,19,2,19,22,3,22,26,4,26,29,5,29,32,6,
32,36,7,36,39,8,39,42,9,42,46,10,46,49,11,49,52,12,52,56,13,56,59,14,59,63,15,63,10,0,0,13,1,
3,17,2,6,20,3,9,24,4,12,27,5,15,31,6,18,34,7,21,38,8,24,41,9,27,45,10,30,48,
11,33,52,12,36,55,13,39,59,14,42,63,15,45,15,0,0,18,3,3,21,7,7,24,11,10,27,15,
14,31,19,17,34,23,21,37,27,24,40,30,28,43,34,31,47,38,35,50,42,38,53,46,42,56,
50,45,59,54,49,63,58,53,10,0,0,13,4,0,17,8,0,20,12,0,24,16,0,27,20,0,31,25,0,
34,29,0,38,33,0,41,37,0,45,41,0,48,46,0,52,50,0,55,54,0,59,58,0,63,63,0,10,0,
0,12,2,1,15,5,3,18,8,5,21,10,7,24,13,9,27,16,11,30,18,13,33,21,14,36,24,16,39,
26,18,42,29,20,45,32,22,48,34,24,51,37,26,54,40,28,15,0,0,18,3,2,21,6,4,24,9,
6,27,13,9,31,16,11,34,19,13,37,23,15,40,26,18,43,29,20,47,33,22,50,36,24,53,
39,27,56,43,29,59,46,31,63,50,34,16,2,0,19,6,2,22,10,4,25,14,6,28,18,9,31,22,
11,34,26,13,37,30,16,41,34,18,44,38,20,47,42,23,50,46,25,53,50,27,56,54,30,59,
58,32,63,63,35,0,10,0,2,13,2,4,17,4,6,20,6,9,24,9,11,27,11,13,31,13,16,34,16,
18,38,18,20,41,20,23,45,23,25,48,25,27,52,27,30,55,30,32,59,32,35,63,35,0,10,
0,0,13,0,0,17,0,0,20,0,0,24,0,0,27,0,0,31,0,0,34,0,0,38,0,0,41,0,0,45,0,0,48,
0,0,52,0,0,55,0,0,59,0,0,63,0,0,10,10,3,13,13,6,17,17,9,20,20,13,24,24,16,27,
27,19,31,31,23,34,34,26,38,38,29,41,41,33,45,45,36,48,48,39,52,52,43,55,55,46,
59,59,50,63,63};    
bytef Pal[768];     
bytef PalF[768];    
bytef Buf[1000];
byte huge Page1[TAILLE_IMG_MAX];

bytef *Video=MK_FP(0xa000,0);

typedef struct _wispal
{
  dword TailleImg;    //Dimension de l'image
  long  LgImg,HtImg,  //Largeur et hauteur de l'image
        XwMax,YwMax,  //Position maximale de Xw et Yw
        Xw,Yw;        //Position courante
  byte  Prct,         //Pourcentage courant
        Img,Pal,      //Booleen de l'image et de la palette
        Load;         //Booleen du chargement
  char  Fch[25];      //Dernier fichier chargee
}t_wispal;

t_wispal Wis;

/*--------------------------------------------------------------------------*/

void SetPalette(void)
{
  word i;

  for(i=0;i<768;i++) PalF[i]=Pal[i];
  SetAllPal(Pal);
}

void CpyMemVid(byte huge *src,bytef *dst,word x,word y)
{
  register word i,j;
  byte huge *s;

  src+=y*Wis.LgImg+x;

  for(j=0;j<200;j++)
  {
    s=src;
    for(i=0;i<320;i++) *(dst++)=*(s++);
    src+=Wis.LgImg;
  }
}

void LoadFnt(char *path)
{
  int h;

  sprintf(Buf,"%s.fnt",path);
  h=open(Buf,O_RDONLY);
  read(h,&MtxFnt,256*8*8);
  close(h);
}

void PrintTxt(int pos_x,int pos_y,char *chaine,byte coul_t,char coul_f,bytef *scr)
{
  int x=pos_x;
  byte i0=0,i1,i2,c;

  i0=0;
  x=pos_x;
  while((c=chaine[i0])!='\0')
  {
    for(i1=0;i1<8;i1++)
      for(i2=0;i2<8;i2++)
      {
        if(MtxFnt[c][i1][i2]==1)
          scr[(pos_y+i2)*320+x+i1]=coul_t;
        else if(coul_f>=0)
          scr[(pos_y+i2)*320+x+i1]=coul_f;
      }
    i0++;
    x+=PAS_TXT;
  }
}

char ScanTxt(int x,int y,char *chaine,int ct,int cf,int l,byte t,bytef *scr)
{
   byte buf[2],i=0,bt;
   int key;

   buf[1]='\0';
   do
   {
     bt=0;
     do
     {
       key=getch();
       if(!key) getch();
     }while(!key);
     key=toupper(key);

     switch(t)
     {
       case 0: if(isalpha(key)) bt=1;break;  
       case 1: if(isdigit(key)) bt=1;break;  
       case 2: if(isprint(key)) bt=1;break;  
     }
     if(bt && i<l)
     {
       chaine[i]=key;
       buf[0]=key;
       PrintTxt(x+(i++)*PAS_TXT,y,buf,ct,cf,scr);
     }
     else
       if(key==DEL && i>0)
       {
         i--;
         FBox(x+i*PAS_TXT,y,PAS_TXT-1,PAS_TXT-1,Video,cf);
       }
   }while(key!=RET && key!=ESC);
   chaine[i]='\0';
   if(key==RET) return 1;
   else return 0;
}

void KeyTxt(int x,int y,char *txt,byte c_t,char c_f,bytef *scr)
{
  byte i;

  PrintTxt(x,y,txt,c_t,c_f,scr);
  ReadKey(0);
  for(i=0;txt[i];i++) PrintTxt(x+i*PAS_TXT,y," ",c_t,c_f,scr);
}

void InitScreen(void)
{
  ClrScr(Video);
  Box(0,0,320,14,Video,255);    
  Box(18,18,163,163,Video,255); 
  Box(0,186,320,14,Video,255);  
  PrintTxt(10,3," - WISPAL -  (C) WISDOM SOFTWARE",255,0,Video);
}

byte LoadPCXMem(char *path,byte huge *scr,byte *pal)
{
  int canal;
  byte octet;
  word i,rep,cpt_buf=0;
  dword dim;

  sprintf(Buf,"%s.pcx",path); 
  canal=open(Buf,O_RDONLY|O_BINARY); 

  if(canal!=-1)
  {
    
    lseek(canal,8,SEEK_SET); 
    read(canal,&Wis.LgImg,2);  
    Wis.LgImg++;
    read(canal,&Wis.HtImg,2); 
    Wis.HtImg++;
    Wis.TailleImg=Wis.LgImg*Wis.HtImg; 
    dim=Wis.TailleImg;

    if(Wis.LgImg<320 || Wis.HtImg<200 || Wis.TailleImg>TAILLE_IMG_MAX)
    {
      close(canal);
      FBox(1,187,318,12,Video,0);
      KeyTxt(2,189,"ERREUR !",255,0,Video);
      InitWisPal();
      return 0; 
    }
    
    Wis.XwMax=((Wis.LgImg-320)/PAS_SCR)*PAS_SCR;
    Wis.YwMax=((Wis.HtImg-200)/PAS_SCR)*PAS_SCR;
    lseek(canal,128,SEEK_SET); 
    read(canal,Buf,BUF_PCX); 

    while(dim)
    {
      octet=Buf[cpt_buf++]; 
      read_buf_pcx(canal,Buf,&cpt_buf); 

      if(octet>191) 
      {
        rep=octet-192; 
        octet=Buf[cpt_buf++]; 
        read_buf_pcx(canal,Buf,&cpt_buf); 

        for(i=0;i<rep;i++) 
        {
          *(scr++)=octet; 
          dim--;
        }
      }
      else          
      {
        *(scr++)=octet; 
        dim--;
      }
    }
    
    lseek(canal,-768L,SEEK_END);
    for(i=0;i<768;i++)
    {
      read(canal,&pal[i],1);
      pal[i]>>=2;
    }
    close(canal);
    return 1;
  }
  close(canal);
  return 0;
}

byte SaveMemPCX(char *path,byte huge *scr,byte *pal)
{
  int canal;
  byte octet;
  short entier;
  dword dim=Wis.TailleImg;
  word i,ligne,rep,cpt_buf=0;

  sprintf(Buf,"%s.pcx",path); 
  _fmode=O_BINARY;
  canal=creat(Buf,S_IREAD|S_IWRITE); 

  if(canal!=-1)
  {
    
    octet=10;write(canal,&octet,1);
    octet=5;write(canal,&octet,1);
    octet=1;write(canal,&octet,1);
    octet=8;write(canal,&octet,1);
    entier=0;write(canal,&entier,2);    
    entier=0;write(canal,&entier,2);    
    entier=Wis.LgImg-1;write(canal,&entier,2);  
    entier=Wis.HtImg-1;write(canal,&entier,2);  
    entier=Wis.LgImg;write(canal,&entier,2);  
    entier=Wis.HtImg;write(canal,&entier,2);  
    entier=0;for(i=0;i<48/2;i++) write(canal,&entier,2);
    octet=0;write(canal,&octet,1);
    octet=1;write(canal,&octet,1);
    entier=Wis.LgImg;write(canal,&entier,2);  
    entier=1;write(canal,&entier,2);
    entier=0;for(i=0;i<58/2;i++) write(canal,&entier,2);
    
    octet=*scr;
    rep=0;
    ligne=0;

    while(dim)
    {
      do
      {
        scr++;   
        rep++;   
        ligne++; 
        dim--;
      }while(*scr==octet && rep<63 && ligne<Wis.LgImg && dim);

      if(ligne==Wis.LgImg) ligne=0;

      if(rep==1)
      {
        if(octet<192)
        {
          Buf[cpt_buf++]=octet; 
          write_buf_pcx(canal,Buf,&cpt_buf);
        }
        else
        {
          Buf[cpt_buf++]=193; 
          write_buf_pcx(canal,Buf,&cpt_buf);

          Buf[cpt_buf++]=octet; 
          write_buf_pcx(canal,Buf,&cpt_buf);
        }
      }
      else
      {
        rep+=192;
        Buf[cpt_buf++]=rep; 
        write_buf_pcx(canal,Buf,&cpt_buf);

        Buf[cpt_buf++]=octet; 
        write_buf_pcx(canal,Buf,&cpt_buf);
      }
      octet=*scr;
      rep=0;
    }

    if(cpt_buf>0) write(canal,Buf,cpt_buf);
    octet=0x0c;write(canal,&octet,1); 
    
    for(i=0;i<768;i++)
    {
      octet=pal[i];
      octet<<=2;
      write(canal,&octet,1);
    }
    close(canal);
    return 1;
  }
  close(canal);
  return 0;
}

void Efface(byte ed,byte ef)
{
  byte i;
  word e;

  for(e=ed;e<=ef;e++)
    for(i=0;i<3;i++) Pal[e*3+i]=0;
  SetPalette();
}

void Copie(byte ed,byte ef,byte d)
{
  word i,e;

  for(i=0;i<768;i++) Buf[i]=Pal[i];
  for(e=ed;e<=ef;e++,d++)
    for(i=0;i<3;i++) Pal[d*3+i]=Buf[e*3+i];
  SetPalette();
}

void Deplace(byte ed,byte ef,byte d)
{
  word i,e;

  for(i=0;i<768;i++) Buf[i]=Pal[i];
  for(e=ed;e<=ef;e++)
    for(i=0;i<3;i++) Pal[e*3+i]=0;
  for(e=ed;e<=ef;e++,d++)
    for(i=0;i<3;i++) Pal[d*3+i]=Buf[e*3+i];
  SetPalette();
}

void Supprime(byte ed,byte ef)
{
  word i,e;

  for(i=0;i<768;i++) Buf[i]=Pal[i];
  for(e=ef+1;e<256;e++,ed++)
    for(i=0;i<3;i++) Pal[ed*3+i]=Buf[e*3+i];
  for(e=ed;e<256;e++,ed++)
    for(i=0;i<3;i++) Pal[ed*3+i]=0;
  SetPalette();
}

void Insert(byte ed,byte ef,byte d)
{
  word i,e,d0=d;

  for(i=0;i<768;i++) Buf[i]=Pal[i];
  for(e=ed;e<=ef;e++,d++)
    for(i=0;i<3;i++) Pal[d*3+i]=Buf[e*3+i];
  for(e=d0;e<256;e++,d++)
    for(i=0;i<3;i++) Pal[d*3+i]=Buf[e*3+i];
  SetPalette();
}

void Inverse(byte ed,byte ef)
{
  byte j,aux;
  word n;
  dword i;

  n=(ef-ed)/2;
  for(i=0;i<=n*3;i+=3)
    for(j=0;j<3;j++) 
    {
      aux=Pal[ed*3+i+j];
      Pal[ed*3+i+j]=Pal[ef*3-i+j];
      Pal[ef*3-i+j]=aux;
    }
  
  if(Wis.Img==OUI)
  {
    n=ef-ed;
    for(i=0;i<256;i++) Buf[i]=i;
    for(i=0;i<=n;i++) Buf[ed+i]=ef-i;
    for(i=0;i<Wis.TailleImg;i++) Page1[i]=Buf[Page1[i]];
  }
  SetPalette();
}

void RotHaut(byte ed,byte ef)
{
  word j;
  dword i;
  byte table[256];

  if(ef>0)
  {
    for(i=0;i<256;i++) table[i]=i;
    j=table[ed];
    for(i=ed;i<=ef-1;i++) table[i]=table[i+1];
    table[ef]=j;

    for(i=0;i<768;i++) Buf[i]=Pal[i];
    for(i=0;i<256;i++)
    {
      j=table[i]*3;
      Pal[i*3]=Buf[j];
      Pal[i*3+1]=Buf[j+1];
      Pal[i*3+2]=Buf[j+2];
    }

    if(Wis.Img==OUI)
    {
      for(i=0;i<256;i++)   
      {
        for(j=0;j<256;j++) 
          if(table[j]==i) break;
        Buf[i]=j;        
      }
      for(i=0;i<Wis.TailleImg;i++) Page1[i]=Buf[Page1[i]];
    }
    SetPalette();
  }
}

void RotBas(byte ed,byte ef)
{
  word j;
  dword i;
  byte table[256];

  if(ed<255)
  {
    for(i=0;i<256;i++) table[i]=i;
    j=table[ef];
    for(i=ef;i>=ed+1;i--) table[i]=table[i-1];
    table[ed]=j;

    for(i=0;i<768;i++) Buf[i]=Pal[i];
    for(i=0;i<256;i++)
    {
      j=table[i]*3;
      Pal[i*3]=Buf[j];
      Pal[i*3+1]=Buf[j+1];
      Pal[i*3+2]=Buf[j+2];
    }
    
    if(Wis.Img==OUI)
    {
      for(i=0;i<256;i++)   
      {
        for(j=0;j<256;j++) 
          if(table[j]==i) break;
        Buf[i]=j;        
      }
      for(i=0;i<Wis.TailleImg;i++) Page1[i]=Buf[Page1[i]];
    }
    SetPalette();
  }
}

void Echange(byte ed,byte ef,byte d)
{
  word j,aux,dst,src;
  int n=-1;
  byte table[256];
  dword i;

  if(ed!=d)
  {
    for(i=0;i<256;i++)
    {
      Buf[i]=i;   
      table[i]=i;
    }
    dst=d;
    for(i=ed;i<=ef;i++) table[dst++]=Buf[i]; 

    for(i=d;i<=d+(ef-ed);i++)
    {
      if(!(ed<=i && i<=ef) && n==-1) 
      {
        src=i;
        n=0;
      }
      if(!(ed<=i && i<=ef) && n!=-1) n++; 
    }
    
    if(d<ed && n<(ef-ed)+1)  
      dst=d+(ef-ed)+1; 
    else
      dst=ed; 
    for(i=0;i<n;i++) table[dst++]=Buf[src++];
    
    for(i=0;i<768;i++) Buf[i]=Pal[i]; 
    for(i=0;i<768;i+=3) 
    {
      aux=table[i/3]*3;
      Pal[i]=Buf[aux];
      Pal[i+1]=Buf[aux+1];
      Pal[i+2]=Buf[aux+2];
    }
    
    if(Wis.Img)
    {
      for(i=0;i<256;i++)
        for(j=0;j<256;j++)
          if(table[j]==i)
          {
            Buf[i]=j;
            break;
          }
      for(i=0;i<Wis.TailleImg;i++) Page1[i]=Buf[Page1[i]];
    }
    SetPalette();
  }
}

void Fondu(byte ed,byte ef,word p)
{
  word i;
  float t;

  if(p<=100)
  {
    t=p/100.0;
    for(i=ed*3;i<ef*3+3;i++) Pal[i]=PalF[i]*t;
  }
  else       
  {
    t=(p-100)/100.0; 
    for(i=ed*3;i<ef*3+3;i++) Pal[i]=(63-PalF[i])*t+PalF[i];
  }
  SetAllPal(Pal);
}

byte Degrade(byte ed,byte ef)
{
  byte c,rvb;
  int n;
  word i;
  float rp;

  if(ef>ed)
  {
    n=ef-ed;
    for(rvb=0;rvb<3;rvb++)
    {
      c=Pal[ed*3+rvb];
      rp=((Pal[ef*3+rvb]-c)/(float)n);
      for(i=1;i<n;i++) Pal[ed*3+i*3+rvb]=c+rp*i;
    }
    SetPalette();
    return 1;
  }
  return 0;
}

byte SearchInk(byte r,byte v,byte b,bytef *p,byte fin)
{
  word i,e=0,
       s=189,s0; 

  for(i=0;i<=fin;i++)
  {
    s0=abs(*p-r)+abs(*(p+1)-v)+abs(*(p+2)-b); 
    p+=3;
    if(s0<=s)
    {
      s=s0;
      e=i;
    }
  }
  return e;
}

void Organise(void)
{
  byte r0=0,v0=0,b0=0;
  word j,e;
  dword i;
  byte Table[256];

  for(i=0;i<768;i++) Buf[i]=Pal[i];
  for(i=0;i<256;i++)
  {
    e=SearchInk(r0,v0,b0,Buf,255)*3;
    r0=Buf[e];
    v0=Buf[e+1];
    b0=Buf[e+2];
    Buf[e]=0xff;
    Buf[e+1]=0xff;
    Buf[e+2]=0xff;
    Table[i]=e/3;
  }

  for(i=0;i<256;i++)
  {
    j=Table[i]*3;
    Buf[i*3]=Pal[j];
    Buf[i*3+1]=Pal[j+1];
    Buf[i*3+2]=Pal[j+2];
  }
  for(i=0;i<768;i++) Pal[i]=Buf[i];

  if(Wis.Img==OUI)
  {
    for(j=0;j<256;j++)
    {
      for(i=0;i<256;i++)
        if(Table[i]==j) break;
      Buf[j]=i;
    }
    for(i=0;i<Wis.TailleImg;i++) Page1[i]=Buf[Page1[i]];
  }
  SetPalette();
}

void Gris(byte ed,byte ef)
{
  word i;

  for(i=ed*3;i<ef*3+3;i+=3)
  {
    Pal[i]=(Pal[i]+Pal[i+1]+Pal[i+2])/3;
    Pal[i+1]=Pal[i];
    Pal[i+2]=Pal[i];
  }
  SetPalette();
}

void NoirBlanc(byte ed,byte ef,byte s)
{
  word i;

  for(i=ed*3;i<ef*3+3;i+=3)
  {
    if(((Pal[i]+Pal[i+1]+Pal[i+2])/3)<=s)
    {
      Pal[i]=0;
      Pal[i+1]=0;
      Pal[i+2]=0;
    }
    else
    {
      Pal[i]=63;
      Pal[i+1]=63;
      Pal[i+2]=63;
    }
  }
  SetPalette();
}

void CorrespI(word p)
{
  byte r0,v0,b0;
  dword i;

  for(i=0;i<768;i+=3)
  {
    if(p<=100)  
    {
      r0=Pal[i]*(p/100.0);
      v0=Pal[i+1]*(p/100.0);
      b0=Pal[i+2]*(p/100.0);
      Buf[i/3]=SearchInk(r0,v0,b0,Pal,255);
    }
    else  
    {
      r0=(63-Pal[i])*((p-100)/100.0)+Pal[i];
      v0=(63-Pal[i+1])*((p-100)/100.0)+Pal[i+1];
      b0=(63-Pal[i+2])*((p-100)/100.0)+Pal[i+2];
      Buf[i/3]=SearchInk(r0,v0,b0,Pal,255);
    }
  }
  for(i=0;i<Wis.TailleImg;i++) Page1[i]=Buf[Page1[i]];
}

void CorrespP(byte ed,byte ef,word p)
{
  byte r0,v0,b0;
  word i,e;

  for(i=0;i<768;i++) Buf[i]=Pal[i];
  if(p<=100)  
  {
    for(i=ed*3;i<ef*3+3;i+=3)
    {
      r0=Buf[i]*(p/100.0);
      v0=Buf[i+1]*(p/100.0);
      b0=Buf[i+2]*(p/100.0);
      e=SearchInk(r0,v0,b0,Buf,255)*3;
      Pal[i]=Buf[e];
      Pal[i+1]=Buf[e+1];
      Pal[i+2]=Buf[e+2];
    }
  }
  else  
  {
    for(i=ed*3;i<ef*3+3;i+=3)
    {
      r0=(63-Buf[i])*((p-100)/100.0)+Buf[i];
      v0=(63-Buf[i+1])*((p-100)/100.0)+Buf[i+1];
      b0=(63-Buf[i+2])*((p-100)/100.0)+Buf[i+2];
      e=SearchInk(r0,v0,b0,Buf,255)*3;
      Pal[i]=Buf[e];
      Pal[i+1]=Buf[e+1];
      Pal[i+2]=Buf[e+2];
    }
  }
  SetPalette();
}

void Negatif(byte ed,byte ef)
{
  word i;

  for(i=ed*3;i<ef*3+3;i+=3)
  {
    Pal[i]=63-Pal[i];
    Pal[i+1]=63-Pal[i+1];
    Pal[i+2]=63-Pal[i+2];
  }
  SetPalette();
}

void Solarise(byte ed,byte ef)
{
  byte r0,v0,b0;
  word i;

  for(i=ed*3;i<ef*3+3;i+=3)
  {
    r0=Pal[i];
    v0=Pal[i+1];
    b0=Pal[i+2];
    if(!(r0==v0 && v0==b0))
    {
      while(r0!=63 && v0!=63 && b0!=63)
      {
        r0++;
        v0++;
        b0++;
      }
    }
    else 
    {
      if(r0<32) r0=63-r0;
      if(v0<32) v0=63-v0;
      if(b0<32) b0=63-b0;
    }
    Pal[i]=r0;
    Pal[i+1]=v0;
    Pal[i+2]=b0;
  }
  SetPalette();
}

void NormPal(bytef *pl1)
{
  word i;

  for(i=0;i<768;i++) Pal[i]=pl1[i];
  SetPalette();
}

void TrspPal(byte *pl,bytef *pl1,byte ed,byte ef)
{
  word i;
  
  for(i=ed*3;i<=ef*3+2;i+=3)
  {
    pl[i]=pl1[i];
    pl[i+1]=pl1[i+1];
    pl[i+2]=pl1[i+2];
  }
  SetPalette();
}

void TrscPal(bytef *pl,bytef *pl1)
{
  dword i;
  byte Table[256];

  for(i=0;i<768;i+=3) Table[i/3]=SearchInk(pl[i],pl[i+1],pl[i+2],pl1,255);
  for(i=0;i<Wis.TailleImg;i++) Page1[i]=Table[Page1[i]];
  for(i=0;i<768;i++) Pal[i]=pl1[i]; 
  SetPalette();
}

void MoyPal(bytef *pl,bytef *pl1,byte ed,byte ef)
{
  word i;
  
  for(i=ed*3;i<=ef*3+2;i+=3)
  {
    pl[i]=(pl[i]+pl1[i])/2;
    pl[i+1]=(pl[i+1]+pl1[i+1])/2;
    pl[i+2]=(pl[i+2]+pl1[i+2])/2;
  }
  SetPalette();
}

void Tri(word s)
{
  byte aux,ind[256],rvb[256];
  word j;
  dword i;

  for(i=0;i<256;i++)
  {
    ind[i]=i;  
    rvb[i]=Pal[i*3]+Pal[i*3+1]+Pal[i*3+2];
  }
  
  for(i=0;i<255;i++)  
    for(j=255;j>=i+1;j--)
      if(rvb[j-s]<rvb[j+(s-1)])  
      {
        aux=ind[j];        
        ind[j]=ind[j-1];
        ind[j-1]=aux;
        aux=rvb[j];        
        rvb[j]=rvb[j-1];
        rvb[j-1]=aux;
      }
  
  for(i=0;i<768;i++) Buf[i]=Pal[i];
  for(i=0;i<256;i++)
  {
    Pal[i*3]=Buf[ind[i]*3];
    Pal[i*3+1]=Buf[ind[i]*3+1];
    Pal[i*3+2]=Buf[ind[i]*3+2];
  }
  
  if(Wis.Img==OUI)
  {
    for(i=0;i<256;i++)   
    {
      for(j=0;j<256;j++) 
        if(ind[j]==i) break;
      rvb[i]=j;          
    }
    for(i=0;i<Wis.TailleImg;i++) Page1[i]=rvb[Page1[i]];
  }
  SetPalette();
}

void Reduction(word nb)
{
  byte ind[256];
  word j;
  dword aux,i,frq[256];

  for(i=0;i<256;i++)
  {
    ind[i]=i; 
    frq[i]=0; 
  }
  for(i=0;i<Wis.TailleImg;i++) frq[Page1[i]]++;
  
  for(i=0;i<255;i++)  
    for(j=255;j>=i+1;j--)
      if(frq[j]>frq[j-1])  
      {
        aux=ind[j];        
        ind[j]=ind[j-1];
        ind[j-1]=aux;
        aux=frq[j];        
        frq[j]=frq[j-1];
        frq[j-1]=aux;
      }
  
  for(i=0;i<768;i++) Buf[i]=Pal[i]; 
  for(i=0;i<nb;i++)  
  {
    aux=i*3;
    j=ind[i]*3;
    Pal[aux]=Buf[j];
    Pal[aux+1]=Buf[j+1];
    Pal[aux+2]=Buf[j+2];
  }
  for(i=nb*3;i<768;i++) Pal[i]=63;
  
  for(i=0;i<256;i++)
  {
    aux=i*3;
    frq[i]=SearchInk(Buf[aux],Buf[aux+1],Buf[aux+2],Pal,nb-1);
  }
  for(i=0;i<Wis.TailleImg;i++) Page1[i]=frq[Page1[i]];
  SetPalette();
}

word NbInk(void)
{
  word n=0;
  dword i;

  for(i=0;i<256;i++) Buf[i]=0;
  for(i=0;i<Wis.TailleImg;i++) Buf[Page1[i]]=1;
  for(i=0;i<256;i++)
    if(Buf[i]) n++;
  return n;
}

dword UseInk(byte ink)
{
  dword i,n=0;

  for(i=0;i<Wis.TailleImg;i++)
     if(Page1[i]==ink) n++;
  return n;
}

void Modifie(byte ed,byte ef,byte val)
{
  dword i;

  for(i=0;i<Wis.TailleImg;i++)
    if(Page1[i]>=ed && Page1[i]<=ef) Page1[i]=val;
}

void Resize(bytef *scr,byte mode)
{
  word i,j;
  float rpx,rpy;
  bytef *p=scr;

  rpx=Wis.LgImg/320.0;
  rpy=Wis.HtImg/200.0;

  ClrScr(scr);
  for(j=0;j<200;j++)
    for(i=0;i<320;i++)
      *(scr++)=Page1[(dword)(j*rpy)*Wis.LgImg+i*rpx];

  if(mode)
  {
    for(i=0;i<64000;i++) Page1[i]=p[i];
    Wis.TailleImg=64000;
    Wis.LgImg=320;
    Wis.HtImg=200;
    Wis.XwMax=0;
    Wis.YwMax=0;
    Wis.Xw=0;
    Wis.Yw=0;
  }
}

void DecRVB(byte ed,byte ef,byte rvb)
{
  word i;

  for(i=ed;i<=ef;i++)
    if(Pal[i*3+rvb]>0) Pal[i*3+rvb]--;
  SetPalette();
}

void IncRVB(byte ed,byte ef,byte rvb)
{
  word i;

  for(i=ed;i<=ef;i++)
    if(Pal[i*3+rvb]<63) Pal[i*3+rvb]++;
  SetPalette();
}

void SavePal(char *path)
{
  int h;

  sprintf(Buf,"%s.pal",path);
  _fmode=O_BINARY;
  h=creat(Buf,S_IREAD|S_IWRITE);
  write(h,Pal,768);
  close(h);
}

void LoadPal(char *path)
{
  char chemin[20];
  int h;

  sprintf(chemin,"%s.pal",path);
  h=open(chemin,O_RDONLY);
  read(h,Buf,768);
  close(h);
}

void Aide(void)
{
  ModeTxt();
  textmode(C4350);
  _setcursortype(_NOCURSOR);
  clrscr();

  puts("WISPAL (320x200x256 - 700x570x256) -  V.3.5  - (c)1995/96 S.W\\wiSdom \n\n");
  puts("h                  ³ Affiche cette page d'aide");
  puts("TOUCHES FLECHEES   ³ (*)Encre courante / Destination du bloc / D‚place l'image");
  puts("HOME ³ END         ³ (*)Encre de d‚but ³ Encre de fin / D‚place l'image");
  puts("PGUP ³ PGDW        ³ D‚place l'image");
  puts("i                  ³ Recalcul l'image -> (i)");
  puts("p                  ³ Recalcul la palette -> (p)");
  puts("F1 ³ F2            ³ D‚but ³ Fin du bloc");
  puts("F3                 ³ (*)Solarisation du bloc");
  puts("F4                 ³ (*)Negatif du bloc");
  puts("F5 ³ F6            ³ (*)Diminue ³ (*)Augmente R,V et B du bloc (0% … 200%)");
  puts("F7                 ³ (*)Niveau de gris du bloc");
  puts("F8                 ³ Noir et blanc du bloc (0% … 100%)");
  puts("F9 ³ F10           ³ Charge image .PCX ³ palette .PAL (p)");
  puts("SHFT F9 ³ SHFT F10 ³ Sauve image .PCX  ³ palette .PAL");
  puts("s                  ³ Sauve la partie visible de l'image .PCX");
  puts("RETURN             ³ Palette m‚moris‚e … 100% (aprŠs F5, F6)");
  puts("e                  ³ Efface le bloc");
  puts("c                  ³ Copie le bloc");
  puts("d                  ³ D‚place le bloc");
  puts("x                  ³ Echange le bloc (i)");
  puts("INSERT             ³ Insert le bloc");
  puts("SUPPR              ³ Supprime le bloc");
  puts("m                  ³ Modifie un bloc d'encres dans l'image");
  puts("g                  ³ D‚grad‚ de couleur du bloc");
  puts("v                  ³ Inverse le bloc (i)");
  puts("1,2,3 ³ 4,5,6      ³ (*)Diminue ³ (*)augmente R,V ou B du bloc");
  puts("/ ³ *              ³ (*)Diminue ³ (*)augmente R,V et B du bloc");
  puts("- ³ +              ³ Rotation (*)gauche (i) ³ (*)droite du bloc (i)");
  puts("b                  ³ Correspondance-palette du bloc de 0% … 200%");
  puts("k                  ³ Correspondance-image de 0% … 200%");
  puts("a                  ³ Nombre d'apparition d'une encre");
  puts("t                  ³ Tri croissant ou d‚croissant de la palette (i)");
  puts("o                  ³ Organise la palette (i)");
  puts("r                  ³ R‚duction de la palette");
  puts("n                  ³ Nombre d'encre diff‚rente dans l'image");
  puts("w                  ³ (*)Redimensionne l'image / Visualise l'image");
  puts("ESPACE             ³ (*)Affiche l'image / Retour … l'‚diteur");
  puts("DEL                ³ Recharge le dernier fichier");
  puts("ESC                ³ Annule une option / Quitte WisPal\n");
  puts("(*)                ³ Option valable sous l'‚diteur ou l'image");
  puts("\n\n\n\n                                <RETURN>  ");
}

int ReadKey(byte *bool)
{
  int key;

  while(!kbhit) getch();
  key=getch();
  if(!key)
  {
    key=getch();
    *bool=1;
  }
  else
  {
    key=tolower(key);
    *bool=0;
  }
  return key;
}

void InitWisPal(void)
{
  dword i;

  Wis.TailleImg=64000; 
  Wis.LgImg=320;
  Wis.HtImg=200;
  Wis.XwMax=0;
  Wis.YwMax=0;
  Wis.Xw=0;
  Wis.Yw=0;
  Wis.Img=OUI;
  Wis.Pal=NORMAL;

  for(i=0;i<768;i++) Pal[i]=PalD[i];
  SetPalette();
  for(i=0;i<TAILLE_IMG_MAX;i++) Page1[i]=0;
  InitScreen();
}

/*--------------------------------------------------------------------------*/

void main(int argc,char **argv)
{
  word i,j,
       val1,val2,val3,
       prct=100;
  int handle,
      key=BAS;
  byte fch[22],
       encre,
       fcnt;
  word x=0,      
       y=0,
       a1=0,     
       a2=0;     

  if(argc>1)
  {
    clrscr();
    puts("WISPAL - (c) 1995/96 Siracusa Walter\\wiSdom -");
    puts("Sous l'‚diteur, appuyez sur 'h' pour afficher l'aide\n");
    exit(0);
  }

  ModeVGA();
  LoadFnt("wpal");
  InitWisPal();

  do
  {
    encre=y+(x<<4); 
    sprintf(Buf,"E:%03d",encre);PrintTxt(190,30,Buf,255,0,Video);
    sprintf(Buf,"R:%02d",Pal[encre*3]);PrintTxt(190,45,Buf,255,0,Video);
    sprintf(Buf,"V:%02d",Pal[encre*3+1]);PrintTxt(190,55,Buf,255,0,Video);
    sprintf(Buf,"B:%02d",Pal[encre*3+2]);PrintTxt(190,65,Buf,255,0,Video);
    sprintf(Buf,"F:%03u%",prct);PrintTxt(190,80,Buf,255,0,Video);
    if(Wis.Img==OUI) PrintTxt(190,95,"I:OUI",255,0,Video);
    else PrintTxt(190,95,"I:NON",255,0,Video);
    switch(Wis.Pal)
    {
       case NORMAL       : PrintTxt(190,105,"P:NORMAL      ",255,0,Video);break;
       case TRANSPARENCE : PrintTxt(190,105,"P:TRANSPARENCE",255,0,Video);break;
       case TRANSCRIT    : PrintTxt(190,105,"P:TRANSCRIT   ",255,0,Video);break;
       case MOYENNE      : PrintTxt(190,105,"P:MOYENNE     ",255,0,Video);break;
    }
    sprintf(Buf,"C:%02d",x);PrintTxt(190,120,Buf,255,0,Video);
    sprintf(Buf,"L:%02d",y);PrintTxt(190,130,Buf,255,0,Video);
    sprintf(Buf,"D:%03lu*%03lu",Wis.LgImg,Wis.HtImg);PrintTxt(190,150,Buf,255,0,Video);
    
    val2=20;
    val3=0;
    for(j=0;j<16;j++)
    {
      val1=20;
      for(i=0;i<16;i++)
      {
        FBox(val2,val1,9,9,Video,val3);
        if(val3>=a1 && val3<=a2)
          Box(val2,val1,9,9,Video,255);
        val1+=10;
        val3++;
      }
      val2+=10;
    }
    
    val1=19+x*10;
    val2=19+y*10;
    if(key>=HOME && key<=BAS)
    {
      Box(val1,val2,11,11,Video,255);
      Box(20+x*10,20+y*10,9,9,Video,255);
    }
    key=ReadKey(&fcnt);
    if(key>=HOME && key<=BAS)
      Box(val1,val2,11,11,Video,0);

    if(fcnt) 
    {
      switch(key)
      {
        case DROITE : if(x<15) x++;
                      else x=0;break;

        case GAUCHE : if(x>0) x--;
                      else x=15;break;

        case HAUT   : if(y>0) y--;
                      else y=15;break;

        case BAS    : if(y<15) y++;
                      else y=0;break;

        case HOME   : x=0;y=0;break;

        case END    : x=15;y=15;break;

        case F1     : a1=encre;break;

        case F2     : a2=encre;
                      if(a2<a1) a2=a1;break;

        case F3     : Solarise(a1,a2);break;

        case F4     : Negatif(a1,a2);break;

        case F5     : if(prct>0) prct--;
                      Fondu(a1,a2,prct);break;

        case F6     : if(prct<200) prct++;
                      Fondu(a1,a2,prct);break;

        case F7     : Gris(a1,a2);break;

        case F8     : PrintTxt(2,189,"SEUIL (0-63):",255,0,Video);
                      if(ScanTxt(125,189,Buf,255,0,2,N,Video))
                      {
                        val1=atoi(Buf);
                        if(val1<=63) NoirBlanc(a1,a2,val1);
                      }
                      FBox(1,187,318,12,Video,0);break;

        case F9     : PrintTxt(2,189,"CHARGER IMAGE:",255,0,Video);
                      if(ScanTxt(130,189,Wis.Fch,255,0,19,AN,Video))
                      {
                        LoadPCXMem(Wis.Fch,Page1,Pal);
                        SetPalette();
                        prct=100;
                        Wis.Xw=0;
                        Wis.Yw=0;
                        Wis.Load=IMAGE;
                      }
                      FBox(1,187,318,12,Video,0);break;

        case F10    : PrintTxt(2,189,"CHARGER PALETTE:",255,0,Video);
                      if(ScanTxt(148,189,Wis.Fch,255,0,19,AN,Video))
                      {
                        LoadPal(Wis.Fch); 
                        prct=100;
                        switch(Wis.Pal)
                        {
                          case NORMAL       : NormPal(Buf);break;
                          case TRANSPARENCE : TrspPal(Pal,Buf,a1,a2);break;
                          case TRANSCRIT    : TrscPal(Pal,Buf);break;
                          case MOYENNE      : MoyPal(Pal,Buf,a1,a2);break;
                        }
                        Wis.Load=PALETTE;
                      }
                      FBox(1,187,318,12,Video,0);break;

        case SF9   : PrintTxt(2,189,"SAUVER IMAGE:",255,0,Video);
                     if(ScanTxt(122,189,fch,255,0,19,AN,Video))
                       SaveMemPCX(fch,Page1,Pal);
                     FBox(1,187,318,12,Video,0);break;

        case SF10   : PrintTxt(2,189,"SAUVER PALETTE:",255,0,Video);
                      if(ScanTxt(138,189,fch,255,0,19,AN,Video))
                        SavePal(fch);
                      FBox(1,187,318,12,Video,0);break;

        case INSERT : if(encre+(a2-a1)<256) Insert(a1,a2,encre);
                      else KeyTxt(2,189,"ERREUR !",255,0,Video);break;

        case SUPPR  : Supprime(a1,a2);break;
      }
    }
    else 
    {
      switch(key)
      {
        case 'h'    : Aide();
                      do
                      {
                        key=ReadKey(&fcnt);
                      }while(key!=RET);
                      ModeVGA();
                      SetPalette();
                      InitScreen();
                      key=BAS;break;

        case 'i'    : Wis.Img=OUI-Wis.Img;break;

        case 'p'    : if(++Wis.Pal==4) Wis.Pal=0;break;

        case RET    : prct=100;
                      for(i=0;i<768;i++) PalF[i]=Pal[i];
                      KeyTxt(2,189,"PALETTE MEMORISEE",255,0,Video);break;

        case 'e'    : Efface(a1,a2);break;

        case 'c'    : if(encre+(a2-a1)<256) Copie(a1,a2,encre);
                      else KeyTxt(2,189,"ERREUR !",255,0,Video);break;

        case 'd'    : if(encre+(a2-a1)<256) Deplace(a1,a2,encre);
                      else KeyTxt(2,189,"ERREUR !",255,0,Video); break;

        case 'x'    : if(encre+(a2-a1)<256) Echange(a1,a2,encre);
                      else KeyTxt(2,189,"ERREUR !",255,0,Video);break;

        case 'g'    : if(!Degrade(a1,a2)) KeyTxt(2,189,"ERREUR !",255,0,Video);
                      break;

        case 'v'    : Inverse(a1,a2);break;

        case 'b'    : PrintTxt(2,189,"SEUIL PALETTE (0%-200%):",255,0,Video);
                      if(ScanTxt(216,189,Buf,255,0,3,N,Video))
                      {
                        val1=atoi(Buf);
                        if(val1<=200) CorrespP(a1,a2,val1);
                      }
                      FBox(1,187,318,12,Video,0);break;

        case '1'    : DecRVB(a1,a2,0);break;

        case '2'    : DecRVB(a1,a2,1);break;

        case '3'    : DecRVB(a1,a2,2);break;

        case '4'    : IncRVB(a1,a2,0);break;

        case '5'    : IncRVB(a1,a2,1);break;

        case '6'    : IncRVB(a1,a2,2);break;

        case '/'    : DecRVB(a1,a2,0);
                      DecRVB(a1,a2,1);
                      DecRVB(a1,a2,2);break;

        case '*'    : IncRVB(a1,a2,0);
                      IncRVB(a1,a2,1);
                      IncRVB(a1,a2,2);break;

        case '-'    : RotHaut(a1,a2);break;

        case '+'    : RotBas(a1,a2);break;

        case 'k'    : PrintTxt(2,189,"SEUIL IMAGE (0%-200%):",255,0,Video);
                      if(ScanTxt(200,189,Buf,255,0,3,N,Video))
                      {
                        val1=atoi(Buf);
                        if(val1<=200) CorrespI(val1);
                      }
                      FBox(1,187,318,12,Video,0);
                      CpyMemVid(Page1,Video,Wis.Xw,Wis.Yw);
                      getch();
                      InitScreen();break;

        case 't'    : PrintTxt(2,189,"TRI (C,D):",255,0,Video);
                      if(ScanTxt(96,189,Buf,255,0,1,A,Video))
                      {
                        if(Buf[0]=='C') Tri(0);
                        if(Buf[0]=='D') Tri(1);
                      }
                      FBox(1,187,318,12,Video,0);break;

        case 'o'    : Organise();break;

        case 'n'    : sprintf(Buf,"TOTAL COULEUR:%03d",NbInk());
                      KeyTxt(2,189,Buf,255,0,Video);break;

        case 'a'    : sprintf(Buf,"TOTAL APPARITION:%lu",UseInk(encre));
                      KeyTxt(2,189,Buf,255,0,Video);break;

        case 'm'    : PrintTxt(2,189,"MODIFIE (0-255):",255,0,Video);
                      if(ScanTxt(150,189,Buf,255,0,3,N,Video))
                      {
                        val1=atoi(Buf);
                        if(val1<=255) Modifie(a1,a2,val1);
                      }
                      FBox(1,187,318,12,Video,0);break;

        case 'r'    : val2=NbInk();
                      sprintf(Buf,"REDUCTION (1-%03d):",val2);
                      PrintTxt(2,189,Buf,255,0,Video);
                      if(ScanTxt(164,189,Buf,255,0,3,N,Video))
                      {
                        val1=atoi(Buf);
                        if(val1>0 && val1<=val2) Reduction(val1);
                      }
                      FBox(1,187,318,12,Video,0);break;

        case 's'    : PrintTxt(2,189,"SAUVER ECRAN:",255,0,Video);
                      if(ScanTxt(122,189,fch,255,0,19,AN,Video))
                      {
                        CpyMemVid(Page1,Video,Wis.Xw,Wis.Yw);
                        SavePCX(fch,Video,Pal);
                        InitScreen();
                        key=BAS;
                      }
                      FBox(1,187,318,12,Video,0);break;

        case 'w'    : Resize(Video,1);
                      InitScreen();
                      key=BAS;break;

        case DEL    : prct=100;
                      switch(Wis.Load)
                      {
                        case RIEN    : for(i=0;i<768;i++) Pal[i]=PalD[i];
                                       break;
                        case IMAGE   : LoadPCXMem(Wis.Fch,Page1,Pal);
                                       Wis.Xw=0;Wis.Yw=0;break;
                        case PALETTE : LoadPal(Wis.Fch); 
                                       for(i=0;i<768;i++) Pal[i]=Buf[i];break;
                      }
                      SetPalette();break;

        case SPACE  : CpyMemVid(Page1,Video,Wis.Xw,Wis.Yw);
                      do
                      {
                        key=ReadKey(&fcnt);

                        if(fcnt) 
                        {
                          switch(key)
                          {
                            case F3  : Solarise(a1,a2);break;

                            case F4  : Negatif(a1,a2);break;

                            case F5  : if(prct>0) prct--;
                                       Fondu(a1,a2,prct);break;

                            case F6  : if(prct<200) prct++;
                                       Fondu(a1,a2,prct);break;

                            case F7  : Gris(a1,a2);break;

                            case GAUCHE : if(Wis.Xw>=PAS_SCR) Wis.Xw-=PAS_SCR;
                                          CpyMemVid(Page1,Video,Wis.Xw,Wis.Yw);
                                          break;
                            case DROITE : if(Wis.Xw<=Wis.XwMax-PAS_SCR) Wis.Xw+=PAS_SCR;
                                          CpyMemVid(Page1,Video,Wis.Xw,Wis.Yw);
                                          break;
                            case HAUT   : if(Wis.Yw>=PAS_SCR) Wis.Yw-=PAS_SCR;
                                          CpyMemVid(Page1,Video,Wis.Xw,Wis.Yw);
                                          break;
                            case BAS    : if(Wis.Yw<=Wis.YwMax-PAS_SCR) Wis.Yw+=PAS_SCR;
                                          CpyMemVid(Page1,Video,Wis.Xw,Wis.Yw);
                                          break;
                            case HOME   : Wis.Xw=0;
                                          CpyMemVid(Page1,Video,Wis.Xw,Wis.Yw);
                                          break;
                            case END    : Wis.Xw=Wis.XwMax;
                                          CpyMemVid(Page1,Video,Wis.Xw,Wis.Yw);
                                          break;
                            case PGUP   : Wis.Yw=0;
                                          CpyMemVid(Page1,Video,Wis.Xw,Wis.Yw);
                                          break;
                            case PGDW   : Wis.Yw=Wis.YwMax;
                                          CpyMemVid(Page1,Video,Wis.Xw,Wis.Yw);
                                          break;
                          }
                        }
                        else 
                        {
                          switch(key)
                          {
                            case '1' : DecRVB(a1,a2,0);break;

                            case '2' : DecRVB(a1,a2,1);break;

                            case '3' : DecRVB(a1,a2,2);break;

                            case '4' : IncRVB(a1,a2,0);break;

                            case '5' : IncRVB(a1,a2,1);break;

                            case '6' : IncRVB(a1,a2,2);break;

                            case '/' : DecRVB(a1,a2,0);
                                       DecRVB(a1,a2,1);
                                       DecRVB(a1,a2,2);break;

                            case '*' : IncRVB(a1,a2,0);
                                       IncRVB(a1,a2,1);
                                       IncRVB(a1,a2,2);break;

                            case '-' : if(Wis.Img==NON) RotHaut(a1,a2);break;

                            case '+' : if(Wis.Img==NON) RotBas(a1,a2);break;

                            case 'w' : Resize(Video,0);break;

                          }
                        }
                      }while(key!=SPACE);
                      InitScreen();
                      key=BAS;
                      break;
      }
    }
  }while(key!=ESC);

  ModeTxt();
  clrscr();
  puts("WISPAL - (c) 1995/96 Siracusa Walter\\wiSdom -");
  _setcursortype(_NORMALCURSOR);
}
