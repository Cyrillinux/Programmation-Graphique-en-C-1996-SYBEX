
/* ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
   ³                                WISPAL                                  ³
   ³                        Siracusa walter\wiSdom                          ³
   ³                              (c)1995/96                                ³
   ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*/

#define TAILLE_IMG_MAX 400000
#define BUF_PCX 100

#define PAS_TXT 9
#define PAS_SCR 4

#define DEL 8
#define ESC 27
#define RET 13
#define SPACE 32
#define HOME 71
#define HAUT 72
#define PGUP 73
#define GAUCHE 75
#define DROITE 77
#define END 79
#define BAS 80
#define PGDW 81
#define INSERT 82
#define SUPPR 83
#define F1 59
#define F2 60
#define F3 61
#define F4 62
#define F5 63
#define F6 64
#define F7 65
#define F8 66
#define F9 67
#define F10 68
#define SF9 92
#define SF10 93

enum C_Img{NON,OUI};
enum C_Pal{NORMAL,TRANSPARENCE,TRANSCRIT,MOYENNE};
enum C_Load{RIEN,IMAGE,PALETTE};
enum C_Txt{A,N,AN};

void SetPalette(void);
void LoadFnt(char *path);
void CpyMemVid(byte huge *src,bytef *dst,word x,word y);
void InitWisPal(void);
void PrintTxt(int pos_x,int pos_y,char *chaine,byte coul_t,char coul_f,bytef *scr);
char ScanTxt(int x,int y,char *chaine,int ct,int cf,int l,byte t,bytef *scr);
void KeyTxt(int x,int y,char *txt,byte c_t,char c_f,bytef *scr);
void InitScreen(void);
byte LoadPCXMem(char *path,byte huge *scr,byte *pal);
byte SaveMemPCX(char *path,byte huge *scr,byte *pal);
void Efface(byte ed,byte ef);
void Deplace(byte ed,byte ef,byte d);
void Copie(byte ed,byte ef,byte d);
void Supprime(byte ed,byte ef);
void Insert(byte ed,byte ef,byte d);
void Echange(byte ed,byte ef,byte d);
void RotHaut(byte ed,byte ef);
void RotBas(byte ed,byte ef);
void Fondu(byte ed,byte ef,word p);
byte Degrade(byte ed,byte ef);
byte SearchInk(byte r,byte v,byte b,bytef *p,byte fin);
void Organise(void);
void Gris(byte ed,byte ef);
void NoirBlanc(byte ed,byte ef,byte s);
void CorrespI(word p);
void CorrespP(byte ed,byte ef,word p);
void Negatif(byte ed,byte ef);
void Solarise(byte ed,byte ef);
void NormPal(bytef *pl1);
void TrspPal(byte *pl,bytef *pl1,byte ed,byte ef);
void TrscPal(bytef *pl,bytef *pl1);
void MoyPal(bytef *pl,bytef *pl1,byte ed,byte ef);
void Tri(word s);
void Reduction(word nb);
dword UseInk(byte ink);
void Modifie(byte ed,byte ef,byte val);
void Resize(bytef *scr,byte mode);
void DecRVB(byte ed,byte ef,byte rvb);
void IncRVB(byte ed,byte ef,byte rvb);
void Inverse(byte ed,byte ef);
void SavePal(char *path);
void LoadPal(char *path);
void Aide(void);
int ReadKey(byte *bool);
void main(int argc,char **argv);
