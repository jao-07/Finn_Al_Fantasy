ALLEGRO_VERSION=5.0.10
MINGW_VERSION=4.7.0
FOLDER=C:

FOLDER_NAME=\allegro-$(ALLEGRO_VERSION)-mingw-$(MINGW_VERSION)
PATH_ALLEGRO=$(FOLDER)$(FOLDER_NAME)
LIB_ALLEGRO=\lib\liballegro-$(ALLEGRO_VERSION)-monolith-mt.a
INCLUDE_ALLEGRO=\include

all: Finn_Al_Fantasy.exe
	
Finn_Al_Fantasy.exe: Finn_Al_Fantasy.o
	gcc -o Finn_Al_Fantasy.exe Finn_Al_Fantasy.o $(PATH_ALLEGRO)$(LIB_ALLEGRO)

Finn_Al_Fantasy.o: Finn_Al_Fantasy.c
	gcc -I $(PATH_ALLEGRO)$(INCLUDE_ALLEGRO) -c Finn_Al_Fantasy.c	
	
	
clean:

	del Finn_Al_Fantasy.o
	del Finn_Al_Fantasy.exe

