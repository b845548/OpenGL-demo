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
#include <SDL_image.h>

/*!\brief identifiant du programme GLSL */
//static GLuint _pId = 0;
/*!\brief position de la lumière relativement à la sphère éclairée */
//static GLfloat _lumPos0[4] = {1500.0, 20.0, 30.0, 1.0};
/*!\brief tableau des identifiants de texture à charger */
//static GLuint _tId[4] = {0};
/*!\brief pour les deux astres : terre et lune */
//static GLuint _sphere = {0};


/*!\brief dimensions de la fenêtre */
static int _windowWidth = 800, _windowHeight = 600;
/*!\brief identifiant du (futur) vertex array object */
static GLuint _vao = 0;
/*!\brief identifiant du buffer de data de la sphère indexée */
static GLuint _ibuffer[2] = {0, 0};
/*!\brief identifiant des GLSL program */
static GLuint _pId = 0, _pId2 = 0;
/*!\brief nombre de longitudes et latitudes de la sphere */
static GLuint _longitudes = 50, _latitudes = 50;
/*!\brief arrete l'animation */
static GLuint _pause = 0;
/*!\brief normale à la face ou au point ? */
static GLuint _faceNormal = 0;
/*!\brief flag pour Phong ou pas */
static GLuint _phong = 0;
/*!\brief flag pour savoir si la touche shift est enfoncée */
//static GLuint _shift = GL_FALSE;
/*!\brief flag pour afficher les coordonnées de texture */
static GLuint _tc = 0;
/*!\brief position de la lumière relativement à la sphère éclairée */
static GLfloat _lumPos0[4] = {0.0, 0.0, 0,0.0};
/////////////////////////////////////
static int * im=NULL;
Uint32 * pixels2=NULL;
#define TEXW 1000
#define TEXH 1000
/////////////////////////////////////////
/// MODIF
static GLuint _texture=0;
/// MODIF
void fractal2(int * im, int x0, int y0, int w, int h, int width){
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
	fractal2(im,p[0][0],p[0][1],w_2,h_2,width);
	if(w_21>1||h_2>1)
	fractal2(im,p[4][0],p[4][1],w_21,h_2,width);
	if(w_21>1||h_21>1)
	fractal2(im,p[8][0],p[8][1],w_21,h_21,width);
	if(w_2>1||h_21>1)
	fractal2(im,p[7][0],p[7][1],w_2,h_21,width);
		
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
      data[k++] = y+gl4dmSURand()*0.0;
      data[k++] = r * sin(phi)+gl4dmSURand()*0.10;
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

void soleil_init(void) {
  glClearColor(0,0,0,0);
  _pId  = gl4duCreateProgram("<vs>shaders/SHD1/basic.vs", "<gs>shaders/SHD1/basic.gs", "<fs>shaders/SHD1/basic.fs", NULL);
  _pId2 = gl4duCreateProgram("<vs>shaders/SHD1/sol.vs", "<fs>shaders/SHD1/sol.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  glViewport(0, 0, _windowWidth, _windowHeight);
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.5, 0.5, -0.5 * _windowHeight / _windowWidth, 0.5 * _windowHeight / _windowWidth, 1.0, 2000.0);
  gl4duBindMatrix("modelViewMatrix");

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
  

  /// MODIF
 
glGenTextures(1, &_texture);
  glBindTexture(GL_TEXTURE_2D,_texture);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

   pixels2=(Uint32 *)malloc(TEXW* TEXH*sizeof(Uint32));
    memset(pixels2, 0, TEXW* TEXH * sizeof(Uint32));
	im=(int *)malloc(TEXW* TEXH*sizeof(int));	
	
 int pos[4],i;
 for(i=0;i<4;i++)
   pos[i]=rand()%255+1;
	
im[0]=pos[0];
im[TEXW-1]=pos[1];
im[TEXW*(TEXH-1)]=pos[2];
	im[TEXW*TEXH-1]=pos[3];

	fractal2(im,0,0,TEXW-1,TEXH-1,TEXW);
	for(i=0;i<TEXW *TEXH;i++)
	  pixels2[i]=(255<<24)|(/*(150-im[i])*/0<<16)|((150-im[i])<<8)|255;



#if __APPLE__
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEXW ,TEXH, 0, GL_BGRA, GL_UNSIGNED_BYTE,pixels2);
#else
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEXW ,TEXH, 0, GL_RGBA, GL_UNSIGNED_BYTE,pixels2);
#endif

  glBindTexture(GL_TEXTURE_2D, 0);



_tc = !_tc;
}

/*!\brief Cette fonction paramétre la vue (viewport) OpenGL en
 * fonction des dimensions de la fenêtre.
 */
void soleil_draw(void) {
  static GLfloat a0 = 0.0;
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
  gl4duTranslatef(0, 0, -3);
  mat = gl4duGetMatrixData();
  MMAT4XVEC4(lumPos, mat, _lumPos0);
  glUseProgram(_pId); // CHANGE AUTOMATIQUE
  
static float sc=0;
if(!_pause)sc+=1*dt;
if(sc>2.0)sc=0;

static float posZ[50];
//static float posY[50];
//static float posX[50];
static int rot[50];
static int indice=1;
static int init=0;
int j;
static int timer=0;
if(timer==0)timer=SDL_GetTicks();
if(init==0)
for(j=0;j<indice;j++){
posZ[j]=-200*j;
//posY[j]=rand()%range-(range/2);
//posX[j]=rand()%range-(range/2);
rot[j]=rand()%360;
init++;
}
if(SDL_GetTicks()-timer>300*indice)indice++;

for(j=0;j<indice;j++)
posZ[j]+=dt*100;

glEnable(GL_BLEND);
  /// MODIF
  /// MODIF
  glActiveTexture(GL_TEXTURE0);
  glUniform1i(glGetUniformLocation(_pId, "myTexture"), 0);
  glUniform4fv(glGetUniformLocation(_pId, "lumPos"), 1, lumPos);
  glUniform1i(glGetUniformLocation(_pId, "faceNormal"), _faceNormal);
  glUniform1i(glGetUniformLocation(_pId, "phong"), _phong);
  glUniform1i(glGetUniformLocation(_pId, "tc"), _tc);
  /* envoi de toutes les matrices stockées par GL4D */
gl4duRotatef(a0, 0, 1, 0);
 gl4duScalef(sc,sc,sc);
  glBindVertexArray(_vao);
  /* dessiner une première fois la sphère avec le _pId */
//int j;

gl4duPushMatrix();
for(j=0;j<indice;j++){

gl4duPushMatrix();
  glBindTexture(GL_TEXTURE_2D, _texture);


gl4duRotatef(rot[j],0,0, 1);
gl4duRotatef(rot[j],1,0, 0);
gl4duRotatef(rot[j], 0, 1, 0);
 // gl4duScalef(sc*(posZ[j]/400), sc*(posZ[j]/400), sc*(posZ[j]/400));
  gl4duSendMatrices();
  static int animT=0;
  if(animT==0)animT=SDL_GetTicks();
  
  if(SDL_GetTicks()-animT<6000)
  glDrawElements(GL_TRIANGLES, 6 * _longitudes * _latitudes, GL_UNSIGNED_INT, 0);

  gl4duPopMatrix();
}
  gl4duPopMatrix();
  //gl4duTranslatef(_lumPos0[0], _lumPos0[1], _lumPos0[2]);
 // gl4duScalef(0.05, 0.05, 0.05);
  glUseProgram(_pId2);
  gl4duSendMatrices();
  /* dessiner une seconde fois la sphère avec le _pId2 */
 // glDrawElements(GL_TRIANGLES, 6 * _longitudes * _latitudes, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
  if(!_pause)

    a0 += 360 * dt*0.05;
  ///////////////////////////////////
int i;
static Uint32 debut=0,maintenant;
if(debut==0)debut=SDL_GetTicks();


maintenant=SDL_GetTicks();
	for(i=0;i<TEXW*TEXH;i++){
	  im[i]++;
	  im[i]%=256;
if(maintenant-debut<2000)
	  pixels2[i]=(255<<24)|(0<<16)|(im[i]<<8)|255;
else if(maintenant-debut>=2000&&maintenant-debut<4000)
	  pixels2[i]=(255<<24)|(255<<16)|(im[i]<<8)|(180-im[i]);
else if(maintenant-debut>=4000&&maintenant-debut<6000)
	  pixels2[i]=(255<<24)|(0<<16)|((120-im[i])<<8)|0;
else if(maintenant-debut>=6000&&maintenant-debut<8000)
	  pixels2[i]=(255<<24)|((60-im[i])<<16)|(0<<8)|(150-im[i]);
else if(maintenant-debut>=8000&&maintenant-debut<10000)
    pixels2[i]=(255<<24)|((0)<<16)|((-im[i])<<8)|(20);
else 
pixels2[i]=(im[i]<<24)|((im[i])<<16)|((-im[i])<<8)|(im[i]);
}




  glBindTexture(GL_TEXTURE_2D,_texture);
 glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEXW,TEXH, 0, GL_RGBA, GL_UNSIGNED_BYTE,pixels2);
  glBindTexture(GL_TEXTURE_2D,0);
  ///////////////////////////////////

}
void soleil_free(void) {
  if(_vao)
    glDeleteVertexArrays(1, &_vao);
  if(_ibuffer[0])
    glDeleteBuffers(2, _ibuffer);
free(pixels2);
free(im);
}
