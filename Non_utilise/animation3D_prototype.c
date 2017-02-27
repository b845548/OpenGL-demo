/*!\file window.c
 *
 * \brief Début de code pour "la chasse aux champignons"
 *
 * La transparence qui ne marchait pas en fin de cours était due au
 * shader qui n'utilisait pas l'alpha de la texture et mettait
 * directement 1 ... on pouvais chercher longtemps dans le code C et
 * ne jamais trouver ... DAMNED !
 *
 * Le Bug du gl4duLookAt était un "vrai" Bug dans le gl4duLookAt
 * (corrigé sur le git, sera remonté)
 *
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date February 17 2016
 */

#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL_image.h>

#include "glm/glm.h"
/*
 * Prototypes des fonctions statiques contenues dans ce fichier C
 */
void tron_initGL(void);
void tron_initData(void);
void tron_draw(void);

static int _windowWidth = 800, _windowHeight = 600;
/*!\brief identifiant des vertex array objects */
static GLuint _planVao = 0;
static GLuint _skyboxtabVao[5]={0,0,0,0,0};
/*!\brief identifiant des buffers de data */
static GLuint _planBuffer = 0;
static GLuint _skyboxtabBuffer[5]={0,0,0,0,0};
/*!\brief identifiant du (futur) GLSL program */
static GLuint _pId = 0;
/*!\brief identifiant des textures */
static GLuint _planTexId = 0;

//////////////// OBJET LOADING //////////////////

#define OBJMAX 2
#define OBJNB 2
static GLMmodel * _tabModel[OBJNB]={NULL,NULL};
static GLuint _objTabVao[OBJNB] = {0,0,}, _objTabBD[OBJNB] = {0,0,}, _objTabBI[OBJNB] ={0,0,};

enum objetType {
OBJ_YWING =1,
};

static char * OBJFilePath[]={"Objet3D/pig/pig.obj","Objet3D/pig/pig.obj"};
//////////////// OBJET LOADING //////////////////


static GLuint _skyboxtabTexId[5]={0,0,0,0,0};
/*!\brief indices des touches de clavier */

enum kyes_t {
  KLEFT = 0,
  KRIGHT,
  KUP,
  KDOWN
};

/*!\brief clavier virtuel */
static GLuint _keys[] = {0, 0, 0, 0};

typedef struct cam_t cam_t;
/*!\brief structure de données pour la caméra */
struct cam_t {
  GLfloat x,y,z;
  GLfloat theta;
};
/*!\brief la caméra */
static cam_t _cam = {0, 60,0, 0};
/*
void HBB_objLoader(void){
int i,j,lire; 


for(lire=0;lire<OBJNB;lire++){
if((_tabModel[lire] = glmReadOBJ(objFilePath[lire]))==NULL){
      fprintf(stderr, "Impossible d'ouvrir le fichier %s \n",objFilePath[lire]);
      exit(1);
}

 GLfloat * allData = NULL;
  GLuint * ti3= NULL;


  glmVertexNormals(_tabModel[lire], 90, GL_FALSE);
  allData = malloc(3 * 2 * (_tabModel[lire]->numvertices + 1) * sizeof *allData);
  assert(allData);
  memcpy(allData, _tabModel[lire]->vertices, 3 * _tabModel[lire]->numvertices * sizeof *allData);
  for(i = 0; i < (int)_tabModel[lire]->numtriangles; i++) {
    GLMtriangle * triangle = &(_tabModel[lire]->triangles[i]);
    for(j = 0; j < 3; j++)
      memcpy(&allData[3 * _tabModel[lire]->numvertices + 3 * triangle->vindices[j]], & _tabModel[0]->normals[3 * triangle->nindices[j]], 3 * sizeof *allData);
  }
  if(!_objTabVao[lire])
    glGenVertexArrays(1, &(_objTabVao[lire]));
  glBindVertexArray(_objTabVao[lire]);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
  if(!_objTabBD[lire])
    glGenBuffers(1, &(_objTabBD[lire]));
  if(!_objTabBI[lire])
    glGenBuffers(1, &(_objTabBI[lire]));
  glBindBuffer(GL_ARRAY_BUFFER,_objTabBD[lire]);
  glBufferData(GL_ARRAY_BUFFER,
  	       3 * (2 * _tabModel[lire]->numvertices) * sizeof *allData, allData, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
  glVertexAttribPointer(1, 3, GL_FLOAT,  GL_TRUE, 0, (const void *)(3 *_tabModel[lire]->numvertices * sizeof *allData));

  free(allData);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _objTabBI[lire]);

  ti3 = malloc(_tabModel[lire]->numtriangles * 3 * sizeof *ti3);
  assert(ti3);
  for(i = 0; i < (int)_tabModel[lire]->numtriangles; i++) {
    GLMtriangle * triangle = &(_tabModel[lire]->triangles[i]);
    memcpy(&ti3[3 * i], triangle->vindices, 3 * sizeof *ti3);
    ti3[3 * i] = triangle->vindices[0];
    ti3[3 * i + 1] = triangle->vindices[1];
    ti3[3 * i + 2] = triangle->vindices[2];
  }





  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
  	       _tabModel[lire]->numtriangles * 3 * sizeof *ti3, ti3, GL_STATIC_DRAW);
  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  free(ti3);
}



}
*/
void HBB_objLoader4(void){
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
 printf("pass");

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
  gl4duwDisplayFunc(draw);
  gl4duwIdleFunc(idle);
  gl4duwMainLoop();
  return 0;
}
*/
void tron_initGL(void){
  glClearColor(0.0f, 0.0f,0.0f, 0.0f);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
 glEnable(GL_DEPTH);
  glEnable(GL_TEXTURE_2D);
  _pId  = gl4duCreateProgram("<vs>shaders/SHD4/basic.vs", "<fs>shaders/SHD4/basic.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
 // resize(_windowWidth, _windowHeight);
}

static GLfloat _plan_r = 1000.0,_tex_repeat = 20,_repeat=9;

void tron_initData(void){
int i;
  SDL_Surface * texSurface;
  GLfloat s = _plan_r, data_plan[] = {
   /* 4 coordonnées de sommets */
    -s, 0.0f, -s,
    s , 0.0f, -s,
    -s, 0.0f,  s,
    s , 0.0f,  s,
    /* 4 normales */
    0.0f, 1.0f, 0.0f, 
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    /* 4 coordonnées de texture, une par sommet */
    0.0f, 0.0f, 
_tex_repeat, 0.0f,
    0.0f, _tex_repeat,
   _tex_repeat,_tex_repeat
  };
//V4 V6 V3 V7
GLfloat data_skyboxt[] = {
    s , s, s, 
    s , s, -s,
    s , -s, s,
    s , -s, -s,
    /* 4 normales */
    0.0f, 1.0f, 0.0f, 
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    /* 4 coordonnées de texture, une par sommet */

    0.0f, 0.0f,
    0.0f, _repeat,
    _repeat, 0.0f,
    _repeat,_repeat,
  };
//V1 V4 V2 V3
GLfloat data_skybox[] = {
   /* 4 coordonnées de sommets */
    -s , s, s, 
    s , s, s,
    -s , -s, s,
    s , -s, s,
    /* 4 normales */
    0.0f, 1.0f, 0.0f, 
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    /* 4 coordonnées de texture, une par sommet */
  
    0.0f, 0.0f,
    0.0f, _repeat,
    _repeat, 0.0f,
    _repeat, _repeat,
  };
//V5 V6 V7 V8
GLfloat data_skyboxv[] = {
    -s , s, -s, 
    s , s, -s,
    -s , -s, -s,
    s , -s, -s,
    /* 4 normales */
    0.0f, 1.0f, 0.0f, 
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    /* 4 coordonnées de texture, une par sommet */
    
    0.0f, 0.0f,
    0.0f, _repeat,
    _repeat, 0.0f, 
    _repeat,_repeat,
  };
//V5 V6 V1 V4
GLfloat data_skyboxw[] = {
   /* 4 coordonnées de sommets */
    -s, s, -s,
    s , s, -s,
    -s, s,  s,
    s , s,  s,
    /* 4 normales */
    0.0f, 1.0f, 0.0f, 
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    /* 4 coordonnées de texture, une par sommet */

    0.0f, 0.0f,
    0.0f, _repeat,
    _repeat, 0.0f, 
    _repeat, _repeat,
  };
// V1 V5 V2 V8
  GLfloat data_skyboxz[] = {
    -s , s, s,
    -s , s, -s,
-s , -s, s, 
    -s , -s, -s,
    /* 4 normales */
    0.0f, 1.0f, 0.0f, 
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    /* 4 coordonnées de texture, une par sommet */

    0.0f, 0.0f,
    0.0f, _repeat,
    _repeat, 0.0f, 
    _repeat, _repeat,
  };

  glGenVertexArrays(1, &_planVao);
  glBindVertexArray(_planVao);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glGenBuffers(1, &_planBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, _planBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof data_plan, data_plan, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);  
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE,  0, (const void *)(4 * 3 * sizeof *data_plan));
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (const void *)(4 * 6 * sizeof *data_plan));
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);


for(i=0;i<5;i++){
  glGenVertexArrays(1, &_skyboxtabVao[i]);
  glBindVertexArray(_skyboxtabVao[i]);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glGenBuffers(1, &_skyboxtabBuffer[i]);
  glBindBuffer(GL_ARRAY_BUFFER, _skyboxtabBuffer[i]);
if(i==0)glBufferData(GL_ARRAY_BUFFER, sizeof data_skyboxt, data_skyboxt, GL_STATIC_DRAW);
if(i==1)glBufferData(GL_ARRAY_BUFFER, sizeof data_skyboxv, data_skyboxv, GL_STATIC_DRAW);  
if(i==2)glBufferData(GL_ARRAY_BUFFER, sizeof data_skyboxw, data_skyboxw, GL_STATIC_DRAW);  
if(i==3)glBufferData(GL_ARRAY_BUFFER, sizeof data_skybox, data_skybox, GL_STATIC_DRAW); 
if(i==4)glBufferData(GL_ARRAY_BUFFER, sizeof data_skybox, data_skyboxz, GL_STATIC_DRAW);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);  
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE,  0, (const void *)(4 * 3 * sizeof *data_skybox));
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (const void *)(4 * 6 * sizeof *data_skybox));

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

  glGenTextures(1, &_planTexId);
  glBindTexture(GL_TEXTURE_2D, _planTexId);
  /* l'Anisotropic sous GL, on en parlait en cours, ne marche que si
     la version de la lib le supporte ; supprimer le bloc ci-après si
     problème. */
#ifdef GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
  GLfloat max;
  glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max); 
#endif

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  texSurface = IMG_Load("neon.png");
  assert(texSurface);
#if __APPLE__
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texSurface->w, texSurface->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, texSurface->pixels);
#else
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texSurface->w, texSurface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, texSurface->pixels);
#endif
  glGenerateMipmap(GL_TEXTURE_2D);
  SDL_FreeSurface(texSurface);


for(i=0;i<2;i++){
  glGenTextures(1, &_skyboxtabTexId[i]);
  glBindTexture(GL_TEXTURE_2D, _skyboxtabTexId[i]);
  /* l'Anisotropic sous GL, on en parlait en cours, ne marche que si
     la version de la lib le supporte ; supprimer le bloc ci-après si
     problème. */
#ifdef GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
  glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max); 
#endif

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
 // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);texSurface = SDL_ConvertSurfaceFormat(IMG_Load("left.jpg"),SDL_PIXELFORMAT_ABGR8888, 0);

if(i==0)texSurface = SDL_ConvertSurfaceFormat(IMG_Load("right.jpg"),SDL_PIXELFORMAT_ABGR8888, 0);
  assert(texSurface);
if(i==1)texSurface = SDL_ConvertSurfaceFormat(IMG_Load("star6.jpg"),SDL_PIXELFORMAT_ABGR8888, 0);
  assert(texSurface);

#if __APPLE__
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texSurface->w, texSurface->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, texSurface->pixels);
#else
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texSurface->w, texSurface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, texSurface->pixels);
#endif
  glGenerateMipmap(GL_TEXTURE_2D);
  SDL_FreeSurface(texSurface);
}



//////////////// OBJET LOADING //////////////////
////////////////////////////////////////////////
}

static void resize(int w, int h) {
  _windowWidth = w; 
  _windowHeight = h;
  glViewport(0, 0, _windowWidth, _windowHeight);
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.5, 0.5, -0.5 * _windowHeight / _windowWidth, 0.5 * _windowHeight / _windowWidth, 1.0, _plan_r*4.0);
}

static void idle(void) {
  double dt, dtheta = M_PI, pas = 5.0;
  static Uint32 t0 = 0, t;
  dt = ((t = SDL_GetTicks()) - t0) / 1000.0;
  t0 = t;
  if(_keys[KLEFT]) {
    _cam.theta += dt * dtheta;
  }
  if(_keys[KRIGHT]) {
    _cam.theta -= dt * dtheta;
  }
  if(_keys[KUP]) {
    _cam.x += -dt * pas * sin(_cam.theta);
    _cam.z += -dt * pas * cos(_cam.theta);
  }
  if(_keys[KDOWN]) {
    _cam.x += dt * pas * sin(_cam.theta);
    _cam.z += dt * pas * cos(_cam.theta);
  }
  
}

static void keydown(int keycode) {
  switch(keycode) {
  case SDLK_LEFT:
    _keys[KLEFT] = 1;
    break;
  case SDLK_RIGHT:
    _keys[KRIGHT] = 1;
    break;
  case SDLK_UP:
    _keys[KUP] = 1;
    break;
  case SDLK_DOWN:
    _keys[KDOWN] = 1;
    break;
  case SDLK_SPACE:
    break;
  case SDLK_ESCAPE:
  case 'q':
    exit(0);
  default:
    break;
  }
}

static void keyup(int keycode) {
  switch(keycode) {
  case SDLK_LEFT:
    _keys[KLEFT] = 0;
    break;
  case SDLK_RIGHT:
    _keys[KRIGHT] = 0;
    break;
  case SDLK_UP:
    _keys[KUP] = 0;
    break;
  case SDLK_DOWN:
    _keys[KDOWN] = 0;
    break;
   }
}

static void draw_one_champi(void) {
  GLfloat cam[2] = {0, 0};
  glUniform2fv(glGetUniformLocation(_pId, "cam"), 1, cam);
 gl4duLoadIdentityf();
  glBindTexture(GL_TEXTURE_2D, _planTexId);
glBindVertexArray(_planVao);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glBindTexture(GL_TEXTURE_2D, _skyboxtabTexId[1]);
  glBindVertexArray(_skyboxtabVao[0]);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  
  glBindTexture(GL_TEXTURE_2D, _skyboxtabTexId[1]);
  glBindVertexArray(_skyboxtabVao[1]);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  
  glBindTexture(GL_TEXTURE_2D, _skyboxtabTexId[1]);
  glBindVertexArray(_skyboxtabVao[2]);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindTexture(GL_TEXTURE_2D, _skyboxtabTexId[1]);
  glBindVertexArray(_skyboxtabVao[3]);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  
  glBindTexture(GL_TEXTURE_2D, _skyboxtabTexId[1]);
  glBindVertexArray(_skyboxtabVao[4]);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

}


void tron_draw(void) {
  int xm, ym;
  SDL_PumpEvents();
  //SDL_GetMouseState(&xm, &ym);
  GLfloat lumpos[4] = {100, 2000, 100, 1.0}, cam[2];
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();
  gl4duLookAtf(_cam.x, _cam.y, _cam.z, 
	       _cam.x - sin(_cam.theta),_cam.y - (_windowHeight >> 1) / (GLfloat)_windowHeight, _cam.z - cos(_cam.theta), 
	       0.0, 1.0,0.0);
  glUseProgram(_pId);
  glActiveTexture(GL_TEXTURE0);
  glUniform1i(glGetUniformLocation(_pId, "myTexture"), 0);
//////////////// OBJET LOADING //////////////////
float dt;
  static Uint32 t0 = 0, t;
  static int timer=0;
  static float distance=0;
  if(timer==0)timer=SDL_GetTicks();
  dt = ((t = SDL_GetTicks()) - t0) / 1000.0;
  t0 = t;
  distance+=dt*10;

  gl4duPushMatrix();
  gl4duRotatef(-30,1, 0, 0);
  gl4duTranslatef(0,0, distance); 
  gl4duSendMatrices();

//////////////// OBJET LOADING //////////////////

  glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lumpos);
  cam[0] = _cam.x / (2 * _plan_r / _tex_repeat);
  cam[1] = _cam.z / (2 * _plan_r / _tex_repeat);
  glUniform2fv(glGetUniformLocation(_pId, "cam"), 1, cam);
  gl4duPushMatrix(); {
    //gl4duTranslatef(_cam.x, 0, _cam.z);
    gl4duSendMatrices(); // apres translation et rotation on fait SendMatrice
    draw_one_champi();
  } gl4duPopMatrix();// remettre
  gl4duPushMatrix(); { // sauvegarder 
  gl4duLoadIdentityf(); // reinit 
   // gl4duRotatef(20, 0, 1,0);
	gl4duPushMatrix(); 
	//gl4duRotatef(20, 0, 1,0);
    gl4duScalef(0.08, 0.08,0.08);  // z eloine
    gl4duTranslatef(3.3, -5, -22); // apartir de 1 on voit donc -1
    gl4duSendMatrices();
 gl4duPopMatrix();
////////////////// gun objet ////////////
   gl4duTranslatef(-5, 0, -300); // apartir de 1 on voit donc -1
    gl4duSendMatrices();
 
glDisable(GL_DEPTH_TEST);

   glEnable(GL_DEPTH_TEST); 
  glBindVertexArray(0);

  } gl4duPopMatrix();
 gl4duPopMatrix();


}
void quit(void) {
  if(_planTexId)
    glDeleteTextures(1, &_planTexId);
  if(_planVao)
    glDeleteVertexArrays(1, &_planVao);
  if(_planBuffer)
    glDeleteBuffers(1, &_planBuffer);

int freeMemory;
for(freeMemory=0;freeMemory<5;freeMemory++){
  if(_skyboxtabVao[freeMemory])
    glDeleteVertexArrays(1, &_skyboxtabVao[freeMemory]);
  if(_skyboxtabBuffer[freeMemory])
    glDeleteBuffers(1, &_skyboxtabBuffer[freeMemory]);
}


for(freeMemory=0;freeMemory<OBJNB;freeMemory++){
if(_objTabVao[freeMemory])
    glDeleteVertexArrays(1, &_objTabVao[freeMemory]);
if(_objTabBI[freeMemory])
    glDeleteBuffers(1, &_objTabBI[freeMemory]);
if(_objTabBD[freeMemory])
    glDeleteBuffers(1, &_objTabBD[freeMemory]);
    glmDelete(_tabModel[freeMemory]);
    _tabModel[freeMemory] = NULL;
}
  gl4duClean(GL4DU_ALL);
}

