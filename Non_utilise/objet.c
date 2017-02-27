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
#define OBJMAX 6
#define OBJNB 5// Pour ajouter un objet,augumenter la valeur +1
enum objetType {// Pour ajouter l'objet suivent
OBJ_LFA = 0,
OBJ_BUGATTI=1,
  OBJ_BERLINETTA=2,
  OBJ_ZONDA=3,
  OBJ_CITY2=4,// revoir
};
static char *OBJFilePath[]={"Objet3D/LFA(bon)/LFA.obj",
			    "Objet3D/Bugatti(bon)/Bugatti-Veyron.obj",
			    "Objet3D/Berlinetta(bon)/Berlinetta.obj",
			    "Objet3D/zonda(bon)/pag.obj",
			    "Objet3D/wildTown/wildTown.obj"
};
static GLMmodel *_tabModel[OBJMAX];
static GLuint _objTabVao[OBJMAX],_objTabBD[OBJMAX],_objTabBI[OBJMAX];

//////////////// OBJET LOADING //////////////////

/*!\brief identifiant du programme GLSL */
static GLuint _pId = 0;
static GLuint _pId2 = 0;
/*!\brief position de la lumière relativement à la sphère éclairée */
static GLfloat _lumPos0[4] = {0.0, 0.0, 0.0, 10.0};
/*!\brief tableau des identifiants de texture à charger */
static GLuint _tId[10] = {0};
static int NBTEX=10;
/*!\brief pour les deux astres : terre et lune */
static GLuint _sphere = {0};

//////////////// OBJET LOADING //////////////////
void HBB_objLoader(void){
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
 printf("Bien charge les models");

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

int _landscape_w = 50, _landscape_h = 50;

GLuint _planVao = 0;
GLuint _planBuffer[2] = {0};
GLfloat *  _landscape;
void HBB_initWaterVAO(void){
GLfloat * data = NULL; 
  GLuint * idata = NULL;
  
  _landscape = calloc(_landscape_w * _landscape_h, sizeof * _landscape);
assert(_landscape);
  triangle_edge(_landscape, 0, 0, _landscape_w - 1, _landscape_h - 1, _landscape_w);
  data = heightMap2Data(_landscape, _landscape_w, _landscape_h);
  idata = heightMapIndexedData(_landscape_w, _landscape_h);
  

  glGenVertexArrays(1, &_planVao);
  glBindVertexArray(_planVao);
assert(_planVao);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glGenBuffers(2, _planBuffer); 
  glBindBuffer(GL_ARRAY_BUFFER, _planBuffer[0]);
  glBufferData(GL_ARRAY_BUFFER, 6 * _landscape_w * _landscape_h * sizeof *data, data, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof *data, (const void *)0); 
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof *data, (const void *)(3 * sizeof *data));  

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _planBuffer[1]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2 * 3 * (_landscape_w - 1) * (_landscape_h - 1) * sizeof *idata, idata, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 
free(_landscape);
  free(data); 
  free(idata); 
}

void sea_init(void) {
  int i;
  SDL_Surface * t;
  int vp[4], w, h;
  static char * files[] = {"images/gold.jpg", 
			"images/land_ocean_ice_2048_glossmap.png", 
			"images/moon.jpg", 
			"images/sky3.png",
			"images/bois2.jpg",
			"images/bl2.png",
			"images/red.png",
			"images/green.png",
			"images/purple.jpg",
			"images/yellow.png"};
  _pId  = gl4duCreateProgram("<vs>shaders/earth.vs", "<fs>shaders/earth.fs", NULL);
  _pId2  = gl4duCreateProgram("<vs>shaders/cpy/earth.vs", "<fs>shaders/cpy/earth.fs", NULL);
  glGetIntegerv(GL_VIEWPORT, vp);
  w = vp[2] - vp[0];
  h = vp[3] - vp[1];
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.5, 0.5, -0.5 * h / w, 0.5 * h / w, 1.0, 1000.0);
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
  HBB_objLoader();
HBB_initWaterVAO();

}

void decalage(GLfloat * pixels,int w,int h,GLfloat * ligne){
	int i,j;
	for(j=h-1;j>0;j--)
		for(i=0;i<w;i++)
			pixels[i+j*w]=pixels[i+(j-1)*w];

	for(i=0;i<w;i++)
		pixels[i]=ligne[i];	
	
}



static GLfloat * water=NULL;

void waterAnalyzer_draw(float freq[],int NB){
static int waterTimer=0;
static int init=0;
static float intensite=0.008;
float ligne[_landscape_w];
int f;
float rdnb;

GLfloat * data = NULL; 
  GLuint * idata = NULL;

  static GLfloat a0 = 0.0;
  static Uint32 t0 = 0, t;
static int globalTimer=0;
  GLfloat dt = 0.0, lumPos[4], *mat;

////////////// INIT TIMER & DONNER ////////// 
if(globalTimer==0){
globalTimer=SDL_GetTicks();
}
if(waterTimer==0){
waterTimer=SDL_GetTicks();
}
if(init==0){

water = calloc(_landscape_w * _landscape_h, sizeof * water);

assert(water);
init++;
}
//////////// ACTIVE TRANSPARENCE ///////////
 glEnable(GL_BLEND); /// a deactiver
 glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//////////// ACTIVE TRANSPARENCE ///////////


//////////// REINIT WATER VAO ///////////
if(SDL_GetTicks()-waterTimer>100){
for(f=0,rdnb=(rand()%2*0.001);f<_landscape_w;f++)
ligne[f]=freq[f]*intensite+rdnb;

decalage(water,_landscape_w,_landscape_h,ligne);
waterTimer=SDL_GetTicks();
 data = heightMap2Data(water, _landscape_w, _landscape_h);
  idata = heightMapIndexedData(_landscape_w, _landscape_h);
  
  glBindVertexArray(_planVao);
assert(_planVao);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glGenBuffers(2, _planBuffer); 
  glBindBuffer(GL_ARRAY_BUFFER, _planBuffer[0]);
  glBufferData(GL_ARRAY_BUFFER, 6 * _landscape_w * _landscape_h * sizeof *data, data, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof *data, (const void *)0); 
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof *data, (const void *)(3 * sizeof *data));  

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _planBuffer[1]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2 * 3 * (_landscape_w - 1) * (_landscape_h - 1) * sizeof *idata, idata, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 
  free(data); 
  free(idata); 
}
//////////// REINIT WATER VAO ///////////


  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.6, 0.7,0.7, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();
  mat = gl4duGetMatrixData();
  MMAT4XVEC4(lumPos, mat, _lumPos0);

  glUseProgram(_pId);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _tId[0]);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, _tId[1]);
  glUniform1i(glGetUniformLocation(_pId, "tex0"), 0);
  glUniform1i(glGetUniformLocation(_pId, "tex1"), 1);
  glUniform4fv(glGetUniformLocation(_pId, "lumPos"), 1, lumPos);
  glUniform1i(glGetUniformLocation(_pId, "specular"), 1);
  /* envoi de toutes les matrices stockées par GL4D */
  glUniform1i(glGetUniformLocation(_pId, "specular"), 0);
  //  gl4dgDraw(_sphere);

  /////////  DEPLACEMENT DE WATER ///////////////////
	gl4duPushMatrix();{
  gl4duTranslatef(0, 20, -150);
gl4duScalef(50,50,100);
	gl4duRotatef(20,1,0,0);
if(SDL_GetTicks()-globalTimer>10000)
	gl4duRotatef(a0,0,1,0);
 glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, _tId[3]);
	gl4duSendMatrices();
	glBindVertexArray(_planVao);
	glDrawElements(GL_TRIANGLES, 2 * 3 * (_landscape_w - 1) * (_landscape_h - 1), GL_UNSIGNED_INT, 0);
	}gl4duPopMatrix();
  /////////  DEPLACEMENT DE WATER ///////////////////



////////////////// ROTATION ///////////////
  dt = ((t = SDL_GetTicks()) - t0) / 1000.0;
  t0 = t;
  a0 += 360.0 * dt / (6.0)/2;
////////////////// ROTATION ///////////////
/*
if(SDL_GetTicks()-globalTimer>18000){
gl4dfSobelSetMixMode(GL4DF_SOBEL_MIX_MULT);
gl4dfSobel(0, 0, GL_FALSE);
}
*/
}



void voiture_draw3(Uint16 tabOrg[],int NB){
  static GLfloat a0 = 0.0;
  static Uint32 t0 = 0, t;
  GLfloat dt = 0.0, lumPos[4], *mat;
  dt = ((t = SDL_GetTicks()) - t0) / 1000.0;
  t0 = t;
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.0,0.0,0.0,0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();
  //gl4duTranslatef(0, 0, -14);
  mat = gl4duGetMatrixData();
  MMAT4XVEC4(lumPos, mat, _lumPos0);

  glUseProgram(_pId);

 glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _tId[7]);
 glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, _tId[7]);
  glUniform1i(glGetUniformLocation(_pId, "tex0"), 0);
  glUniform1i(glGetUniformLocation(_pId, "tex1"), 1);
 glUniform4fv(glGetUniformLocation(_pId, "lumPos"), 1, lumPos);
  glUniform1i(glGetUniformLocation(_pId, "specular"), 1);
  /* envoi de toutes les matrices stockées par GL4D */
  glUniform1i(glGetUniformLocation(_pId, "specular"), 0);

  ///////// MODIF ///////////////////


gl4duPushMatrix();
gl4duTranslatef(6,0,-30);
 gl4duRotatef(-90,0,1,0);
 gl4duScalef(1.2,1.2,1.2);
 
gl4duRotatef(a0,0,1,0);
gl4duSendMatrices();
glBindVertexArray(_objTabVao[OBJ_LFA]);
glDrawElements(GL_TRIANGLES, 3 * (_tabModel[OBJ_LFA]->numtriangles-1), GL_UNSIGNED_INT, 0);
gl4duPopMatrix();




gl4duPushMatrix();
gl4duTranslatef(-10,-0.5,-30);
gl4duRotatef(90,0,1,0);
gl4duScalef(0.01,0.01,0.01);
gl4duRotatef(a0,0,1,0);
gl4duSendMatrices();
glBindVertexArray(_objTabVao[OBJ_BUGATTI]);
  glDrawElements(GL_TRIANGLES, 3 * (_tabModel[OBJ_BUGATTI]->numtriangles-1), GL_UNSIGNED_INT, 0);
gl4duPopMatrix();



 



 
gl4duPushMatrix();
gl4duTranslatef(0,-2,-30);
gl4duScalef(4,4,4);
gl4duRotatef(a0,0,1,0);
gl4duSendMatrices();
glBindVertexArray(_objTabVao[OBJ_BERLINETTA]);
  glDrawElements(GL_TRIANGLES, 3 * (_tabModel[OBJ_BERLINETTA]->numtriangles-3), GL_UNSIGNED_INT, 0);
gl4duPopMatrix();


 
gl4duPushMatrix();
gl4duTranslatef(0,2,-30);
 gl4duScalef(0.08,0.08,0.08);
gl4duRotatef(a0,0,1,0);
gl4duSendMatrices();
glBindVertexArray(_objTabVao[OBJ_ZONDA]);
  glDrawElements(GL_TRIANGLES, 3 * (_tabModel[OBJ_ZONDA]->numtriangles-1), GL_UNSIGNED_INT, 0);
gl4duPopMatrix();



 
  ///////// MODIF ///////////////////
  a0 += 360.0 * dt / (6.0);
 //gl4dfBlur(0, 0, 5, 1, 0, GL_FALSE);
 gl4dfSobelSetMixMode(GL4DF_SOBEL_MIX_MULT);
 gl4dfSobel(0, 0, GL_FALSE);

}








void city_draw(Uint16 tabOrg[],int NB){
  static GLfloat a0 = 0.0;
  static Uint32 t0 = 0, t;
static int timer=0;
if(timer==0)timer=SDL_GetTicks();
//////////////////////////////////////////////
  GLint v[2];
    glGetIntegerv(GL_POLYGON_MODE, v);
    if(v[0] == GL_FILL)
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


GLfloat dt = 0.0, lumPos[4], *mat;
  dt = ((t = SDL_GetTicks()) - t0) / 1000.0;
  t0 = t;
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.0,0.0,0.0,0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();
  //gl4duTranslatef(0, 0, -14);
  mat = gl4duGetMatrixData();
  MMAT4XVEC4(lumPos, mat, _lumPos0);

  glUseProgram(_pId);





 glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _tId[3]);
 glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, _tId[3]);
  glUniform1i(glGetUniformLocation(_pId, "tex0"), 0);
  glUniform1i(glGetUniformLocation(_pId, "tex1"), 1);
 glUniform4fv(glGetUniformLocation(_pId, "lumPos"), 1, lumPos);
  glUniform1i(glGetUniformLocation(_pId, "specular"), 1);
  /* envoi de toutes les matrices stockées par GL4D */
  glUniform1i(glGetUniformLocation(_pId, "specular"), 0);
  //  gl4dgDraw(_sphere);

  ///////// MODIF ///////////////////
if(SDL_GetTicks()-timer>100){
////////// changement de texture ///////////
static Uint32 image[100000];
static int init_image=0;
int i;
if(init_image==0)
for(init_image++,i=0;i<100000;i++)
image[i]|=-1;


for(i=0;i<100000;i++){
int rdnb;
if(2==(rdnb=(rand()%3)))
image[i]-=1;
else if(rdnb==1)
image[i]-=(1<<8)*(rand()%3)*8;
else 
image[i]-=(1<<16)*(rand()%3)*5;
}

  glBindTexture(GL_TEXTURE_2D, _tId[3]);
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 100, 1000, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

  glBindTexture(GL_TEXTURE_2D,0);
////////// changement de texture ///////////




////////// cam movement ///////////
 gl4duTranslatef(0,0,a0*0.1);
 gl4duRotatef(-a0*0.02,1,0,0);
 gl4duRotatef(-a0*0.02,0,1,0);
 gl4duRotatef(-a0*0.02,0,0,1);
////////// cam movement ///////////

gl4duPushMatrix();
 gl4duTranslatef(0,-0,-120);
gl4duRotatef(30,1,0,0);
gl4duScalef(0.06,0.06,0.06);
//gl4duScalef(0.4,0.4,0.4);
//gl4duScalef(0.01,0.01,0.01);
//gl4duRotatef(a0,0,1,0);
gl4duSendMatrices();
glBindVertexArray(_objTabVao[OBJ_CITY2]);
  glDrawElements(GL_TRIANGLES, 3 * (_tabModel[OBJ_CITY2]->numtriangles-10), GL_UNSIGNED_INT, 0);
gl4duPopMatrix();


  ///////// MODIF ///////////////////
  a0 += 360.0 * dt / (6.0);
}
 //gl4dfBlur(0, 0, 5, 1, 0, GL_FALSE);
 //gl4dfSobelSetMixMode(GL4DF_SOBEL_MIX_MULT); 
 //gl4dfSobel(0, 0, GL_FALSE);

}






void HBB_ModelFree(void){
int freeMemory;

for(freeMemory=0;freeMemory<OBJNB;freeMemory++){
if(_objTabVao[freeMemory])
    glDeleteVertexArrays(1, &_objTabVao[freeMemory]);
if(_objTabBI[freeMemory])
    glDeleteBuffers(1, &_objTabBI[freeMemory]);
if(_objTabBD[freeMemory])
    glDeleteBuffers(1, &_objTabBD[freeMemory]);
printf("FREE ADRESS %p\n",_tabModel[freeMemory]);
if(_tabModel[freeMemory]){
  glmDelete(_tabModel[freeMemory]);
  _tabModel[freeMemory] = NULL;
}
}

printf("SUCESS MODEL FREE\n");
for(freeMemory=0;freeMemory<NBTEX;freeMemory++)
    glDeleteTextures(NBTEX, &_tId[freeMemory]);
    glDeleteVertexArrays(1,&_planVao);
 glDeleteBuffers(2,_planBuffer);

printf("SUCESS TEXTURE FREE\n");
free(water);
 //glBindVertexArray(0);
//gl4duClean(GL4DU_ALL);
}






