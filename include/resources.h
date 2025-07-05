#ifndef RESOURCES_H
#define RESOURCES_H

#define IMAGES							\
	/* LOADIMG(name, path) */				\
	LOADIMG(Character, content/img/character.png)	\
	LOADIMG(Objects, content/img/objects.png)		\
	LOADIMG(Overworld, content/img/overworld.png)	\
	LOADIMG(Grassland, content/img/grassland.png)

#define FONTS										\
	/* LOADFONT(name, path) */							\
	LOADFONT(Merriweather, content/font/Merriweather/Merriweather-Regular.ttf)	\
	LOADFONT(Roboto, content/font/Roboto/Roboto-Regular.ttf)

#define MAPS							\
	/* LOADMAP(name, path) */				\
	LOADMAP(Overworld, content/maps/overworld.tmj)	\
	LOADMAP(Grassland, content/maps/grassland.tmj)

#define SFX							\
	/* LOADSFX(name, path) */				\
	LOADSFX(leaves01, content/sfx/leaves01.ogg)	\
	LOADSFX(leaves02, content/sfx/leaves02.ogg)	\
	LOADSFX(keypress, content/sfx/keypress-019.wav)

#define MUSIC							\
	/* LOADMUSIC(name, path) */				\
	LOADMUSIC(forest, content/music/forest.ogg)

#endif /* RESOURCES_H */
