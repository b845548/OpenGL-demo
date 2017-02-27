#include<stdio.h>
#include<math.h>
#include<time.h>
#include<assert.h>
#include<stdlib.h>
#include<SDL2/SDL.h>
#include"animations.h"


enum{// SDL
  RED=16,// decalage premiere 0 bit
  GREEN=8, // decalage 0~8 bit
  BLUE=0, // decalage 8~16 bit
  ALPHA=24 // decalage 16~24 bit
};
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
	pix[pos]=(255<<ALPHA)+(r<<RED)+(g<<GREEN)+(b<<BLUE);
    }
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
char *flag;
flag=(char *)calloc(max+1,sizeof(char));
if(flag==NULL){
fprintf(stderr,"err allocation HBB_afficheGraph");
return;	
}
sous_movement(g,flag,w,h,dt,rapide);
free(flag);
}


static void sous_reinitPosition(graph g,char * flag,int w,int h){
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
char *flag;
flag=(char *)calloc(max,sizeof(char));
if(flag==NULL){
fprintf(stderr,"err allocation HBB_afficheGraph");
return;	
}
sous_reinitPosition(g,flag,w,h);
free(flag);
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
char *flag;
flag=(char *)calloc(max,sizeof(char));
if(flag==NULL){
fprintf(stderr,"err allocation HBB_afficheGraph");
return;	
}
sous_affiche(g,flag,pixels,w,h,x,y);
free(flag);
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

graph HBB_createGraph(int nodeMax,int connexionMin,int connexionMax,int w,int h){
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
	return adress[0];
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


void HBB_deleteGraph(graph g,int max){
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


