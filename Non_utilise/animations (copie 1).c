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

	static Uint32 t0 = 0, t2;
  	static int timer=0;
 	float dt = 0.0;
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
  	dt = ((t2 = SDL_GetTicks()) - t0) / 1000.0;
  	t0 = t2;
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

#define NB 15
int l, i,j; Uint16 * s; 
static Uint16 tab[NB]; 
static Uint16 texTab[NB*NB];

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
		tab[i] = (NB * s[i] / ((1 << 15) - 1.0))/2; 
    return;
  default: /* GL4DH_DRAW */
	if(timer==0)timer=SDL_GetTicks();



memset(texTab,0,NB*NB*sizeof(Uint16));
for(i=0;i<NB;i++)
texTab[i+tab[i]*NB]=2;

for(j=0;j<NB;j++)
for(i=0;i<NB;i++){
if(texTab[j+i*NB]==2)break;
	texTab[j+i*NB]=1;
}



//sea_draw(texTab,NB);// revoir
//if(SDL_GetTicks()-timer>=0&&SDL_GetTicks()-timer<11000)analyser_draw(texTab,NB);
//else if(SDL_GetTicks()-timer>=11000&&SDL_GetTicks()-timer<19000)voiture_draw3();
//else if(SDL_GetTicks()-timer>=19000&&SDL_GetTicks()-timer<28000)city_draw();
// voiture_draw3();
//city_draw();
 superHot_draw();
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

enum{// SDL
  RED=16,// decalage premiere 0 bit
  GREEN=8, // decalage 0~8 bit
  BLUE=0, // decalage 8~16 bit
  ALPHA=24 // decalage 16~24 bit
};

typedef struct node * graph;
typedef struct node node;
struct node{
	int x,y;
	int nodeID;
	int nbfils;
	graph * fils;
};
static int _max=100;
static graph _g;
static int HBB_circleFil(int x0,int y0,int r,Uint32 * pix,int w,int h,Uint32 color){
  // commentaire : j'utilise les fonction round() floor()
  // pour effacer les trous.
  // On peux recuperer avec floor() la valeur exposante 
  // qui a ete perdu par round(), le cas contraire aussi
  // la ceil() marche aussi mais le cercle etait moin arrondi
  double precis= -0.3; 
  int x,i,j,k,posRnd,posFlr,fini=1;
  for(k=0;k<2;k++)  
    for(j=-1;j<2;j+=2)
      for(i=-1;i<2;i+=2)
		for(x=0;x<r;x++){
	  	if(k){
	  	posRnd=(x0+i*x)+(y0+j*(int)round(precis+sqrt(r*r-x*x)))*w;
	  	posFlr=(x0+i*x)+(y0+j*(int)floor(precis+sqrt(r*r-x*x)))*w;
	 	}
	  	else{
		posRnd=(x0+i*(int)round(precis+sqrt(r*r-x*x)))+(y0+j*x)*w;
	    posFlr=(x0+i*(int)floor(precis+sqrt(r*r-x*x)))+(y0+j*x)*w; 		  	
		}
		if(posRnd>=0&&posRnd<w*h&&pix[posRnd]==0){
	    	pix[posRnd]=color;
			fini=0;
		}
		if(posFlr>=0&&posFlr<w*h&&pix[posFlr]==0){
		    pix[posFlr]=color;	
			fini=0;
		}
	}  
	return fini;
}
static void HBB_line(int x0,int y0,int u,int v,Uint32 * pix,int w,int h,int r,int g,int b){
  int x,pos;
  float d,y;
  
  if((abs(u)>abs(v)&&u>0&&v>0)||(abs(u)>abs(v)&&u>0&&v<=0))
    for(y=v/(float)u,x=0,d=0;x<abs(u);x++,d+=y){
      pos=(x+x0)+((int)round(d)+y0)*w;
      if(pos>=0&&pos<w*h)
	pix[pos]=(255<<ALPHA)+(r<<RED)+(g<<GREEN)+(b<<BLUE);
	  }
  else if((abs(u)>abs(v)&&u<=0&&v>0)||(abs(u)>abs(v)&&u<=0&&v<=0))
    for(y=v/(float)u,x=0,d=0;x<abs(u);x++,d+=y){
      pos=(-x+x0)+(-(int)round(d)+y0)*w;
      if(pos>=0&&pos<w*h)
	pix[pos]=(255<<ALPHA)+(r<<RED)+(g<<GREEN)+(b<<BLUE);
    }
  else if((abs(u)<=abs(v)&&u>0&&v>0)||(abs(u)<=abs(v)&&u<=0&&v>0))
    for(y=u/(float)v,x=0,d=0;x<abs(v);x++,d+=y){
      pos=((int)round(d)+x0)+(x+y0)*w;
      if(pos>=0&&pos<w*h)
	pix[pos]=(255<<ALPHA)+(r<<RED)+(g<<GREEN)+(b<<BLUE);
    }
  else if((abs(u)<=abs(v)&&u<=0&&v<=0)||(abs(u)<=abs(v)&&u>0&&v<=0))
    for(y=u/(float)v,x=0,d=0;x<abs(v);x++,d+=y){
      pos=(-(int)round(d)+x0)+(-x+y0)*w;
      if(pos>=0&&pos<w*h)
	pix[pos]=(255<<ALPHA)+(r<<RED)+(g<<GREEN)+(b<<BLUE);(255<<ALPHA)+(r<<RED)+(g<<GREEN)+(b<<BLUE);
    }
}


static graph associer_node(graph * adress,int max,int minNode,int maxNode){

	int i,j;
	int rdnb;
	int rdnode;
	char * rdtab;
	rdtab=(char *)malloc(sizeof(char)*max+1);
	if(rdtab==NULL){
	fprintf(stderr,"err allocation associer_node");
	return NULL;	 
	}	

for(j=0;j<max;j++){
	for(i=0;i<max;i++)
		rdtab[i]=0;
	rdnb=rand()%(maxNode-minNode)+minNode;
	adress[j]->fils=(graph*)malloc(rdnb*sizeof(graph));
	if(adress==NULL){
	fprintf(stderr,"err allocation associer_node");
	return NULL;	
	}	
	adress[j]->nbfils=rdnb;	
	for(i=0;i<rdnb;i++){
		do{
		rdnode=rand()%max;
		}while(rdnode==j||rdtab[rdnode]==1);
		adress[j]->fils[i]=adress[rdnode];
		rdtab[rdnode]=1;	
//		printf("NODE %d FILS %d ADR %p\n",j,rdnode,adress[rdnode]);
	    }
	}
	free(rdtab);
	return adress[0];
}

static graph HBB_createGraph(int nodeMax,int connexionMin,int connexionMax,int w,int h){
	int i;
	graph * adress=NULL;
	adress=(graph*)malloc(nodeMax*sizeof(graph));
	if(adress==NULL){
		fprintf(stderr,"err allocation HBB_createGraph");
		return NULL;	
	}
//	printf("create graph aleatoire en %d noeud\n",max);
	for(i=0;i<nodeMax;i++){
	adress[i]=(graph)malloc(sizeof(node));	
	if(adress[i]==NULL){
		fprintf(stderr,"err allocation HBB_createGraph");
		return NULL;	
	}	// init 
	adress[i]->nodeID=i;
	adress[i]->x=rand()%w; //rand()%WIDTH;	
	adress[i]->y=rand()%h;	//rand()%HEIGHT;	
	// init 	
//	printf("NB %d PTR %p\n",adress[i]->nodeID,adress[i]);
	}
//	printf("\n");
	associer_node(adress,nodeMax,connexionMin,connexionMax);
//printf("ascc  %d\n",adress[1]->fils[0]->fils[1]->nodeID);
	
}

static void sous_delete(graph g,char * flag,graph * adr){
if(g==NULL||flag[g->nodeID]==1)return;
int i;
flag[g->nodeID]=1;
///////////// operation 
adr[g->nodeID]=g;
///////////// operation 
for(i=0;i<g->nbfils;i++)
	sous_delete(g->fils[i],flag,adr);

}


static void HBB_deleteGraph(graph g,int max){
if(g==NULL)return;
char * flag;
graph * adr;
int i;
adr=(graph *)malloc(max*sizeof(graph));
if(adr==NULL){
	fprintf(stderr,"err allocation HBB_deleteGraph");
	return;	
}
flag=(char *)calloc(max,sizeof(char));
if(flag==NULL){
	fprintf(stderr,"err allocation HBB_deleteGraph");
	return;	
}
sous_delete(g,flag,adr);
for(i=0;i<max;i++){
//	printf("chaque node free %p\n",adr[i]);
	if(adr[i]!=NULL)
		free(adr[i]);
	adr[i]=NULL;
}
//printf("ptr graph free %p\n",adr);	
if(adr!=NULL)
free(adr);
if(flag!=NULL)
free(flag);
g=NULL;
adr=NULL;
}


static void sous_affiche(graph g,char * flag,Uint32 *pixels,int w,int h,int x,int y){
if(g==NULL||flag[g->nodeID]==1)return;
flag[g->nodeID]=1;
///////////// operation 
//pixels[g->x+g->y*w]=255;
int i; 
int x0,y0,x1,y1;
for(i=0;i<g->nbfils;i++){
x0=g->x+x;
y0=g->y+y;
x1=g->fils[i]->x-g->x;
y1=g->fils[i]->y-g->y;
if(flag[g->fils[i]->nodeID]!=1)
	HBB_line(x0,y0,x1,y1,pixels,w,h,rand()%256,rand()%256,255);
pixels[g->x+g->y*w]=-1;
HBB_circleFil(x0,y0,1,pixels,w,h,-1);
HBB_circleFil(x0,y0,3,pixels,w,h,-1);
}
///////////// operation

for(i=0;i<g->nbfils;i++)
	sous_affiche(g->fils[i],flag,pixels,w,h,x,y);

}


void HBB_afficheGraph(graph g,int max,Uint32 *pixels,int w,int h,int x,int y){
if(g==NULL)return;
int i;
char *flag;
flag=(char *)calloc(max,sizeof(char));
if(flag==NULL){
fprintf(stderr,"err allocation HBB_afficheGraph");
return;	
}
sous_affiche(g,flag,pixels,w,h,x,y);
free(flag);
}

static void sous_movement(graph g,char * flag,int w,int h,float dt,int rapide){
if(g==NULL||flag[g->nodeID]==1)return;
flag[g->nodeID]=1;
///////////// operation 
if(g->x!=w/2){
if(g->x>w/2)g->x-=round(rapide*dt*w/h);
else if(g->x<w/2)g->x+=round(rapide*dt*w/h);
}
if(g->y!=h/2){
if(g->y>h/2)g->y-=round(rapide*dt);
else if(g->y<h/2)g->y+=round(rapide*dt);
}///////////// operation

int i; 
for(i=0;i<g->nbfils;i++)
	sous_movement(g->fils[i],flag,w,h,dt,rapide);
}

void HBB_movementGraph(graph g,int max,int w,int h,float dt,int rapide){
if(g==NULL)return;
int i;
char *flag;
flag=(char *)calloc(max+1,sizeof(char));
if(flag==NULL){
fprintf(stderr,"err allocation HBB_afficheGraph");
return;	
}
sous_movement(g,flag,w,h,dt,rapide);
free(flag);
}
void sous_reinitPosition(graph g,char * flag,int w,int h){
if(g==NULL||flag[g->nodeID]==1)return;
flag[g->nodeID]=1;
///////////// operation 
	g->x=rand()%w; //rand()%WIDTH;	
	g->y=rand()%h;	//rand()%HEIGHT;
///////////// operation 

int i; 
for(i=0;i<g->nbfils;i++)
	sous_reinitPosition(g->fils[i],flag,w,h);

}


void HBB_reinitPositionGraph(graph g,int max,int w,int h){
int i;
char *flag;
flag=(char *)calloc(max,sizeof(char));
if(flag==NULL){
fprintf(stderr,"err allocation HBB_afficheGraph");
return;	
}
sous_reinitPosition(g,flag,w,h);
free(flag);
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
  
init_graph(windowW,windowH);
 //glEnable(GL_DEPTH);
    return;
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    if(tId) {
      glDeleteTextures(1, &tId);
      tId = 0;
    }
free(pixels);
delete_graph();
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
affiche_graph(pixels,windowW,windowH);

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
