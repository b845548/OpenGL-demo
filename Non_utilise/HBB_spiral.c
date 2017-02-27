/*!\file window.c
 * \brief géométries lumière diffuse et transformations de base en GL4Dummies
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date April 15 2016 */
#include <stdio.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4df.h>
#include <GL4D/gl4duw_SDL2.h>
/* Prototypes des fonctions statiques contenues dans ce fichier C */
void sprial_initGL(void);
void sprial_initData(void);
static void resize(int w, int h);
void sprial_draw(void);
/*!\brief dimensions de la fenêtre */
static int _windowWidth =1280, _windowHeight = 1000;
/*!\brief identifiant du programme GLSL */
static GLuint _pId = 0;
/*!\brief quelques objets géométriques */
static GLuint _sphere = 0, _cube = 0, _quad = 0;
/*!\brief La fonction principale créé la fenêtre d'affichage,
 * initialise GL et les données, affecte les fonctions d'événements et
 * lance la boucle principale d'affichage.*/
/*
int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "GL4Dummies", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,_windowWidth, _windowHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN))return 1;
  initGL();
  initData();
  atexit(quit);
  gl4duwResizeFunc(resize);
  gl4duwDisplayFunc(draw);
  gl4duwMainLoop();
  return 0;
}
*/
/*!\brief initialise les paramètres OpenGL */
void sprial_initGL(void) {
  //gl4duClean(GL4DU_ALL);
  //glEnable(GL_DEPTH_TEST);
  //glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  //glClearColor(1.0f, 0.7f, 0.7f, 0.0f);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  _pId  = gl4duCreateProgram("<vs>shaders/shaders/basic.vs", "<fs>shaders/shaders/basic.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  resize(_windowWidth, _windowHeight);



}
GLuint tId;
/*!\brief initialise les données */
void sprial_initData(void) {
  _sphere = gl4dgGenSpheref(30, 30);
  _cube = gl4dgGenCubef();
  _quad = gl4dgGenQuadf();




    glGenTextures(1, &tId);
    glBindTexture(GL_TEXTURE_2D, tId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
}
/*!\brief Cette fonction paramétre la vue (viewport) OpenGL en
 * fonction des dimensions de la fenêtre.*/
static void resize(int w, int h) {
  _windowWidth  = w; _windowHeight = h;
  glViewport(0, 0, _windowWidth, _windowHeight);
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.5, 0.5, -0.5 * _windowHeight / _windowWidth, 0.5 * _windowHeight / _windowWidth, 1.0, 1000.0);
  gl4duBindMatrix("modelViewMatrix");
}
/*!\brief dessine dans le contexte OpenGL actif. */
/*

static void HBB_circle(int x0,int y0,int r,GLuint * pix,int w,int h,GLuint color){
  double add= -0.3;
  int x,i,j,k,pos;
  for(k=0;k<2;k++)  
    for(j=-1;j<2;j+=2)
      for(i=-1;i<2;i+=2)
		for(x=0;x<r;x++){
	  if(k)
	    pos=(x0+i*x)+(y0+j*(int)round(add+sqrt(r*r-x*x)))*w;
	  else
	    pos=(x0+i*(int)round(add+sqrt(r*r-x*x)))+(y0+j*x)*w;
	  if(pos>=0&&pos<w*h)
	    pix[pos]=color;
	}  
}





static void HBB_spiralCombine(int x0, int y0,int w,int h,Uint32 * pixels,int largeur,int iteration,Uint32 color){
	int i;
	for(i=0;i<iteration;i+=2){
	HBB_spiral(x0+largeur*2,y0,largeur*2+i*largeur,pixels,w,h,color,-1);
	HBB_spiral(x0+largeur*2-largeur*2,y0,largeur*2+i*largeur,pixels,w,h,color,1);
	}
}
static void HBB_spiralSimple(int x0, int y0,int w,int h,Uint32 * pixels,int largeur,int iteration,Uint32 color){
	int i;
	for(i=1;i<iteration;i++){
		HBB_spiral(x0+largeur/2, y0,largeur/2+i*largeur,pixels,w,h,color,1);
		HBB_spiral(x0+largeur, y0,largeur+i*largeur,pixels,w,h,color,-1);
	}
}
*/
static void HBB_spiral(int x0,int y0,int r,GLuint * pix,int w,int h,GLuint color,int key){
  double precis= -0.3;
  int x,i,j,pos;
  for(j=0;j<2;j++)  
      for(i=-1;i<2;i+=2)
		for(x=0;x<r;x++){
	  if(j)
	    pos=(x0+i*x)+(y0+key*(int)round(precis+sqrt(r*r-x*x)))*w;
	  else
	    pos=(x0+i*(int)round(precis+sqrt(r*r-x*x)))+(y0+key*x)*w;
	  if(pos>=0&&pos<w*h)
	    pix[pos]=color;
	}  
}
static void HBB_spiralSpecial(int x0, int y0,int w,int h,Uint32 * pixels,int largeur,int iteration,Uint32 color){
	int i;

	for(i=2;i<iteration;i++){
	HBB_spiral(x0+largeur/2,y0,largeur/2+i*largeur,pixels,w,h,color,1);
	HBB_spiral(x0+largeur*2,y0,largeur*2+i*largeur,pixels,w,h,color,-1);
	}
	
	for(i=2;i<iteration;i++){
		HBB_spiral(x0+largeur*2-largeur/2,y0,largeur/2+i*largeur,pixels,w,h,color,-1);
		HBB_spiral(x0+largeur*2-largeur*2,y0,largeur*2+i*largeur,pixels,w,h,color,1);
	}
}


void sprial_draw(void) {
  static GLfloat a = 0;
  GLfloat rouge[] = {1, 0, 0, 1};//, vert[] = {0, 1, 0, 1}, bleu[] = {0, 0, 1, 0.7};

glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();
  glUseProgram(_pId);


static Uint32 t0 = 0, t2;
float dt = 0.0;
static int timer =0;
static int timer2 =0;
if(timer==0){
timer=SDL_GetTicks();
timer2=SDL_GetTicks();
}  	dt = ((t2 = SDL_GetTicks()) - t0) / 1000.0;
  	t0 = t2;

int i,j,k;
#define W 100
#define H 100
static int init=0; 
static Uint32 forceVX[W*H];
static Uint32 forceVY[W*H];
static Uint32 forceVZ[W*H];
static Uint32 transtaleX[W*H];
static Uint32 transtaleY[W*H];
static Uint32 transtaleZ[W*H];

static Uint32 pix[W*H];
//static Uint32 randPix[W*H];
if(SDL_GetTicks()-timer2>100){ // probleme de commencement
if(init==0){
	memset(transtaleX,0,W*H*sizeof(Uint32));
	memset(transtaleY,0,W*H*sizeof(Uint32));
	memset(transtaleZ,0,W*H*sizeof(Uint32));
        memset(pix,0,W*H*sizeof(Uint32));
        HBB_spiralSpecial(W/2,H/2,W,H,pix,4,9,255);
	for(i=0;i<W*H;i++){
	    forceVX[i]=rand()%100+50;
	    forceVY[i]=rand()%100+50;
	    forceVZ[i]=rand()%100+50;
	//((float)rand()/(float)(RAND_MAX)) *100;// random number float
	}
	//for(i=0/*e*/;i<40;i+=2)
	//    HBB_circle(W/2,H/2,i,pix,W,H,255);
 init++;
}
static int changement=0;
static int inversement=-1;

if((SDL_GetTicks()-timer>2500&&changement==0)||(SDL_GetTicks()-timer>1500&&changement==1)){
inversement=-inversement;
timer=SDL_GetTicks();
if(SDL_GetTicks()-timer2>4000)changement=1;
}

	for(i=0;i<W*H;i++){

if(inversement==-1){        
	transtaleX[i]+=dt*forceVX[i];
        transtaleY[i]+=dt*forceVY[i]; 
        transtaleZ[i]+=dt*forceVZ[i];
}else if(inversement==1){
   transtaleX[i]-=dt*forceVX[i];
 transtaleY[i]-=dt*forceVY[i];
 transtaleZ[i]-=dt*forceVZ[i];
if( transtaleX[i]>4294967000) transtaleX[i]=0;
if( transtaleY[i]>4294967000) transtaleY[i]=0;
if( transtaleZ[i]>4294967000) transtaleZ[i]=0;

}
}
static int ab=0;
a+=dt*20;
ab+=dt*100;
gl4duRotatef(90,0,0,1);
  gl4duSendMatrices();
gl4duTranslatef(1,-3.5, -35);
gl4duRotatef(170+ab, 1, 1,0);
gl4duSendMatrices();
for(k=0;k<1;k++){
  gl4duSendMatrices();
for(j=0;j<H;j++){
  for(i=0;i<W;i++){
    gl4duPushMatrix();
    gl4duTranslatef(i*0.07, 0,0);
    gl4duScalef(0.05, 0.05, 0.05);
    gl4duTranslatef(transtaleX[i+j*W],transtaleY[i+j*W],transtaleZ[i+j*W]);
//gl4duRotatef(80, 0,0,1);
    gl4duSendMatrices();
    glUniform4fv(glGetUniformLocation(_pId, "couleur"), 1, rouge);  

    if(pix[i+j*W]==255/*&&randPix[i+j*W]==1*/)
   	 gl4dgDraw(_sphere);
    gl4duPopMatrix();
  }
  gl4duTranslatef(0, 0.07, 0);
gl4duRotatef(a*0.10, 1, 0,0);
if(changement)
gl4duRotatef(a*0.45, 0, 1,0);
  gl4duSendMatrices();
}
  gl4duSendMatrices();
}
}


 // gl4dfBlur(0, 0, 1, 1, 0, GL_FALSE);
  //gl4dfSobelSetMixMode(GL4DF_SOBEL_MIX_MULT);
  //gl4dfSobel(0, 0, GL_FALSE); 
}
/*!\brief appelée au moment de sortir du programme (atexit), libère les éléments utilisés */
