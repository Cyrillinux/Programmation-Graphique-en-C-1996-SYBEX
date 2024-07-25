COMMENT\  ROUTINES utilisees en 3D, mode MCGA 320x200x256
            *********
        (C) 1995 Patrice Siracusa
            *********
       \

JUMPS
.ALPHA
LOCALS
  .MODEL HUGE
  .386

  .stack 0400h   ; PILE DE 1 Ko
;-------------------------------------------------------
  .DATA
  EVEN

;=======================================================
  .CODE
  EVEN
;-------------------------------------------------------

public _ligneh           ;trace optimise pour les lignes horizontales

public _lignemap         ;trace une ligne horiz. a partir d'une ligne quelconque
                         ;=> mapping
                         ;=> mapping + pas de tests de debordement

public _lignemapflat     ;trace une ligne horiz. a partir d'une ligne quelconque
                         ;=> mapping + flat-shading

;-----------------------------------------------------------------------------
  EVEN

;**************************************************************
;                        PROCEDURE ligneh
;**************************************************************
; Entree :  XD     =  abscisse de depart
;            XA     =  abscisse d'arrivee
;            YC     =  ordonne constante
;           ECRAN  =  adresse d'une page
;            COUL   =  couleur du trace
;**************************************************************
;  Role : trace d'une ligne horizontale
;
; - ne trace pas les lignes non visibles(y<0 ou y>199)
; - clipping des pixels non visibles (x<0 ou x>319)
; - precondition : xd<=xa
;**************************************************************

_ligneh proc far
  arg xd:word,xa:word,yc:word,ecran:far ptr,coul:byte

  push    bp
  mov     bp,sp
  push    es di ax cx bx dx

; test si yc < 0, return
  mov      cx,yc
  cmp      cx,0
  jl      endlh

; test si yc > 199, return
  cmp      cx,199
  jg      endlh

; bx=xd, dx=xa
  mov      bx,xd
  mov      dx,xa

; test si xd < 0, xd = 0
  cmp      bx,0
  jge      contlh1
  xor      bx,bx   ; bx = 0

contlh1:
; test si xd > 319, xd = 319
  cmp      dx,319
  jle      contlh2
  mov      dx,319

contlh2:
; test si xd <= xa return
  cmp     bx,dx
  jg      endlh

  cld
  les     di,ecran        ; ES:DI=adresse ecran
  imul    di,cx,320       ; cx == yc
                          ; en vrai :
                          ;  imul    ax,yc,320
                          ;  add     di,ax

  add     di,bx           ; DI=offset ou on doit afficher le point
  mov     al,coul

  mov     cx,dx
  sub     cx,bx
  inc     cx              ; cx = nb pixels a afficher

  rep stosb           ; ES:DI=coul

endlh:
  pop     dx bx cx ax di es bp

  ret

_ligneh endp
;-----------------------------------------------------------------
  EVEN

;*****************************************************************
; lignemap
;
; Entree : X1,Y1
;           X2,Y2 = coordonnes de la ligne dans la map
;           ECR1  = adresse ecran correspondante
;           X3,Y3 = coordonnes de la ligne horizontale
;           L     = longueur de la ligne horizontale
;           ECR2  = adresse ecran correspondante
;
; Role :   trace une ligne horizontale a patir d'une ligne
;           quelconque prise dans une image (map)
;          tests des debordements en x et y
;****************************************************************

_lignemap proc far
  arg x1:word,y1:word,x2:word,y2:word,ecr1:far ptr,x3:word,y3:word,L:word,ecr2:far ptr
  push   bp
  mov   bp,sp
  pusha
  push   es ds fs gs

  ; si L <= 0, return
  cmp     L,0
  jle     endligmap2

  ; si y3 < 0, return
  mov      cx,y3
  cmp      cx,0
  jl      endligmap2

  ; si y3 > 199, return
  cmp      cx,199
  jg      endligmap2

  ;begin
  mov   cx,x1
  mov   ax,x2
  sub   ax,cx   ; x2-x1
  shl   ax,7
  cwd
  idiv   L
  mov   fs,ax   ; v1 = ((x2-x1)<<7)/L

  mov   bx,y1
  mov   ax,y2
  sub   ax,bx    ; y2-y1
  shl   ax,7
  cwd
  idiv   L
  mov   gs,ax   ; w1 = ((y2-y1)<<7)/L

  ; y
  shl   bx,7    ; y1<<7

  ; x
  mov   dx,cx   ; x1<<7
  shl   dx,7

  mov   cx,L    ; on va afficher L pixels horizontalement
  les   di,ecr2
  lds   si,ecr1
  imul   di,y3,320  ; di = x3
  add    di,x3

  ; code genere
  mov  word ptr cs:[addgs+2],gs
  mov  word ptr cs:[addfs+2],fs

boucleMap2:

; test si debordement a droite en x
  cmp      x3,319  ; x3 > 319
  jg      endligmap2

addgs:
  add   bx,0ffffh   ; y1 += w1 => add bx,gs
  mov   ax,bx
  shr   ax,7
  imul   si,ax,320

addfs:
  add   dx,0ffffh   ; x2 += v1 => add dx,fs
  mov   ax,dx
  shr   ax,7
  add   si,ax

; coul = get_pixel(x,y,ecr1)

; put_pixel(x3+i,y3,ecr2,coul)

; test si debordement a gauche
  cmp  x3,0  ; x3 < 0
  jl   nextpix2
  movsb          ; di=di+1

  inc   x3   ; x3++

  loop  boucleMap2  ; cx--

  jmp   endligmap2  ; ne pas executer la suite

nextpix2 :  ; on arrive ici quand x3<0
           ; on calcule tout, mais on affiche rien

  inc   di
  inc   x3  ; x3++
  loop  boucleMap2  ; cx--

endligmap2 :

  pop   gs fs ds es
  popa
  pop   bp
  ret

_lignemap endp

;------------------------------------------------------------------------------
   EVEN

;*****************************************************************
; lignemapflat
;
; Entree : X1,Y1
;           X2,Y2 = coordonnes de la ligne dans la map
;           ECR1  = adresse ecran correspondante
;           X3,Y3 = coordonnes de la ligne horizontale
;           L     = longueur de la ligne horizontale
;          COSA  = coef. d'eclairage [0..15]
;           ECR2  = adresse ecran correspondante
;
; Role :   trace une ligne horizontale a patir d'une ligne
;           quelconque prise dans une image (map)
;          tests des debordements en x et y
;****************************************************************

_lignemapflat proc far
  arg x1:word,y1:word,x2:word,y2:word,ecr1:far ptr,x3:word,y3:word,L:word,cosa:word,ecr2:far ptr
  push   bp
  mov   bp,sp
  pushad ; 32 bits
  push   es ds fs gs

  ; si L <= 0, return
  cmp     L,0
  jle     endligmapflat

  ; si y3 < 0, return
  mov      cx,y3
  cmp      cx,0
  jl      endligmapflat

  ; si y3 > 199, return
  cmp      cx,199
  jg      endligmapflat

lignemap2flat:

  mov   cx,x1
  mov   ax,x2
  sub   ax,cx   ; x2-x1
  shl   ax,7
  cwd
  idiv   L
  mov   fs,ax   ; v1 = ((x2-x1)<<7)/L

  mov   bx,y1
  mov   ax,y2
  sub   ax,bx    ; y2-y1
  shl   ax,7
  cwd
  idiv   L
  mov   gs,ax   ; w1 = ((y2-y1)<<7)/L

  ; y
  shl   bx,7    ; y1<<7

  ; x
  mov   dx,cx   ; x1<<7
  shl   dx,7

  mov   cx,L    ; on va afficher L pixels horizontalement
  les   di,ecr2
  lds   si,ecr1
  imul   di,y3,320  ; di = x3
  add    di,x3

  ; code genere
  mov  word ptr cs:[addfsmflat+2],fs
  mov  word ptr cs:[addgsmflat+2],gs

boucleMapFlat:

; test si debordement a droite en x
  cmp      x3,319
  jg      endligmapflat

addgsmflat:
  add   bx,0ffffh  ; y1 += w1 => add bx,gs
  mov   ax,bx
  shr   ax,7
  imul   si,ax,320

addfsmflat:
  add   dx,0ffffh  ; x1 += v1 => add ds,fs
  mov   ax,dx
  shr   ax,7
  add   si,ax

; calcul de la nlle couleur
; formule generale
;  e1 = (coul>>4)<<4;
;  nind = ((long)coul-(long)e1)*(long)cosa;
;  nind += e1<<4;

; pour ce calcul, on utilise 3 registres qu'on protege
;  push ebx
;  push ecx
;  push edx

;  lodsb       ; al = coul = DS:[SI]
;  mov  ebx,eax
;  shr  eax,4
;  shl  eax,4
;  mov  ecx,eax
;  sub  ebx,eax
;  mov  eax,ebx
;  mul  cosa
;  mov  ebx,eax
;  mov  eax,ecx
;  shl  eax,4
;  add  ebx,eax
;  shr  ebx,4
;  mov  al,bl

; restaurer les valeurs des 3 registres
;  pop edx
;  pop ecx
;  pop ebx

; formule adaptee
; Seuls les derniers niveaux de chaque degrades sont utilises

  lodsb       ; al = coul = DS:[SI]
  sub ax,15
  add ax,cosa

; test si debordement a gauche
  cmp  x3,0
  jl   nextpixflat

; affectation de la nlle couleur
  mov es:[di],al
  inc di

  inc   x3

  loop  boucleMapFlat  ; cx--

  jmp   endligmapflat  ; ne pas executer la suite

nextpixflat :  ; on arrive ici quand x3<0
           ; on calcule tout, mais on affiche rien

  inc   di
  inc   x3
  loop  boucleMapFlat  ; cx--

endligmapflat :

  pop   gs fs ds es
  popad ; 32 bits
  pop   bp
  ret

_lignemapflat endp

;------------------------------------------------------------------------------
   EVEN

 END
