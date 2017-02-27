/*!\file earth.c
 *
 * \brief Terre en GL / GL4Dummies + Texture + Lumière positionnelle +
 * Phong + Spéculaire
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date April 15 2016 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4df.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL_image.h>

#include "fractal.h"
#include "glm/glm.h"

//////////////// OBJET LOADING //////////////////
#define OBJMAX 3
#define OBJNB 1// Pour ajouter un objet,augumenter la valeur +1
enum objetType {// Pour ajouter l'objet suivent
OBJ_SPACE = 0,
};

static char *OBJFilePath[]={"Objet3D/space/Space_Invader.obj"
};
static GLMmodel *_tabModel[OBJMAX];
static GLuint _objTabVao[OBJMAX],_objTabBD[OBJMAX],_objTabBI[OBJMAX];

//////////////// OBJET LOADING //////////////////

/*!\brief identifiant du programme GLSL */
static GLuint _pId = 0;
/*!\brief position de la lumière relativement à la sphère éclairée */
static GLfloat _lumPos0[4] = {-800.0, 800.0, -800.0, 0.0};
/*!\brief tableau des identifiants de texture à charger */
static GLuint _tId[5] = {0};
static int NBTEX=5;
/*!\brief pour les deux astres : terre et lune */
static GLuint _sphere = {0};

//////////////// OBJET LOADING //////////////////

static GLuint _skyboxtabVao[6]={0,0,0,0,0};
static GLuint _skyboxtabBuffer[6]={0,0,0,0,0};

void HBB_initSkyboxVAO(void){
int i;  GLfloat s = 1000;
//V4 V6 V3 V7
GLfloat data_skyboxbas[] = {
    s , 0, s, 
    s , 0, -s,
    s , 0, s,
    s , 0, -s,
    /* 4 normales */
    0.0f, 1.0f, 0.0f, 
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    /* 4 coordonnées de texture, une par sommet */

    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
  };
GLfloat data_skyboxt[] = {
    s , s, s, 
    s , s, -s,
    s , 0, s,
    s , 0, -s,
    /* 4 normales */
    0.0f, 0.0f, 1.0f, 
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    /* 4 coordonnées de texture, une par sommet */

    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
  };
//V1 V4 V2 V3
GLfloat data_skybox[] = {
   /* 4 coordonnées de sommets */
    -s , s, s, 
    s , s, s,
    -s , 0, s,
    s , 0, s,
    /* 4 normales */
    0.0f, -1.0f, 0.0f, 
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    /* 4 coordonnées de texture, une par sommet */
  
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
  };
//V5 V6 V7 V8
GLfloat data_skyboxv[] = {
    -s , s, -s, 
    s , s, -s,
    -s , 0, -s,
    s , 0, -s,
    /* 4 normales */
    0.0f, 1.0f, 0.0f, 
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    /* 4 coordonnées de texture, une par sommet */
    
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 0.0f, 
    1.0f, 1.0f,
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
    0.0f,1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    /* 4 coordonnées de texture, une par sommet */

    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 0.0f, 
    1.0f, 1.0f,
  };
// V1 V5 V2 V8
  GLfloat data_skyboxz[] = {
    -s , s, s,
    -s , s, -s,
    -s , 0, s, 
    -s , 0, -s,
    /* 4 normales */
    0.0f, 1.0f, 0.0f, 
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    /* 4 coordonnées de texture, une par sommet */

    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 0.0f, 
    1.0f, 1.0f,
  };


for(i=0;i<6;i++){
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
if(i==5)glBufferData(GL_ARRAY_BUFFER, sizeof data_skybox, data_skyboxbas, GL_STATIC_DRAW);


glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);  
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE,  0, (const void *)(4 * 3 * sizeof *data_skybox));
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (const void *)(4 * 6 * sizeof *data_skybox));

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

} 


static void HBB_drawSkybox(void) {
  GLfloat cam[2] = {0, 0};
  glUniform2fv(glGetUniformLocation(_pId, "cam"), 1, cam);

  glBindTexture(GL_TEXTURE_2D,_tId[0]);
  glBindVertexArray(_skyboxtabVao[0]);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  
  //glBindTexture(GL_TEXTURE_2D,_explosionTex);
  glBindVertexArray(_skyboxtabVao[1]);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  
  glBindVertexArray(_skyboxtabVao[2]);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(_skyboxtabVao[3]);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  
  glBindVertexArray(_skyboxtabVao[4]);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

}
static void HBB_objLoader2(void){
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

void voiture_init(void){
  int i;
  SDL_Surface * t;
  int vp[4], w, h;
  static char * files[] = {"images/yellow.png", "images/land_ocean_ice_2048_glossmap.png", "images/moon.jpg", "images/wall.jpg","images/bl2.png"};
  _pId  = gl4duCreateProgram("<vs>shaders/SHD3/earth.vs", "<fs>shaders/SHD3/earth.fs", NULL);
  glGetIntegerv(GL_VIEWPORT, vp);
  w = vp[2] - vp[0];
  h = vp[3] - vp[1];
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.5, 0.5, -0.5 * h / w, 0.5 * h / w, 1.0, 10000.0);
  gl4duBindMatrix("modelViewMatrix");
  glGenTextures(NBTEX, _tId);
  for(i = 0; i < NBTEX; i++) {
    glBindTexture(GL_TEXTURE_2D, _tId[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    if( (t = IMG_Load(files[i])) != NULL ) {
#ifdef __APPLE__
      int mode = t->format->BytesPerPixel == 4 ? GL_BGRA : GL_BGR;
#else
      int mode = t->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;
#endif 
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t->w, t->h, 0, mode, GL_UNSIGNED_BYTE, t->pixels);
      SDL_FreeSurface(t);
    } else {
      fprintf(stderr, "can't open file %s : %s\n", files[i], SDL_GetError());
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }
  }
  _sphere = gl4dgGenSpheref(30, 30);
  HBB_objLoader2();

}

void voiture_draw2(Uint16 tabOrg[],int NB){
//printf("%d %d\n",tab[0],NB);
static int timerMusic=0;
  static GLfloat a0 = 0.0;
  static Uint32 t0 = 0, t;
static Uint16 tab[2000];
if(timerMusic==0)timerMusic=SDL_GetTicks();

  GLfloat dt = 0.0, lumPos[4], *mat;
  dt = ((t = SDL_GetTicks()) - t0) / 1000.0;
  t0 = t;
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glClearColor(0,0,0,0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();
  //gl4duTranslatef(0, 0, -14);
  mat = gl4duGetMatrixData();
  MMAT4XVEC4(lumPos, mat, _lumPos0);

  glUseProgram(_pId);

 glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _tId[4]);
 glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, _tId[4]);
  glUniform1i(glGetUniformLocation(_pId, "tex0"), 0);
  glUniform1i(glGetUniformLocation(_pId, "tex1"), 1);
 glUniform4fv(glGetUniformLocation(_pId, "lumPos"), 1, lumPos);
  //glUniform1i(glGetUniformLocation(_pId, "specular"), 1);
  /* envoi de toutes les matrices stockées par GL4D */
  //glUniform1i(glGetUniformLocation(_pId, "specular"), 0);
  //  gl4dgDraw(_sphere);

  ///////// MODIF ///////////////////

  static int startTimer=0;
if(startTimer==0)startTimer=SDL_GetTicks();


  static float dist=-300;
  
//gl4duScalef(sc,sc,sc);
//gl4duTranslatef(0,0,-50);
  static int timer=0;
static float rot=0; 
if(timer==0)timer=SDL_GetTicks();
  dist+=30*dt;

int i,j;
rot+=50*dt;

gl4duPushMatrix();
static int translate=0;
if(SDL_GetTicks()-timer>3500){
gl4duTranslatef(-850,-700,-2500+translate);
translate+=dt*400;
}else
gl4duTranslatef(-850,-700,-2500);

gl4duSendMatrices();


gl4duPushMatrix();

if((SDL_GetTicks()-timerMusic)>70){
timerMusic=SDL_GetTicks();
for(i=0;i<NB*NB;i++)
tab[i]=tabOrg[i];
}


for(j=0;j< NB; j++){
for(i=0;i< NB; i++){
gl4duPushMatrix();
gl4duTranslatef(i*120,0,0);
gl4duRotatef(rot*0.08,0,1,0);
gl4duRotatef(90,0,1,0);
gl4duSendMatrices();

if(tab[i+j*NB])
{

 glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _tId[0]);
 glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, _tId[0]);
}else {

 glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _tId[4]);
 glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, _tId[4]);
}
glBindVertexArray(_objTabVao[OBJ_SPACE]);
glDrawElements(GL_TRIANGLES, 3 * (_tabModel[OBJ_SPACE]->numtriangles), GL_UNSIGNED_INT, 0);
gl4duPopMatrix();
}
gl4duTranslatef(0,90,0);
}
gl4duPopMatrix();
/////////////  TUNA //////////////////////////////

  ///////// MODIF ///////////////////
  a0 += 360.0 * dt / (6.0);
  //gl4dfBlur(0, 0, 5, 1, 0, GL_FALSE);
  gl4dfSobelSetMixMode(GL4DF_SOBEL_MIX_MULT);
 gl4dfSobel(0, 0, GL_FALSE);

}



void HBB_ModelFree2(void){
int freeMemory;

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

for(freeMemory=0;freeMemory<6;freeMemory++){
  if(_skyboxtabVao[freeMemory])
    glDeleteVertexArrays(1, &_skyboxtabVao[freeMemory]);
  if(_skyboxtabBuffer[freeMemory])
    glDeleteBuffers(1, &_skyboxtabBuffer[freeMemory]);
}

}






