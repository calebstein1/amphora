#ifndef RESOURCES_H
#define RESOURCES_H

#define IMAGES							\
	/* LOADIMG(name, path) */				\
	LOADIMG(Character, "../content/img/character.png")	\
	LOADIMG(Objects, "../content/img/objects.png")		\
	LOADIMG(Overworld, "../content/img/overworld.png")

#define FONTS										\
	/* LOADFONT(name, path) */							\
	LOADFONT(Merriweather, "../content/font/Merriweather/Merriweather-Regular.ttf")	\
	LOADFONT(Roboto, "../content/font/Roboto/Roboto-Regular.ttf")

#define MAPS							\
	/* LOADMAP(name, path) */				\
	LOADMAP(OverworldMap, "../content/maps/overworld.tmx")

#endif /* RESOURCES_H */
