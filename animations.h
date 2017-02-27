/*!\file animations.h
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
#ifndef _ANIMATIONS_H

#define _ANIMATIONS_H

#ifdef __cplusplus
extern "C" {
#endif

  extern void transition_fondu(void (* a0)(int), void (* a1)(int), Uint32 t, Uint32 et, int state);
  extern void animation_flash(int state);
  extern void animation_vide(int state);
  // extern void animation_damier(int state);
  //  extern void animation_base(int state);
  //extern void animation_earth(int state);

//////////// NOUVEAU ANIMATION ////////////
  extern void animation_opening(int state);
  //extern void animation_soleil(int state);
  extern void animation_objet(int state);
  extern void animation_tron(int state);
  extern void animation_voiture(int state); 
  //  extern void animation_ending(int state); 
  //extern void animation_spiral(int state);
  //extern void animation_graph(int state);
  //extern void animation_water(int state);
  extern void animation_city(int state);
  extern void animation_superhot(int state);
  extern void animation_battleship(int state);
//////////// NOUVEAU ANIMATION ////////////
  extern void animationsInit(void);
  /* Dans base.c */
  /*
  extern void base_init(void);
  extern void base_draw(void);
  */
  /* Dans earth.c */
  /*
  extern void earth_init(void);
  extern void earth_draw(void);
*/


extern void init_superhot(void);
extern void draw_superhot(void);




  
//////// ANIMATION OPENING ///////////
  extern void sea_init(void);
  extern void waterAnalyzer_draw(float tabOrg[],int NB);
  extern void HBB_objLoader(void);
  extern void HBB_ModelFree(void);
extern void HBB_openingApi8(Uint32 * pixels,int w,int h,float dt);
  extern void 	HBB_endingApi8(Uint32 * pixels,int w,int h,float dt);
//////// ANIMATION OPENING ///////////

//////// ANIMATION VOITURE ///////////
 // extern void voiture_init(void);
  extern void HBB_initSkyboxVAO(void);
  extern void voiture_draw(void);
 // extern void HBB_objLoader2(void);
 // extern void HBB_ModelFree2(void);
//////// ANIMATION VOITURE ///////////


//////// ENDING /////////////
extern void HBB_flip(Uint32 * pix,int w,int h);
//////// ENDING /////////////


//////// ANIMATION TRON ///////////
extern void tron_draw(void);
extern void tron_initGL(void);
extern void tron_initData(void);
extern void tron_free(void);
//////// ANIMATION TRON ///////////


//////// ANIMATION OBJ ///////////
extern void voiture_draw3(void);
extern void city_draw(void);
//////// ANIMATION OBJ ///////////



extern void init_battleship(void);
extern void draw_battleship(void);





#ifdef __cplusplus
}
#endif

#endif
