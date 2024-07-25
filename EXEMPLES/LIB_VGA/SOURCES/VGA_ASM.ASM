COMMENT\ TITRE   : BIBLIOTHEQUE GRAPHIQUE VGA STANDARD
COMMENT\ AUTEUR  : Patrice SIRACUSA /wiSdom (1995/1996)
COMMENT\ FICHIER : VGA_ASM.ASM \


JUMPS
.ALPHA
LOCALS
  .MODEL LARGE
  .386

  .stack 0400h   ; PILE DE 1 Ko
;-------------------------------------------------------
  .DATA
  EVEN

;=======================================================
  .CODE
  EVEN
;-------------------------------------------------------

;================ Fonctions exportees ==================

public _ModeVGA
public _Mode320x100
public _ModeTxt
public _Ink
public _SetInk
public _ClrScr
public _CpyScr
public _SwpScr
public _CpyBnd
public _GetBlk1
public _GetBlk2
public _GetBlk4
public _PutBlk1
public _PutBlk2
public _PutBlk4
public _PutSpr
public _Dbl160x100
public _CpyBlk
public _CpyBlk1
public _CpyBlk2
public _CpyBlk4
public _CpySpr
public _LineMap
public _LineMapH


;================ Implementation des fonctions =========

;*******************************************************
; _ModeVGA()
;
; Parametres :
;
; aucun
;
; Specification :
;
; Initialise le mode VGA 320*200, 256 couleurs
;
;*******************************************************

_ModeVGA proc far

  push ax
  mov ax,0013h
  int 10h
  pop ax
  ret

_ModeVGA endp

;*******************************************************
; _Mode320x100()
;
; Parametres :
;
; aucun
;
; Specification :
;
; Initialise le mode VGA 320*100, 256 couleurs
;
;*******************************************************

_Mode320x100 proc far

  push ax dx

  mov ax,0013h
  int 10h

  mov dx,03d4h
  mov al,9
  out dx,al

  mov dx,03d5h
  in al,dx
  and al,70h
  or al,3
  out dx,al

  pop dx ax
  ret

_Mode320x100 endp

;*******************************************************
; _ModeTxt()
;
; Parametres :
;
; aucun
;
; Specification :
;
; Initialise le mode texte 80*25, 16 couleurs
;
;*******************************************************

_ModeTxt proc far

  push ax

  mov ax,0003h
  int 10h

  pop ax

  ret

_ModeTxt endp

;*******************************************************
; _Ink(byte e,byte r,byte v,byte b)
;
; Parametres :
;
;
; Specification :
;
; Met la couleur(r,v,b) a l'encre e
;
;*******************************************************

_Ink proc far
 arg e:byte,r:byte,v:byte,b:byte
 push bp
 mov bp,sp

 push ax dx

 mov al,e
 mov dx,03c8h
 out dx,al
 mov dx,03c9h
 mov al,r
 out dx,al
 mov al,v
 out dx,al
 mov al,b
 out dx,al

 pop dx ax

 pop bp
 ret

_Ink endp

;*******************************************************
; _SetInk(byte e,byte r,byte v,byte b,byte *pal)
;
; Parametres :
;
;
; Specification :
;
; Met la couleur(r,v,b) a l'encre e et dans la palette pal
;
;*******************************************************

_SetInk proc far
 arg e:byte,r:byte,v:byte,b:byte,pal:far ptr
 push bp
 mov bp,sp
 push ax dx

 mov al,e
 mov dx,03c8h
 out dx,al
 mov dx,03c9h
 mov al,r
 out dx,al
 mov al,v
 out dx,al
 mov al,b
 out dx,al

 ; dans la palette

 lds si,pal
 xor ax,ax
 mov al,e
 imul ax,ax,3
 add si,ax
 mov al,r
 mov ds:[si],al
 mov al,v
 mov ds:[si+1],al
 mov al,b
 mov ds:[si+2],al

 pop dx ax

 pop bp
 ret

_SetInk endp

;*******************************************************
; _Dbl160x100(page1:far ptr,page2:far ptr)
;
; Parametres :
;
; page1 : buffer, taille 160*100
; page2 : page, taille 320*100
;
; Specification :
;
; Copie page1 dans page2 en doublant horizontalement les pixels
;*******************************************************

_Dbl160x100 proc far
  arg page1:far ptr,page2:far ptr

  push bp
  mov bp,sp
  push es di ds si cx ebx ax

  les di,page1
  lds si,page2

  mov cx,8000

boucle:

  mov al,es:[di+1]
  mov bl,al
  mov bh,al
  shl ebx,16
  mov al,es:[di]
  inc di
  inc di
  mov bl,al
  mov bh,al
  mov ds:[si],ebx
  add si,4

  loop boucle

  pop ax ebx cx si ds di es
  pop bp

  ret

_Dbl160x100 endp

;*******************************************************
; _ClrScr(page1:far ptr)
;
; Parametres :
;
; page1 : page a initialiser, taille 320*200
;
; Specification :
;
; Initialise tous les pixels de page a 0
;*******************************************************

_ClrScr proc far
  arg page1:far ptr

  push bp
  mov bp,sp
  push cx eax es di

  les di,page1
  mov cx,16000
  xor eax,eax
  cld
  rep stosd

  pop di es eax cx
  pop bp

  ret

_ClrScr endp

;*******************************************************
; _CpyScr(src:far ptr,dst:far ptr)
;
; Parametres :
;
; src : page source, taille 320*200
; dst : page destination, taille 320*200
;
; Specification :
;
; Copie src dans dst
;*******************************************************

_CpyScr proc far
  arg src:far ptr,dst:far ptr

  push bp
  mov bp,sp
  push cx es di ds si

  lds si,src
  les di,dst
  mov cx,16000
  cld
  rep movsd

  pop si ds di es cx
  pop bp

  ret

_CpyScr endp

;*******************************************************
; _SwpScr(src:far ptr,dst:far ptr)
;
; Parametres :
;
; src : page source, taille 320*200
; dst : page destination, taille 320*200
;
; Specification :
;
; Echange src et dst
;*******************************************************

_SwpScr proc far
  arg src:far ptr,dst:far ptr

  push bp
  mov bp,sp
  push ds si es di cx eax ebx

  lds si,src
  les di,dst
  mov cx,16000
bswp:
  mov eax,ds:[si]
  mov ebx,es:[di]
  mov es:[di],eax
  mov ds:[si],ebx
  add si,4
  add di,4
  dec cx
  or cx,cx
  jnz bswp

  pop ebx eax cx di es si ds
  pop bp

  ret

_SwpScr endp

;*******************************************************
; _CpyBnd(ys:word,ht:word,yd:word,src:far ptr,dst:far ptr)
;
; Parametres :
;
; ys : numero de ligne de debut de bande
; ht : hauteur de la bande
; yd : numero de ligne dans la page de destination
; src : page source, taille 320*200
; dst : page destination, taille 320*200
;
; Specification :
;
; Copie src dans dst
;*******************************************************

_CpyBnd proc far
  arg ys:word,ht:word,yd:word,src:far ptr,dst:far ptr

  push bp
  mov bp,sp
  push ds esi es edi cx

  lds si,src
  les di,dst
  imul cx,ys,320
  add si,cx
  imul cx,yd,320
  add di,cx
  imul cx,ht,320
  shr ecx,2
  cld
  rep movsd

  pop cx edi es esi ds
  pop bp

  ret

_CpyBnd endp

;*******************************************************
; _GetBlk1(x:word,y:word,l:word,h:word,src:far ptr,buff:far ptr)
;
; Parametres :
;
; x : abscisse ecran
; y : ordonnee ecran
; l : largeur du bloc en pixels
; h : hauteur du bloc en pixels
; src : page source
; buff : buffer destination
;
; Specification :
;
; Copie le bloc de taille l*h dans buff octet par octet
;*******************************************************

_GetBlk1 proc far
  arg x:word,y:word,l:word,h:word,src:far ptr,buff:far ptr

  push bp
  mov bp,sp
  push es di ds si cx ax bx dx

  les di,buff
  lds si,src
  imul cx,y,320
  add cx,x
  add si,cx
  mov ax,si
  mov bx,h
  mov dx,l
  cld
blk1h:
  mov cx,dx
  rep movsb
  add ax,320
  mov si,ax
  dec bx
  or bx,bx
  jnz blk1h

  pop dx bx ax cx si ds di es
  pop bp

  ret

_GetBlk1 endp

;*******************************************************
; _GetBlk2(x:word,y:word,l:word,h:word,src:far ptr,buff:far ptr)
;
; Parametres :
;
; x : abscisse ecran
; y : ordonnee ecran
; l : largeur du bloc en pixels
; h : hauteur du bloc en pixels
; src : page source
; buff : buffer destination
;
; Specification :
;
; Copie le bloc de taille l*h dans buff mot par mot (16 bits)
;*******************************************************

_GetBlk2 proc far
  arg x:word,y:word,l:word,h:word,src:far ptr,buff:far ptr

  push bp
  mov bp,sp
  push es di ds si cx ax bx dx

  les di,buff
  lds si,src
  imul cx,y,320
  add cx,x
  add si,cx
  mov ax,si
  mov bx,h
  mov dx,l
  shr dx,1
  cld
blk2h:
  mov cx,dx
  rep movsw
  add ax,320
  mov si,ax
  dec bx
  or bx,bx
  jnz blk2h

  pop dx bx ax cx si ds di es
  pop bp

  ret

_GetBlk2 endp

;*******************************************************
; _GetBlk4(x:word,y:word,l:word,h:word,src:far ptr,buff:far ptr)
;
; Parametres :
;
; x : abscisse ecran
; y : ordonnee ecran
; l : largeur du bloc en pixels
; h : hauteur du bloc en pixels
; src : page source
; buff : buffer destination
;
; Specification :
;
; Copie le bloc de taille l*h dans buff long par long (32 bits)
;*******************************************************

_GetBlk4 proc far
  arg x:word,y:word,l:word,h:word,src:far ptr,buff:far ptr

  push bp
  mov bp,sp
  push es di ds si cx ax bx dx

  les di,buff
  lds si,src
  imul cx,y,320
  add cx,x
  add si,cx
  mov ax,si
  mov bx,h
  mov dx,l
  shr dx,2
  cld
blk4h:
  mov cx,dx
  rep movsd
  add ax,320
  mov si,ax
  dec bx
  or bx,bx
  jnz blk4h

  pop dx bx ax cx si ds di es
  pop bp

  ret

_GetBlk4 endp

;*******************************************************
; _PutBlk1(x:word,y:word,l:word,h:word,src:far ptr,buff:far ptr)
;
; Parametres :
;
; x : abscisse ecran
; y : ordonnee ecran
; l : largeur du bloc en pixels
; h : hauteur du bloc en pixels
; src : page dest.
; buff : buffer source
;
; Specification :
;
; Ecrit dans src aux coord x,y le buffer buff octet par octet
;*******************************************************

_PutBlk1 proc far
  arg x:word,y:word,l:word,h:word,src:far ptr,buff:far ptr

  push bp
  mov bp,sp
  push es di ds si cx ax bx dx

  les di,src
  lds si,buff
  imul cx,y,320
  add cx,x
  add di,cx
  mov ax,di
  mov bx,h
  mov dx,l
  cld
plk1h:
  mov cx,dx
  rep movsb
  add ax,320
  mov di,ax
  dec bx
  or bx,bx
  jnz plk1h

  pop dx bx ax cx si ds di es
  pop bp

  ret

_PutBlk1 endp

;*******************************************************
; _PutBlk2(x:word,y:word,l:word,h:word,src:far ptr,buff:far ptr)
;
; Parametres :
;
; x : abscisse ecran
; y : ordonnee ecran
; l : largeur du bloc en pixels
; h : hauteur du bloc en pixels
; src : page dest.
; buff : buffer source
;
; Specification :
;
; Ecrit dans src aux coord x,y le buffer buff mot par mot (16 bits)
;*******************************************************

_PutBlk2 proc far
  arg x:word,y:word,l:word,h:word,src:far ptr,buff:far ptr

  push bp
  mov bp,sp
  push es di ds si cx ax bx dx

  les di,src
  lds si,buff
  imul cx,y,320
  add cx,x
  add di,cx
  mov ax,di
  mov bx,h
  mov dx,l
  shr dx,1
  cld
plk2h:
  mov cx,dx
  rep movsw
  add ax,320
  mov di,ax
  dec bx
  or bx,bx
  jnz plk2h

  pop dx bx ax cx si ds di es
  pop bp

  ret

_PutBlk2 endp

;*******************************************************
; _PutBlk4(x:word,y:word,l:word,h:word,src:far ptr,buff:far ptr)
;
; Parametres :
;
; x : abscisse ecran
; y : ordonnee ecran
; l : largeur du bloc en pixels
; h : hauteur du bloc en pixels
; src : page dest.
; buff : buffer source
;
; Specification :
;
; Ecrit dans src aux coord x,y le buffer buff long par long (32 bits)
;*******************************************************

_PutBlk4 proc far
  arg x:word,y:word,l:word,h:word,src:far ptr,buff:far ptr

  push bp
  mov bp,sp
  push es di ds si cx ax bx dx

  les di,src
  lds si,buff
  imul cx,y,320
  add cx,x
  add di,cx
  mov ax,di
  mov bx,h
  mov dx,l
  shr dx,2
  cld
plk4h:
  mov cx,dx
  rep movsd
  add ax,320
  mov di,ax
  dec bx
  or bx,bx
  jnz plk4h

  pop dx bx ax cx si ds di es
  pop bp

  ret

_PutBlk4 endp

;*******************************************************
; _PutSpr(x:word,y:word,l:word,h:word,src:far ptr,buff:far ptr)
;
; Parametres :
;
; x : abscisse ecran
; y : ordonnee ecran
; l : largeur du bloc en pixels
; h : hauteur du bloc en pixels
; src : page dest.
; buff : buffer source
;
; Specification :
;
; Ecrit dans src aux coord x,y le buffer buff
; N'ecrit pas l'encre 0
;*******************************************************

_PutSpr proc far
  arg x:word,y:word,l:word,h:word,src:far ptr,buff:far ptr

  push bp
  mov bp,sp
  push es di ds si cx ax bx dx

  les di,src
  lds si,buff
  imul cx,y,320
  add cx,x
  add di,cx
  mov ax,di
  mov bx,h
psprh:
  mov cx,l
psprl:
  mov dl,ds:[si]
  or dl,dl
  jz psprl2       ; ne pas afficher l'encre 0
  mov es:[di],dl
psprl2:
  inc si
  inc di
  dec cx
  or cx,cx
  jnz psprl

  add ax,320
  mov di,ax
  dec bx
  or bx,bx
  jnz psprh

  pop dx bx ax cx si ds di es
  pop bp

  ret

_PutSpr endp

;*******************************************************
; _CpyBlk(xs:word,ys:word,l:word,h:word,xd:word,yd:word,src:far ptr,dst:far ptr)
;
; Parametres :
;
; xs : abscisse source
; ys : ordonnee source
; xd : abscisse dest.
; yd : ordonnee dest.
; l : largeur du bloc en pixels
; h : hauteur du bloc en pixels
; src : page source
; dest : page dest.
;
; Specification :
;
; Copie le bloc defini par xs,ys,l,h de src dans dst aux pos. xd,yd
;
; La copie se fait au maximum en long , les "restes" en octet
;
;*******************************************************

_CpyBlk proc far
  arg xs:word,ys:word,l:word,h:word,xd:word,yd:word,src:far ptr,dst:far ptr

  push bp
  mov bp,sp
  push es di ds si ax bx cx dx

  ; si h <= 0, ne rien faire
  cmp h,0
  jle endCpyBlk

  les di,dst
  lds si,src
  imul cx,ys,320
  add cx,xs
  add si,cx
  mov ax,si

  imul cx,yd,320
  add cx,xd
  add di,cx
  mov bx,di

  mov dx,h

  ; nbre de long a copier
  mov cx,l
  shr cx,2
  ; code genere
  mov word ptr cs:[cpyNbLong+1],cx
  ; nbre d'octets a copier
  mov cx,l
  and cx,3
  ; code genere
  mov word ptr cs:[cpyNbByte+1],cx

  cld

bcpyblk:

cpyNbLong:
  ; copier en long
  mov cx,0ffffh   ; NbLong
  rep movsd
cpyNbByte:
  ; puis en octet
  mov cx,0ffffh   ; NbByte
  rep movsb

  add ax,320
  mov si,ax
  add bx,320
  mov di,bx

  dec dx
  or dx,dx
  jnz bcpyblk

endCpyBlk:
  pop dx cx bx ax si ds di es
  pop bp
  ret

_CpyBlk endp

;*******************************************************
; _CpyBlk1(xs:word,ys:word,l:word,h:word,xd:word,yd:word,src:far ptr,dst:far ptr)
;
; Parametres :
;
; xs : abscisse source
; ys : ordonnee source
; xd : abscisse dest.
; yd : ordonnee dest.
; l : largeur du bloc en pixels
; h : hauteur du bloc en pixels
; src : page source
; dest : page dest.
;
; Specification :
;
; Copie le bloc defini par xs,ys,l,h de src dans dst aux pos. xd,yd
;
; La copie se fait octet par octet
;
;*******************************************************

_CpyBlk1 proc far
  arg xs:word,ys:word,l:word,h:word,xd:word,yd:word,src:far ptr,dst:far ptr

  push bp
  mov bp,sp
  push es di ds si ax bx cx dx

  les di,dst
  lds si,src
  imul cx,ys,320
  add cx,xs
  add si,cx
  mov ax,si

  imul cx,yd,320
  add cx,xd
  add di,cx
  mov bx,di

  mov dx,h
  cld

bcpyblk1:

  mov cx,l
  rep movsb
  add ax,320
  mov si,ax
  add bx,320
  mov di,bx

  dec dx
  or dx,dx
  jnz bcpyblk1

  pop dx cx bx ax si ds di es
  pop bp
  ret

_CpyBlk1 endp

;*******************************************************
; _CpyBlk2(xs:word,ys:word,l:word,h:word,xd:word,yd:word,src:far ptr,dst:far ptr)
;
; Parametres :
;
; xs : abscisse source
; ys : ordonnee source
; xd : abscisse dest.
; yd : ordonnee dest.
; l : largeur du bloc en pixels
; h : hauteur du bloc en pixels
; src : page source
; dst : page dest.
;
; Specification :
;
; Copie le bloc defini par xs,ys,l,h de src dans dst aux pos. xd,yd
; La copie se fait mot par mot (16 bits)
;
;*******************************************************

_CpyBlk2 proc far
  arg xs:word,ys:word,l:word,h:word,xd:word,yd:word,src:far ptr,dst:far ptr

  push bp
  mov bp,sp
  push es di ds si ax bx cx dx

  les di,dst
  lds si,src
  imul cx,ys,320
  add cx,xs
  add si,cx
  mov ax,si

  imul cx,yd,320
  add cx,xd
  add di,cx
  mov bx,di

  mov dx,h
  cld

bcpyblk2:

  mov cx,l
  shr cx,1
  rep movsw
  add ax,320
  mov si,ax
  add bx,320
  mov di,bx

  dec dx
  or dx,dx
  jnz bcpyblk2

  pop dx cx bx ax si ds di es
  pop bp
  ret

_CpyBlk2 endp


;*******************************************************
; _CpyBlk4(xs:word,ys:word,l:word,h:word,xd:word,yd:word,src:far ptr,dst:far ptr)
;
; Parametres :
;
; xs : abscisse source
; ys : ordonnee source
; xd : abscisse dest.
; yd : ordonnee dest.
; l : largeur du bloc en pixels
; h : hauteur du bloc en pixels
; src : page source
; dst : page dest.
;
; Specification :
;
; Copie le bloc defini par xs,ys,l,h de src dans dst aux pos. xd,yd
; La copie se fait long mot par long mot (32 bits)
;
;*******************************************************

_CpyBlk4 proc far
  arg xs:word,ys:word,l:word,h:word,xd:word,yd:word,src:far ptr,dst:far ptr

  push bp
  mov bp,sp
  push es di ds si ax bx cx dx

  les di,dst
  lds si,src
  imul cx,ys,320
  add cx,xs
  add si,cx
  mov ax,si

  imul cx,yd,320
  add cx,xd
  add di,cx
  mov bx,di

  mov dx,h
  cld

bcpyblk4:

  mov cx,l
  shr cx,2
  rep movsd
  add ax,320
  mov si,ax
  add bx,320
  mov di,bx

  dec dx
  or dx,dx
  jnz bcpyblk4

  pop dx cx bx ax si ds di es
  pop bp
  ret

_CpyBlk4 endp

;*******************************************************
; _CpySpr(xs:word,ys:word,l:word,h:word,xd:word,yd:word,src:far ptr,dst:far ptr)
;
; Parametres :
;
; xs : abscisse source
; ys : ordonnee source
; xd : abscisse dest.
; yd : ordonnee dest.
; l : largeur du bloc en pixels
; h : hauteur du bloc en pixels
; src : page source
; dest : page dest.
;
; Specification :
;
; Copie le bloc source dans dest aux pos xd,yd
; N'ecrit pas l'encre 0
;
;*******************************************************

_CpySpr proc far
  arg xs:word,ys:word,l:word,h:word,xd:word,yd:word,src:far ptr,dst:far ptr

  push bp
  mov bp,sp
  push es di ds si ax bx cx dx

  les di,dst
  lds si,src
  imul cx,ys,320
  add cx,xs
  add si,cx
  mov ax,si

  imul cx,yd,320
  add cx,xd
  add di,cx
  mov bx,di

  mov dx,h
  cld

bcpyspr1:   ; boucle sur les lignes

; boucle pour tracer une ligne

  mov cx,l
  push ax   ; sauver le reg. ax
bsprl:
  mov al,ds:[si]
  or al,al
  jz bsprl2       ; ne pas afficher l'encre 0
  mov es:[di],al
bsprl2:
  inc si
  inc di
  dec cx
  or cx,cx
  jnz bsprl
  pop ax    ; apres la boucle sur les lignes, on recupere ax

  add ax,320     ; passer aux lignes suivantes
  mov si,ax
  add bx,320
  mov di,bx

  dec dx
  or dx,dx
  jnz bcpyspr1

  pop dx cx bx ax si ds di es
  pop bp
  ret

_CpySpr endp

;*******************************************************
; LineMapH(x1,y1,x2,ecr1,x3,y3,l,ecr2)
;
; Parametres :
;
; X1,Y1 = point de depart
; X2    = point d'arrivee
; ECR1  = adresse ecran correspondante
; X3,Y3 = coordonnes de la ligne horizontale
; L     = longueur de la ligne horizontale
; ECR2  = adresse ecran correspondante
;
; Specification :
;
; trace une ligne horizontale a patir d'une ligne
; horizontale prise dans une image (map)
; tests des debordements en x et y
;
;*******************************************************

_LineMapH proc far
  arg x1:dword,y1:dword,x2:dword,ecr1:far ptr,x3:dword,y3:dword,L:dword,ecr2:far ptr
  push  bp
  mov   bp,sp
  pushad
  push  es ds fs

  ; si L <= 0, return
  cmp     L,0
  jle     endligmaph

  ; si y3 < 0, return
  mov     ecx,y3
  cmp     ecx,0
  jl      endligmaph

  ; si y3 > 199, return
  cmp     ecx,199
  jg      endligmaph

lignemap2h:

  mov   ecx,x1
  mov   eax,x2
  sub   eax,ecx   ; x2-x1
  shl   eax,7
  cdq
  idiv  L
  mov   fs,eax   ; v1 = ((x2-x1)<<7)/L

  ; x
  mov   edx,ecx   ; x1<<7
  shl   edx,7

  mov   ecx,L    ; on va afficher L pixels horizontalement
  les   di,ecr2
  lds   si,ecr1
  imul  edi,y3,320  ; di = x3
  add   edi,x3

  ; code genere
  mov word ptr cs:[addfs+3],fs

boucleMaph:

; test si debordement a droite en x
  cmp     x3,319
  jg      endligmaph

  imul  esi,y1,320

addfs:
  add   edx,0ffffh ; x1 += v1 => add edx,fs
  mov   eax,edx
  shr   eax,7
  add   esi,eax

; coul = get_pixel(x,y,ecr1)

; put_pixel(x3+i,y3,ecr2,coul)

; test si debordement a gauche
  cmp  x3,0
  jl   nextpix
  movsb         ; di=di+1

  inc   x3

  loop  boucleMaph  ; cx--

  jmp   endligmaph  ; ne pas executer la suite

nextpixh :  ; on arrive ici quand x3<0
             ; on calcule tout, mais on affiche rien

  inc   di
  inc   x3
  loop  boucleMaph  ; cx--

endligmaph :

  pop  fs ds es
  popad
  pop   bp
  ret

_LineMapH endp

;*******************************************************
; LineMap(x1,y1,x2,y2,ecr1,x3,y3,l,ecr2)
;
; Parametres :
;
; X1,Y1
; X2,Y2 = coordonnes de la ligne dans la map
; ECR1  = adresse ecran correspondante
; X3,Y3 = coordonnes de la ligne horizontale
; L     = longueur de la ligne horizontale
; ECR2  = adresse ecran correspondante
;
; Specification :
;
; Trace une ligne horizontale a patir d'une ligne
; quelconque prise dans une image (map)
; tests des debordements en x et y
;
;*******************************************************

_LineMap proc far
  arg x1:dword,y1:dword,x2:dword,y2:dword,ecr1:far ptr,x3:dword,y3:dword,L:dword,ecr2:far ptr
  push  bp
  mov   bp,sp
  pushad
  push  es ds fs gs

  ; si L <= 0, return
  cmp     L,0
  jle     endligmap

  ; si y3 < 0, return
  mov     ecx,y3
  cmp     ecx,0
  jl      endligmap

  ; si y3 > 199, return
  cmp     ecx,199
  jg      endligmap

lignemap2:

  mov   ecx,x1
  mov   eax,x2
  sub   eax,ecx   ; x2-x1
  shl   eax,7
  cdq
  idiv  L
  mov   fs,eax   ; v1 = ((x2-x1)<<7)/L

  mov   ebx,y1
  mov   eax,y2
  sub   eax,ebx    ; y2-y1
  shl   eax,7
  cdq
  idiv  L
  mov   gs,eax   ; w1 = ((y2-y1)<<7)/L

  ; y
  shl   ebx,7    ; y1<<7

  ; x
  mov   edx,ecx   ; x1<<7
  shl   edx,7

  mov   ecx,L    ; on va afficher L pixels horizontalement
  les   di,ecr2
  lds   si,ecr1
  imul  edi,y3,320  ; di = x3
  add   edi,x3

boucleMap:

; test si debordement a droite en x
  cmp     x3,319
  jg      endligmap

  mov   eax,gs     ; y1 += w1
  add   ebx,eax
  mov   eax,ebx
  shr   eax,7
  imul  esi,eax,320

  mov   eax,fs     ; x1 += v1
  add   edx,eax
  mov   eax,edx
  shr   eax,7
  add   esi,eax

; coul = get_pixel(x,y,ecr1)

; put_pixel(x3+i,y3,ecr2,coul)

; test si debordement a gauche
  cmp  x3,0
  jl   nextpix
  movsb         ; di=di+1

  inc   x3

  loop  boucleMap  ; cx--

  jmp   endligmap  ; ne pas executer la suite

nextpix :  ; on arrive ici quand x3<0
           ; on calcule tout, mais on affiche rien

  inc   di
  inc   x3
  loop  boucleMap  ; cx--

endligmap :

  pop   gs fs ds es
  popad
  pop   bp
  ret

_LineMap endp

 END
