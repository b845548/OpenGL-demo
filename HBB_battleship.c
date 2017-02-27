#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL_image.h>

#include "glm/glm.h"
#include "HBB_fractal.h"
#define HBB_PI 3.14159265

//////////////// PROTOTYPE //////////////////

extern void HBB_flip(Uint32 *,int,int);
static void quit(void);
static void initGL(void);
static void initData(void);
//static void resize(int w, int h);
static void idle(void);
//static void keydown(int keycode);
//static void keyup(int keycode);
static void draw(void);
static void draw2(void);

static void HBB_objLoader5(void);
static void HBB_textureLoader(void);
static void HBB_initWaterVAO(void);
static void HBB_initAvion(void);
static void HBB_initSkyboxVAO(void);
static void HBB_initTurretAI(void);
static void HBB_initWater(void);
static void HBB_deplacementAI(double dt);
static void HBB_deplacementAvion(double dt);
static void HBB_deplacementProjectile(double dt,GLuint w,GLuint h);
static void HBB_deplacementWater(double dt);
static void HBB_missileTracking(GLfloat x0,GLfloat y0,GLfloat z0,GLfloat x1,GLfloat y1,GLfloat z1,GLfloat *theta,GLfloat *thetaHauter);
static void HBB_drawSkybox(void); 

//////////////// PROTOTYPE //////////////////

/////////////// ETC /////////////
#define WORLDSCALE 15000
static int _windowWidth = 1280, _windowHeight = 1000,_landscape_w = 203, _landscape_h = 613;

static GLuint _planVao = 0;
static GLuint _planBuffer[2] = {0};

static GLuint _skyboxtabVao[5]={0,0,0,0,0};
static GLuint _skyboxtabBuffer[5]={0,0,0,0,0};

static GLuint _pId = 0;

static GLfloat * _landscape;
static GLuint _quad = 0;
/////////////// ETC /////////////

//////////////// OBJET LOADING //////////////////
#define OBJMAX 8
#define OBJNB 7// Pour ajouter un objet,augumenter la valeur +1
enum objetType {// Pour ajouter l'objet suivent	
OBJ_F15 = 0,
OBJ_PORTEAVION = 1,
OBJ_MISSILE = 2,
OBJ_CRUISER = 3,
OBJ_F4E = 4,
OBJ_B2 = 5,
OBJ_PIG = 6,
};

static char *OBJFilePath[]={"Objet3D/F-15/F-15C_Eagle.obj"
			,"Objet3D/AircraftCarrier/AircraftCarrier.obj"
			,"Objet3D//MK83Bomb/Files/mk83.obj"
			,"Objet3D/cruiser/cruiser.obj"
			,"Objet3D/F-4E/F-4E_Phantom_II.obj"
			,"Objet3D/B2Spirit/B2_Spirit.obj"
			,"Objet3D/pig/pig.obj"
};
static GLMmodel *_tabModel[OBJMAX];
static GLuint _objTabVao[OBJMAX],_objTabBD[OBJMAX],_objTabBI[OBJMAX];

//////////////// OBJET LOADING //////////////////


/////////////////// TEXTURE LOADING ////////////

enum texture_t{
TEX_GRAYPOLYGON = 0,
TEX_SEA=1,
TEX_CAMOUFLAGE2=2,
TEX_METALBLACK=3,
TEX_FLAME=4,
TEX_PANORAMA=5,
TEX_SCREEN=6
};
#define NBTEXTURE 7
static GLuint _tabTexId[100] = {0,0,};
static char *TEXFilePath[]={"Texture/a.png"
			   ,"Texture/left.jpg"
			   ,"Texture/camouflage2.jpg"
			   ,"Texture/metalBlack.jpg"
			   ,"Texture/flame.jpg"
			   ,"Texture/Panorama1.jpg"
			    ,"Texture/bluescreen.png"
};



/////////////////// TEXTURE LOADING ////////////


//////////////////  WEAPON MODE /////////////////
enum play_mode_t{
  MODE_AVION=0,
  MODE_MISSILE=1,
};
static int _weaponMode = MODE_AVION;
//////////////////  WEAPON MODE /////////////////


////////// KEYS ////////////
enum kyes_t {
  KLEFT = 0,
  KRIGHT,
  KUP,
  KDOWN,
  KAVION_DEMARER,
  KAVION_RECULER
};

static GLuint _keys[] = {0, 0, 0, 0,0,0};
////////// KEYS ////////////







//////////////////// CAMERA ////////////////////////////////////

typedef struct cam_t cam_t;
/*!\brief structure de données pour la caméra */
struct cam_t {
  GLfloat x,y, z;
  GLfloat theta;
};
/*!\brief la caméra */
static cam_t _cam = {0,370,0,0};


//////////////////// CAMERA ////////////////////////////////////



//////////// PLAYER AVION MOVEMENT ///////////
typedef struct avion_t avion_t;
struct avion_t {
  GLfloat x,y, z;
  GLfloat theta,thetaHauter;
  GLfloat force;
GLfloat centre;
GLfloat centre2;
GLfloat centre3;
};

avion_t avion;
#define AVIONRAPIDITE 1700
#define AVIONFORCE 0.0014
static void HBB_initAvion(){

avion.x= _cam.x;
avion.y= _cam.y;
avion.z= _cam.z;
avion.theta =_cam.theta;// -6.00 ~ 6.00
avion.thetaHauter=0;// 0.00 ~ 0.75
avion.force=0;//0 ~ 1
avion.centre=0; //-1.5 ~ 1.5 z
avion.centre2=0; //-1.5 ~ 1.5 y
avion.centre3=0; //0.0 ~ 1.0 x
}


static void HBB_deplacementAvion(double dt){
int xm, ym; double degree= HBB_PI /4;
//SDL_PumpEvents();
SDL_GetMouseState(&xm, &ym);
xm=220;/// 
ym=250;/// 

if(ym>=300)
avion.thetaHauter=degree*(double)ym/(double)_windowHeight;

if(ym<300)
avion.thetaHauter=-(degree-degree*(double)ym/(double)_windowHeight);

_cam.x += -dt* sin(avion.theta)*avion.force*AVIONRAPIDITE;

if(avion.force>0.35)
_cam.y += dt*sin(avion.thetaHauter)*avion.force*(avion.thetaHauter>=0?(avion.thetaHauter/(double)degree)*800:-(avion.thetaHauter/(double)degree)*800);
_cam.z += -dt* cos(avion.theta)*avion.force*AVIONRAPIDITE;

//printf("theta H %f\n",avion.thetaHauter);
//printf("cam %f %f %f\n",_cam.x,_cam.y,_cam.z);
//printf("avion force %f\n",avion.force);


}


//////////// PLAYER AVION MOVEMENT ///////////


///////////////// PARTICLE GEN ///////////////

typedef struct particle_st particle_st;
struct particle_st{
	GLfloat x,y,z;
	GLfloat xv,yv,zv;
	particle_st * nextParticle;
};


static particle_st *_ptc=NULL;


particle_st * ajouter_particle(particle_st * ptc,int initNB){
particle_st * p;
	int i;
	for(i=0;i<initNB;i++){		
	p=(particle_st *)malloc(sizeof(particle_st));
	p->x=0;
	p->y=370;
	p->z=0;
	p->xv=rand()%1000-500;
	p->yv=rand()%1000;
	p->zv=rand()%1000-500;
	p->nextParticle=ptc;
	ptc=p;
	p=NULL;
	}
	return ptc;
}
particle_st * detruire_tous_particle(particle_st * ptc){
particle_st * next;
while(ptc!=NULL){
next=ptc->nextParticle;
free(ptc);
ptc=next;
}

ptc=NULL;
return ptc;
}

///////////////// PARTICLE GEN ///////////////


//////////////// PLAYER MISSILE ////////////////////
typedef struct missile missile;
struct missile{
GLuint missileOn,mouseY;
GLfloat missileX,missileY,missileZ,missileTheta;
GLfloat force;
};
#define MAXMISSILE 200
#define RAPIDITEMISSILE 2000
static int NB_missile=0;
static missile missileTab[MAXMISSILE];


static void HBB_deplacementProjectile(double dt,GLuint w,GLuint h){
double gravity= 2.5;
int i;
for(i=0;i<NB_missile;i++)
	if(missileTab[i].missileOn==1){

	// Calcule X
    	missileTab[i].missileX -= dt * sin(missileTab[i].missileTheta)*RAPIDITEMISSILE*missileTab[i].force;
	if(missileTab[i].force==0.000)missileTab[i].force-=0.001;// diminuer la force
	
	// Calcule Y
/*
	if(missileTab[i].mouseY>(h>>1))//half of screen 
	missileTab[i].missileY -= (missileTab[i].mouseY-(h>>1)) / (GLfloat)h;//down position  
	else
*/
	missileTab[i].missileY -= dt *1000; 
	
	// Calcule Z
	missileTab[i].missileZ -= dt * cos(missileTab[i].missileTheta)*RAPIDITEMISSILE;

	// gravity
	if(missileTab[i].mouseY<h)
		missileTab[i].mouseY += gravity; 
	}
//pu("missile pos %f %f %f %ud\n",missileTab[0].missileX,missileTab[0].missileY,missileTab[0].missileZ,missileTab[0].mouseY);
}
//////////////// PLAYER MISSILE ////////////////////


//////////////////// AI WATER MOVEMENT //////////////////////////////
typedef struct water water;

struct water{
GLfloat x,z;
GLfloat forceX,forceZ;
};

static water wtr;
#define movementCadreX 300
#define movementCadreZ 600
static void HBB_initWater(){
GLfloat rdNb;
wtr.x=movementCadreX/2;
wtr.z=movementCadreZ/2;
rdNb=rand()%(movementCadreX/10)+(movementCadreX/10);
wtr.forceX=rdNb>=0?rdNb:-rdNb;

rdNb=rand()%(movementCadreZ/10)+(movementCadreZ/10);
wtr.forceZ= rdNb>=0?rdNb:-rdNb;
}
static void HBB_deplacementWater(double dt){
wtr.x+= wtr.forceX*dt;
wtr.z+= wtr.forceZ*dt;
if(wtr.x<=0){
wtr.forceX=-wtr.forceX;
//wtr.forceX+=rand()%(movementCadreX/20)-(movementCadreX/20);
}
if(wtr.z<=0){
wtr.forceZ=-wtr.forceZ;
//wtr.forceZ-=rand()%(movementCadreZ/20)-(movementCadreZ/20);
}
if(wtr.x>movementCadreX){
wtr.forceX=-wtr.forceX;
//wtr.forceX+=rand()%(movementCadreX/20)-(movementCadreX/20);
}
if(wtr.z>movementCadreZ){
wtr.forceZ=-wtr.forceZ;
//wtr.forceZ-=rand()%(movementCadreZ/20)-(movementCadreZ/20);

}
//printf("pos %f %f force %f %f\n",wtr.x,wtr.z,wtr.forceX,wtr.forceZ);

}

//////////////////// AI WATER MOVEMENT //////////////////////////



/////////////////// AI AVION OBJET /////////////////

typedef struct avionAI_st avionAI_st;
struct avionAI_st{
GLfloat x,y,z;
GLfloat theta;
};
#define NBAVIONAI 20
static avionAI_st avionAI[NBAVIONAI];


static void HBB_initAvoinAI(void){
	
int i; 
//srand(time(NULL));
for(i=0;i<NBAVIONAI;i++){
	avionAI[i].x=rand()%(15000/2)-(15000/4);
	avionAI[i].y=rand()%600+2000;
	avionAI[i].z=rand()%(15000/2)-(15000/4);
	avionAI[i].theta=((HBB_PI*4) * (rand() / (RAND_MAX + (HBB_PI*4))));
 }
}


static void HBB_deplacementAvoinAI(double dt){
	
int i,rapide=700; 
GLfloat cadreTaille=WORLDSCALE;
GLfloat cadreX0=-cadreTaille,cadreZ0=-cadreTaille,cadreX1=cadreTaille,cadreZ1=cadreTaille;
for(i=0;i<NBAVIONAI;i++){
	avionAI[i].x-=sin(avionAI[i].theta)*rapide*dt;	
	avionAI[i].z-=cos(avionAI[i].theta)*rapide*dt;	

	if(avionAI[i].x<cadreX0)avionAI[i].theta+= (HBB_PI/(float)4);
	else if(avionAI[i].x>cadreX1)avionAI[i].theta+= (HBB_PI/(float)4);
	else if(avionAI[i].z<cadreZ0)avionAI[i].theta+= (HBB_PI/(float)4);
	else if(avionAI[i].z>cadreZ1)avionAI[i].theta+= (HBB_PI/(float)4);
}

}

/////////////////// AI AVION OBJET /////////////////



///////////////////// AI DEFENSE TURRET //////////////////////

typedef struct bullet_st bullet_st;
struct bullet_st{
GLfloat x,y,z;
GLfloat theta,thetaHauter;	
};
typedef struct defenseTurret_st defenseTurret_st;
struct defenseTurret_st{
GLfloat x,y,z;
GLfloat theta,thetaHauter;
int bulletNB;
bullet_st bullet[3000];	
};
#define NBTURRET 2000
static defenseTurret_st defenseAI[NBTURRET];

static void HBB_missileTracking(GLfloat x0,GLfloat y0,GLfloat z0,GLfloat x1,GLfloat y1,GLfloat z1,GLfloat *theta,GLfloat *thetaHauter){
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


int actuel= 2000;
static void HBB_initTurretAI(){
int i;
srand(time(NULL));
for(i=0;i<200;i++){
defenseAI[i].x=1950+rand()%30-30;
defenseAI[i].y=150+rand()%20;
defenseAI[i].z=200+rand()%30-30;
}
for(i=200;i<400;i++){
defenseAI[i].x=1000+rand()%30-30;
defenseAI[i].y=150+rand()%20;
defenseAI[i].z=2200+rand()%30-30;
}
for(i=400;i<600;i++){
defenseAI[i].x=3000+rand()%30-30;
defenseAI[i].y=150+rand()%20;
defenseAI[i].z=900+rand()%30-30;
}
for(i=600;i<800;i++){
defenseAI[i].x=1000+rand()%30-30;
defenseAI[i].y=150+rand()%20;
defenseAI[i].z=800+rand()%30-30;
}
for(i=800;i<1000;i++){
defenseAI[i].x=2950+rand()%30-30;
defenseAI[i].y=150+rand()%20;
defenseAI[i].z=300+rand()%30-30;
}
for(i=1200;i<1400;i++){
defenseAI[i].x=4050+rand()%30-30;
defenseAI[i].y=150+rand()%20;
defenseAI[i].z=1200+rand()%30-30;
}

for(i=1400;i<1600;i++){
defenseAI[i].x=-200+rand()%100-100;
defenseAI[i].y=400+rand()%20;
defenseAI[i].z=2000+rand()%100-100;
}
for(i=1800;i<2000;i++){
defenseAI[i].x=2500+rand()%100-100;
defenseAI[i].y=150+rand()%20;
defenseAI[i].z=-2000+rand()%100-100;
}
for(i=0;i<NBTURRET;i++){
HBB_missileTracking(defenseAI[i].x,defenseAI[i].y,defenseAI[i].z,_cam.x,_cam.y,_cam.z+1,&defenseAI[i].theta,&defenseAI[i].thetaHauter);
}
}

static void HBB_deplacementAI(double dt){
	int i,trp;
	int temporisation=100;// par ms	
	double rapide=7000;
	static int bulletTimer=0;

for(trp=0;trp<NBTURRET;trp++){HBB_missileTracking(defenseAI[trp].x,defenseAI[trp].y,defenseAI[trp].z,_cam.x,_cam.y,_cam.z-1,&defenseAI[trp].theta,&defenseAI[trp].thetaHauter);
	////////// TEMPORISATION 

	if(!bulletTimer || SDL_GetTicks()-bulletTimer>temporisation+rand()%50){

	bulletTimer=SDL_GetTicks();
	defenseAI[trp].bullet[defenseAI[trp].bulletNB].x=defenseAI[trp].x;
	defenseAI[trp].bullet[defenseAI[trp].bulletNB].y=defenseAI[trp].y;
	defenseAI[trp].bullet[defenseAI[trp].bulletNB].z=defenseAI[trp].z;
	defenseAI[trp].bullet[defenseAI[trp].bulletNB].theta=defenseAI[trp].theta;
	defenseAI[trp].bullet[defenseAI[trp].bulletNB].thetaHauter=defenseAI[trp].thetaHauter;
	defenseAI[trp].bulletNB++;
	}

	////////// TEMPORISATION 
	for(i=0;i<defenseAI[trp].bulletNB;i++){
	defenseAI[trp].bullet[i].x+=-sin(defenseAI[trp].bullet[i].theta)*rapide*dt;// movement ver x
	defenseAI[trp].bullet[i].y+=+sin(defenseAI[trp].bullet[i].thetaHauter)*rapide*dt;// movement ver y
	defenseAI[trp].bullet[i].z+=-cos(defenseAI[trp].bullet[i].theta)*rapide*dt;// movement ver z	
	}
}

}

///////////////////// AI DEFENSE TURRET //////////////////////









static void HBB_objLoader5(void){
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

static SDL_Surface * screen;
static SDL_Surface * panorama;
static void HBB_textureLoader(void){
SDL_Surface * texSurface;
int lire;
GLfloat max;
for(lire=0;lire<NBTEXTURE;lire++){
  glGenTextures(1, &_tabTexId[lire]);
  glBindTexture(GL_TEXTURE_2D, _tabTexId[lire]);
#ifdef GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
  glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max); 
#endif
glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
texSurface = SDL_ConvertSurfaceFormat(IMG_Load(TEXFilePath[lire]),SDL_PIXELFORMAT_ABGR8888, 0);
	  assert(texSurface);
	  if(TEX_SCREEN==lire){
	    screen=SDL_ConvertSurfaceFormat(IMG_Load(TEXFilePath[lire]),SDL_PIXELFORMAT_ABGR8888, 0);
	    Uint32 *pix;
	    pix=screen->pixels;
	    HBB_flip(pix,screen->w,screen->h);
	    pix=texSurface->pixels;
	    HBB_flip(pix,texSurface->w,texSurface->h);
	  }

	  
	  if(TEX_PANORAMA==lire){
	    panorama=SDL_ConvertSurfaceFormat(IMG_Load(TEXFilePath[lire]),SDL_PIXELFORMAT_ABGR8888, 0);
	  }


	  
	  if(TEX_SEA==lire){
	  Uint32 * pix;
	  pix=(Uint32 *)texSurface->pixels;
	  int i;
	  for(i=0;i<texSurface->h*texSurface->w;i++)
	    pix[i]-=(30<<24);
	  }
#if __APPLE__
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texSurface->w, texSurface->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, texSurface->pixels);
#else
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texSurface->w, texSurface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, texSurface->pixels);
#endif
	  glGenerateMipmap(GL_TEXTURE_2D);
	  SDL_FreeSurface(texSurface);
}
}
static void HBB_initWaterVAO(void){
GLfloat * data = NULL; 
  GLuint * idata = NULL;
  
  _landscape = calloc(_landscape_w * _landscape_h, sizeof * _landscape);
assert(_landscape);
  triangle_edge(_landscape, 0, 0, _landscape_w - 1, _landscape_h - 1, _landscape_w);
  data = heightMap2Data(_landscape, _landscape_w, _landscape_h);
  idata = heightMapIndexedData(_landscape_w, _landscape_h);
  

  glGenVertexArrays(1, &_planVao);
  glBindVertexArray(_planVao);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
    //glEnableVertexAttribArray(2); 

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
static void HBB_initSkyboxVAO(void){
int i;  GLfloat s = WORLDSCALE;
//V4 V6 V3 V7
GLfloat data_skyboxt[] = {
    s , s/2, s, 
    s , s/2, -s,
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
//V1 V4 V2 V3
GLfloat data_skybox[] = {
   /* 4 coordonnées de sommets */
    -s , s/2, s, 
    s , s/2, s,
    -s , 0, s,
    s , 0, s,
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
//V5 V6 V7 V8
GLfloat data_skyboxv[] = {
    -s , s/2, -s, 
    s , s/2, -s,
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
    -s, s/2, -s,
    s , s/2, -s,
    -s, s/2,  s,
    s , s/2,  s,
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
// V1 V5 V2 V8
  GLfloat data_skyboxz[] = {
    -s , s/2, s,
    -s , s/2, -s,
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
static GLuint _pId2=0;
static void initGL(void) {
 // glClearColor(0.0f, 0.4f, 0.9f, 0.0f);
  glClearColor(0.0f, 0.4f, 0.9f, 0.0f);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_1D);
  glEnable(GL_TEXTURE_2D);
  _pId  = gl4duCreateProgram("<vs>shaders/SHD_SHIP/basic.vs", "<fs>shaders/SHD_SHIP/basic.fs", NULL);
    _pId2 = gl4duCreateProgram("<vs>shaders/SHDS/basic2.vs", "<fs>shaders/SHDS/basic2.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
//  resize(_windowWidth, _windowHeight);
//glViewport(0, 0, _windowWidth, _windowHeight);
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.5, 0.5, -0.5 * _windowHeight / _windowWidth, 0.5 * _windowHeight / _windowWidth, 1.0, WORLDSCALE*4.0);
}

static GLfloat _tex_repeat = 5.0;

static void initData(void) {
//////////////// INIT OBJET //////////////////
HBB_initWaterVAO();
HBB_initSkyboxVAO();
HBB_textureLoader();
HBB_objLoader5();
 
  _quad = gl4dgGenQuadf();
//////////////// INIT OBJET //////////////////

//////////////// INIT GAMESTATE //////////////////
HBB_initTurretAI();
HBB_initWater();
HBB_initAvion();
HBB_initAvoinAI();
//////////////// INIT GAMESTATE //////////////////
 
}
/*
static void resize(int w, int h) {
//  _windowWidth = w; 
//  _windowHeight = h;
//glViewport(0, 0, _windowWidth, _windowHeight);
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.5, 0.5, -0.5 * _windowHeight / _windowWidth, 0.5 * _windowHeight / _windowWidth, 1.0, WORLDSCALE*4.0);
}
*/


static void HBB_deplacementParticle(double dt){
	particle_st *racine;
	racine=_ptc;  
static Uint32 t0 = 0, t;
t=SDL_GetTicks();




while(racine!=NULL){
	racine->x+=racine->xv*dt;
	racine->y+=racine->yv*dt;
	racine->z+=racine->zv*dt;
	racine=racine->nextParticle;	
	}

if((t=SDL_GetTicks())-t0>=8000){
  t0=SDL_GetTicks();
  _ptc=detruire_tous_particle(_ptc);
  _ptc=ajouter_particle(_ptc,30000);
}

}

static int _explosion=0;

static void idle(void) {
  double dt, dtheta = M_PI, pas = 300.0;
  static Uint32 t0 = 0, t;
  dt = ((t = SDL_GetTicks()) - t0) / 1000.0;
  t0 = t;
  static int timer=0;


static int animInit=0;
static int animTimer=0;
if(animInit==0){
  if(timer==0)timer=SDL_GetTicks();
animTimer=SDL_GetTicks();
_cam.x=2000.000000;
_cam.y=5000.000000;
_cam.z=-15000.000000;
_cam.theta=3.000635;
avion.theta=3.000635;
animInit++;
}

  
 if(SDL_GetTicks()-timer>100/*200 en cas d'err*/){

int xm, ym;
SDL_PumpEvents();
SDL_GetMouseState(&xm, &ym);

/////////// CHANGEMENT DE POINT VUE DE JOUER /////////
if(_weaponMode==MODE_AVION){
/*
if(xm>=300)
_cam.theta -= dtheta*(xm/(double)_windowWidth)*dt*0.05;
else
_cam.theta += dtheta*(1-xm/(double)_windowWidth)*dt*0.05;

avion.theta = _cam.theta;
*/

avion.centre2=(((_windowHeight/2)-(_windowHeight/2)+50+(rand()%3))/(double) _windowHeight)*3.0*0.3;
avion.centre3=-(((_windowWidth/2)-(_windowWidth/2)+(rand()%3))/(double)_windowWidth)*3.0*0.3;


  if(_keys[KRIGHT]) {
    _cam.theta -= dt * dtheta*0.08;
  avion.theta-= dt * dtheta*0.08;
  if(avion.centre>-1.5)avion.centre-=0.001;

  }else if(avion.centre<0)avion.centre+=0.005;
  if(_keys[KLEFT]) {
    _cam.theta += dt * dtheta*0.08;
  avion.theta+= dt * dtheta*0.08;
  if(avion.centre<1.5)avion.centre+=0.001;
  }else if(avion.centre>0)avion.centre-=0.005;

  if(_keys[KUP]) {
    if(avion.force<1.5)avion.force+=AVIONFORCE;
    if(avion.force>1.5)avion.force=1.5000;
    }

  if(_keys[KDOWN]) {
    if(avion.force>0)avion.force-=AVIONFORCE;
    if(avion.force<0)avion.force=0;
  }

}else{

  if(_keys[KRIGHT])
    _cam.theta += dt * dtheta;
if(_keys[KLEFT])
    _cam.theta -= dt * dtheta;
  if(_keys[KUP]) {
    _cam.x += -dt * pas * sin(_cam.theta);
    _cam.z += -dt * pas * cos(_cam.theta);
  }
  if(_keys[KDOWN]) {
    _cam.x += dt * pas * sin(_cam.theta);
    _cam.z += dt * pas * cos(_cam.theta);
  }
}





///////// DEPLACEMENT OBJ ///////////////
HBB_deplacementProjectile(dt,_windowWidth,_windowHeight);
HBB_deplacementAI(dt);
HBB_deplacementWater(dt);
HBB_deplacementAvion(dt);
 if(_explosion>=0)HBB_deplacementAvoinAI(dt);
 if(_explosion==2)HBB_deplacementParticle(dt);

///////// DEPLACEMENT OBJ ///////////////


////////// ANIMATION ////////////////////
static int missileRance=0;

    if(avion.force<1.5)avion.force+=AVIONFORCE;
    if(avion.force>1.5)avion.force=1.5000;

if(SDL_GetTicks()-animTimer>9000+500&&_explosion==0)_weaponMode = MODE_MISSILE;

if(SDL_GetTicks()-animTimer>10500+1000&&missileRance==0){
missileTab[NB_missile].mouseY=210;//ym;
missileTab[NB_missile].missileOn=1;
missileTab[NB_missile].missileX= _cam.x;
missileTab[NB_missile].missileY= _cam.y;
missileTab[NB_missile].missileZ= _cam.z;
missileTab[NB_missile].missileTheta = _cam.theta;
missileTab[NB_missile].force = 1.0;
NB_missile++;
missileRance++;
}

if(missileTab[0].missileY<210&&missileRance!=0&&_explosion<=1){
_weaponMode = MODE_AVION;
_cam.x=928.000000;
_cam.y=2714.000000;
_cam.z=-7329.000000;
_cam.theta=3.000635;
_explosion++;
}



//printf("%d %d\n",ym,xm);
//printf("%f %f %f %f\n",_cam.x,_cam.y,_cam.z,_cam.theta);

////////// ANIMATION ////////////////////
  }
}
/*

static void keydown(int keycode) {

  GLint v[2];
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
  case SDLK_LSHIFT:
_cam.x=avionAI[0].x;
_cam.y=avionAI[0].y;
_cam.z=avionAI[0].z;
  break;
  case 's':
    _keys[KAVION_RECULER] = 1;
  break;
  case 'y':
_cam.x=2000;
_cam.y=5000;
_cam.z=-15000;
_cam.theta=HBB_PI;
avion.theta=HBB_PI;
  break;
  case SDLK_SPACE:
    break;
  case 'p':
    glGetIntegerv(GL_POLYGON_MODE, v);
    if(v[0] == GL_FILL)
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    break;
  case 'v': /// world view
_cam.x=defenseAI[actuel+1].x;
_cam.y=defenseAI[actuel+1].y;
_cam.z=defenseAI[actuel+1].z;
_cam.theta=defenseAI[actuel+1].theta;
break;
case SDLK_LCTRL: // missile
if(MAXMISSILE ==NB_missile-1)NB_missile=0;
//int xm, ym;
//SDL_PumpEvents();
//SDL_GetMouseState(&xm, &ym);
missileTab[NB_missile].mouseY=210;//ym;
missileTab[NB_missile].missileOn=1;
missileTab[NB_missile].missileX= _cam.x;
missileTab[NB_missile].missileY= _cam.y;
missileTab[NB_missile].missileZ= _cam.z;
missileTab[NB_missile].missileTheta = _cam.theta;
missileTab[NB_missile].force = 1.0;
NB_missile++;

break;
case SDLK_LALT: // suivre missile
_weaponMode = MODE_MISSILE;
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
  case SDLK_LSHIFT:
    _keys[KAVION_DEMARER] = 0;
  case 's':
    _keys[KAVION_RECULER] = 0;
  break;
    break;
   }
}
*/
static void HBB_drawSkybox(void) {
  GLfloat cam[2] = {0, 0};
  glUniform2fv(glGetUniformLocation(_pId, "cam"), 1, cam);

  static int trans=0;
  static int init=0;
  static int timer=0;
  if(timer==0)timer=SDL_GetTicks();
    int i;
    Uint32 * pix;
    pix=(Uint32 *)panorama->pixels;
  if(init==0){
      for(i=0;i<panorama->w*panorama->h;i++)
	pix[i]-=(255<<24);
      init++;
    }
  
  
  if(SDL_GetTicks()-timer>20){
    timer=SDL_GetTicks();
    if(trans<255){
      trans++;
      for(i=0;i<panorama->w*panorama->h;i++)
	pix[i]+=(1<<24);
    }
  }
  glBindTexture(GL_TEXTURE_2D,_tabTexId[TEX_PANORAMA]);
  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, panorama->w, panorama->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, panorama->pixels);
  
  glBindTexture(GL_TEXTURE_2D,0);
  
  
  glBindTexture(GL_TEXTURE_2D,_tabTexId[TEX_PANORAMA]);
  glBindVertexArray(_skyboxtabVao[0]);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  
  glBindTexture(GL_TEXTURE_2D,_tabTexId[TEX_PANORAMA]);
  glBindVertexArray(_skyboxtabVao[1]);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  
  glBindTexture(GL_TEXTURE_2D, _tabTexId[TEX_PANORAMA]);
  glBindVertexArray(_skyboxtabVao[2]);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindTexture(GL_TEXTURE_2D,_tabTexId[TEX_PANORAMA]);
  glBindVertexArray(_skyboxtabVao[3]);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  
  glBindTexture(GL_TEXTURE_2D,_tabTexId[TEX_PANORAMA]);
  glBindVertexArray(_skyboxtabVao[4]);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

}

static void draw(void) { 
 glEnable(GL_BLEND);
 glEnable(GL_DEPTH);
 glEnable(GL_DEPTH_TEST);
glDisable(GL_CULL_FACE);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
//////////////// CAMERA VIEW //////////////////
	//int xm, ym;
	//SDL_PumpEvents();
	//SDL_GetMouseState(&xm, &ym);
	GLfloat lumpos[4] = {100, 3000, 100, 1.0}, cam[2];
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gl4duBindMatrix("modelViewMatrix");
	

gl4duLoadIdentityf();
	if(_weaponMode==MODE_AVION)
	gl4duLookAtf(_cam.x, _cam.y /*1.0*/, _cam.z,_cam.x - sin(_cam.theta),_cam.y + (/*ym*/220 - (_windowHeight >> 1)) / (GLfloat)_windowHeight, _cam.z - cos(_cam.theta),0.0, 1.0,0.0);
	else /// missile mode
	gl4duLookAtf(_cam.x, _cam.y /*1.0*/, _cam.z,_cam.x - sin(_cam.theta),_cam.y - (/*ym*/(_windowHeight-100)- (_windowHeight >> 1)) / (GLfloat)_windowHeight, _cam.z - cos(_cam.theta),0.0, 1.0,0.0);

	glUseProgram(_pId);


	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(_pId, "myTexture"), 0);

//////////////// CAMERA VIEW //////////////////

//HBB_missileTracking(tr.x,tr.y,tr.z,_cam.x,_cam.y,_cam.z,&tr.theta,&tr.thetaHauter);
//printf("cam%f %f %f tr %f %f %f\n",_cam.x,_cam.y,_cam.z,tr.x,tr.y,tr.z);
int i;


//////////////////////// WATER OBJET /////////////////////
	gl4duPushMatrix();{
	gl4duTranslatef(wtr.x,WORLDSCALE+1000,wtr.z);
	gl4duScalef(WORLDSCALE+1000,WORLDSCALE+1000,WORLDSCALE+1000);
	gl4duRotatef(90,0, 1,0);
	gl4duSendMatrices();
	glBindVertexArray(_planVao);
glBindTexture(GL_TEXTURE_2D,_tabTexId[TEX_SEA]);
	glDrawElements(GL_TRIANGLES, 2 * 3 * (_landscape_w - 1) * (_landscape_h - 1), GL_UNSIGNED_INT, 0);
	}gl4duPopMatrix();
//////////////////////// WATER OBJET /////////////////////


////////////// AI DEFENSE ///////////////

int trtNB;
for(trtNB=0;trtNB<NBTURRET;trtNB++){ 
	for(i=0;i<defenseAI[trtNB].bulletNB;i++){
		gl4duPushMatrix();{
		gl4duTranslatef(defenseAI[trtNB].bullet[i].x,defenseAI[trtNB].bullet[i].y,defenseAI[trtNB].bullet[i].z);
		gl4duRotatef(defenseAI[trtNB].bullet[i].theta/HBB_PI*180-90,0,1,0);
		gl4duRotatef(defenseAI[trtNB].bullet[i].thetaHauter/HBB_PI*180+30,1,0,0);
		gl4duSendMatrices();


		gl4duScalef(3,3,3);
		gl4duSendMatrices();

		glBindVertexArray(_objTabVao[OBJ_MISSILE]);
		glBindTexture(GL_TEXTURE_2D,_tabTexId[TEX_FLAME]);
		glDrawElements(GL_TRIANGLES, 3 * (_tabModel[OBJ_MISSILE]->numtriangles-10), GL_UNSIGNED_INT, 0);
		}gl4duPopMatrix();
	}
}

////////////// AI DEFENSE ///////////////

////////////// AI AVION ///////////////

	gl4duPushMatrix(); {
	  /*
	  static int init=0;
	  if(init==0){
	for(i=0;i<NBAVIONAI;i++){
	avionAI[i].x=0;rand()%(1000)-(1000);
	avionAI[i].y=rand()%600+1800;
	avionAI[i].z=rand()%(1000)-(1000);
	avionAI[i].theta=((HBB_PI*2) * (rand() / (RAND_MAX + (HBB_PI*2))));
	}
	init++;
	  }
	  */
int i;
	for(i=0;i<NBAVIONAI;i++){
	gl4duPushMatrix(); 
	gl4duTranslatef(avionAI[i].x,avionAI[i].y,avionAI[i].z);
	gl4duRotatef(avionAI[i].theta/HBB_PI*180,0,1,0);
	gl4duScalef(16,16,16);
	gl4duSendMatrices();
	glBindVertexArray(_objTabVao[OBJ_F15]);
	glBindTexture(GL_TEXTURE_2D,_tabTexId[TEX_CAMOUFLAGE2]);
	glDrawElements(GL_TRIANGLES, 3 * (_tabModel[OBJ_F15]->numtriangles-1), GL_UNSIGNED_INT, 0);
	gl4duPopMatrix();
		}
	}gl4duPopMatrix();


////////////// AI AVION ///////////////

/////////////////// PARTICLE ////////////////

	particle_st *racine;
	racine=_ptc;
	if(_explosion>1){
	  static int exptimer=0;
	  if(exptimer==0)exptimer=SDL_GetTicks();
	  if(SDL_GetTicks()-exptimer>500)_explosion=2;
	  
	    while(racine!=NULL){
	gl4duPushMatrix(); 
	gl4duTranslatef(racine->x,racine->y,racine->z);

	gl4duScalef(100,100,100);
	gl4duRotatef(_cam.theta/HBB_PI*180,0, 1, 0);
	gl4duSendMatrices();
	glBindVertexArray(_objTabVao[OBJ_PIG]);
	glBindTexture(GL_TEXTURE_2D,_tabTexId[TEX_FLAME]);
	glDrawElements(GL_TRIANGLE_STRIP, 4/*3 * (_tabModel[OBJ_PIG]->numtriangles-2128)*/, GL_UNSIGNED_INT, 0);
	gl4duPopMatrix();
	racine=racine->nextParticle;	
	}
	}
/////////////////// PARTICLE ////////////////


/////////////////// PLAYER MISSILE OBJET ////////////////
/*
	 gl4duPushMatrix(); {  
for(i=0;i<NB_missile;i++){
	if(missileTab[i].missileOn==1){
	 gl4duPushMatrix(); {     // BUG

 gl4duTranslatef(missileTab[i].missileX,missileTab[i].missileY,missileTab[i].missileZ); 
	gl4duRotatef(missileTab[i].missileTheta/HBB_PI*180-90,0,1,0);
	gl4duRotatef(30,0,0,1);
	gl4duSendMatrices();
	glBindVertexArray(_objTabVao[OBJ_MISSILE]);
	glBindTexture(GL_TEXTURE_2D,_tabTexId[TEX_METALBLACK]);
	glDrawElements(GL_TRIANGLES, 3 * (_tabModel[OBJ_MISSILE]->numtriangles-5), GL_UNSIGNED_INT, 0);
	}gl4duPopMatrix();
	}
	}


	}gl4duPopMatrix();
*/
/////////////////// PLAYER MISSILE OBJET ////////////////

///////////////////// CRUISER OBJET ////////////////
	gl4duPushMatrix(); {
	gl4duTranslatef(0,300,0);
	gl4duSendMatrices();
	glBindVertexArray(_objTabVao[OBJ_CRUISER]);
	glBindTexture(GL_TEXTURE_2D,_tabTexId[TEX_GRAYPOLYGON]);
//////////////// 1ER Cruiser 
	gl4duTranslatef(2000,-157,0);
	gl4duScalef(10,10,10);
	gl4duSendMatrices();
	glDrawElements(GL_TRIANGLES, 3 * (_tabModel[OBJ_CRUISER]->numtriangles/*enleve un autre bateau*/-1370), GL_UNSIGNED_INT, 0);



//////////////// 2eme Cruiser et Corvette
	glBindVertexArray(_objTabVao[OBJ_CRUISER]);
	gl4duTranslatef(100, 0,100);
	gl4duSendMatrices();
	glDrawElements(GL_TRIANGLES, 3 * (_tabModel[OBJ_CRUISER]->numtriangles-1), GL_UNSIGNED_INT, 0);

//////////////// 3eme Cruiser
	gl4duTranslatef(100, 0,100);
	gl4duSendMatrices();
	glDrawElements(GL_TRIANGLES, 3 * (_tabModel[OBJ_CRUISER]->numtriangles/*enleve un autre bateau*/-1370), GL_UNSIGNED_INT, 0);

//////////////// 4eme Cruiser
	gl4duTranslatef(0, 0,-500);
	gl4duSendMatrices();
	glDrawElements(GL_TRIANGLES, 3 * (_tabModel[OBJ_CRUISER]->numtriangles/*enleve un autre bateau*/-1370), GL_UNSIGNED_INT, 0);

//////////////// 5eme Cruiser
	gl4duTranslatef(-150, 0,100);
	gl4duSendMatrices();
	glDrawElements(GL_TRIANGLES, 3 * (_tabModel[OBJ_CRUISER]->numtriangles/*enleve un autre bateau*/-1370), GL_UNSIGNED_INT, 0);
//////////////// 6eme Cruiser
	gl4duTranslatef(-150, 0,-50);
	gl4duSendMatrices();
	glDrawElements(GL_TRIANGLES, 3 * (_tabModel[OBJ_CRUISER]->numtriangles/*enleve un autre bateau*/-1370), GL_UNSIGNED_INT, 0);


//////////////// 7eme Cruiser et Corvette
	gl4duTranslatef(0, 0,400);
	gl4duSendMatrices();
	glDrawElements(GL_TRIANGLES, 3 * (_tabModel[OBJ_CRUISER]->numtriangles-1), GL_UNSIGNED_INT, 0);
	}gl4duPopMatrix();

///////////////////// CRUISER OBJET ////////////////






//////////////////// DEBUT DE PUSH //////////////////
	gl4duPushMatrix(); {
	gl4duTranslatef(50,330,100); 

	gl4duScalef(10.0,10.0,10.0);
	gl4duSendMatrices();

///////////////////////   F15 OBJET ///////////////////
int multiple;
	gl4duPushMatrix();{
	gl4duTranslatef(-5,1.4,75); 
	gl4duScalef(0.70,0.70,0.70);
	gl4duSendMatrices();

for(multiple=0;multiple<4;multiple++){

	glBindVertexArray(_objTabVao[OBJ_B2]);
	glBindTexture(GL_TEXTURE_2D,_tabTexId[TEX_METALBLACK]);
	glDrawElements(GL_TRIANGLES, 3 * (_tabModel[OBJ_B2]->numtriangles-1), GL_UNSIGNED_INT, 0);
	gl4duTranslatef(0,0,-25); 
	gl4duSendMatrices();
	}


	}gl4duPopMatrix();


	gl4duTranslatef(0,0,-190); 
for(multiple=0;multiple<10;multiple++){
	gl4duTranslatef(0,0,25); 
	gl4duRotatef(40,0, 1,0);
	gl4duSendMatrices();
	glBindVertexArray(_objTabVao[OBJ_F15]);
	glBindTexture(GL_TEXTURE_2D,_tabTexId[TEX_CAMOUFLAGE2]);
	glDrawElements(GL_TRIANGLES, 3 * (_tabModel[OBJ_F15]->numtriangles-1), GL_UNSIGNED_INT, 0);
	gl4duSendMatrices();
	gl4duRotatef(-40,0, 1,0);
	gl4duTranslatef(2,0,-multiple); 
	gl4duSendMatrices();
	}
	}gl4duPopMatrix();
///////////////////////   F15 OBJET ///////////////////




///////////////////// PORTEAVION OBJET ////////////////
	gl4duPushMatrix(); {
	gl4duTranslatef(-10,300, -15); 
	gl4duScalef(0.008, 0.008,0.008);
	gl4duSendMatrices();
	glBindVertexArray(_objTabVao[OBJ_PORTEAVION]);
	glBindTexture(GL_TEXTURE_2D,_tabTexId[TEX_GRAYPOLYGON]);
	glDrawElements(GL_TRIANGLES, 3 * (_tabModel[OBJ_PORTEAVION]->numtriangles-1), GL_UNSIGNED_INT, 0);
	}gl4duPopMatrix();
///////////////////// PORTEAVION OBJET ////////////////











//////////////// DRAW SKYBOX //////////////////


	gl4duTranslatef(0,-1000,0); // apartir de 1 on voit donc -1
	gl4duSendMatrices();
	glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lumpos);
	cam[0] = _cam.x / (2 * WORLDSCALE  / _tex_repeat);
	cam[1] = _cam.z / (2 * WORLDSCALE  / _tex_repeat);
	glUniform2fv(glGetUniformLocation(_pId, "cam"), 1, cam);
	  	
	gl4duPushMatrix(); {
	//gl4duTranslatef(_cam.x, 0, _cam.z);
	gl4duSendMatrices(); // apres translation et rotation on fait SendMatrice
	HBB_drawSkybox();
	}gl4duPopMatrix();// remettre
	gl4duPushMatrix(); { // sauvegarder 
	gl4duLoadIdentityf(); // reinit 
	gl4duPushMatrix(); 
	gl4duScalef(0.08, 0.08,0.08);  // z eloine
	gl4duTranslatef(3.3, 3, -22); // apartir de 1 on voit donc -1
	gl4duSendMatrices();
	gl4duPopMatrix();

//////////////// DRAW SKYBOX //////////////////



//////////////// WEAPON LOADING //////////////////


	gl4duPushMatrix(); {
switch(_weaponMode){
case MODE_AVION:
	gl4duPushMatrix(); {
	gl4duTranslatef(0.6,-30,-230);
	gl4duRotatef(-90,1, 0, 0);
	gl4duRotatef(avion.centre/HBB_PI*180,0, 0, 1);
	gl4duRotatef(-avion.centre2/HBB_PI*180,1,0,0);
	gl4duRotatef(avion.centre3/HBB_PI*180,0,1,0);
	gl4duScalef(16,16,16);
	gl4duSendMatrices();
	glBindVertexArray(_objTabVao[OBJ_F4E]);
	glBindTexture(GL_TEXTURE_2D,_tabTexId[TEX_METALBLACK]);
	glDrawElements(GL_TRIANGLES, 3 * (_tabModel[OBJ_F4E]->numtriangles-10), GL_UNSIGNED_INT, 0);
	}gl4duPopMatrix();


break;

case MODE_MISSILE: /// suivre derniere missile qui a ete lancé
  
	gl4duPushMatrix(); {
	  //gl4duLoadIdentityf();

	gl4duTranslatef(0,-1.5,-7);
	gl4duRotatef(-90,0,1, 0);
	  gl4duRotatef(20,1, 0, 0);
	gl4duScalef(0.47,0.47,0.47);
	gl4duSendMatrices();
glBindVertexArray(_objTabVao[OBJ_MISSILE]);
	glBindTexture(GL_TEXTURE_2D,_tabTexId[TEX_METALBLACK]);
	glDrawElements(GL_TRIANGLES, 3 * (_tabModel[OBJ_MISSILE]->numtriangles-5), GL_UNSIGNED_INT, 0);
	

	}gl4duPopMatrix();
 
if(NB_missile>0){
_cam.x=missileTab[NB_missile-1].missileX;
_cam.y=missileTab[NB_missile-1].missileY;
_cam.z=missileTab[NB_missile-1].missileZ-10;
}
break;
default:
	break;
}// fin de swtich

}gl4duPopMatrix();
//////////////// WEAPON LOADING //////////////////

}gl4duPopMatrix();

//////////////////// FIN DE POP //////////////////


}


static cam_t _cam2 = {0,370,0,0};

static void draw2(void) {
  static int globaltimer=0;
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  if(SDL_GetTicks()-globaltimer>2000){
  //glEnable(GL_BLEND);
 glEnable(GL_DEPTH);
 glEnable(GL_DEPTH_TEST);
glDisable(GL_CULL_FACE);
//////////////// CAMERA VIEW //////////////////
	//int xm, ym;
	//SDL_PumpEvents();
	//SDL_GetMouseState(&xm, &ym);
	GLfloat lumpos[4] = {100, 3000, 100, 1.0}, cam[2];
	gl4duBindMatrix("modelViewMatrix");
	static float view=-300;
gl4duLoadIdentityf();
	gl4duLookAtf(_cam2.x, _cam2.y /*1.0*/, _cam2.z,_cam2.x - sin(_cam.theta),_cam2.y + (/*ym*/view - (_windowHeight >> 1)) / (GLfloat)_windowHeight, _cam2.z - cos(_cam2.theta),0.0, 1.0,0.0);
	glUseProgram(_pId);


	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(_pId, "myTexture"), 0);

//////////////// CAMERA VIEW //////////////////


  double dt;
  static Uint32 t0 = 0, t;
  dt = ((t = SDL_GetTicks()) - t0) / 1000.0;
  t0 = t;
static float tour=0;
static float transi=0;
 static int timer=0;
static float apla=WORLDSCALE+1000+0.000000000001;
 if(timer==0)timer=SDL_GetTicks();
   if(SDL_GetTicks()-timer>100){   
       if(view<300)view+=dt*100;
     if(tour<113)tour+=dt*10;
     if(apla>=0)apla-=dt*1000;
     if(tour>=113&&view<520)view+=dt*120;
     if(transi<1500)transi+=dt*200;
   }
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   gl4duTranslatef(0,-transi,0);
   
   //////////////////////// WATER OBJET /////////////////////
   gl4duPushMatrix();{
     gl4duTranslatef(0,WORLDSCALE,0);
     gl4duTranslatef(0,-apla,0);
     gl4duScalef(WORLDSCALE+1000,WORLDSCALE+1000-apla,WORLDSCALE+1000);
     gl4duRotatef(30+tour,0,1,0);
     gl4duSendMatrices();
     glBindVertexArray(_planVao);
     
     glBindTexture(GL_TEXTURE_2D,_tabTexId[TEX_SEA]);
     
     glBindTexture(GL_TEXTURE_2D,_tabTexId[TEX_SEA]);
     glDrawElements(GL_TRIANGLES, 2 * 3 * (_landscape_w - 1) * (_landscape_h - 1), GL_UNSIGNED_INT, 0);
   }gl4duPopMatrix();
   //////////////////////// WATER OBJET /////////////////////
   
   
   ////////////// AI DEFENSE ///////////////

   //////////////// DRAW SKYBOX //////////////////
   
   gl4duSendMatrices();
   glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lumpos);
   cam[0] = _cam.x / (2 * WORLDSCALE  / _tex_repeat);
   cam[1] = _cam.z / (2 * WORLDSCALE  / _tex_repeat);
   glUniform2fv(glGetUniformLocation(_pId, "cam"), 1, cam);
   
   gl4duPushMatrix(); {
     //gl4duTranslatef(_cam.x, 0, _cam.z);
     gl4duTranslatef(0, -1500,0); // apartir de 1 on voit donc -1
     gl4duSendMatrices(); // apres translation et rotation on fait SendMatrice
     HBB_drawSkybox();
   }gl4duPopMatrix();// remettre
   
   gl4duPushMatrix();// sauvegarder 
   gl4duLoadIdentityf(); // reinit 
   gl4duPushMatrix(); 
   gl4duScalef(0.08, 0.08,0.08);  // z eloine
   gl4duTranslatef(3.3, 3, -22); // apartir de 1 on voit donc -1
   gl4duSendMatrices();
   gl4duPopMatrix();
   
   //////////////// DRAW SKYBOX //////////////////
   
   if(SDL_GetTicks()-timer>10000&&view>=520){
     static float distance=-6000;
     if(distance<300)
       distance+=dt*1500;
     
     gl4duPushMatrix(); {
       gl4duTranslatef(0.6,-10,distance);
       gl4duRotatef(178,0, 1, 0);
       gl4duRotatef(-90,1, 0, 0);
       gl4duRotatef(avion.centre/HBB_PI*180,0, 0, 1);
       gl4duRotatef(-avion.centre2/HBB_PI*180,1,0,0);
       gl4duRotatef(avion.centre3/HBB_PI*180,0,1,0);
       gl4duScalef(16,16,16);
       gl4duSendMatrices();
       glBindVertexArray(_objTabVao[OBJ_F4E]);
       glBindTexture(GL_TEXTURE_2D,_tabTexId[TEX_METALBLACK]);
       glDrawElements(GL_TRIANGLES, 3 * (_tabModel[OBJ_F4E]->numtriangles-10), GL_UNSIGNED_INT, 0);
     }gl4duPopMatrix();
   }
   glDisable(GL_BLEND);
  }
  

  static int transparence=256;
    if(transparence>2){
      
    
      glUseProgram(_pId2); // CHANGE AUTOMATIQUE
    
    
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glBindTexture(GL_TEXTURE_2D,_tabTexId[TEX_SCREEN]);
      int j;  Uint32 * pix;
      pix=screen->pixels;
      
     
    transparence-=2;
    for(j=0;j<screen->h*screen->w;j++)
      pix[j]-=(2<<24);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,screen->w,screen->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pix);
      glBindTexture(GL_TEXTURE_2D,0);
      
    
    
      glBindTexture(GL_TEXTURE_2D,_tabTexId[TEX_SCREEN]);
      gl4dgDraw(_quad);
    
      glDisable(GL_BLEND);
    }
 
  

}











static void quit(void) {

  if(_planVao)
    glDeleteVertexArrays(1, &_planVao);
  if(_planBuffer[0])
    glDeleteBuffers(2, _planBuffer);

int freeMemory;
for(freeMemory=0;freeMemory<5;freeMemory++){
  if(_skyboxtabVao[freeMemory])
    glDeleteVertexArrays(1, &_skyboxtabVao[freeMemory]);
  if(_skyboxtabBuffer[freeMemory])
    glDeleteBuffers(1, &_skyboxtabBuffer[freeMemory]);
}



for(freeMemory=0;freeMemory<NBTEXTURE;freeMemory++)
    glDeleteTextures(1, &_tabTexId[freeMemory]);


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

if(_landscape) {
    free(_landscape);
    _landscape = NULL;
  }



while(_ptc!=NULL){
	particle_st * freememory=_ptc->nextParticle;
	free(_ptc);
	_ptc=freememory;
}


 SDL_FreeSurface(screen);

	  SDL_FreeSurface(panorama);
  gl4duClean(GL4DU_ALL);
}



void init_battleship(void){
  initGL();
  initData();
  atexit(quit);
//  gl4duwResizeFunc(resize);
 // gl4duwKeyUpFunc(keyup);
  //gl4duwKeyDownFunc(keydown);
}

void draw_battleship(void){
  static int timer=0;
  if(timer==0)timer=SDL_GetTicks();
  
  if(SDL_GetTicks()-timer<18000){
    draw2();
  }else{
    idle();
    if(SDL_GetTicks()-timer>=18100)
    draw();
     }
}
