#include "App.h"
#include "Render.h"
#include "Textures.h"

#include "Defs.h"
#include "Log.h"

#include "SDL_image/include/SDL_image.h"
//#pragma comment(lib, "../Game/Source/External/SDL_image/libx86/SDL2_image.lib")

Textures::Textures(bool startEnabled) : Module(startEnabled)
{
	name.Create("textures");
}

// Destructor
Textures::~Textures()
{}

// Called before render is available
bool Textures::Awake(pugi::xml_node& config)
{
	LOG("Init Image library");
	bool ret = true;

	// Load support for the PNG image format
	int flags = IMG_INIT_PNG;
	int init = IMG_Init(flags);

	if((init & flags) != flags)
	{
		LOG("Could not initialize Image lib. IMG_Init: %s", IMG_GetError());
		ret = false;
	}

	return ret;
}

// Called before the first frame
bool Textures::Start()
{
	LOG("start textures");
	bool ret = true;
	return ret;
}

// Called before quitting
bool Textures::CleanUp()
{
	LOG("Freeing textures and Image library");
	ListItem<SDL_Texture*>* item;

	for(item = textures.start; item != NULL; item = item->next)
	{
		SDL_DestroyTexture(item->data);
	}

	textures.Clear();
	IMG_Quit();
	return true;
}

// Load new texture from file path
SDL_Texture* const Textures::Load(const char* path)
{
	SDL_Texture* texture = NULL;
	SDL_Surface* surface = IMG_Load(path);

	if(surface == NULL)
	{
		LOG("Could not load surface with path: %s. IMG_Load: %s", path, IMG_GetError());
	}
	else
	{
		texture = LoadSurface(surface);
		SDL_FreeSurface(surface);
	}

	return texture;
}

// Unload texture
bool Textures::UnLoad(SDL_Texture* texture)
{
	ListItem<SDL_Texture*>* item;

	for(item = textures.start; item != NULL; item = item->next)
	{
		if(texture == item->data)
		{
			SDL_DestroyTexture(item->data);
			textures.Del(item);
			return true;
		}
	}

	return false;
}

// Translate a surface into a texture
SDL_Texture* const Textures::LoadSurface(SDL_Surface* surface)
{
	SDL_Texture* texture = SDL_CreateTextureFromSurface(app->render->renderer, surface);

	if(texture == NULL)
	{
		LOG("Unable to create texture from surface! SDL Error: %s\n", SDL_GetError());
	}
	else
	{
		textures.Add(texture);
	}

	return texture;
}

// Retrieve size of a texture
void Textures::GetSize(const SDL_Texture* texture, uint& width, uint& height) const
{
	SDL_QueryTexture((SDL_Texture*)texture, NULL, NULL, (int*) &width, (int*) &height);
}

// Load new texture from file path
int Textures::LoadSprite(const char* texture_path, uint columns, uint rows)
{
	int len = columns * rows;
	int id = -1;

	if (texture_path == nullptr || columns == NULL || rows == NULL)
	{
		LOG("Could not load font");
		return id;
	}

	SDL_Texture* tex = Load(texture_path);

	if (tex == nullptr || len >= MAX_TETROMINO_BLOCKS)
	{
		LOG("Could not load font at %s with '%d' columns", texture_path, columns);
		return id;
	}

	id = 0;
	for (; id < MAX_TETROMINOS; ++id)
		if (tetrominos[id].texture == nullptr)
			break;

	if (id == MAX_TETROMINOS)
	{
		LOG("Cannot load font %s. Array is full (max %d).", texture_path, MAX_TETROMINOS);
		return id;
	}

	Tetrominos& tetromino = tetrominos[id];

	tetromino.texture = tex;
	tetromino.rows = rows;
	tetromino.columns = columns;
	tetromino.totalLength = len;

	uint tex_w, tex_h;
	GetSize(tex, tex_w, tex_h);
	tetromino.block_w = tex_w / tetromino.columns;
	tetromino.block_h = tex_h / tetromino.rows;

	LOG("Successfully loaded BMP font from %s", texture_path);

	return id;
}