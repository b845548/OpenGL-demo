#  Makefile
#  Auteur : Farès BELHADJ
#  Email  : amsi@ai.univ-paris8.fr
#  Date   : 03/02/2014

# Modification
#  Auteur : BAE HOBEAN
#  Email  : b845548@gmail.com
#  Date   : 24/05/2016


SHELL = /bin/sh
# définition des commandes utilisées
CC = gcc
ECHO = echo
RM = rm -f
TAR = tar
MKDIR = mkdir
CHMOD = chmod
CP = rsync -R
# déclaration des options du compilateur
PG_FLAGS =
SDL_CFLAGS = $(shell sdl2-config --cflags)
SDL_LDFLAGS = $(shell sdl2-config --libs) -lSDL2_mixer -lSDL2_image -lSDL2_ttf
CPPFLAGS = -I. $(SDL_CFLAGS) 
CFLAGS = -Wall -O3 -I/home/b845548/local/include -I/usr/local/include/SDL2 -I/usr/include/lib
LDFLAGS = -lm $(SDL_LDFLAGS) -lGL4Dummies -L/home/b845548/local/lib 
UNAME := $(shell uname)
ifeq ($(UNAME),Darwin)
	MACOSX_DEPLOYMENT_TARGET = 10.8
        CFLAGS += -I/usr/X11R6/include -mmacosx-version-min=$(MACOSX_DEPLOYMENT_TARGET)
        LDFLAGS += -framework OpenGL -mmacosx-version-min=$(MACOSX_DEPLOYMENT_TARGET)
else
        LDFLAGS +=  -L/usr/lib -L/usr/X11R6/lib -lGL -lGLU
endif

#définition des fichiers et dossiers
PROGNAME = API8
PACKAGE=$(PROGNAME)
VERSION = 1.43
distdir = $(PACKAGE)-$(VERSION)
HEADERS = audioHelper.h animations.h HBB_fractal.h glm/glm.h glm/glmint.h
SOURCES = audioHelper.c animations.c window.c HBB_superhot.c HBB_battleship.c HBB_objet.c HBB_credit.c HBB_tron.c HBB_fractal.c glm/glm.c glm/glm_util.c glm/glmimg_png.c glm/glmimg_jpg.c glm/glmimg_sim.c glm/glmimg_devil.c glm/glmimg_sdl.c glm/glmimg.c
OBJ = $(SOURCES:.c=.o)
DOXYFILE = documentation/Doxyfile
EXTRAFILES = COPYING shaders/basic.vs shaders/mix.fs shaders/basic.fs	\
shaders/base.vs shaders/base.fs shaders/earth.vs shaders/earth.fs	\
takeonme.mod images/land_ocean_ice_2048_glossmap.png			\
images/land_ocean_ice_2048.png images/moon.jpg
DISTFILES = $(SOURCES) Makefile $(HEADERS) $(DOXYFILE) $(EXTRAFILES)

all: $(PROGNAME)

$(PROGNAME): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $(PROGNAME)

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

dist: distdir
	$(CHMOD) -R a+r $(distdir)
	$(TAR) zcvf $(distdir).tgz $(distdir)
	$(RM) -r $(distdir)

distdir: $(DISTFILES)
	$(RM) -r $(distdir)
	$(MKDIR) $(distdir)
	$(CHMOD) 777 $(distdir)
	$(CP) $(DISTFILES) $(distdir)

doc: $(DOXYFILE)
	cat $< | sed -e "s/PROJECT_NAME *=.*/PROJECT_NAME = $(PROGNAME)/" | sed -e "s/PROJECT_NUMBER *=.*/PROJECT_NUMBER = $(VERSION)/" >> $<.new
	mv -f $<.new $<
	cd documentation && doxygen && cd ..

clean:
	@$(RM) -r $(PROGNAME) $(OBJ) *~ $(distdir).tgz gmon.out core.* documentation/*~ shaders/*~ GL4D/*~ documentation/html
