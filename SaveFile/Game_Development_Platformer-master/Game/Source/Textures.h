#ifndef __TEXTURES_H__
#define __TEXTURES_H__

#include "Module.h"

#include "List.h"

#define MAX_TETROMINOS 10
#define MAX_TETROMINO_BLOCKS 256

struct SDL_Texture;
struct SDL_Surface;

struct Tetrominos
{
	// The font texture
	SDL_Texture* texture = nullptr;

	// Font setup data
	uint totalLength;
	uint rows, columns;
	uint block_w, block_h;
};

class Textures : public Module
{
public:

	Textures(bool startEnabled);

	// Destructor
	virtual ~Textures();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called before quitting
	bool CleanUp();

	// Load Texture
	SDL_Texture* const Load(const char* path);
	SDL_Texture* const LoadSurface(SDL_Surface* surface);
	int LoadSprite(const char* texturePath, uint columns, uint rows = 1);
	bool UnLoad(SDL_Texture* texture);
	void GetSize(const SDL_Texture* texture, uint& width, uint& height) const;


public:
	Tetrominos tetrominos[MAX_TETROMINOS];
	List<SDL_Texture*> textures;
};


#endif // __TEXTURES_H__