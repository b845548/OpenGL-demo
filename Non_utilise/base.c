/*!\file base.c
 * \brief géométries lumière diffuse et transformations de base en GL4Dummies
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date April 15 2016 */
#include <stdio.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4df.h>

/*!\brief identifiant du programme GLSL */
static GLuint _pId = 0;
/*!\brief quelques objets géométriques */
static GLuint _sphere = 0, _cube = 0, _quad = 0;

void base_init(void) {
  glBindVertexArray(0);
  int vp[4], w, h;
  glGetIntegerv(GL_VIEWPORT, vp);
  w = vp[2] - vp[0];
  h = vp[3] - vp[1];
  _pId  = gl4duCreateProgram("<vs>shaders/particle.vs", "<fs>shaders/particle.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.5, 0.5, -0.5 * h / w, 0.5 * h / w, 1.0, 1000.0);
  gl4duBindMatrix("modelViewMatrix");
  _sphere = gl4dgGenSpheref(30, 30);
  _cube = gl4dgGenCubef();
  _quad = gl4dgGenQuadf();




}

void base_draw(void) {
static Uint32 t0 = 0, t2;
float dt = 0.0;
static int timer =0;
if(timer==0)timer=SDL_GetTicks();
  	dt = ((t2 = SDL_GetTicks()) - t0) / 1000.0;
  	t0 = t2;

  //GLfloat rouge[] = {1, 0, 0, 1}, vert[] = {0, 1, 0, 1};
GLfloat bleu[] = {0, 0, 1, 0.7};

 //glEnable(GL_DEPTH_TEST);
 //glEnable(GL_BLEND);
 glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
 //glClearColor(1.0f, 0.7f, 0.7f, 0.0f);
glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();
  glUseProgram(_pId);
if(SDL_GetTicks()-timer>500){
  gl4duTranslatef(0, 0, -30.0);
static int rapide=0;
rapide+=dt*100;
  gl4duRotatef(rapide, 1,0,0);
  gl4duRotatef(rapide, 0,1,0);
int i;
static int init=0;
#define NBPARTICLE 1000
static float tabX[NBPARTICLE];
static float tabY[NBPARTICLE];
static float tabZ[NBPARTICLE];
static float tabVX[NBPARTICLE];
static float tabVY[NBPARTICLE];
static float tabVZ[NBPARTICLE];

///////// INIT PARTICLE /////////
if(init==0){
	for(i=0;i<NBPARTICLE;i++){
	tabX[i]=tabY[i]=tabZ[i]=0;
	tabVX[i]=gl4dmSURand()* 5;
	tabVY[i]=gl4dmSURand()* 5;
	tabVZ[i]=gl4dmSURand()* 5;
	}
init++;
}

///////// MOVEMENT PARTICLE /////////
if(SDL_GetTicks()-timer>700){
for(i=0;i<NBPARTICLE;i++){
tabX[i]+=tabVX[i]*dt;
tabY[i]+=tabVY[i]*dt;
tabZ[i]+=tabVZ[i]*dt;
}


///////// AFFICHE PARTICLE /////////
for(i=0;i<NBPARTICLE;i++){
	  gl4duPushMatrix();{
	  gl4duTranslatef(tabX[i], tabY[i],tabZ[i]);
    gl4duScalef(0.07,0.07,0.07);
	  gl4duRotatef(tabX[0], 1,0,1);
	  gl4duSendMatrices();
	  glUniform4fv(glGetUniformLocation(_pId, "couleur"), 1,bleu);
	  //gl4dgDraw(_cube);  
	gl4dgDraw(_sphere);
	  } gl4duPopMatrix();
  }
}

}
  gl4dfBlur(0, 0, 6, 1, 0, GL_FALSE);
  gl4dfSobelSetMixMode(GL4DF_SOBEL_MIX_MULT);
  gl4dfSobel(0, 0, GL_FALSE);
}
