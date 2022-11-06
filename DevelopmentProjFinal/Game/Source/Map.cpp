#include "App.h"
#include "Render.h"
#include "Textures.h"
#include "Map.h"
#include "Physics.h"
#include "Player.h"

#include "Defs.h"
#include "Log.h"

#include <math.h>
#include "SDL_image/include/SDL_image.h"

Map::Map() : Module(), mapLoaded(false)
{
    name.Create("map");
}

// Destructor
Map::~Map()
{}

// Called before render is available
bool Map::Awake(pugi::xml_node& config)
{
    LOG("Loading Map Parser");
    bool ret = true;

    mapFileName = config.child("mapfile").attribute("path").as_string();
    mapFolder = config.child("mapfolder").attribute("path").as_string();

    return ret;
}

void Map::Draw()
{
    if (mapLoaded == false)
        return;

    /*
    // L04: DONE 6: Iterate all tilesets and draw all their
    // images in 0,0 (you should have only one tileset for now)
    ListItem<TileSet*>* tileset;
    tileset = mapData.tilesets.start;
    while (tileset != NULL) {
        app->render->DrawTexture(tileset->data->texture,0,0);
        tileset = tileset->next;
    }
    */

    // L05: DONE 5: Prepare the loop to draw all tiles in a layer + DrawTexture()

    ListItem<MapLayer*>* mapLayerItem;
    mapLayerItem = mapData.maplayers.start;

    while (mapLayerItem != NULL) {

        //L06: DONE 7: use GetProperty method to ask each layer if your “Draw” property is true.
        if (mapLayerItem->data->properties.GetProperty("Draw") != NULL && mapLayerItem->data->properties.GetProperty("Draw")->value) {

            for (int x = 0; x < mapLayerItem->data->width; x++)
            {
                for (int y = 0; y < mapLayerItem->data->height; y++)
                {
                    // L05: DONE 9: Complete the draw function
                    int gid = mapLayerItem->data->Get(x, y);

                    //L06: DONE 3: Obtain the tile set using GetTilesetFromTileId
                    TileSet* tileset = GetTilesetFromTileId(gid);

                    SDL_Rect r = tileset->GetTileRect(gid);
                    iPoint pos = MapToWorld(x, y);

                    app->render->DrawTexture(tileset->texture,
                        pos.x,
                        pos.y,
                        &r);
                }
            }
        }
        mapLayerItem = mapLayerItem->next;

    }
}

// L05: DONE 8: Create a method that translates x,y coordinates from map positions to world positions
iPoint Map::MapToWorld(int x, int y) const
{
    iPoint ret;

    ret.x = x * mapData.tileWidth;
    ret.y = y * mapData.tileHeight;

    return ret;
}

// Get relative Tile rectangle
SDL_Rect TileSet::GetTileRect(int gid) const
{
    SDL_Rect rect = { 0 };
    int relativeIndex = gid - firstgid;

    // L05: DONE 7: Get relative Tile rectangle
    rect.w = tileWidth;
    rect.h = tileHeight;
    rect.x = margin + (tileWidth + spacing) * (relativeIndex % columns);
    rect.y = margin + (tileWidth + spacing) * (relativeIndex / columns);

    return rect;
}


// L06: DONE 2: Pick the right Tileset based on a tile id
TileSet* Map::GetTilesetFromTileId(int gid) const
{
    ListItem<TileSet*>* item = mapData.tilesets.start;
    TileSet* set = NULL;

    while (item)
    {
        set = item->data;
        if (gid < (item->data->firstgid + item->data->tilecount))
        {
            break;
        }
        item = item->next;
    }

    return set;
}

// Called before quitting
bool Map::CleanUp()
{
    LOG("Unloading map");

    // L04: DONE 2: Make sure you clean up any memory allocated from tilesets/map
    ListItem<TileSet*>* item;
    item = mapData.tilesets.start;

    while (item != NULL)
    {
        RELEASE(item->data);
        item = item->next;
    }
    mapData.tilesets.Clear();

    // L05: DONE 2: clean up all layer data
    // Remove all layers
    ListItem<MapLayer*>* layerItem;
    layerItem = mapData.maplayers.start;

    while (layerItem != NULL)
    {
        RELEASE(layerItem->data);
        layerItem = layerItem->next;
    }

    return true;
}

// Load new map
bool Map::Load()
{
    bool ret = true;

    pugi::xml_document mapFileXML;
    pugi::xml_parse_result result = mapFileXML.load_file(mapFileName.GetString());

    if (result == NULL)
    {
        LOG("Could not load map xml file %s. pugi error: %s", mapFileName, result.description());
        ret = false;
    }

    if (ret == true)
    {
        ret = LoadMap(mapFileXML);
    }

    if (ret == true)
    {
        ret = LoadTileSet(mapFileXML);
    }

    // L05: DONE 4: Iterate all layers and load each of them
    if (ret == true)
    {
        ret = LoadAllLayers(mapFileXML.child("map"));
    }

    // L07 TODO 3: Create colliders
    // Later you can create a function here to load and create the colliders from the map
    // GROUND colliders
    //PhysBody* c1 = app->physics->CreateRectangle(0 + 16, (32 * 3) + 240, 32, 32 * 15, STATIC, ColliderType::PLATFORM);
    //PhysBody* c2 = app->physics->CreateRectangle(32 + 16, (32 * 7) + 176, 32, 32 * 11, STATIC, ColliderType::PLATFORM);
    //PhysBody* c3 = app->physics->CreateRectangle((32 * 2) + 16, (32 * 8) + 160, 32, 32 * 10, STATIC, ColliderType::PLATFORM);
    //PhysBody* c4 = app->physics->CreateRectangle((32 * 3) + 224, (32 * 17) + 16, 32 * 14, 32, STATIC, ColliderType::PLATFORM);
    //PhysBody* c5 = app->physics->CreateRectangle((32 * 17) + 48, (32 * 15) + 48, 32 * 3, 32 * 3, STATIC, ColliderType::PLATFORM);
    //PhysBody* c6 = app->physics->CreateRectangle((32 * 15) + 32, (32 * 12) + 16, 32 * 2, 32, STATIC, ColliderType::PLATFORM);
    //PhysBody* c7 = app->physics->CreateRectangle((32 * 20) + 80, (32 * 11) + 32, 32 * 5, 32 * 2, STATIC, ColliderType::PLATFORM);
    //PhysBody* c8 = app->physics->CreateRectangle((32 * 28) + 64, (32 * 14) + 16, 32 * 4, 32, STATIC, ColliderType::PLATFORM);
    //PhysBody* c9 = app->physics->CreateRectangle((32 * 34) + 32, (32 * 12) + 16, 32 * 2, 32, STATIC, ColliderType::PLATFORM);
    //PhysBody* c10 = app->physics->CreateRectangle((32 * 38) + 192, (32 * 10) + 128, 32 * 12, 32 * 8, STATIC, ColliderType::PLATFORM);
    //PhysBody* c11 = app->physics->CreateRectangle((32 * 50) + 64, (32 * 14) + 64, 32 * 4, 32 * 4, STATIC, ColliderType::PLATFORM);

    //int points1[6] = { 32 * 50, 32 * 10,
    //                   32 * 50, 32 * 14,
    //                   32 * 54, 32 * 14 };
    //PhysBody* c12 = app->physics->CreateChain(0, 0, points1, 6, STATIC, ColliderType::PLATFORM);

    //PhysBody* c13 = app->physics->CreateRectangle((32 * 57) + 16, (32 * 13) + 80, 32, 32 * 5, STATIC, ColliderType::PLATFORM);
    //PhysBody* c14 = app->physics->CreateRectangle((32 * 61) + 48, (32 * 11) + 32, 32 * 3, 32 * 2, STATIC, ColliderType::PLATFORM);
    //PhysBody* c15 = app->physics->CreateRectangle((32 * 67) + 48, (32 * 15) + 48, 32 * 3, 32 * 3, STATIC, ColliderType::PLATFORM);
    //PhysBody* c16 = app->physics->CreateRectangle((32 * 68) + 48, (32 * 9) + 16, 32 * 3, 32, STATIC, ColliderType::PLATFORM);
    //PhysBody* c17 = app->physics->CreateRectangle((32 * 72) + 32, (32 * 12) + 16, 32 * 2, 32, STATIC, ColliderType::PLATFORM);
    //PhysBody* c18 = app->physics->CreateRectangle((32 * 75) + 32, (32 * 8) + 160, 32 * 2, 32 * 10, STATIC, ColliderType::PLATFORM);
    //PhysBody* c19 = app->physics->CreateRectangle((32 * 77) + 16, (32 * 6) + 192, 32, 32 * 12, STATIC, ColliderType::PLATFORM);
    //PhysBody* c20 = app->physics->CreateRectangle((32 * 78) + 64, (32 * 5) + 208, 32 * 4, 32 * 13, STATIC, ColliderType::PLATFORM);
    //PhysBody* c21 = app->physics->CreateRectangle((32 * 82) + 32, (32 * 13) + 80, 32 * 2, 32 * 5, STATIC, ColliderType::PLATFORM);

    //int points2[6] = { 32 * 82, 32 * 11,
    //                   32 * 82, 32 * 13,
    //                   32 * 84, 32 * 13 };
    //PhysBody* c22 = app->physics->CreateChain(0, 0, points2, 6, STATIC, ColliderType::PLATFORM);

    //PhysBody* c23 = app->physics->CreateRectangle((32 * 85) + 32, (32 * 4) + 16, 32 * 2, 32, STATIC, ColliderType::PLATFORM);
    //PhysBody* c24 = app->physics->CreateRectangle((32 * 90) + 48, (32 * 6) + 32, 32 * 3, 32 * 2, STATIC, ColliderType::PLATFORM);
    //PhysBody* c25 = app->physics->CreateRectangle((32 * 95) + 96, (32 * 11) + 112, 32 * 6, 32 * 7, STATIC, ColliderType::PLATFORM);
    //PhysBody* c26 = app->physics->CreateRectangle((32 * 101) + 64, (32 * 15) + 48, 32 * 4, 32 * 3, STATIC, ColliderType::PLATFORM);

    //int points3[6] = { 32 * 101, 32 * 11,
    //                  32 * 101, 32 * 15,
    //                  32 * 105, 32 * 15 };
    //PhysBody* c27 = app->physics->CreateChain(0, 0, points3, 6, STATIC, ColliderType::PLATFORM);

    //// WATER collider
    //PhysBody* c28 = app->physics->CreateRectangle(0 + 1648, 560 + 40, 32 * 103, (32 * 2) + 16, STATIC, ColliderType::WATER);

    ////Camera Fixed To Player Colliders (left side)
    //PhysBody* c29 = app->physics->CreateRectangleSensor((32 * 16) + 27, 0 + 288, 10, 32 * 18, STATIC, ColliderType::CAMERAFIX);
    //PhysBody* c30 = app->physics->CreateRectangleSensor((32 * 16) - 8, 0 + 288, 10, 32 * 18, STATIC, ColliderType::NONCAMERAFIX);

    ////Camera Fixed To Player Colliders (right side)
    //PhysBody* c31 = app->physics->CreateRectangleSensor((2858) - 24, 0 + 288, 10, 32 * 18, STATIC, ColliderType::NONCAMERAFIX_2);
    //PhysBody* c32 = app->physics->CreateRectangleSensor((2858) + 8, 0 + 288, 10, 32 * 18, STATIC, ColliderType::CAMERAFIX_2);

    //// WIN collider (if player touches it, player wins)
    //PhysBody* c33 = app->physics->CreateRectangleSensor((32 * 103) + 16, (32 * 8) + 80, 32, 32 * 5, STATIC, ColliderType::WIN_ZONE);


    //CreateColliders();

    if (ret == true)
    {
        // L04: DONE 5: LOG all the data loaded iterate all tilesets and LOG everything

        LOG("Successfully parsed map XML file :%s", mapFileName.GetString());
        LOG("width : %d height : %d", mapData.width, mapData.height);
        LOG("tile_width : %d tile_height : %d", mapData.tileWidth, mapData.tileHeight);

        LOG("Tilesets----");

        ListItem<TileSet*>* tileset;
        tileset = mapData.tilesets.start;

        while (tileset != NULL) {
            LOG("name : %s firstgid : %d", tileset->data->name.GetString(), tileset->data->firstgid);
            LOG("tile width : %d tile height : %d", tileset->data->tileWidth, tileset->data->tileHeight);
            LOG("spacing : %d margin : %d", tileset->data->spacing, tileset->data->margin);
            tileset = tileset->next;
        }

        // L05: DONE 4: LOG the info for each loaded layer
        ListItem<MapLayer*>* mapLayer;
        mapLayer = mapData.maplayers.start;

        while (mapLayer != NULL) {
            LOG("id : %d name : %s", mapLayer->data->id, mapLayer->data->name.GetString());
            LOG("Layer width : %d Layer height : %d", mapLayer->data->width, mapLayer->data->height);
            mapLayer = mapLayer->next;
        }
    }

    if (mapFileXML) mapFileXML.reset();

    mapLoaded = ret;

    return ret;
}

// L04: DONE 3: Implement LoadMap to load the map properties
bool Map::LoadMap(pugi::xml_node mapFile)
{
    bool ret = true;
    pugi::xml_node map = mapFile.child("map");

    if (map == NULL)
    {
        LOG("Error parsing map xml file: Cannot find 'map' tag.");
        ret = false;
    }
    else
    {
        //Load map general properties
        mapData.height = map.attribute("height").as_int();
        mapData.width = map.attribute("width").as_int();
        mapData.tileHeight = map.attribute("tileheight").as_int();
        mapData.tileWidth = map.attribute("tilewidth").as_int();
    }

    return ret;
}

// L04: DONE 4: Implement the LoadTileSet function to load the tileset properties
bool Map::LoadTileSet(pugi::xml_node mapFile) {

    bool ret = true;

    pugi::xml_node tileset;
    for (tileset = mapFile.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
    {
        TileSet* set = new TileSet();

        // L04: DONE 4: Load Tileset attributes
        set->name = tileset.attribute("name").as_string();
        set->firstgid = tileset.attribute("firstgid").as_int();
        set->margin = tileset.attribute("margin").as_int();
        set->spacing = tileset.attribute("spacing").as_int();
        set->tileWidth = tileset.attribute("tilewidth").as_int();
        set->tileHeight = tileset.attribute("tileheight").as_int();
        set->columns = tileset.attribute("columns").as_int();
        set->tilecount = tileset.attribute("tilecount").as_int();

        // L04: DONE 4: Load Tileset image
        SString tmp("%s%s", mapFolder.GetString(), tileset.child("image").attribute("source").as_string());
        set->texture = app->tex->Load(tmp.GetString());

        mapData.tilesets.Add(set);
    }

    return ret;
}

// L05: DONE 3: Implement a function that loads a single layer layer
bool Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
    bool ret = true;

    //Load the attributes
    layer->id = node.attribute("id").as_int();
    layer->name = node.attribute("name").as_string();
    layer->width = node.attribute("width").as_int();
    layer->height = node.attribute("height").as_int();

    //L06: DONE 6 Call Load Propoerties
    LoadProperties(node, layer->properties);

    //Reserve the memory for the data 
    layer->data = new uint[layer->width * layer->height];
    memset(layer->data, 0, layer->width * layer->height);

    //Iterate over all the tiles and assign the values
    pugi::xml_node tile;
    int i = 0;
    for (tile = node.child("data").child("tile"); tile && ret; tile = tile.next_sibling("tile"))
    {
        layer->data[i] = tile.attribute("gid").as_int();
        i++;
    }

    return ret;
}

// L05: DONE 4: Iterate all layers and load each of them
bool Map::LoadAllLayers(pugi::xml_node mapNode) {
    bool ret = true;

    for (pugi::xml_node layerNode = mapNode.child("layer"); layerNode && ret; layerNode = layerNode.next_sibling("layer"))
    {
        //Load the layer
        MapLayer* mapLayer = new MapLayer();
        ret = LoadLayer(layerNode, mapLayer);

        //add the layer to the map
        mapData.maplayers.Add(mapLayer);
    }

    return ret;
}

// L06: DONE 6: Load a group of properties from a node and fill a list with it
bool Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{
    bool ret = false;

    for (pugi::xml_node propertieNode = node.child("properties").child("property"); propertieNode; propertieNode = propertieNode.next_sibling("property"))
    {
        Properties::Property* p = new Properties::Property();
        p->name = propertieNode.attribute("name").as_string();
        p->value = propertieNode.attribute("value").as_bool(); // (!!) I'm assuming that all values are bool !!

        properties.list.Add(p);
    }

    return ret;
}


// L06: DONE 7: Ask for the value of a custom property
Properties::Property* Properties::GetProperty(const char* name)
{
    ListItem<Property*>* item = list.start;
    Property* p = NULL;

    while (item)
    {
        if (item->data->name == name) {
            p = item->data;
            break;
        }
        item = item->next;
    }

    return p;
}
//
//bool Map::Parallax() {
//
//    //// L05: DONE 9: Complete the draw function
//    //int gid = mapLayerItem->data->Get(x, y);
//
//    //TileSet* tileset = GetTilesetFromTileId(gid);
//
//    //SDL_Rect r = tileset->GetTileRect(gid);
//    //iPoint pos = MapToWorld(x, y);
//
//    //app->render->DrawTexture(tileset->texture,
//    //    pos.x,
//    //    pos.y,
//    //    &r);
//
//}

bool Map::CreateColliders()
{
    bool ret = true;

    ListItem<MapLayer*>* mapLayerItem;
    mapLayerItem = mapData.maplayers.start;

    while (mapLayerItem != NULL)
    {
        if (mapLayerItem->data->name == "Collisions")
        {
            int halfTileHeight = mapData.tileHeight / 2;
            int halfTileWidth = mapData.tileWidth / 2;

            for (int x = 0; x < mapLayerItem->data->width; x++)
            {
                for (int y = 0; y < mapLayerItem->data->height; y++)
                {
                    if (mapLayerItem->data->Get(x, y) == 695)
                    {
                        iPoint pos = MapToWorld(x, y);
                        app->physics->CreateRectangle(pos.x + halfTileHeight, pos.y + halfTileWidth, mapData.tileWidth, mapData.tileHeight, STATIC, ColliderType::PLATFORM);
                    }
                }
            }
        }

        mapLayerItem = mapLayerItem->next;
    }
    return ret;
}
