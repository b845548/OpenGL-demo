/*!\file window.c
 *
 * \brief Sphère en GL / GL4Dummies + Lumière positionnelle + Phong
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date October 30 2014, modified on Februry 02 2016
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4df.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL_image.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "glm/glm.h"
#include "animations.h"

/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void         initGL(void);
static void         initData(void);
static void         resize(int w, int h);
static void         keyup(int keycode);
static void         keydown(int keycode);
void draw_superhot(void);
static GLuint *     mkSphereIndex(GLuint longitudes, GLuint latitudes);
static GLfloat *    mkSphereVertices(GLuint longitudes, GLuint latitudes);
static void         quit(void);

/*!\brief dimensions de la fenêtre */
static int _windowWidth = 800, _windowHeight = 600;
/*!\brief identifiant du (futur) vertex array object */
static GLuint _vao = 0;
/*!\brief identifiant du buffer de data de la sphère indexée */
static GLuint _ibuffer[2] = {0, 0};
/*!\brief identifiant des GLSL program */
static GLuint _pId = 0, _pId2 = 0;
/*!\brief nombre de longitudes et latitudes de la sphere */
static GLuint _longitudes = 10, _latitudes = 10;
/*!\brief arrete l'animation */
static GLuint _pause = 0;
/*!\brief normale à la face ou au point ? */
static GLuint _faceNormal = 0;

static GLuint _body = 0,_drawBody = 0;
/*!\brief flag pour Phong ou pas */
static GLuint _phong = 0;
/*!\brief flag pour savoir si la touche shift est enfoncée */
static GLuint _shift = GL_FALSE;
/*!\brief flag pour afficher les coordonnées de texture */
static GLuint _tc = 0;
static int blurMode=1;
/*!\brief position de la lumière relativement à la sphère éclairée */
static GLfloat _lumPos0[4] = {1.1, 10, 100, 1.0};
static GLuint _sphere = 0,_cone = 0,_quad = 0,_cube=0;
///// FONT 

TTF_Font* font;
TTF_Font* petitfont;

  SDL_Surface* resulting_text;
/////////////////////////////////////
static int * im=NULL;
Uint32 * pixels=NULL;
#define TEXW 1000
#define TEXH 1000


typedef struct cam_t cam_t;
/*!\brief structure de données pour la caméra */
struct cam_t {
  GLfloat x, y,z;
  GLfloat theta;
};

/*!\brief la caméra */
static cam_t _cam = {0, 0, 0};

/////////////////////////////////////////
/// MODIF
static GLuint _texture=0,_texture2=0,_texture3=0;
/// MODIF
  
/*!\brief La fonction principale créé la fenêtre d'affichage,
 * initialise GL et les données, affecte les fonctions d'événements et
 * lance la boucle principale d'affichage.
 */
enum objetType {// Pour ajouter l'objet suivent
OBJ_SUPERHOT = 0,
OBJ_HOUSE=1,
OBJ_MP5=2
};


#define OBJMAX 5
#define OBJNB 3// Pour ajouter un objet,augumenter la valeur +1
static char *OBJFilePath[]={"Objet3D/Basemesh_arshlevon_shoes.obj",
"Objet3D/interior.obj",
"Objet3D/MP5K/MP5K.obj",
		
};

static GLMmodel *_tabModel[OBJMAX];
static GLuint _objTabVao[OBJMAX],_objTabBD[OBJMAX],_objTabBI[OBJMAX];

static void HBB_objLoader7(void){
int i,j,lire;

///////////// Initialiser Vao BD BI Model ///////////////
for(lire=0;lire<OBJNB;lire++){
	_tabModel[lire]=NULL;
	_objTabVao[lire]=0;
	_objTabBD[lire]=0;
	_objTabBI[lire]=0;
}
///////////// Verifier des erreur ///////////////
if(lire!=OBJNB){
	fprintf(stderr,"ERREUR : OBJFilePath contient %d path, OBJNB est %d\n",lire,OBJNB);
	fprintf(stderr,"ERREUR : #define OBJNB <- augumenter cette valeur\n");
	exit(1);
}
if(OBJNB>OBJMAX){
	fprintf(stderr,"ERREUR : OBJNB : %d, OBJMAX = %d \n",OBJNB,OBJMAX);
	fprintf(stderr,"ERREUR : #define OBJMAX <- augumenter cette valeur \n");
	exit(1);
}



///////////// Lire les objets ///////////////

for(lire=0;lire<OBJNB;lire++){
if((_tabModel[lire] = glmReadOBJ(OBJFilePath[lire]))==NULL){
      fprintf(stderr, "ERREUR : Impossible d'ouvrir le fichier : %s \n",OBJFilePath[lire]);
      exit(1);
}


 GLfloat * allData = NULL;
  GLuint * ti3= NULL;

//////////// modified ////////////
printf("vrtxnb %d txnb :%d\n",_tabModel[lire]->numvertices,_tabModel[lire]->numtexcoords);
int TEXNB=2;
  glmVertexNormals(_tabModel[lire], 90, GL_FALSE);
  allData = malloc(3*2* (_tabModel[lire]->numvertices + 1+_tabModel[lire]->numtexcoords) * sizeof *allData);
  assert(allData);
  memcpy(allData, _tabModel[lire]->vertices, 3 * _tabModel[lire]->numvertices * sizeof *allData);
  for(i = 0; i < (int)_tabModel[lire]->numtriangles; i++) {
    GLMtriangle * triangle = &(_tabModel[lire]->triangles[i]);
    for(j = 0; j < 3; j++)
      memcpy(&allData[3 * _tabModel[lire]->numvertices + 3 * triangle->vindices[j]], & _tabModel[lire]->normals[3 * triangle->nindices[j]], 3 * sizeof *allData);
  }
//////////// modified ////////////
for(i=0;i<_tabModel[lire]->numtexcoords*2;i++)
	allData[(6*_tabModel[lire]->numvertices)+i]=_tabModel[lire]->texcoords[i];
 printf("Bien charge les models\n");

//////////// modified ////////////

  if(!_objTabVao[lire])
    glGenVertexArrays(1, &_objTabVao[lire]);
  glBindVertexArray(_objTabVao[lire]);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);

  if(!_objTabBD[lire])
    glGenBuffers(1, &(_objTabBD[lire]));
  if(!_objTabBI[lire])
    glGenBuffers(1, &(_objTabBI[lire]));
  glBindBuffer(GL_ARRAY_BUFFER,(_objTabBD[lire]));
  glBufferData(GL_ARRAY_BUFFER,/*modified*/TEXNB*3* (2 * _tabModel[lire]->numvertices) * sizeof *allData, allData, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
  glVertexAttribPointer(1, 3, GL_FLOAT,  GL_TRUE, 0, (const void *)(3 *_tabModel[lire]->numvertices * sizeof *allData));
  glVertexAttribPointer(2, 2, GL_FLOAT,  GL_TRUE, 0, (const void *)(6 *_tabModel[lire]->numvertices * sizeof *allData));
  free(allData);



  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _objTabBI[lire]);
  ti3 = malloc(_tabModel[lire]->numtriangles/**/ * 3 * sizeof *ti3);
  assert(ti3);

for(i = 0; i < (int)_tabModel[lire]->numtriangles/**/; i++) {
GLMtriangle * triangle = &(_tabModel[lire]->triangles[i]);
memcpy(&ti3[3 * i], triangle->vindices, 3 * sizeof *ti3);
ti3[3 * i] = triangle->vindices[0];
ti3[3 * i + 1] = triangle->vindices[1];
ti3[3 * i + 2] = triangle->vindices[2];
  }

  glBufferData(GL_ELEMENT_ARRAY_BUFFER,_tabModel[lire]->numtriangles * 3 * sizeof *ti3, ti3, GL_STATIC_DRAW);
  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  free(ti3);
}
}



/*

int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "GL4Dummies", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
			 _windowWidth, _windowHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN))
    return 1;
  initGL();
  initData();
  atexit(quit);
  gl4duwResizeFunc(resize);
  gl4duwKeyUpFunc(keyup);
  gl4duwKeyDownFunc(keydown);
  gl4duwDisplayFunc(draw_superhot);
  gl4duwMainLoop();
  return 0;
}
*/
void te(int * im, int x0, int y0, int w, int h, int width){
	int p[9][2];
	int i,v,w_2=w>>1,h_2=h>>1,w_21=w_2+(w&1),h_21=h_2+(h&1);
	float ri = MAX(w_2,h_2)/(float)width;
	p[0][0]=x0; p[0][1]=y0; 
	p[1][0]=x0+w; p[1][1]=y0; 
	p[2][0]=x0+w; p[2][1]=y0+h; 
	p[3][0]=x0; p[3][1]=y0+h; 
	p[4][0]=x0+w_2; p[4][1]=y0; 
	p[5][0]=x0+w; p[5][1]=y0+h_2;
	p[6][0]=x0+w_2; p[6][1]=y0+h; 
	p[7][0]=x0; p[7][1]=y0+h_2; 
	p[8][0]=x0+w_2; p[8][1]=y0+h_2;
	
	for(i=4;i<8;i++){
		int a1=i-4,a2=(i-3)%4;
		v=p[i][0]+p[i][1]*width;
		if(im[v]>0)continue;
		im[v]=(im[p[a1][0]+p[a1][1]*width]+im[p[a2][0]+p[a2][1]*width])/2;
		im[v]+= (2.0 * (rand() / (RAND_MAX + 1.0)) - 1.0) * ri * 256;
		im[v]=MIN(MAX(im[v],1),255);
	}
	if(im[(i=p[8][0]+p[8][1]*width)]<1){
		im[i]=(im[p[0][0]+p[0][1]*width]+im[p[1][0]+p[1][1]*width]+
		im[p[2][0]+p[2][1]*width]+im[p[3][0]+p[3][1]*width])/4;
		im[i]+= (2.0 * (rand() / (RAND_MAX + 1.0)) - 1.0) * sqrt(2) * ri * 256;
		im[i]=MIN(MAX(im[i],1),255);
	}
	if(w_2>1||h_2>1)
	te(im,p[0][0],p[0][1],w_2,h_2,width);
	if(w_21>1||h_2>1)
	te(im,p[4][0],p[4][1],w_21,h_2,width);
	if(w_21>1||h_21>1)
	te(im,p[8][0],p[8][1],w_21,h_21,width);
	if(w_2>1||h_21>1)
	te(im,p[7][0],p[7][1],w_2,h_21,width);
		
}
/*!\brief initialise les paramètres OpenGL */
static void initGL(void) {
 glClearColor(0, 0.680375/*(x=gl4dmSURand()*1)*/, 1, 1);
  _pId  = gl4duCreateProgram("<vs>shaders/SHDS/basic.vs", "<gs>shaders/SHDS/basic.gs", "<fs>shaders/SHDS/basic.fs", NULL);
    _pId2 = gl4duCreateProgram("<vs>shaders/SHDS/basic2.vs", "<fs>shaders/SHDS/basic2.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  resize(_windowWidth, _windowHeight);


}
/*!\brief initialise les données */
SDL_Surface * credit;
static void initData(void) {
credit=IMG_Load("Texture/fond1280x1000.png");
//credit=IMG_Load("neon.png");
  GLfloat * idata = NULL;
  GLuint * index = NULL;
  idata = mkSphereVertices(_longitudes, _latitudes);
  index = mkSphereIndex(_longitudes, _latitudes);
  if(!_vao)
    glGenVertexArrays(1, &_vao);
  glBindVertexArray(_vao);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  if(!_ibuffer[0])
    glGenBuffers(2, _ibuffer);
  /* sphère où tous les sommets ne sont présent qu'une fois */
  glBindBuffer(GL_ARRAY_BUFFER, _ibuffer[0]);
  glBufferData(GL_ARRAY_BUFFER, 
	       5 * (_longitudes + 1) * (_latitudes + 1) * sizeof *idata, idata, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (5 * sizeof *idata), (const void *)0);  
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, (5 * sizeof *idata), (const void *)(3 * sizeof *idata));  
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibuffer[1]);
 glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
	       6 * _longitudes * _latitudes * sizeof *index, index, GL_STATIC_DRAW);
 free(idata);
  free(index);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

glGenTextures(1, &_texture3);
  glBindTexture(GL_TEXTURE_2D,_texture3);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
Uint32 tab[100];
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 10,10, 0, GL_BGRA, GL_UNSIGNED_BYTE, tab);
  glBindTexture(GL_TEXTURE_2D, 0);
  /// MODIF
glGenTextures(1, &_texture2);
  glBindTexture(GL_TEXTURE_2D,_texture2);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 10,10, 0, GL_BGRA, GL_UNSIGNED_BYTE, tab);
  glBindTexture(GL_TEXTURE_2D, 0);
  /// MODIF
 
glGenTextures(1, &_texture);
  glBindTexture(GL_TEXTURE_2D,_texture);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   pixels=(Uint32 *)malloc(TEXW* TEXH*sizeof(Uint32));
    memset(pixels, 0, TEXW* TEXH * sizeof(Uint32));
	im=(int *)malloc(TEXW* TEXH*sizeof(int));	
	
	int pos[4],i;
 	for(i=0;i<4;i++)
	   pos[i]=rand()%255+1;
	im[0]=pos[0];
	im[TEXW-1]=pos[1];
	im[TEXW*(TEXH-1)]=pos[2];
	im[TEXW*TEXH-1]=pos[3];
	te(im,0,0,TEXW-1,TEXH-1,TEXW);
	for(i=0;i<TEXW *TEXH;i++)
	   pixels[i]=(255<<24)|(/*(150-im[i])*/0<<16)|((150-im[i])<<8)|255;
#if __APPLE__
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEXW ,TEXH, 0, GL_RGBA, GL_UNSIGNED_BYTE,pixels);
#else
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEXW ,TEXH, 0, GL_RGBA, GL_UNSIGNED_BYTE,pixels);
#endif
  glBindTexture(GL_TEXTURE_2D, 0);


TTF_Init();

font=TTF_OpenFont("Font/Requiem.otf", 300);
petitfont=TTF_OpenFont("Font/cyberdynecondital.ttf", 30);
assert(font);
_cube =gl4dgGenSpheref(300,300);
  _quad = gl4dgGenQuadf();
_sphere = gl4dgGenSpheref(30, 30);
_cone=gl4dgGenConef(1000,100);
HBB_objLoader7();
}

/*!\brief Cette fonction paramétre la vue (viewport) OpenGL en
 * fonction des dimensions de la fenêtre.
 */
static void resize(int w, int h) {
  _windowWidth  = w;
  _windowHeight = h;
  glViewport(0, 0, _windowWidth, _windowHeight);
  /*
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.5, 0.5, -0.5 * _windowHeight / _windowWidth, 0.5 * _windowHeight / _windowWidth, 1.0, 1000.0);
  gl4duBindMatrix("modelViewMatrix");
  */
}

static void keyup(int keycode) {
  switch(keycode) {
  case SDLK_RSHIFT:
  case SDLK_LSHIFT:
    _shift = GL_FALSE;
    break;
  default:
    break;
  }
}

static void keydown(int keycode) {
  GLint v[2];

  switch(keycode) {
  case SDLK_UP:
    if(_shift) {
      _lumPos0[2] -= 0.1;
    } else {
      _latitudes++;
    }
    break;
  case SDLK_DOWN:

    if(_shift) {
      _lumPos0[2] += 0.1;
    } else {
      if(_latitudes > 2) {
	_latitudes--;
      }
    }
    break;
  case SDLK_RIGHT:

    if(_shift) {
      _lumPos0[0] += 0.1;
    } else {
      _longitudes++;
    }
    break;
  case SDLK_LEFT:

    if(_shift) {
      _lumPos0[0] -= 0.1;
    } else {
      if(_longitudes > 3) {
	_longitudes--;
      }
    }
    break;
  case 'u':
    _lumPos0[1] += 0.1;
    break;
  case 'd':
    _lumPos0[1] -= 0.1;
    break;
  case 'w':
    glGetIntegerv(GL_POLYGON_MODE, v);
    if(v[0] == GL_FILL)
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    break;
  case ' ':
    _pause = !_pause;
    break;
  case 'p':
    _phong = !_phong;
    break;
  case 't':
    _tc = !_tc;    
break;
  case 'b':
blurMode=-blurMode;
    break;
  case 'f':
    _faceNormal = !_faceNormal;
    break;
  case SDLK_ESCAPE:
  case 'q':
    exit(0);
  case SDLK_RSHIFT:
  case SDLK_LSHIFT:
    _shift = GL_TRUE;
    break;
  default:
    break;
  }
}
typedef struct enemie enemie;
struct enemie{
float x,y,z;
float bulletX,bulletY,bulletZ;
float theta;
float scale;
};


static void HBB_missileTracking(GLfloat x0,GLfloat y0,GLfloat z0,GLfloat x1,GLfloat y1,GLfloat z1,GLfloat *theta,GLfloat *thetaHauter){
#define HBB_PI 3.14159265
// x0 y0 z0 position defense turret
// y1 y1 z1 position target
// theta est angle a tourner
//double x0=0,x1=10,
//		z0=0,z1=10;
GLfloat absX=abs(x0-x1),absZ=abs(z0-z1);
// DROIT HAUT
if(x0>=x1&&z0>=z1)
	*theta=acos(absZ/sqrt(pow(absX,2)+pow(absZ,2)));
// DROIT BAS
if(x0>=x1&&z0<z1)
	*theta=asin(absZ/sqrt(pow(absX,2)+pow(absZ,2)))+(HBB_PI/2);
// GAUCHE HAUT
if(x0<x1&&z0<z1)
	*theta=acos(absZ/sqrt(pow(absX,2)+pow(absZ,2)))+HBB_PI;
// GAUCHE BAS
if(x0<x1&&z0>=z1)
	*theta=asin(absZ/sqrt(pow(absX,2)+pow(absZ,2)))+(HBB_PI/2)+HBB_PI;
//double y0=0,y1=10;
GLfloat absXZ=sqrt(absX*absX+absZ*absZ); //DIAGONALE ENTRE X Z
GLfloat absY=abs(y0-y1); // DISTANCE ABSOLUE DE Y

if(y0<y1)
	*thetaHauter=asin(absY/sqrt(absXZ*absXZ+absY*absY));
else
	*thetaHauter=-asin(absY/sqrt(absXZ*absXZ+absY*absY));
}

/*!\brief Cette fonction dessine dans le contexte OpenGL actif.
 */
/*
static void HBB_flip(Uint32 tab[],int tabW,int tabH){
  Uint32 tmp;
  int colonne,ligne;
  for(ligne=0;ligne<tabH/2;ligne++)
    for(colonne=0;colonne<tabW;colonne++){
      tmp=tab[colonne+ligne*tabW];
      tab[colonne+ligne*tabW]=tab[colonne+(tabH-ligne-1)*tabW];
      tab[colonne+(tabH-ligne-1)*tabW]=tmp;
    }
}
*/



#define NBTEXT 73
static SDL_Surface* resulting_textTab[NBTEXT];

static SDL_Surface * mirror;

char * creditSource2[]={
"FONT :",
"Cyberdynecondital.ttf",
" ",
"MUSIC :",
"Hero Radio 50K Special - NCS MIX",
"https://www.youtube.com/watch?v=lEFWZZAfrKk",
" ",
"3D MODEL :",
"Low poly male",
"http://wiki.polycount.com/wiki/File:Basemesh",
"_arshlevon_shoes.obj",
" ",
"MK83Bomb",
"http://tf3dm.com/3d-model/mk-83-bomb-71139.html",
" ",
"B2 Spirit",
"http://tf3dm.com/3d-model/b2-spirit-6393.html",
" ",
"F-15C Eagle",
"http://tf3dm.com/3d-model/f-15c-eagle-38758.html",
" ",
"F-4E Phantom II",
"http://tf3dm.com/3d-model/f-4e-phantom-ii-79",
"52.html",
" ",
"Kara Class Cruiser",
"http://tf3dm.com/3d-model/kara-class-cruiser-a",
"nd-nanuchka-class-corvette-69648.html",
" ",
"Lamborghini Aventador",
"http://tf3dm.com/3d-model/lamborghini-aventado",
"r-42591.html",
" ",
"Fantasy wild town ",
"http://tf3dm.com/3d-model/fantasy-wild-town-53",
"140.html",
" ",
"F-4E Phantom II ",
"http://tf3dm.com/3d-model/f-4e-phantom-ii-7952",
".html",
" ",
"Aircraft Carrier",
"http://tf3dm.com/3d-model/aircraft-carrier-430",
"59.html",
" ",
" ",
"LIBRARY :",
"GL4Dummies",
"OpenGL 3.2",
"GLM",
"SDL2",
"SDL2_Image",
"SDL2_Mixer",
"SDL2_TTF",
" ",
" ",
" ",
" ",
" ",
" ",
" ",
" ",
"REMERCIEMENT :",
"Fares Belhadj",
" ",
" ",
" ",
" ",
" ",
" ",
"Realisé Par :",
"Hobean BAE",
"http://b845548.wix.com/portfolio",
" "
};

static void draw_house(void) {
  static Uint32 t0 = 0;
  GLfloat dt = 0.0;
  GLfloat lumPos[4], *mat;
  Uint32 t;
  dt = ((t = SDL_GetTicks()) - t0) / 1000.0;
  t0 = t;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();
  /*
 gl4duLookAtf(_cam.x, _cam.y, _cam.z, 
	       _cam.x - sin(_cam.theta), 1.0  - (_windowHeight >> 1) / (GLfloat)_windowHeight, _cam.z - cos(_cam.theta), 
	       0.0, 1.0,0.0);
*/
  mat = gl4duGetMatrixData();
  MMAT4XVEC4(lumPos, mat, _lumPos0);
  glUseProgram(_pId); // CHANGE AUTOMATIQUE

/////////////////////// cam movement //////////////////

static int camTimer=0;
static float movement=0;
if(camTimer==0)camTimer=SDL_GetTicks();
if(SDL_GetTicks()-camTimer>1000){

gl4duTranslatef(-movement,0,0);
  gl4duSendMatrices();
if(movement<190)movement+=dt*15;

}


 
/////////////////////// cam movement //////////////////


#define BALLNB 5
static int initEnemie=0;
static enemie enemiePos[BALLNB];
//static float diff[BALLNB];
int i;

///////////////////// initialisation enemie ////////////////////////
if(initEnemie==0){
for(i=0,initEnemie=1;i<BALLNB;i++){
enemiePos[i].x=enemiePos[i].bulletX=(rand()%200+100)-20;
enemiePos[i].y=enemiePos[i].bulletY=0;
enemiePos[i].z=enemiePos[i].bulletZ=-250-(rand()%300-300);
// diff[i]=(rand()/(float)RAND_MAX)*0.08;

}
}

///////////////////// initialisation enemie ////////////////////////
/*

///////////////////// movement enemie ////////////////////////
for(i=0;i<BALLNB;i++){
HBB_missileTracking(enemiePos[i].x,enemiePos[i].y,enemiePos[i].z,_cam.x,_cam.y-0,_cam.z-0.5,&enemiePos[i].theta,&thetah);
if(enemiePos[i].scale<100)enemiePos[i].bulletX-=sin(enemiePos[i].theta)*dt*80;
if(enemiePos[i].scale<100)enemiePos[i].bulletZ-=cos(enemiePos[i].theta)*dt*80;
if(enemiePos[i].scale<50)enemiePos[i].scale+=dt*80;
else{
enemiePos[i].bulletX-=sin(enemiePos[i].theta)*dt*80;
enemiePos[i].bulletZ-=cos(enemiePos[i].theta)*dt*80;
}
}

///////////////////// movement enemie ////////////////////////
*/
glEnable(GL_BLEND);
  glActiveTexture(GL_TEXTURE0);
  glUniform1i(glGetUniformLocation(_pId, "myTexture"), 0);
  glUniform4fv(glGetUniformLocation(_pId, "lumPos"), 1, lumPos);
  glUniform1i(glGetUniformLocation(_pId, "faceNormal"), !_faceNormal); 
glUniform1f(glGetUniformLocation(_pId, "movement"), movement); 
glUniform1i(glGetUniformLocation(_pId, "emposX"), enemiePos[0].x);
glUniform1i(glGetUniformLocation(_pId, "emposY"), enemiePos[0].y); 
glUniform1i(glGetUniformLocation(_pId, "emposZ"), enemiePos[0].z);
  glUniform1i(glGetUniformLocation(_pId, "phong"), _phong);
  glUniform1i(glGetUniformLocation(_pId, "tc"), !_tc);


int j;
for(i=0;i<BALLNB;i++){

  glBindTexture(GL_TEXTURE_2D,_texture);
 glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEXW,TEXH, 0, GL_RGBA, GL_UNSIGNED_BYTE,pixels);
  glBindTexture(GL_TEXTURE_2D,0);

/////////// changent de texture plasma ///////////


  gl4duPushMatrix();
  glUniform1i(glGetUniformLocation(_pId, "body"), !_body);
gl4duTranslatef(10, -50,0);
  gl4duTranslatef(enemiePos[i].x, enemiePos[i].y, enemiePos[i].z);
  gl4duRotatef(enemiePos[i].theta/HBB_PI*180-30, 0, 1, 0);
  gl4duSendMatrices();
  glBindTexture(GL_TEXTURE_2D,_texture);
  glBindVertexArray(_objTabVao[OBJ_SUPERHOT]);
  glDrawElements(GL_TRIANGLES, 3 * (_tabModel[OBJ_SUPERHOT]->numtriangles-9), GL_UNSIGNED_INT, 0);
 gl4duPopMatrix();



  ///////////////////// w enemie  //////////////



  gl4duPushMatrix();
/////////// changent de texture plasma ///////////
	for(j=0;j<TEXW*TEXH;j++){
	  im[j]++;
 	  im[j]%=256;
	  pixels[j]=(255<<24)|(0<<16)|(im[j]<<8)|255;// rouge
     //  pixels[i]=(255<<24)|(255<<16)|(im[i]<<8)|0;
	}

  glBindTexture(GL_TEXTURE_2D,_texture);
 glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEXW,TEXH, 0, GL_RGBA, GL_UNSIGNED_BYTE,pixels);
  glBindTexture(GL_TEXTURE_2D,0);

/*
////// line de ball ////////
  glBindTexture(GL_TEXTURE_2D,_texture2);
Uint32 noir[100];
int k;
for(k=0;k<100;k++)
noir[k]=(255<<0);
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 10,10, 0, GL_RGBA, GL_UNSIGNED_BYTE,noir);
  glBindTexture(GL_TEXTURE_2D,0);

  gl4duTranslatef(enemiePos[i].bulletX+diff[i], enemiePos[i].bulletY+diff[i], enemiePos[i].bulletZ+diff[i]);
 gl4duRotatef(enemiePos[i].theta/HBB_PI*180, 0, 1, 0);
  gl4duScalef(0.015,0.015,enemiePos[i].scale);
  gl4duSendMatrices();

  glBindTexture(GL_TEXTURE_2D, _texture2);
	gl4dgDraw(_sphere);
 gl4duPopMatrix();


*/








}


  gl4duPushMatrix();
  glUniform1i(glGetUniformLocation(_pId, "tc"), _tc);
 gl4duScalef(1.5, 1.5, 1.5);
 gl4duRotatef(320, 0, 1, 0);
gl4duTranslatef(6,-33,-300);
  gl4duSendMatrices();
  glBindTexture(GL_TEXTURE_2D,_texture);
  glBindVertexArray(_objTabVao[OBJ_HOUSE]);
  glDrawElements(GL_TRIANGLES, 3 * (_tabModel[OBJ_HOUSE]->numtriangles-9), GL_UNSIGNED_INT, 0);

 gl4duPopMatrix();

static int iteration=0;
static int timerBlur=0;
if(timerBlur==0){
timerBlur=SDL_GetTicks();
}

if(movement>=190)
if(SDL_GetTicks()-timerBlur>200){
timerBlur=SDL_GetTicks();
if(iteration<7)iteration++;

}



//if(blurMode==-1)
	gl4dfBlur(0, 0, iteration, 1, 0, GL_FALSE);

  gl4dfSobelSetMixMode(GL4DF_SOBEL_MIX_MULT);
  gl4dfSobel(0, 0, GL_FALSE); 

  glUseProgram(_pId2); // CHANGE AUTOMATIQUE

 glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,_texture3);

static int timer=0;

Uint32 * pix;

static float a=0;
static SDL_Rect textPos;
static SDL_Color tmpfontcolor = {255,255,255,200};
static int initText=0;



if(timer==0)timer=SDL_GetTicks();
if(SDL_GetTicks()-timer>100&&movement>=190)a+=dt*170;
if(initText==0){
initText++;
for(j=0;j<NBTEXT;j++)
  resulting_textTab[j] = TTF_RenderText_Blended(petitfont,creditSource2[j], tmpfontcolor);
} 





/////////// VIDER L'ECRAN ////////////////
pix=(Uint32 *)credit->pixels;
for(i=0;i<credit->w*credit->h;i++)
pix[i]=0;
/////////// VIDER L'ECRAN ////////////////

/////////// DEFINIR LA POSTION ET DESSINER SUR L'ECRAN ////////////////
for(i=0;i<NBTEXT;i++){
	textPos.x=80;
	textPos.y=1100+(i*45)-a;
   SDL_BlitSurface(resulting_textTab[i], NULL, credit, &textPos);
}
HBB_flip(pix,credit->w,credit->h);




 glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, credit->w,credit->h, 0, GL_RGBA, GL_UNSIGNED_BYTE,pix);


  glBindTexture(GL_TEXTURE_2D,0);
 gl4duTranslatef(0,0, -10);
gl4duRotatef(30,0,1,0);
  gl4duSendMatrices();
    glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _texture3);
glUniform1i(glGetUniformLocation(_pId2, "myTexture"), 0);
gl4dgDraw(_quad);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glDisable(GL_BLEND);
  ///////////////////// DRAW CREDIT  //////////////

}


static void draw_shot(void) {
  static Uint32 t0 = 0;

  GLfloat dt = 0.0;
  GLfloat lumPos[4], *mat;
  Uint32 t;
  dt = ((t = SDL_GetTicks()) - t0) / 1000.0;
  t0 = t;
glClearColor(1, 1, 1, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();
  /*
gl4duLookAtf(_cam.x, _cam.y, _cam.z, 
	       _cam.x - sin(_cam.theta), 1.0  - (_windowHeight >> 1) / (GLfloat)_windowHeight, _cam.z - cos(_cam.theta), 
	       0.0, 1.0,0.0);
  */
  mat = gl4duGetMatrixData();
  MMAT4XVEC4(lumPos, mat, _lumPos0);
  glUseProgram(_pId); // CHANGE AUTOMATIQUE

static GLfloat thetah;
#define NBENEMIE 1
static enemie enemiePos[NBENEMIE];
static int initEnemie=0;
int i;

 
///////////////////// initialisation enemie ////////////////////////
if(initEnemie==0){
for(i=0,initEnemie=1;i<NBENEMIE;i++){
if(i==0){
enemiePos[i].x=enemiePos[i].bulletX=-35;
enemiePos[i].y=enemiePos[i].bulletY=0;
enemiePos[i].z=enemiePos[i].bulletZ=-190;
}else{
enemiePos[i].x=enemiePos[i].bulletX=-35;
enemiePos[i].y=enemiePos[i].bulletY=0;
enemiePos[i].z=enemiePos[i].bulletZ=-190;
}
}



}
///////////////////// initialisation enemie ////////////////////////



 static float thetaTab[1000];
 static float ballX[1000];
 static float ballZ[1000];
 static int ballTimer=0;
 static int NBBALL=0;
 static int posinit=0;
 if(posinit==0){

   for(i=0;i<1000;i++){
     ballX[i]=0;
     ballZ[i]=0;
   }
   posinit=1;
 }
 

 if(ballTimer==0){
   ballTimer=SDL_GetTicks();
 }
 static int animTimer=0;
 if(animTimer==0)animTimer=SDL_GetTicks();
 
 if(SDL_GetTicks()-ballTimer>100&&SDL_GetTicks()-animTimer<=2000){
   
   thetaTab[NBBALL]=_cam.theta;
   if(NBBALL<1000)NBBALL++;
   ballTimer=SDL_GetTicks();
 }

 gl4duRotatef(_cam.theta,0,1,0);

 
 if(SDL_GetTicks()-animTimer>2000&&SDL_GetTicks()-animTimer<=3000){
 _cam.theta+=dt*HBB_PI*4;
 }else if(SDL_GetTicks()-animTimer>3000&&SDL_GetTicks()-animTimer<=5000){
 _cam.theta-=dt*HBB_PI*4;
 }else if(SDL_GetTicks()-animTimer>5000&&SDL_GetTicks()-animTimer<=6000){
 _cam.theta+=dt*HBB_PI*4;
 }

 if(SDL_GetTicks()-animTimer>6000)
for(i=0;i<NBENEMIE;i++){
HBB_missileTracking(enemiePos[i].x-0.9,enemiePos[i].y,enemiePos[i].z,_cam.x,_cam.y,_cam.z-0.5,&enemiePos[i].theta,&thetah/*non necessaire*/);

if(enemiePos[i].scale<110)enemiePos[i].bulletX-=sin(enemiePos[i].theta)*dt*40;
if(enemiePos[i].scale<110)enemiePos[i].bulletZ-=cos(enemiePos[i].theta)*dt*40;
if(enemiePos[i].scale<110)enemiePos[i].scale+=dt*40;

}
///////////////////// movement enemie ////////////////////////

  glActiveTexture(GL_TEXTURE0);
  glUniform1i(glGetUniformLocation(_pId, "myTexture"), 0);
  glUniform4fv(glGetUniformLocation(_pId, "lumPos"), 1, lumPos);
  glUniform1i(glGetUniformLocation(_pId, "faceNormal"), !_faceNormal);  
glUniform1i(glGetUniformLocation(_pId, "emposX"), enemiePos[0].x);
glUniform1i(glGetUniformLocation(_pId, "emposY"), enemiePos[0].y); 
glUniform1i(glGetUniformLocation(_pId, "emposZ"), enemiePos[0].z);
  glUniform1i(glGetUniformLocation(_pId, "phong"), _phong);
  glUniform1i(glGetUniformLocation(_pId, "tc"), !_tc);
  /* envoi de toutes les matrices stockées par GL4D */ 

  ///////////////////// draw enemie  //////////////
int j;


     for(i=0;i<NBBALL;i++){
       ballX[i]-=sin(thetaTab[i])*dt*500;
       ballZ[i]-=cos(thetaTab[i])*dt*500;
  gl4duPushMatrix();

  gl4duLoadIdentityf();
    gl4duTranslatef(2.3,-1.5,-8.8);
  //gl4duRotatef(-15, 0, 1, 0);
  gl4duTranslatef(ballX[i],0,ballZ[i]);
  gl4duRotatef(thetaTab[i]/(float)HBB_PI*180, 0, 1, 0);
  gl4duScalef(0.10,0.10,5);
  gl4duSendMatrices();

  glBindTexture(GL_TEXTURE_2D, _texture2);

  if(ballX[i]<1.5&&ballZ[i]<1.5)
  gl4dgDraw(_sphere);
 gl4duPopMatrix();

     }










 
for(i=0;i<NBENEMIE;i++){
/////////// changent de texture plasma ///////////
	for(j=0;j<TEXW*TEXH;j++){
	  im[j]++;
 	  im[j]%=256;
	  pixels[j]=(255<<24)|(0<<16)|(im[j]<<8)|255;// rouge
     //  pixels[i]=(255<<24)|(255<<16)|(im[i]<<8)|0;
	}

  glBindTexture(GL_TEXTURE_2D,_texture);
 glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEXW,TEXH, 0, GL_RGBA, GL_UNSIGNED_BYTE,pixels);
  glBindTexture(GL_TEXTURE_2D,0);

/////////// changent de texture plasma ///////////
  
  if(SDL_GetTicks()-animTimer>6000){
if(i==0){
glUniform1i(glGetUniformLocation(_pId, "drawBody"), !_drawBody);
  gl4duPushMatrix();
glUniform1i(glGetUniformLocation(_pId, "body"), !_body);
gl4duTranslatef(10, -50,0);
  gl4duTranslatef(enemiePos[i].x, enemiePos[i].y, enemiePos[i].z);
  gl4duRotatef(enemiePos[i].theta/HBB_PI*180+180, 0, 1, 0);
 // gl4duScalef(1,1,1);
  gl4duSendMatrices();
  glBindTexture(GL_TEXTURE_2D,_texture);
  glBindVertexArray(_objTabVao[OBJ_SUPERHOT]);
  glDrawElements(GL_TRIANGLES, 3 * (_tabModel[OBJ_SUPERHOT]->numtriangles-9), GL_UNSIGNED_INT, 0);
 gl4duPopMatrix();


  gl4duPushMatrix();
glUniform1i(glGetUniformLocation(_pId, "body"), _body);
gl4duTranslatef(7, -47,0);
  gl4duTranslatef(enemiePos[i].x, enemiePos[i].y, enemiePos[i].z);
  gl4duRotatef(enemiePos[i].theta/HBB_PI*180+180, 0, 1, 0);
  gl4duRotatef(60, 0, 1, 0);
 // gl4duScalef(1,1,1);
  gl4duSendMatrices();
  glBindTexture(GL_TEXTURE_2D,_texture);
  glBindVertexArray(_objTabVao[OBJ_SUPERHOT]);
  glDrawElements(GL_TRIANGLES, 3 * (_tabModel[OBJ_SUPERHOT]->numtriangles-0), GL_UNSIGNED_INT, 0);
 gl4duPopMatrix();

glUniform1i(glGetUniformLocation(_pId, "drawBody"), _drawBody);
}
  }
  ///////////////////// draw enemie  //////////////


////// line de ball ////////
  gl4duPushMatrix();

  glBindTexture(GL_TEXTURE_2D,_texture2);
Uint32 noir[100];
int k;
for(k=0;k<100;k++)
noir[k]=(255<<0);
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 10,10, 0, GL_RGBA, GL_UNSIGNED_BYTE,noir);
  glBindTexture(GL_TEXTURE_2D,0);


  gl4duTranslatef(enemiePos[i].bulletX, enemiePos[i].bulletY, enemiePos[i].bulletZ);
  gl4duRotatef(enemiePos[i].theta/HBB_PI*180, 0, 1, 0);
  gl4duScalef(0.10,0.10,-enemiePos[i].scale);
  gl4duSendMatrices();

  glBindTexture(GL_TEXTURE_2D, _texture2);
  
 if(SDL_GetTicks()-animTimer>6000)
	gl4dgDraw(_sphere);
 gl4duPopMatrix();



/////////// GUN OBJ
for(j=0;j<TEXW*TEXH;j++){
	  im[j]++;
 	  im[j]%=256;
	  pixels[j]=(255<<24)|(255<<16)|((10+im[j])<<8)|0;// bleu
	}

  glBindTexture(GL_TEXTURE_2D,_texture2);
 glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEXW,TEXH, 0, GL_RGBA, GL_UNSIGNED_BYTE,pixels);
  glBindTexture(GL_TEXTURE_2D,0);
  
if(i==0){
  gl4duPushMatrix();

  gl4duTranslatef(enemiePos[i].x+3.2, enemiePos[i].y-0.7, enemiePos[i].z+35);
  gl4duScalef(0.2,0.2,0.2);
  gl4duRotatef(-100, 0, 0, 1);
  gl4duRotatef(180, 0, 1, 0);
  gl4duRotatef(-30, 0, 0, 1);
  gl4duRotatef(20, 1, 0, 0);
  gl4duRotatef(130, 0, 1, 0);
  gl4duRotatef(30, 0, 0, 1);
  gl4duRotatef(60, 0, 1, 0);
  gl4duRotatef(30, 1, 0, 0);
  gl4duSendMatrices();
/////////// changent de texture plasma ///////////
	
 if(SDL_GetTicks()-animTimer>6000){

/////////// changent de texture plasma ///////////
  glBindTexture(GL_TEXTURE_2D,_texture2);
  glBindVertexArray(_objTabVao[OBJ_MP5]);
  glDrawElements(GL_TRIANGLES, 3 * (_tabModel[OBJ_MP5]->numtriangles-0), GL_UNSIGNED_INT, 0);

 }
  gl4duPopMatrix();

}
/////////// GUN OBJ



}

  gl4duPushMatrix();
  gl4duLoadIdentityf();
 gl4duTranslatef(2.5,-4, -10);
 gl4duRotatef(180, 0, 1, 0);
  gl4duScalef(0.1,0.1,0.1);
  gl4duSendMatrices();
  glBindTexture(GL_TEXTURE_2D,_texture2);
  glBindVertexArray(_objTabVao[OBJ_MP5]);
  glDrawElements(GL_TRIANGLES, 3 * (_tabModel[OBJ_MP5]->numtriangles-0), GL_UNSIGNED_INT, 0);

  gl4duPopMatrix();


  gl4duPushMatrix();
  glUniform1i(glGetUniformLocation(_pId, "tc"), _tc);
 gl4duScalef(1.5, 1.5, 1.5);
 gl4duRotatef(-30, 0, 1, 0);
gl4duTranslatef(-70,-33,-350);
  gl4duSendMatrices();
  glBindTexture(GL_TEXTURE_2D,_texture);
  glBindVertexArray(_objTabVao[OBJ_HOUSE]);
  glDrawElements(GL_TRIANGLES, 3 * (_tabModel[OBJ_HOUSE]->numtriangles-9), GL_UNSIGNED_INT, 0);

 gl4duPopMatrix();


if(blurMode==-1)
	gl4dfBlur(0, 0, 5, 1, 0, GL_FALSE);

  gl4dfSobelSetMixMode(GL4DF_SOBEL_MIX_MULT);
  gl4dfSobel(0, 0, GL_FALSE); 

  ///////////////////// DRAW CREDIT  //////////////
  glUseProgram(_pId2); // CHANGE AUTOMATIQUE

 glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,_texture3);

static int timer=0;

Uint32 * pix;

static float a=0;
static SDL_Rect textPos;
 
static SDL_Rect brokePos;
//static SDL_Color tmpfontcolor = {255,255,255,255};
static SDL_Color tmpfontcolor = {0,240,0,255};
static int initText=0;



if(timer==0)timer=SDL_GetTicks();
if(initText==0){
initText++;
for(j=0;j<NBTEXT;j++)
  resulting_textTab[j] = TTF_RenderText_Blended(petitfont,creditSource2[j], tmpfontcolor);
 mirror=IMG_Load("Texture/broke2.png");
 assert(mirror);
} 





/////////// VIDER L'ECRAN ////////////////
pix=(Uint32 *)credit->pixels;
for(i=0;i<credit->w*credit->h;i++)
pix[i]=0;
/////////// VIDER L'ECRAN ////////////////

/////////// DEFINIR LA POSTION ET DESSINER SUR L'ECRAN ////////////////
 
 if(SDL_GetTicks()-animTimer>9000){
for(i=0;i<NBTEXT;i++){
	textPos.x=80;
	textPos.y=1100+(i*45)-a;
   SDL_BlitSurface(resulting_textTab[i], NULL, credit, &textPos);
}

a+=dt*170;
 }
 brokePos.x=20;
 brokePos.y=170;
 if(SDL_GetTicks()-animTimer>8500)
   SDL_BlitSurface(mirror, NULL, credit, &brokePos);
 
HBB_flip(pix,credit->w,credit->h);



 glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, credit->w,credit->h, 0, GL_RGBA, GL_UNSIGNED_BYTE,pix);

 
SDL_FreeSurface(resulting_text);





 
  glBindTexture(GL_TEXTURE_2D,0);
 gl4duTranslatef(0,0, -10);
 //gl4duScalef(3,3,3);
 gl4duRotatef(30,0,1,0);
  gl4duSendMatrices();
    glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _texture3);
glUniform1i(glGetUniformLocation(_pId2, "myTexture"), 0);
gl4dgDraw(_quad);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glDisable(GL_BLEND);
  ///////////////////// DRAW CREDIT  //////////////




}

void init_superhot(void){

  initGL();
  initData();
  atexit(quit);

 // gl4duwResizeFunc(resize);
}

void draw_superhot(void){

 // glDisable(GL_CULL_FACE);
 // glEnable(GL_BLEND);
 // glEnable(GL_DEPTH);
//  glDisable(GL_DEPTH_TEST);
static int animationTimer=0;
if(animationTimer==0)
	animationTimer=SDL_GetTicks();
/*
if(SDL_GetTicks()-animationTimer<33000)
	draw_house();
else
  */
draw_shot();
	
}

static GLfloat * mkSphereVertices(GLuint longitudes, GLuint latitudes) {
  int i, j, k;
  GLdouble phi, theta, r, y;
  GLfloat * data;
  GLdouble c2MPI_Long = 2.0 * M_PI / longitudes;
  GLdouble cMPI_Lat = M_PI / latitudes;
  data = malloc(5 * (longitudes + 1) * (latitudes + 1) * sizeof *data);
  assert(data);
  for(i = 0, k = 0; i <= latitudes; i++) {
    theta  = -M_PI_2 + i * cMPI_Lat;
    y = sin(theta);
    r = cos(theta);
    for(j = 0; j <= longitudes; j++) {
      phi = j * c2MPI_Long;
      data[k++] = r * cos(phi)+gl4dmSURand()*0.1; 
      data[k++] = y; 
      data[k++] = r * sin(phi)+gl4dmSURand()*0.1;
      data[k++] = phi / (2.0 * M_PI); 
      data[k++] = (theta + M_PI_2) / M_PI;
    }
  }
  return data;
}

static GLuint * mkSphereIndex(GLuint longitudes, GLuint latitudes) {
  int i, ni, j, nj, k;
  GLuint * index;
  index = malloc(6 * longitudes * latitudes * sizeof *index);
  assert(index);
  for(i = 0, k = 0; i < latitudes; i++) {
    ni = i + 1;
    for(j = 0; j < longitudes; j++) {
      nj = j + 1;
      index[k++] = i * (longitudes + 1) + j;
      index[k++] = ni * (longitudes + 1) + nj;
      index[k++] = i * (longitudes + 1) + nj;

      index[k++] = i * (longitudes + 1) + j;
      index[k++] = ni * (longitudes + 1) + j;
      index[k++] = ni * (longitudes + 1) + nj;
    }
  }
  return index;
}

/*!\brief appelée au moment de sortir du programme (atexit), libère les éléments utilisés */
static void quit(void) {
int i;
for(i=0;i<NBTEXT;i++)
SDL_FreeSurface(resulting_textTab[i]);

SDL_FreeSurface(mirror);
SDL_FreeSurface(credit);
  if(_vao)
    glDeleteVertexArrays(1, &_vao);
  if(_ibuffer[0])
    glDeleteBuffers(2, _ibuffer);
  if(_texture)
    glDeleteTextures(1, &_texture);
  gl4duClean(GL4DU_ALL);
  if(_texture)
    glDeleteTextures(1, &_texture);
}

