#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<time.h>
#include<math.h>
#include<SDL2/SDL.h>

#include "animations.h"
#include "HBB_alphabet.h"



enum{
	HBB_WINDOW_W=640,
	HBB_WINDOW_H=400,
	HBB_POLICE_W=5,
	HBB_POLICE_H=5,
	BLUE=0,
	GREEN=8,
	RED=16,
	ALPHA=24
};

//les outils pour modifier le cube, 3 colours sont 3 cotes de cube
	/*
static Uint32 ice[3]={(0<<RED)|(180<<GREEN)|(255<<BLUE),
					(0<<RED)|(210<<GREEN)|(255<<BLUE),
					(0<<RED)|(255<<GREEN)|(255<<BLUE)};
	
static Uint32 gN4[3]={(0<<RED)|(100<<GREEN)|(0<<BLUE),

				(0<<RED)|(150<<GREEN)|(0<<BLUE),

				(0<<RED)|(255<<GREEN)|(0<<BLUE)};

	
*/
static Uint32 gold[3]={(255<<RED)|(100<<GREEN)|(0<<BLUE),
					(255<<RED)|(150<<GREEN)|(0<<BLUE),
					(255<<RED)|(220<<GREEN)|(0<<BLUE)};
	static Uint32 bl[3]={(0<<RED)|(0<<GREEN)|(150<<BLUE),
				(0<<RED)|(0<<GREEN)|(255<<BLUE),
				(0<<RED)|(0<<GREEN)|(255<<BLUE)};
	static Uint32 mgnt[3]={(255<<RED)|(0<<GREEN)|(100<<BLUE),
					(200<<RED)|(0<<GREEN)|(160<<BLUE),
					(150<<RED)|(0<<GREEN)|(255<<BLUE)};
	static Uint32 c[3]={(0<<RED)|(250<<GREEN)|(150<<BLUE),
				(0<<RED)|(200<<GREEN)|(200<<BLUE),
				(0<<RED)|(150<<GREEN)|(255<<BLUE)};
	static Uint32 c2[3]={(150<<RED)|(50<<GREEN)|(250<<BLUE),
				(150<<RED)|(100<<GREEN)|(200<<BLUE),
				(150<<RED)|(150<<GREEN)|(150<<BLUE)};
	static Uint32 c3[3]={(200<<RED)|(150<<GREEN)|(200<<BLUE),
				(255<<RED)|(150<<GREEN)|(150<<BLUE),
				(150<<RED)|(150<<GREEN)|(60<<BLUE)};
	static Uint32 blncN3[3]={(150<<RED)|(250<<GREEN)|(50<<BLUE),
					(200<<RED)|(200<<GREEN)|(150<<BLUE),
					(255<<RED)|(155<<GREEN)|(200<<BLUE)};
	static Uint32 rN4[3]={(150<<RED)|(0<<GREEN)|(0<<BLUE),
				(200<<RED)|(0<<GREEN)|(0<<BLUE),
				(255<<RED)|(0<<GREEN)|(0<<BLUE)};
	static Uint32 Gre[3]={(100<<RED)|(100<<GREEN)|(100<<BLUE),
				(150<<RED)|(150<<GREEN)|(150<<BLUE),
				(200<<RED)|(200<<GREEN)|(200<<BLUE)};

	static Uint32 HBB_terre[3]={
				(113<<RED)|(64<<GREEN)|(0<<BLUE),
				(130<<RED)|(70<<GREEN)|(0<<BLUE),
				(145<<RED)|(79<<GREEN)|(0<<BLUE)};
	static Uint32 HBB_blnc2[3]={
			  	(235<<RED)|(235<<GREEN)|(235<<BLUE),
  				(250<<RED)|(250<<GREEN)|(250<<BLUE),
  				(220<<RED)|(220<<GREEN)|(220<<BLUE),};
void HBB_flip(Uint32 * pix,int w,int h){
  int i,j;
  Uint32 tmp;
  for(j=0; j<h/2;j++)
    for(i=0;i<w;i++){
      tmp=pix[i+j*w];
      pix[i+j*w]=pix[i+(h-j-1)*w];
      pix[i+(h-j-1)*w]=tmp;
    }
}
void HBB_cube3D(int posy,int posx,int t,Uint32 * pix, int w,int h,Uint32 *color){
	int i,j;
	float r;
	for(j=0;j<t/2+1;j++)
	for(i=0,r=0;i<t+1;i++,r+=0.5)
	if(posx+i+t>=0&&posx+i+t<w&&posy-j+t>=0&&posy-j+t<h){
	pix[posx+i+2*j+(posy-j+(int)r)*w]=color[0];
	pix[posx+i+2*j+(posy-j-1+(int)r)*w]=color[0];		
	}
	for(i=0,r=0;i<t;i++,r+=0.5)
	for(j=0;j<=t;j++)
	if(posx+i+t>=0&&posx+i+t<w&&posy-j+t>=0&&posy-j+t<h)
	pix[posx+i+(posy+j+(int)r)*w]=color[2];
	
	for(i=0,r=0.5;i<t;i++,r+=0.5)
	for(j=0;j<=t;j++)
	if(posx+i+t>=0&&posx+i+t<w&&posy-j+t>=0&&posy-j+t<h)
	pix[posx+t+i+(posy+t+t/2-(j+(int)r))*w]=color[1];
	
}

void HBB_police(Uint32 *pix,int w,int h,int sizeCB,Uint32 *colorCB,int alpha[],int alphaX,int alphaY){
	int posy=30+alphaY;
	int posx=240+alphaX;
	int i,j,sz=sizeCB;// pair
	float dt;
	for(j=0;j<5;j++)
	for(i=0,dt=0.5;i<HBB_POLICE_W;i++,dt+=0.5)
	if(alpha[i+j*HBB_POLICE_W]==3)HBB_cube3D(posy-j*sz-j*2+(i*sz/2+(int)(dt)),posx+i*sz+i,sz,pix,w,h,colorCB);
}




void HBB_endingApi8(Uint32 * pixels,int w,int h,float dt){
		static int translation[6];
		int i;
		int posX=650,posY=470;
		int * tabAlpha[]={alphaA,alphaP,alphaI,alpha8};
		Uint32 * tabColor[]={HBB_blnc2,HBB_blnc2,HBB_blnc2,rN4};
		translation[0]+=(int)round(130*dt);
		translation[0]%=120;		
		for(i=1;i<4;i++)
			translation[i]=(translation[i-1]+30)%120;
		
	 	for(i=0;i<4;i++)
  		HBB_police(pixels,w,h,21,tabColor[i],tabAlpha[i],-160+i*152+(i==0?-20:0)+posX,(i==0?-10:0)+70+/*translation[i]*/+posY);
}		

void HBB_endingParis8(Uint32 * pixels,int w,int h,float dt){
		int i;
		int posX=600,posY=500;
		int * tabAlpha[]={alphaP,alphaA,alphaR,alphaI,alphaSS,alpha8};
		Uint32 * tabColor[]={HBB_blnc2,HBB_blnc2,HBB_blnc2,HBB_blnc2,HBB_blnc2,rN4};
/*
		translation[0]+=(int)round(10*dt);
		translation[0]%=120;		
		for(i=1;i<6;i++)
			translation[i]=(translation[i-1]+30)%120;
*/
		
	 	for(i=0;i<6;i++)
  		HBB_police(pixels,w,h,25,tabColor[i],tabAlpha[i],-140+i*190+posX,70+posY);
}		
void HBB_endingRealisateur(Uint32 * pixels,int w,int h,float dt){
//	int posX=600,posY=600; // plein ecran
	int posX=750,posY=400;
	int * tabAlpha2[]={alphaR,alphaE,alphaA,alphaL,alphaI,alphaSS,alphaE,alphaP,alphaA,alphaR};
	int i;
		int * tabAlpha5[]={alphaB,alphaA,alphaE,alphaH,alphaO,alphaB,alphaE,alphaA,alphaN,};
 	static int timer=0;
	 if(timer==0)timer=SDL_GetTicks();	
	Uint32 * colors[]={c,c2,c3,blncN3,mgnt,Gre,gold,bl,rN4,HBB_terre};
	Uint32 * colors2[]={gold,gold,gold,gold,gold,gold,gold,gold,gold,gold,};
	Uint32 ** col;
	if(SDL_GetTicks()-timer>0&&SDL_GetTicks()-timer<2500)col=colors2;
	else col=colors;
static int rapidite=100;
	for(i=0;i<7;i++)
	  if(SDL_GetTicks()-timer>rapidite*i)HBB_police(pixels,w,h,17,col[rand()%10],tabAlpha2[i],-250+140*i+posX,20+posY);


	  if(SDL_GetTicks()-timer>rapidite*7)HBB_police(pixels,w,h,17,col[rand()%10],alphaP,30+posX,210+posY);
	  if(SDL_GetTicks()-timer>rapidite*8)HBB_police(pixels,w,h,17,col[rand()%10],alphaA,155+posX,210+posY);
	  if(SDL_GetTicks()-timer>rapidite*9)HBB_police(pixels,w,h,17,col[rand()%10],alphaR,280+posX,210+posY);

if(SDL_GetTicks()-timer>rapidite*11)
		for(i=0;i<9;i++)	
		HBB_police(pixels,w,h,19,col[rand()%10],tabAlpha5[i],-400+i*140+(i>2?30:0)+posX,400+posY);		
  		

}


void HBB_endingRemerciement(Uint32 * pixels,int w,int h){
		static int timer2=0;
		int i;
		//int posX=400,posY=550;// ecran agrandi
		int posX=550,posY=400;// ecran petit
		int * tabAlpha3[]={alphaF,alphaA,alphaR,alphaE,alphaSS};
		int * tabAlpha4[]={alphaB,alphaE,alphaL,alphaH,alphaA,alphaD,alphaJ,};
	 	int * tabAlpha5[]={alphaR,alphaE,alphaM,alphaE,alphaR,alphaC,alphaI,alphaE,alphaM,alphaE,alphaN,alphaT,};
  	    if(timer2==0)timer2=SDL_GetTicks();	

		if(SDL_GetTicks()-timer2>500)
		for(i=0;i<12;i++)
  		HBB_police(pixels,w,h,15,blncN3,tabAlpha5[i],-200+i*113+posX,posY);
		if(SDL_GetTicks()-timer2>1500)
	 	for(i=0;i<5;i++)
  		HBB_police(pixels,w,h,17,c,tabAlpha3[i],50+i*133+posX,180+posY);
		if(SDL_GetTicks()-timer2>2500)
	 	for(i=0;i<7;i++)
  		HBB_police(pixels,w,h,17,c,tabAlpha4[i],50+i*133+posX,350+posY);
}


/*
	static Uint32 t0 = 0, t;
  	static int timer=0;
 	float dt = 0.0;
  	dt = ((t = SDL_GetTicks()) - t0) / 1000.0;
  	t0 = t;
	if(timer==0)timer=SDL_GetTicks();		

	if(SDL_GetTicks()-timer>0&&SDL_GetTicks()-timer<=6000)
	HBB_endingRemerciement(pixels,HBB_WINDOW_W,HBB_WINDOW_H);
	else if(SDL_GetTicks()-timer>6000&&SDL_GetTicks()-timer<=14000)
	HBB_endingRealisateur(pixels,HBB_WINDOW_W,HBB_WINDOW_H,dt);
	else if(SDL_GetTicks()-timer>14000&&SDL_GetTicks()-timer<=21000)
	HBB_endingParis8(pixels,HBB_WINDOW_W,HBB_WINDOW_H,dt);
	else 
	HBB_endingApi8(pixels,HBB_WINDOW_W,HBB_WINDOW_H,dt);

*/
