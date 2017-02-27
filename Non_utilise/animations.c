/*!\file animations.c
 *
 * \brief Votre espace de liberté : c'est ici que vous pouvez ajouter
 * vos fonctions de transition et d'animation avant de les faire
 * référencées dans le tableau _animations du fichier \ref window.c
 *
 * Des squelettes d'animations et de transitions sont fournis pour
 * comprendre le fonctionnement de la bibliothèque. En bonus des
 * exemples dont un fondu en GLSL.
 *
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date May 05, 2014
 */

#include <GL4D/gl4dh.h>
#include "audioHelper.h"
#include "animations.h"
#include <assert.h>
#include <stdlib.h>
#include <GL4D/gl4dg.h>
#include <SDL_image.h>

/*!\brief identifiant de la géométrie QUAD GL4Dummies */
static GLuint _quadId = 0;

void transition_fondu(void (* a0)(int), void (* a1)(int), Uint32 t, Uint32 et, int state) {
  /* INITIALISEZ VOS VARIABLES */
  int vp[4], i;
  GLint tId;
  static GLuint tex[2], pId;
  switch(state) {
  case GL4DH_INIT:
    /* INITIALISEZ VOTRE TRANSITION (SES VARIABLES <STATIC>s) */
    glGetIntegerv(GL_VIEWPORT, vp);
    glGenTextures(2, tex);
    for(i = 0; i < 2; i++) {
      glBindTexture(GL_TEXTURE_2D, tex[i]);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, vp[2], vp[3], 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }
    pId = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/mix.fs", NULL);
    return;
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    if(tex[0]) {
      glDeleteTextures(2, tex);
      tex[0] = tex[1] = 0;
    }
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR LES DEUX ANIMATIONS EN FONCTION DU SON */
    if(a0) a0(state);
    if(a1) a1(state);
    return;
  default: /* GL4DH_DRAW */
    /* RECUPERER L'ID DE LA DERNIERE TEXTURE ATTACHEE AU FRAMEBUFFER */
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &tId);
    /* JOUER LES DEUX ANIMATIONS */
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,  tex[0],  0);
    if(a0) a0(state);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,  tex[1],  0);
    if(a1) a1(state);
    /* MIXER LES DEUX ANIMATIONS */
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,  tId,  0);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glUseProgram(pId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex[1]);
    if(et / (GLfloat)t > 1) {
      fprintf(stderr, "%d-%d -- %f\n", et, t, et / (GLfloat)t);
      exit(0);
    }
    glUniform1f(glGetUniformLocation(pId, "dt"), et / (GLfloat)t);
    glUniform1i(glGetUniformLocation(pId, "tex0"), 0);
    glUniform1i(glGetUniformLocation(pId, "tex1"), 1);
    gl4dgDraw(_quadId);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    return;
  }
}

void animation_flash(int state) {
  /* INITIALISEZ VOS VARIABLES */
  int l, i;
  Uint16 * s;
  static GLfloat c[4] = {0, 0, 0, 0};
  switch(state) {
  case GL4DH_INIT:
    /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    return;
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
    s = (Uint16 *)ahGetAudioStream();
    l = ahGetAudioStreamLength();
    if(l >= 8)
      for(i = 0; i < 4; i++)
	c[i] = s[i] / ((1 << 16) - 1.0);
    return;
  default: /* GL4DH_DRAW */
    /* JOUER L'ANIMATION */
    glClearColor(c[0], c[1], c[2], c[3]);
    glClear(GL_COLOR_BUFFER_BIT);
    return;
  }
}



void animation_eclaire(int state){
  switch(state) {
  case GL4DH_DRAW:
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    return;
  default:
    return;
  }
}

void animation_vide(int state){
  switch(state) {
  case GL4DH_DRAW:
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    return;
  default:
    return;
  }
}

void animation_damier(int state){
  /* INITIALISEZ VOS VARIABLES */
  static GLuint tId = 0, pId;
  static const GLubyte t[16] = {-1,  0,  0, -1, /* rouge */
				 0, -1,  0, -1, /* vert  */
				 0,  0, -1, -1, /* bleu  */
				-1, -1,  0, -1  /* jaune */ };
 static Uint32 * pixels;

  	static int timer=0;
int windowW=1600;
int windowH=1500;

  switch(state) {
  case GL4DH_INIT:
    /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    glGenTextures(1, &tId);
    glBindTexture(GL_TEXTURE_2D, tId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, t);
    pId = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic.fs", NULL);

 glDisable(GL_BLEND);
glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  
 //glEnable(GL_DEPTH);
    return;
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    if(tId) {
      glDeleteTextures(1, &tId);
      tId = 0;
    }
free(pixels);

    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
    return;
  default: /* GL4DH_DRAW */
    /* JOUER L'ANIMATION */
	if(timer==0)timer=SDL_GetTicks();

pixels = (Uint32 *)malloc(windowW*windowH*sizeof(Uint32));
    memset(pixels, 0, windowW *windowH* sizeof(Uint32));

 HBB_endingApi8(pixels,windowW,windowH);
	HBB_flip(pixels,windowW,windowH);

    glUseProgram(pId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,windowW,windowH, 0, GL_BGRA, GL_UNSIGNED_BYTE,pixels);
    glUniform1i(glGetUniformLocation(pId, "myTexture"), 0);
    gl4dgDraw(_quadId);
    return;
  }
}

void animation_opening(int state) {
  /* INITIALISEZ VOS VARIABLES */
  	static GLuint tId = 0, pId;
 	static Uint32 * pixels;
	static Uint32 t0 = 0, t2;
  	static int timer=0;
 	float dt = 0.0;
int windowW=2000;
int windowH=1200;
  switch(state) {
  case GL4DH_INIT:
    /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    glGenTextures(1, &tId);
    glBindTexture(GL_TEXTURE_2D, tId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE,NULL);
    pId = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic.fs", NULL);
    return;
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    if(tId) {
     glDeleteTextures(1, &tId);
      tId = 0;
    }free(pixels);

    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
    return;
  default: 
	/* GL4DH_DRAW */
 	/* JOUER L'ANIMATION */
  	dt = ((t2 = SDL_GetTicks()) - t0) / 1000.0;
  	t0 = t2;
	if(timer==0)timer=SDL_GetTicks();
	pixels = (Uint32 *)malloc(windowW*windowH*sizeof(Uint32));
 	memset(pixels, 0, windowW *windowH* sizeof(Uint32));
	HBB_endingApi8(pixels,windowW,windowH,dt);
	HBB_flip(pixels,windowW,windowH);
	 glDisable(GL_DEPTH);
    glUseProgram(pId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,windowW,windowH, 0, GL_BGRA, GL_UNSIGNED_BYTE,pixels);
    glUniform1i(glGetUniformLocation(pId, "myTexture"), 0);
    gl4dgDraw(_quadId);
    return;
  }
}
void animation_ending(int state) {
  /* INITIALISEZ VOS VARIABLES */
  static GLuint tId = 0, pId;
  static const GLubyte t[16] = {-1,  0,  0, -1, /* rouge */
				 0, -1,  0, -1, /* vert  */
				 0,  0, -1, -1, /* bleu  */
				-1, -1,  0, -1  /* jaune */ };
 static Uint32 * pixels;

	static Uint32 t0 = 0, t2;
  	static int timer=0;
 	float dt = 0.0;
int windowW=2400;
int windowH=1400;
  switch(state) {
  case GL4DH_INIT:
    /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    glGenTextures(1, &tId);
    glBindTexture(GL_TEXTURE_2D, tId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, t);
    pId = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic.fs", NULL);
    return;
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    if(tId) {
      glDeleteTextures(1, &tId);
      tId = 0;
    }
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
    return;
  default: /* GL4DH_DRAW */
    /* JOUER L'ANIMATION */
  	dt = ((t2 = SDL_GetTicks()) - t0) / 1000.0;
  	t0 = t2;
	if(timer==0)timer=SDL_GetTicks();	
pixels = (Uint32 *)malloc(windowW*windowH*sizeof(Uint32));
    memset(pixels, 0, windowW *windowH* sizeof(Uint32));

    if(SDL_GetTicks()-timer>0&&SDL_GetTicks()-timer<=4000)
    HBB_endingRemerciement(pixels,windowW,windowH);
    else if(SDL_GetTicks()-timer>4000&&SDL_GetTicks()-timer<=10000)
    HBB_endingRealisateur(pixels,windowW,windowH,dt);
    else if(SDL_GetTicks()-timer>10500&&SDL_GetTicks()-timer<=13000)
    HBB_endingParis8(pixels,windowW,windowH,dt);
    HBB_flip(pixels,windowW,windowH);

    glDisable(GL_DEPTH);
    glUseProgram(pId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,windowW,windowH, 0, GL_BGRA, GL_UNSIGNED_BYTE,pixels);
    glUniform1i(glGetUniformLocation(pId, "myTexture"), 0);
    gl4dgDraw(_quadId);
free(pixels);
    return;
  }
}
void animation_tron(int state) {
  switch(state) {
  case GL4DH_INIT:
	tron_initData();
	tron_initGL();
    return;
  case GL4DH_FREE:
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    return;
  default: /* GL4DH_DRAW */
    tron_draw();
    return;
  }
}

void animation_earth(int state) {
  switch(state) {
  case GL4DH_INIT:
    earth_init();
    return;
  case GL4DH_FREE:
    HBB_ModelFree2();
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    return;
  default: /* GL4DH_DRAW */
    earth_draw();
    return;
  }
}
/*
void animation_analyser(int state) {
#define NB 15
int l, i,j; Uint16 * s; 
static Uint16 tab[NB]; 
static Uint16 texTab[NB*NB];


  switch(state) {
  case GL4DH_INIT:
   analyser_init();

    return;
  case GL4DH_FREE:
  HBB_ModelFree3();
    return;
  case GL4DH_UPDATE_WITH_AUDIO:

	s = (Uint16 *)ahGetAudioStream();
	l = ahGetAudioStreamLength(); 
	if(l >= 2 * NB)
		for(i = 0; i < NB; i++)
		tab[i] = (NB * s[i] / ((1 << 15) - 1.0))/2; 
    return;
  default: 
	
memset(texTab,0,NB*NB*sizeof(Uint16));
for(i=0;i<NB;i++)
texTab[i+tab[i]*NB]=2;

for(j=0;j<NB;j++)
for(i=0;i<NB;i++){
if(texTab[j+i*NB]==2)break;
	texTab[j+i*NB]=1;
}
   analyser_draw(texTab,NB);
    return;
  }
}
*/
void animation_voiture(int state) {
  switch(state) {
  case GL4DH_INIT:
    voiture_init();
    return;
  case GL4DH_FREE:
    HBB_ModelFree2();
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    return;
  default: /* GL4DH_DRAW */
    voiture_draw();
    return;
  }
}
void animation_soleil(int state) {
  switch(state) {
  case GL4DH_INIT:
    soleil_init();
    return;
  case GL4DH_FREE:
    soleil_free();
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    return;
  default: /* GL4DH_DRAW */
    soleil_draw();
    return;
  }
}

void animation_objet(int state) {

#define NB 50
int l, i,j; Uint16 * s; 
static float tab[NB]; 

static int timer=0;

  switch(state) {
  case GL4DH_INIT:
    sea_init();
    return;
  case GL4DH_FREE:
    HBB_ModelFree();
 case GL4DH_UPDATE_WITH_AUDIO:

	s = (Uint16 *)ahGetAudioStream();
	l = ahGetAudioStreamLength(); 
	if(l >= 2 * NB)
		for(i = 0; i < NB; i++)
		tab[i] = (s[i] / ((1 << 15) - 1.0)); 
    return;
  default: /* GL4DH_DRAW */



waterAnalyzer_draw(tab,NB);


//sea_draw(texTab,NB);// revoir
//if(SDL_GetTicks()-timer>=0&&SDL_GetTicks()-timer<11000)analyser_draw(texTab,NB);
//else if(SDL_GetTicks()-timer>=11000&&SDL_GetTicks()-timer<19000)voiture_draw3();
//else if(SDL_GetTicks()-timer>=19000&&SDL_GetTicks()-timer<28000)city_draw();
// voiture_draw3();
//city_draw();
 //superHot_draw();
    return;
  }
}

void animation_base(int state) {
  switch(state) {
  case GL4DH_INIT:
    base_init();
    return;
  case GL4DH_FREE:
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    return;
  default: /* GL4DH_DRAW */
    base_draw();
    return;
  }
}

void animation_spiral(int state) {
  switch(state) {
  case GL4DH_INIT:
	sprial_initGL();
	sprial_initData();
    return;
  case GL4DH_FREE:
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    return;
  default: /* GL4DH_DRAW */
sprial_draw();
    return;
  }
}


void animation_graph(int state){
  /* INITIALISEZ VOS VARIABLES */
  static GLuint tId = 0, pId;
  static const GLubyte t[16] = {-1,  0,  0, -1, /* rouge */
				 0, -1,  0, -1, /* vert  */
				 0,  0, -1, -1, /* bleu  */
				-1, -1,  0, -1  /* jaune */ };
 static Uint32 * pixels;

	static Uint32 t0 = 0, t2;
  	static int timer=0;
 	float dt = 0.0;
	int windowW=1280;
	int windowH=1000;
	static graph g;
	static int maxNode=100;
  switch(state) {
  case GL4DH_INIT:
    /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    glGenTextures(1, &tId);
    glBindTexture(GL_TEXTURE_2D, tId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, t);
    pId = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic.fs", NULL);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    g=HBB_createGraph(maxNode,1,5,windowW,windowH);
 //glEnable(GL_DEPTH);
    return;
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    if(tId) {
      glDeleteTextures(1, &tId);
      tId = 0;
    }
    free(pixels);
  //  HBB_deleteGraph(g,maxNode);
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
    return;
  default: /* GL4DH_DRAW */
    /* JOUER L'ANIMATION */
  	dt = ((t2 = SDL_GetTicks()) - t0) / 1000.0;
  	t0 = t2;
	if(timer==0)timer=SDL_GetTicks();
	pixels = (Uint32 *)malloc(windowW*windowH*sizeof(Uint32));
	memset(pixels,0, windowW *windowH* sizeof(Uint32));
	HBB_movementGraph(g,maxNode,windowW,windowH,dt,500);
	HBB_afficheGraph(g,maxNode,pixels,windowW,windowH,0,0);

	if(SDL_GetTicks()-timer>500){
	HBB_reinitPositionGraph(g,maxNode,windowW,windowH);
	timer=SDL_GetTicks();
	}
    glUseProgram(pId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,windowW,windowH, 0, GL_BGRA, GL_UNSIGNED_BYTE,pixels);
    glUniform1i(glGetUniformLocation(pId, "myTexture"), 0);
    gl4dgDraw(_quadId);
    return;
  }
}
void animationsInit(void) {
  if(!_quadId)_quadId = gl4dgGenQuadf();
}
