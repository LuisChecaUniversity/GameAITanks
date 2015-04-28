#include "LevelMap.h"
#include <SDL.h>
#include <SDL_image.h>
#include "TinyXML\tinyxml.h"
#include "Texture2D.h"

using namespace::std;

//--------------------------------------------------------------------------------------------------

LevelMap::LevelMap(SDL_Renderer* renderer)
{
	mSpritesheet = new Texture2D(renderer);

	//Get the whole xml document.
	TiXmlDocument doc;
	if(!doc.LoadFile(kTilemapPath))
	{
		cerr << doc.ErrorDesc() << endl;
	}

	//Now get the root element.
	TiXmlElement* root = doc.FirstChildElement();
	if(root == NULL)
	{
		cerr << "Failed to load file: No root element." << endl;
		doc.Clear();
	}
	else
	{
		//We have the root - Get the dimensions and convert to integers.
		mWidth		= atoi(root->Attribute("width"));
		mHeight		= atoi(root->Attribute("height"));
		mTileWidth	= atoi(root->Attribute("tilewidth"));
		mTileHeight	= atoi(root->Attribute("tileheight"));

		mMap = new int*[mWidth];
		for(int i =0; i < mWidth; i++)
			mMap[i] = new int[mHeight];
	}

	//Jump to the first 'tileset' element.
	for(TiXmlElement* tilesetElement = root->FirstChildElement("tileset"); tilesetElement != NULL; tilesetElement = tilesetElement->NextSiblingElement())
	{
		//Jump to the first 'image' element - within 'tileset'
		for(TiXmlElement* imageElement = tilesetElement->FirstChildElement("image"); imageElement != NULL; imageElement = imageElement->NextSiblingElement())
		{
			mSpritesheet->LoadFromFile(imageElement->Attribute("source"));
		}
	}

	int w = 0;
	int h = 0;

	//Jump to the first 'layer' element.
	for(TiXmlElement* layerElement = root->FirstChildElement("layer"); layerElement != NULL; layerElement = layerElement->NextSiblingElement())
	{
		//Jump to the first 'data' element - within 'layer'
		for(TiXmlElement* dataElement = layerElement->FirstChildElement("data"); dataElement != NULL; dataElement = dataElement->NextSiblingElement())
		{
			//Jump to the first 'tile' element - within 'data'
			for(TiXmlElement* tileElement = dataElement->FirstChildElement("tile"); tileElement != NULL; tileElement = tileElement->NextSiblingElement())
			{								
				//Populate this position in the array with the tile type.
				//Need this to be a zero to max value, NOT 1 - max.
				mMap[w][h] = atoi(tileElement->Attribute("gid"))-1;

				//Move the array along.
				w++;

				//Move down to the next row.
				if(w >= mWidth)
				{
					h++;
					w=0;
				}
			}
		}
	}

	//Output to console the tilemap.
/*	for(int w = 0; w < mWidth; w++)
	{
		for(int h = 0; h < mHeight; h++)
		{
			cout << mMap[w][h];
		}
		cout << endl;
	}*/
}

//--------------------------------------------------------------------------------------------------

LevelMap::~LevelMap()
{
	//Delete all elements of the array.
	for(int h = 0; h < mHeight; h++)
	{
		delete [] mMap[h];
	}
	delete [] mMap;

	delete mSpritesheet;
}

//--------------------------------------------------------------------------------------------------

void LevelMap::Render()
{
	//Output to console the screen using the spritesheet.
	for(int w = 0; w < mWidth; w++)
	{
		for(int h = 0; h < mHeight; h++)
		{
			SDL_Rect destRect = {(int)w*mTileWidth, (int)h*mTileWidth, mTileWidth, mTileHeight};
			mSpritesheet->Render(GetTileSourceRect(mMap[w][h]), destRect); 
		}
	}
}

//--------------------------------------------------------------------------------------------------

SDL_Rect LevelMap::GetTileSourceRect(int id)
{
	//NOTE: Spritesheets are currently made of a single column of images.
	SDL_Rect portionOfSpritesheet = {0, (int)id*mTileHeight, mTileWidth, mTileHeight};

	return portionOfSpritesheet;
}

//--------------------------------------------------------------------------------------------------
